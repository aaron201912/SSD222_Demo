#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/prctl.h>

#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include "mi_divp.h"
#include "mi_sensor.h"
#include "mi_sensor_datatype.h"
#include "mi_iqserver.h"
#include "isp_cus3a_if.h"
#include "mi_isp_datatype.h"
#include "mi_isp.h"
#include "mi_isp_iq.h"
#include "st_framequeue.h"

#define SUPPORT_VIF_VPE_REALTIMEMODE            0

#define ASCII_COLOR_RED                         "\033[1;31m"
#define ASCII_COLOR_WHITE                       "\033[1;37m"
#define ASCII_COLOR_YELLOW                      "\033[1;33m"
#define ASCII_COLOR_BLUE                        "\033[1;36m"
#define ASCII_COLOR_GREEN                       "\033[1;32m"
#define ASCII_COLOR_END                         "\033[0m"

#define SENSOR_ID                               E_MI_SNR_PAD_ID_0
#define VIFDEV_ID                               0
#define CAPTURE_SRC_MODULE                      E_MI_MODULE_ID_DIVP // 0: VPE, 1:DIVP

// sensor output (1280 * 720) -> snap pic (640 * 480)
#define VENC_WIDTH                              1280
#define VENC_HEIGHT                             720
#define SNAP_WIDTH                              640
#define SNAP_HEIGHT                             480

#define SENSOR_RGB_FPS                          30
static MI_BOOL g_bSnapYuvRun = FALSE;
static MI_BOOL g_bCatureYuv = FALSE;
static MI_BOOL g_bSnapJpegRun = FALSE;
static MI_BOOL g_bCatureJpeg = FALSE;

#define ST_MAX_VENC_NUM                         (16)
#define venc_info(fmt, args...)     ({do{printf(ASCII_COLOR_WHITE"[APP INFO]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})
#define venc_err(fmt, args...)      ({do{printf(ASCII_COLOR_RED  "[APP ERR ]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})

typedef struct ST_VencAttr_s
{
    MI_U32     u32BindVpeChn;
    MI_U32     u32BindVpePort;
    MI_SYS_BindType_e  eBindType;
    MI_U32  u32BindParam;

    MI_VENC_CHN vencChn;
    MI_VENC_ModType_e eType;
    MI_U32    u32Width;
    MI_U32     u32Height;
    char szStreamName[128];
    MI_BOOL bUsed;
    MI_BOOL bStart;
}ST_VencAttr_t;

static ST_VencAttr_t gstVencattr[ST_MAX_VENC_NUM];
static pthread_t     g_snapYuvThread = NULL;
static pthread_t     g_snapJpegThread = NULL;

MI_BOOL ST_DoSetIqBin(MI_VPE_CHANNEL Vpechn,char *pConfigPath)
{
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    CUS3A_ALGO_STATUS_t stCus3AAlgoStatus;
    MI_U8  u8ispreadycnt = 0;
    MI_ISP_API_BYPASS_TYPE_t stParam;

    printf("%s:%d, ST_DoSetIqBin \n", __FUNCTION__, __LINE__);
	
	memset(&stParam, 0x00, sizeof(MI_ISP_API_BYPASS_TYPE_t));
	stParam.bEnable = TRUE;
	stParam.eAPIIndex = API20_DM;

    if (strlen(pConfigPath) == 0)
    {
        printf("IQ Bin File path NULL!\n");
        return FALSE;
    }
    printf("%s:%d,vpech: %d,iqapi:%s\n", __FUNCTION__, __LINE__,Vpechn,pConfigPath);
    memset(&status, 0x00, sizeof(status));
    memset(&stCus3AAlgoStatus, 0x00, sizeof(stCus3AAlgoStatus));
    do
    {
        if(u8ispreadycnt > 100)
        {
            printf("%s:%d, isp ready time out \n", __FUNCTION__, __LINE__);
            u8ispreadycnt = 0;
            break;
        }

        MI_ISP_IQ_GetParaInitStatus(Vpechn, &status);
        CUS3A_GetAlgoStatus((CUS3A_ISP_CH_e)Vpechn, &stCus3AAlgoStatus);
        //printf("00%s,%d:bFlag:%d, Ae:%d  cnt:%d \n", __FUNCTION__, __LINE__, status.stParaAPI.bFlag, stCus3AAlgoStatus.Ae, u8ispreadycnt);
        if((status.stParaAPI.bFlag != 1) || (stCus3AAlgoStatus.Ae != E_ALGO_STATUS_RUNNING))
        {
            usleep(300*1000);
            u8ispreadycnt++;
            //printf("3%s,%d:bFlag:%d, Ae:%d  cnt:%d \n", __FUNCTION__, __LINE__, status.stParaAPI.bFlag, stCus3AAlgoStatus.Ae, u8ispreadycnt);
            continue;
        }

        u8ispreadycnt = 0;

        printf("loading api bin...path:%s\n",pConfigPath);
        MI_ISP_API_CmdLoadBinFile(Vpechn, (char *)pConfigPath, 1234);

        usleep(10*1000);
    }while((status.stParaAPI.bFlag != 1) || (stCus3AAlgoStatus.Ae != E_ALGO_STATUS_RUNNING));

    return 0;
}

// VIF(0,0,0) 1280*720 -> VPE(0,0,0) 1280*720 -> divp(0,1,0) 640*480

MI_U32 ST_BaseModuleInit(void)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_VPE_3DNR_Level_e en3dNrLevel = E_MI_VPE_3DNR_LEVEL2;
    MI_SYS_Rotate_e enRotation = E_MI_SYS_ROTATE_NONE;
    
    MI_SNR_PAD_ID_e eSnrPad = SENSOR_ID;
    MI_VIF_DEV s32vifDev = VIFDEV_ID;
    MI_VIF_CHN s32vifChn = s32vifDev*4;

    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    STCHECKRESULT(ST_Sys_Init());

    if(eVifHdrType > 0)
        MI_SNR_SetPlaneMode(eSnrPad, TRUE);
    else
        MI_SNR_SetPlaneMode(eSnrPad, FALSE);

    MI_SNR_QueryResCount(eSnrPad, &u32ResCount);
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        MI_SNR_GetRes(eSnrPad, u8ResIndex, &stRes);
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }

    // select the index of res, choose 0 (1280 * 720)
    MI_SNR_SetRes(eSnrPad, 0);
    MI_SNR_Enable(eSnrPad);
    MI_SNR_GetPadInfo(eSnrPad, &stPad0Info);
    MI_SNR_GetPlaneInfo(eSnrPad, 0, &stSnrPlane0Info);
    MI_SNR_SetFps(eSnrPad,SENSOR_RGB_FPS);

    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
    
    // init vif(0,0,0)
    MI_VIF_DevAttr_t stDevAttr;
    memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

    stDevAttr.eIntfMode = stPad0Info.eIntfMode;
    stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
    stDevAttr.eHDRType = eVifHdrType;
    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        stDevAttr.eClkEdge = stPad0Info.unIntfAttr.stBt656Attr.eClkEdge;
    else
        stDevAttr.eClkEdge = E_MI_VIF_CLK_EDGE_DOUBLE;
    
    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
        stDevAttr.eDataSeq =stPad0Info.unIntfAttr.stMipiAttr.eDataYUVOrder;
    else
        stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;

    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        memcpy(&stDevAttr.stSyncAttr, &stPad0Info.unIntfAttr.stBt656Attr.stSyncAttr, sizeof(MI_VIF_SyncAttr_t));

    stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL;

    ExecFunc(MI_VIF_SetDevAttr(s32vifDev, &stDevAttr), MI_SUCCESS);
    ExecFunc(MI_VIF_EnableDev(s32vifDev), MI_SUCCESS);

    ST_VIF_PortInfo_T stVifPortInfoInfo;
    memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
    stVifPortInfoInfo.u32RectX = 0;
    stVifPortInfoInfo.u32RectY = 0;
    stVifPortInfoInfo.u32RectWidth = u32CapWidth;
    stVifPortInfoInfo.u32RectHeight = u32CapHeight;
    stVifPortInfoInfo.u32DestWidth = u32CapWidth;
    stVifPortInfoInfo.u32DestHeight = u32CapHeight;
    stVifPortInfoInfo.eFrameRate = eFrameRate;
    stVifPortInfoInfo.ePixFormat = ePixFormat;
    STCHECKRESULT(ST_Vif_CreatePort(s32vifChn, 0, &stVifPortInfoInfo));
    STCHECKRESULT(ST_Vif_StartPort(0, s32vifChn, 0));
    
    if (enRotation != E_MI_SYS_ROTATE_NONE)
    {
        MI_BOOL bMirror = FALSE, bFlip = FALSE;

        switch(enRotation)
        {
        case E_MI_SYS_ROTATE_NONE:
            bMirror= FALSE;
            bFlip = FALSE;
            break;
        case E_MI_SYS_ROTATE_90:
            bMirror = FALSE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_180:
            bMirror = TRUE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_270:
            bMirror = TRUE;
            bFlip = FALSE;
            break;
        default:
            break;
        }

        MI_SNR_SetOrien(eSnrPad, bMirror, bFlip);
    }

    // init vpe(0,0,0)
    memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
    if(eSnrPad == E_MI_SNR_PAD_ID_0)
        stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR0;
    else if(eSnrPad == E_MI_SNR_PAD_ID_1)
        stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR1;
    else if(eSnrPad == E_MI_SNR_PAD_ID_2)
        stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR2;
    else if(eSnrPad == E_MI_SNR_PAD_ID_3)
        stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR3;
    else
        stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR_INVALID;

    stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
    stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
    stVpeChannelInfo.u32X = 0;
    stVpeChannelInfo.u32Y = 0;
    stVpeChannelInfo.u16VpeCropW = u32CapWidth;
    stVpeChannelInfo.u16VpeCropH = u32CapHeight;
#if SUPPORT_VIF_VPE_REALTIMEMODE
	stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
#else
    stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;//E_MI_VPE_RUN_REALTIME_MODE;
#endif
    stVpeChannelInfo.eFormat = ePixFormat;
    stVpeChannelInfo.e3DNRLevel = en3dNrLevel;
    stVpeChannelInfo.eHDRtype = eVpeHdrType;
    stVpeChannelInfo.bRotation = FALSE;
    STCHECKRESULT(ST_Vpe_CreateChannel(0, &stVpeChannelInfo));
    STCHECKRESULT(ST_Vpe_StartChannel(0));

    // bind vif(0,0,0) to vpe(0,0,0)
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = s32vifDev;
    stBindInfo.stSrcChnPort.u32ChnId = s32vifChn;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
#if SUPPORT_VIF_VPE_REALTIMEMODE
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
#else
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;//E_MI_SYS_BIND_TYPE_REALTIME;
#endif
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    printf("bind vif(%d,%d,%d,%d) to VPE(%d,%d,%d,%d)\n", E_MI_MODULE_ID_VIF, s32vifDev, s32vifChn, 0,
            E_MI_MODULE_ID_VPE, 0, 0, 0);

    ST_VPE_PortInfo_T stVpePortInfo;
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = 0;
    stVpePortInfo.u16OutputWidth = u32CapWidth;
    stVpePortInfo.u16OutputHeight = u32CapHeight;
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    STCHECKRESULT(ST_Vpe_StartPort(0 , &stVpePortInfo)); //bind to disp for panenl & divp for barcode scanning
    
    return MI_SUCCESS;
}

MI_U32 ST_Vpe_Divp_Bind()
{
    MI_VPE_PortMode_t stVpeMode;
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stDivpPortAttr;
    MI_SYS_ChnPort_t stChnOutputPort;
    ST_Sys_BindInfo_T stBindInfo;
    MI_DIVP_CHN divpChn = 0;

    memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    memset(&stVpeMode, 0, sizeof(MI_VPE_PortMode_t));
    memset(&stDivpPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
    memset(&stBindInfo, 0, sizeof(ST_Sys_BindInfo_T));
    memset(&stChnOutputPort, 0, sizeof(MI_SYS_ChnPort_t));
    ExecFunc(MI_VPE_GetPortMode(0, 0, &stVpeMode), MI_VPE_OK);

    // init divp channel
    stDivpChnAttr.bHorMirror = FALSE;
    stDivpChnAttr.bVerMirror = FALSE;
    stDivpChnAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X = 0;
    stDivpChnAttr.stCropRect.u16Y = 0;
	stDivpChnAttr.stCropRect.u16Width = SNAP_WIDTH;
    stDivpChnAttr.stCropRect.u16Height = SNAP_HEIGHT;
    stDivpChnAttr.u32MaxWidth = stVpeMode.u16Width;
    stDivpChnAttr.u32MaxHeight = stVpeMode.u16Height;

    //Create divp(0,0,0)
    ExecFunc(MI_DIVP_CreateChn(divpChn, &stDivpChnAttr), MI_SUCCESS);
    stDivpPortAttr.u32Width = SNAP_WIDTH;
    stDivpPortAttr.u32Height = SNAP_HEIGHT;
    stDivpPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stDivpPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    ExecFunc(MI_DIVP_SetOutputPortAttr(divpChn, &stDivpPortAttr), MI_SUCCESS);
    ExecFunc(MI_DIVP_StartChn(divpChn), MI_SUCCESS);

    //Bind vpe to divp channel
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = divpChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 2, 4), MI_SUCCESS);
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    // IQ setting
	{
        MI_U32 u32VifChnId = 0;
        MI_U32 u32VifPortId = 0;
		MI_U32 u32VpeChnId = 0;
		
		//open ip server
		MI_VIF_ChnPortAttr_t stVifPortInfo;
   		char acharIqApiPath[64]={0};
		memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));
		STCHECKRESULT(MI_VIF_GetChnPortAttr(u32VifChnId, u32VifPortId, &stVifPortInfo));
		STCHECKRESULT(MI_IQSERVER_Open(stVifPortInfo.stDestSize.u16Width, stVifPortInfo.stDestSize.u16Height, u32VpeChnId));

		//Load IQ bin
		memset(acharIqApiPath, 0x0, sizeof(acharIqApiPath));
		snprintf(acharIqApiPath, sizeof(acharIqApiPath), "/config/iqfile/iqapifile%d.bin", u32VpeChnId);
		ST_DoSetIqBin(u32VpeChnId, acharIqApiPath);
	}

    return MI_SUCCESS;
}

MI_U32 ST_Vpe_Divp_UnBind()
{
    ST_Sys_BindInfo_T stBindInfo;
    MI_DIVP_CHN divpChn = 0;

    memset(&stBindInfo, 0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = divpChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    //deinit divp
    ExecFunc(MI_DIVP_StopChn(divpChn), MI_SUCCESS);
    ExecFunc(MI_DIVP_DestroyChn(divpChn), MI_SUCCESS);

    return MI_SUCCESS;
}

MI_U32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PAD_ID_e eSnrPad = SENSOR_ID;
    MI_U32 u32VifDev = VIFDEV_ID;
    MI_U32 u32VifChn = u32VifDev*4;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = u32VifDev;
    stBindInfo.stSrcChnPort.u32ChnId = u32VifChn;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    // destory vpe
    STCHECKRESULT(ST_Vpe_StopPort(0, 0));
    STCHECKRESULT(ST_Vpe_StopChannel(0));
    STCHECKRESULT(ST_Vpe_DestroyChannel(0));

    // destory vif
    STCHECKRESULT(ST_Vif_StopPort(u32VifChn, 0));
    STCHECKRESULT(ST_Vif_DisableDev(u32VifDev));

    // disable sensor
    STCHECKRESULT(MI_SNR_Disable(eSnrPad));

    // sys exit
    STCHECKRESULT(ST_Sys_Exit());

    return MI_SUCCESS;
}

MI_S32 ST_SaveYuvFrame(MI_SYS_BufInfo_t *pstBufInfo)
{
    char szFileName[128];
    struct timeval tv; 
    struct timezone tz;
    struct tm *p;
    int len = 0;
    int fd = -1;

    memset(szFileName, 0, sizeof(szFileName));
    gettimeofday(&tv, &tz);
    p =localtime(&tv.tv_sec);
    
    len = snprintf(szFileName, sizeof(szFileName) - 1, "%04d-%02d-%02d_%02d-%02d-%02d.%03ld_%dx%d.",
        1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000,
        SNAP_WIDTH, SNAP_HEIGHT);
    
    snprintf(szFileName + len, sizeof(szFileName) - 1, "%s", "yuv420");

    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        printf("create %s error\n", szFileName);
        return -1;
    }

	pstBufInfo->stFrameData.u32BufSize = pstBufInfo->stFrameData.u32Stride[0] * pstBufInfo->stFrameData.stContentCropWindow.u16Height;
    write(fd, pstBufInfo->stFrameData.pVirAddr[0], pstBufInfo->stFrameData.u32BufSize);
    write(fd, pstBufInfo->stFrameData.pVirAddr[1], pstBufInfo->stFrameData.u32BufSize/2);
    close(fd);

    return 0;
}

MI_S32 ST_SaveJpegFrame(MI_VENC_Stream_t *pstStream)
{
    char szFileName[128];
    struct timeval tv; 
    struct timezone tz;
    struct tm *p;
    int len = 0;
    int fd = -1;
    int i = 0;

    memset(szFileName, 0, sizeof(szFileName));
    gettimeofday(&tv, &tz);
    p =localtime(&tv.tv_sec);

    len = snprintf(szFileName, sizeof(szFileName) - 1, "%04d-%02d-%02d_%02d-%02d-%02d.%03ld_%dx%d.",
        1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000,
        SNAP_WIDTH, SNAP_HEIGHT);
 
    snprintf(szFileName + len, sizeof(szFileName) - 1, "%s", "jpg");

    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        printf("create %s error\n", szFileName);
        return -1;
    }

    printf("create %s success\n", szFileName);

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        write(fd, pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
            pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset);
    }

    close(fd);
    return 0;
}

void *ST_SnapJpeg_thread(void *args)
{
    MI_SYS_ChnPort_t stVencChnInputPort;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_U32 u32DevId = 0;
    MI_S32 vencFd = -1;
    fd_set read_fds;
    MI_VENC_Pack_t stPack[2];
	MI_VENC_CHN venc_chn = 0;
	
    MI_VENC_GetChnDevid(venc_chn, &u32DevId);

    stVencChnInputPort.eModId = E_MI_MODULE_ID_VENC;
    stVencChnInputPort.u32DevId = u32DevId;
    stVencChnInputPort.u32ChnId = venc_chn;
    stVencChnInputPort.u32PortId = 0;

    vencFd = MI_VENC_GetFd(venc_chn);
    if(vencFd <= 0)
    {
        venc_err("Unable to get FD:%d for chn:%2d\n", vencFd, venc_chn);
        return NULL;
    }
    else
    {
        venc_info("Venc Chn%2d FD:%d\n", venc_chn, vencFd);
    }

    while(g_bSnapJpegRun)
    {
        struct timeval TimeoutVal;
        MI_VENC_ChnStat_t stStat;
        MI_VENC_Stream_t stStream;
        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(vencFd, &read_fds);
        s32Ret = select(vencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            venc_err("select failed\n");
            usleep(10 * 1000);
            continue;
        }
        else if (0 == s32Ret)
        {
            venc_info("select timeout\n");
            usleep(10 * 1000);
            continue;
        }
        else
        {
            if (FD_ISSET(vencFd, &read_fds))
            {
                memset(&stStat, 0, sizeof(MI_VENC_ChnStat_t));
                s32Ret = MI_VENC_Query(venc_chn, &stStat);
                if (MI_SUCCESS != s32Ret || stStat.u32CurPacks == 0)
                {
                    venc_err("MI_VENC_Query error, %X\n", s32Ret);
                    usleep(10 * 1000);//sleep 10 ms
                    continue;
                }

                //printf("u32CurPacks:%d, u32LeftStreamFrames:%d\n", stStat.u32CurPacks, stStat.u32LeftStreamFrames);
                memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
                memset(&stPack, 0, sizeof(MI_VENC_Pack_t)*2);
                stStream.u32PackCount = stStat.u32CurPacks;
                stStream.pstPack = stPack;

                s32Ret = MI_VENC_GetStream(venc_chn, &stStream, 40);
                if (MI_SUCCESS == s32Ret)
                {
                    //printf("u32PackCount:%d, u32Seq:%d, pu8Add:%p, offset:%d, len:%d, type:%d, pts:0x%llx\n", stStream.u32PackCount, stStream.u32Seq,
                    //    stStream.pstPack[0].pu8Addr, stStream.pstPack[0].u32Offset, stStream.pstPack[0].u32Len, stStream.pstPack[0].stDataType.eH264EType, stStream.pstPack[0].u64PTS);

                    if(TRUE == g_bCatureJpeg)
                    {
                        if (ST_SaveJpegFrame(&stStream))
                            printf("save jpeg error!\n");
                    }

                    g_bCatureJpeg = FALSE;
                    MI_VENC_ReleaseStream(venc_chn, &stStream);
                }
                else
                {
                    venc_err("MI_VENC_GetStream error, %X\n", s32Ret);
                    usleep(10 * 1000);//sleep 10 ms
                }
            }
        }
    }

    s32Ret = MI_VENC_CloseFd(venc_chn);
    if(s32Ret != 0)
    {
        venc_err("Chn%02d CloseFd error, Ret:%X\n", venc_chn, s32Ret);
    }

    return NULL;
}

void *ST_SnapYUV_thread(void *argc)
{
    fd_set read_fds;
    struct timeval tv;
    MI_S32 s32Ret = 0;
    MI_SYS_ChnPort_t stChnOutputPort;
    MI_SYS_BufInfo_t        stBufInfo;
    MI_SYS_BUF_HANDLE       stBufHandle;
    tv.tv_sec = 0;
    tv.tv_usec = 100 * 1000;
    //get fd
    MI_S32 s32DivpFd;
    
    stChnOutputPort.eModId      = CAPTURE_SRC_MODULE;
    stChnOutputPort.u32DevId    = 0;
    stChnOutputPort.u32ChnId    = 0;
    stChnOutputPort.u32PortId   = 0;
    s32Ret = MI_SYS_GetFd(&stChnOutputPort, &s32DivpFd);
    if (s32Ret < 0)
    {
        printf("divp ch0, get fd. err\n");
        return NULL;
    }

    while(g_bSnapYuvRun)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32DivpFd, &read_fds);
        s32Ret = select(s32DivpFd + 1, &read_fds, NULL, NULL, &tv);
        if (s32Ret < 0)
        {
            printf("select failed\n");
        }
        else if (0 == s32Ret)
        {
            //printf("select timeout\n");
        }
        else
        {
            if(FD_ISSET(s32DivpFd, &read_fds))
            {
                memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
                s32Ret = MI_SYS_ChnOutputPortGetBuf(&stChnOutputPort, &stBufInfo, &stBufHandle);
                if (MI_SUCCESS != s32Ret)
                {
                    printf("get divp buffer fail,ret:%x\n",s32Ret);
                    continue;
                }
                if(TRUE == g_bCatureYuv)
                {
                    if (ST_SaveYuvFrame(&stBufInfo))
                        printf("save yuv error!\n");
                }

                g_bCatureYuv = FALSE;
                MI_SYS_ChnOutputPortPutBuf(stBufHandle);
            }
        }
    }
	printf("ST_SnapYUV_thread exit \n");
    return NULL;
}

MI_S32 ST_VencStart(MI_U32 u32MaxVencWidth, MI_U32 u32MaxVencHeight, MI_U32 u32VencChn)
{
    MI_U32 u32VenBitRate = 0;
    MI_U32 u32Fps =0;
    ST_VencAttr_t *pStStreamAttr = &gstVencattr[u32VencChn];
    MI_SNR_PAD_ID_e eSnrPadId = E_MI_SNR_PAD_ID_0;
    MI_DIVP_CHN divpChn =0;
    MI_VENC_ChnAttr_t stChnAttr;
	ST_Sys_BindInfo_T stBindInfo;

	memset(pStStreamAttr, 0, sizeof(ST_VencAttr_t));
	pStStreamAttr->vencChn = 0;
	pStStreamAttr->eType = E_MI_VENC_MODTYPE_JPEGE;// E_MI_VENC_MODTYPE_H264E;
	pStStreamAttr->u32Width = SNAP_WIDTH;       // VENC_WIDTH
	pStStreamAttr->u32Height = SNAP_HEIGHT;     // VENC_HEIGHT

    MI_SNR_GetFps(eSnrPadId, &u32Fps);
    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));

    u32VenBitRate = ((pStStreamAttr->u32Width * pStStreamAttr->u32Height + 500000)/1000000)*1024*1024;
    if(u32VenBitRate == 0)
	{
        u32VenBitRate = 2*1024*1024;
	}

    printf("chn %d, pichwidth %d, height %d, MaxWidth %d, MaxHeight %d bitrate %d, fps %d \n", u32VencChn,
        pStStreamAttr->u32Width, pStStreamAttr->u32Height, u32MaxVencWidth, u32MaxVencHeight, u32VenBitRate, u32Fps);
	// system("sync");
	// sleep(1);
    if(pStStreamAttr->eType == E_MI_VENC_MODTYPE_H264E)
    {
		stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_H264E;
        stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = pStStreamAttr->u32Width;       // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = pStStreamAttr->u32Height;     // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = u32MaxVencWidth;            // 16 bytes alignment
		stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = u32MaxVencHeight;          // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
        stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = u32VenBitRate;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = u32Fps;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
    }
	else if(pStStreamAttr->eType == E_MI_VENC_MODTYPE_JPEGE)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = pStStreamAttr->u32Width;        // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = pStStreamAttr->u32Height;      // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = u32MaxVencWidth;             // 16 bytes alignment
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32MaxVencHeight;           // 16 bytes alignment

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32BitRate = 30;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateNum = u32Fps;
        stChnAttr.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateDen = 1;
    }

    stChnAttr.stVeAttr.eType = pStStreamAttr->eType;
    STCHECKRESULT(ST_Venc_CreateChannel(u32VencChn, &stChnAttr));

    MI_VENC_InputSourceConfig_t stVencSourceCfg;
    stVencSourceCfg.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_NORMAL_FRMBASE;
    MI_VENC_SetInputSourceConfig(u32VencChn, &stVencSourceCfg);
    STCHECKRESULT(ST_Venc_StartChannel(u32VencChn));

    // bind divp(0,0,0) -> venc(1,0,0)
	memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = divpChn;
	stBindInfo.stSrcChnPort.u32PortId = 0;

	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
	stBindInfo.stDstChnPort.u32DevId = 1;       // dev1:JPEG
	stBindInfo.stDstChnPort.u32ChnId = u32VencChn;
	stBindInfo.stDstChnPort.u32PortId = 0;

	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	stBindInfo.u32BindParam = 0;
	STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    printf("create snap Jpeg thread ...\n");
    g_bSnapJpegRun = TRUE;
    pthread_create(&g_snapJpegThread, NULL, ST_SnapJpeg_thread, NULL);

    return MI_SUCCESS;
}

MI_S32 ST_VencStop(MI_U32 u32VencChn)
{
	ST_Sys_BindInfo_T stBindInfo;
	
    g_bSnapJpegRun = FALSE;
    pthread_join(g_snapJpegThread, NULL);

	memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;

	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
	stBindInfo.stDstChnPort.u32DevId = 1;
	stBindInfo.stDstChnPort.u32ChnId = u32VencChn;
	stBindInfo.stDstChnPort.u32PortId = 0;

	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    STCHECKRESULT(ST_Venc_StopChannel(u32VencChn));
    STCHECKRESULT(ST_Venc_DestoryChannel(u32VencChn));

    return MI_SUCCESS;
}	

void signalHandler(int signo)
{
    switch (signo){
        case SIGALRM:
        case SIGINT:
            printf("Catch signal !\n");
            g_bSnapYuvRun = FALSE;
            pthread_join(g_snapYuvThread, NULL);

            ST_Vpe_Divp_UnBind();
            ST_VencStop(0);
            STCHECKRESULT(ST_BaseModuleUnInit());
            break;
   }

   exit(0);
   return;
}

void setTimer(MI_U32 u32RunTime)
{
    struct itimerval new_value, old_value;
    if (0 != u32RunTime)
    {
        new_value.it_value.tv_sec = u32RunTime;
        new_value.it_value.tv_usec = 0;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &new_value, &old_value);
    }
    return;
}

int main(int argc, char **argv)
{
    int runTime = 0;
    int s32Opt = 0;
    while ((s32Opt = getopt(argc, argv, "t:")) != -1)
    {
        switch(s32Opt)
        {
            // set the duratuion of app, if set 0, app is always running 
            case 't':
                {
                    runTime = atoi(optarg);
                }
                break;
        }
    }

    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);
    setTimer(runTime);

    STCHECKRESULT(ST_BaseModuleInit());
    ST_Vpe_Divp_Bind();
	ST_VencStart(1280, 720, 0);

    if (!pthread_create(&g_snapYuvThread, NULL, ST_SnapYUV_thread, NULL))
    {
        printf("create snap yuv thread success\n");
        g_bSnapYuvRun = TRUE;
    }
    else
    {
        printf("creat snap yuv thread fail\n");
        goto exit_app;
    }
    
    for(;;)
    {
        char cmd = 0xff;
        printf("Please input:\n");
        printf("\t'y' or 'Y': to snap yuv\n");
        printf("\t'j' or 'J': to snap jpeg\n");
        printf("\t'q' or 'Q': to exit\n");
        scanf("%c", &cmd);
        
        if('q' == cmd || 'Q' == cmd)
        {
            g_bSnapYuvRun = FALSE;
            pthread_join(g_snapYuvThread, NULL);		
            break;
        }
        if('y' == cmd || 'Y' == cmd)
        {
            g_bCatureYuv= TRUE;
        }
        if('j' == cmd || 'J' == cmd)
        {
            g_bCatureJpeg= TRUE;
        }
    }

exit_app:
    ST_Vpe_Divp_UnBind();
	ST_VencStop(0);
    STCHECKRESULT(ST_BaseModuleUnInit());

    return MI_SUCCESS;
}



