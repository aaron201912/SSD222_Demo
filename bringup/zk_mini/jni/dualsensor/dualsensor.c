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
#include <sys/types.h>
#include <sys/stat.h>
#include "dualsensor.h"
#include "mi_isp.h"
#include "mi_iqserver.h"

#define DISP_INPUT_WIDTH    1024
#define DISP_INPUT_HEIGHT   600

#define DISP_OUTPUT_WIDTH    1024
#define DISP_OUTPUT_HEIGHT   600

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE              MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED                MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN              MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE               MAKE_YUYV_VALUE(29,225,107)

#define STCHECKRESULT(result)\
    if (result != MI_SUCCESS)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__);\
        return 1;\
    }\
    else\
    {\
        printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__);\
    }


ST_Config_S g_stConfig =
{
    .s32UseOnvif = 0,//not use
    .s32UseVdf = 0,//not use
    .s32UseAudio = 0,//not use
    .s32LoadIQ = 0,//not use
    .s32UsePanel = 0,//not use
    .u8SensorNum = 2,
    .enPixelFormat = E_MI_SYS_PIXEL_FRAME_FORMAT_MAX,//not use
    .s32HDRtype = 0,
    .enSensorType = ST_Sensor_Type_GC1054,//not use
    .enRotation = E_MI_SYS_ROTATE_NONE,//not use
    .en3dNrLevel = E_MI_VPE_3DNR_LEVEL2,
    .u8SNRChocieRes = 0,
    .stVPEPortCrop =
    {
        .u16X = 0,
        .u16Y = 0,
        .u16Width = 0,
        .u16Height = 0,
    },
    .stVPEPortSize = //not use
    {
        .u16Width = 0,
        .u16Height = 0,
    },
};

ST_DBG_LEVEL_e g_eSTDbgLevel = ST_DBG_ERR;
MI_BOOL g_bSTFuncTrace = 0;
MI_U32 g_u32CapWidth = 0;
MI_U32 g_u32CapHeight = 0;
static MI_BOOL g_bExit = FALSE;

void ST_Flush(void)
{
    char c;

    while((c = getchar()) != '\n' && c != EOF);
}

MI_S32 ST_Sys_Init(void)
{
    MI_SYS_Version_t stVersion;
    MI_U64 u64Pts = 0;

    memset(&stVersion, 0x0, sizeof(MI_SYS_Version_t));
    STCHECKRESULT(MI_SYS_GetVersion(&stVersion));
    DBG_INFO("u8Version:%s\n", stVersion.u8Version);

    STCHECKRESULT(MI_SYS_GetCurPts(&u64Pts));
    DBG_INFO("u64Pts:0x%llx\n", u64Pts);

    u64Pts = 0xF1237890F1237890;
    STCHECKRESULT(MI_SYS_InitPtsBase(u64Pts));

    u64Pts = 0xE1237890E1237890;
    STCHECKRESULT(MI_SYS_SyncPts(u64Pts));

    return MI_SUCCESS;
}

MI_S32 ST_Sys_Exit(void)
{
    STCHECKRESULT(MI_SYS_Exit());

    return MI_SUCCESS;
}

MI_S32 ST_Sys_Bind(ST_Sys_BindInfo_T *pstBindInfo)
{
    /*
    ExecFunc(MI_SYS_BindChnPort(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort, \
        pstBindInfo->u32SrcFrmrate, pstBindInfo->u32DstFrmrate), MI_SUCCESS);
    */
    STCHECKRESULT(MI_SYS_BindChnPort2(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort,
        pstBindInfo->u32SrcFrmrate, pstBindInfo->u32DstFrmrate, pstBindInfo->eBindType, pstBindInfo->u32BindParam));
    DBG_INFO("\n");
    DBG_INFO("src(%d-%d-%d-%d)  dst(%d-%d-%d-%d)  %d...\n", pstBindInfo->stSrcChnPort.eModId, pstBindInfo->stSrcChnPort.u32DevId,
        pstBindInfo->stSrcChnPort.u32ChnId, pstBindInfo->stSrcChnPort.u32PortId,
        pstBindInfo->stDstChnPort.eModId, pstBindInfo->stDstChnPort.u32DevId, pstBindInfo->stDstChnPort.u32ChnId,
        pstBindInfo->stDstChnPort.u32PortId, pstBindInfo->eBindType);
    DBG_INFO("\n");
    return MI_SUCCESS;
}

MI_S32 ST_Sys_UnBind(ST_Sys_BindInfo_T *pstBindInfo)
{
    STCHECKRESULT(MI_SYS_UnBindChnPort(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort));

    return MI_SUCCESS;
}

MI_U64 ST_Sys_GetPts(MI_U32 u32FrameRate)
{
    if (0 == u32FrameRate)
    {
        return (MI_U64)(-1);
    }

    return (MI_U64)(1000 / u32FrameRate);
}

MI_S32 ST_Vif_EnableDev(MI_VIF_DEV VifDev, MI_VIF_WorkMode_e eWorkMode, MI_VIF_HDRType_e eHdrType, MI_SNR_PADInfo_t *pstSnrPadInfo)
{
    MI_VIF_DevAttr_t stDevAttr;

    memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

    stDevAttr.eIntfMode = pstSnrPadInfo->eIntfMode;
    stDevAttr.eWorkMode = eWorkMode;
    stDevAttr.eHDRType = eHdrType;
    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        stDevAttr.eClkEdge = pstSnrPadInfo->unIntfAttr.stBt656Attr.eClkEdge;
    else
        stDevAttr.eClkEdge = E_MI_VIF_CLK_EDGE_DOUBLE;

    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
        stDevAttr.eDataSeq =pstSnrPadInfo->unIntfAttr.stMipiAttr.eDataYUVOrder;
    else
        stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;

    if(stDevAttr.eIntfMode == E_MI_VIF_MODE_BT656)
        memcpy(&stDevAttr.stSyncAttr, &pstSnrPadInfo->unIntfAttr.stBt656Attr.stSyncAttr, sizeof(MI_VIF_SyncAttr_t));

    stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL;

    STCHECKRESULT(MI_VIF_SetDevAttr(VifDev, &stDevAttr));
    STCHECKRESULT(MI_VIF_EnableDev(VifDev));

    return MI_SUCCESS;
}
MI_S32 ST_Vif_DisableDev(MI_VIF_DEV VifDev)
{
    STCHECKRESULT(MI_VIF_DisableDev(VifDev));
	
    return MI_SUCCESS;
}

MI_S32 ST_Vif_CreatePort(MI_VIF_CHN VifChn, MI_VIF_PORT VifPort, ST_VIF_PortInfo_T *pstPortInfoInfo)
{
    MI_VIF_ChnPortAttr_t stChnPortAttr;

    memset(&stChnPortAttr, 0, sizeof(MI_VIF_ChnPortAttr_t));
    stChnPortAttr.stCapRect.u16X = pstPortInfoInfo->u32RectX;
    stChnPortAttr.stCapRect.u16Y = pstPortInfoInfo->u32RectY;
    stChnPortAttr.stCapRect.u16Width = pstPortInfoInfo->u32RectWidth;
    stChnPortAttr.stCapRect.u16Height = pstPortInfoInfo->u32RectHeight;
    stChnPortAttr.stDestSize.u16Width = pstPortInfoInfo->u32DestWidth;
    stChnPortAttr.stDestSize.u16Height = pstPortInfoInfo->u32DestHeight;
    stChnPortAttr.eCapSel = E_MI_SYS_FIELDTYPE_BOTH;
    if (pstPortInfoInfo->u32IsInterlace)
    {
        stChnPortAttr.eScanMode = E_MI_SYS_FRAME_SCAN_MODE_INTERLACE;
    }
    else
    {
        stChnPortAttr.eScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
    }
    stChnPortAttr.ePixFormat = pstPortInfoInfo->ePixFormat;//E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stChnPortAttr.eFrameRate = pstPortInfoInfo->eFrameRate;
    STCHECKRESULT(MI_VIF_SetChnPortAttr(VifChn, VifPort, &stChnPortAttr));

    return MI_SUCCESS;
}

MI_S32 ST_Vif_StartPort(MI_VIF_DEV VifDev, MI_VIF_CHN VifChn, MI_VIF_PORT VifPort)
{
    MI_SYS_ChnPort_t stChnPort;

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VIF;
    stChnPort.u32DevId = VifDev;
    stChnPort.u32ChnId = VifChn;
    stChnPort.u32PortId = VifPort;

    //MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 6);

    STCHECKRESULT(MI_VIF_EnableChnPort(VifChn, VifPort));
    return MI_SUCCESS;
}
MI_S32 ST_Vif_StopPort(MI_VIF_CHN VifChn, MI_VIF_PORT VifPort)
{
    STCHECKRESULT(MI_VIF_DisableChnPort(VifChn, VifPort));

    return MI_SUCCESS;
}
MI_S32 ST_Vpe_CreateChannel(MI_VPE_CHANNEL VpeChannel, ST_VPE_ChannelInfo_T *pstChannelInfo)
{
    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_SYS_WindowRect_t stCropWin;
    MI_VPE_ChannelPara_t stChannelVpeParam;

    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelAttr_t));
    memset(&stCropWin, 0, sizeof(MI_SYS_WindowRect_t));
    memset(&stChannelVpeParam, 0x00, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeParam.eHDRType = pstChannelInfo->eHDRtype;
    stChannelVpeParam.e3DNRLevel = pstChannelInfo->e3DNRLevel;
    MI_VPE_SetChannelParam(VpeChannel, &stChannelVpeParam);

    stChannelVpeAttr.u16MaxW = pstChannelInfo->u16VpeMaxW;
    stChannelVpeAttr.u16MaxH = pstChannelInfo->u16VpeMaxH;
    stChannelVpeAttr.bNrEn= FALSE;
    stChannelVpeAttr.bEdgeEn= FALSE;
    stChannelVpeAttr.bEsEn= FALSE;
    stChannelVpeAttr.bContrastEn= FALSE;
    stChannelVpeAttr.bUvInvert= FALSE;
    stChannelVpeAttr.ePixFmt = pstChannelInfo->eFormat;
    stChannelVpeAttr.eRunningMode = pstChannelInfo->eRunningMode;
    stChannelVpeAttr.bRotation = pstChannelInfo->bRotation;
    stChannelVpeAttr.eHDRType  = pstChannelInfo->eHDRtype;
    stChannelVpeAttr.eSensorBindId = pstChannelInfo->eBindSensorId;
    //DBG_INFO("beal.......mode %d %d  .....\n", E_MI_VPE_RUN_REALTIME_MODE, pstChannelInfo->eRunningMode);
    STCHECKRESULT(MI_VPE_CreateChannel(VpeChannel, &stChannelVpeAttr));

    return MI_SUCCESS;
}

MI_VPE_RunningMode_e ST_Vpe_GetRunModeByVIFMode(VIF_WORK_MODE_E enWorkMode)
{
    return E_MI_VPE_RUN_REALTIME_MODE;
}

MI_S32 ST_Vpe_StartChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_StartChannel (VpeChannel));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StopChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_StopChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_DestroyChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_DestroyChannel(VpeChannel));
	
    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StartPort(MI_VPE_PORT VpePort, ST_VPE_PortInfo_T *pstPortInfo)
{
    MI_VPE_PortMode_t stVpeMode;
    MI_SYS_ChnPort_t stChnPort;
    DBG_INFO("ST_Vpe_StartPort ch:%d port %d,w %d h %d\n", pstPortInfo->DepVpeChannel, VpePort,pstPortInfo->u16OutputWidth, pstPortInfo->u16OutputHeight);

    memset(&stVpeMode, 0, sizeof(stVpeMode));
    STCHECKRESULT(MI_VPE_GetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));
    stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stVpeMode.ePixelFormat = pstPortInfo->ePixelFormat;
    stVpeMode.u16Width = pstPortInfo->u16OutputWidth;
    stVpeMode.u16Height= pstPortInfo->u16OutputHeight;
    STCHECKRESULT(MI_VPE_SetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VPE;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = pstPortInfo->DepVpeChannel;
    stChnPort.u32PortId = VpePort;
    //ExecFunc(MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 5), 0);

    STCHECKRESULT(MI_VPE_EnablePort(pstPortInfo->DepVpeChannel, VpePort));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StopPort(MI_VPE_CHANNEL VpeChannel, MI_VPE_PORT VpePort)
{
    STCHECKRESULT(MI_VPE_DisablePort(VpeChannel, VpePort));

    return MI_SUCCESS;
}
/*
* 初始化vdisp模块
*      v        
* 打开vdisp设备
*      v        
* 设置vdisp input channel/output port 参数
*      v        
* 激活input channel
*      v        
* 开始vdisp设备
*/
MI_S32 ST_Vdisp_Init(void)
{
    MI_VDISP_InputChnAttr_t stInputChnAttr;
    MI_VDISP_OutputPortAttr_t stOutputPortAttr;
    ST_Rect_T stRect;
    MI_VDISP_DEV DevId = 0;
    MI_VDISP_CHN ChnId = 0;

    /*
    *初始化vdisp模块
    */
    STCHECKRESULT(MI_VDISP_Init());
	
    /*
    *打开一个vdisp虚拟设备,以便开始对这个设备进行配置
    */
    STCHECKRESULT(MI_VDISP_OpenDevice(DevId));

    /*
    *为了满足vdisp缩略图窗口的对齐要求，对x坐标做16向下对齐
    *设置input channel 参数
    */
    ChnId = 0;
    memset(&stRect, 0x0, sizeof(ST_Rect_T));
    memset(&stInputChnAttr, 0x0, sizeof(MI_VDISP_InputChnAttr_t));
    stRect.u32X = ALIGN_DOWN(0, 16);
    stRect.u32Y = 0;
    stRect.u16PicW = DISP_INPUT_WIDTH/2;
    stRect.u16PicH = DISP_INPUT_HEIGHT;
    stInputChnAttr.s32IsFreeRun = TRUE;
    stInputChnAttr.u32OutHeight = stRect.u16PicH;
    stInputChnAttr.u32OutWidth = stRect.u16PicW;
    stInputChnAttr.u32OutX = stRect.u32X;
    stInputChnAttr.u32OutY = stRect.u32Y;
    /*
    *为input channel VDISP_OVERLAYINPUTCHNID 设置参数
    */
    STCHECKRESULT(MI_VDISP_SetInputChannelAttr(DevId, ChnId, &stInputChnAttr));

    /*
    *为了满足vdisp缩略图窗口的对齐要求，对x坐标做16向下对齐
    *设置input channel 参数
    */
    ChnId = 1;
    memset(&stRect, 0x0, sizeof(ST_Rect_T));
    memset(&stInputChnAttr, 0x0, sizeof(MI_VDISP_InputChnAttr_t));
    stRect.u32X = ALIGN_DOWN(DISP_INPUT_WIDTH/2, 16);
    stRect.u32Y = 0;
    stRect.u16PicW = DISP_INPUT_WIDTH/2;
    stRect.u16PicH = DISP_INPUT_HEIGHT;
    stInputChnAttr.s32IsFreeRun = TRUE;
    stInputChnAttr.u32OutHeight = stRect.u16PicH;
    stInputChnAttr.u32OutWidth = stRect.u16PicW;
    stInputChnAttr.u32OutX = stRect.u32X;
    stInputChnAttr.u32OutY = stRect.u32Y;
    /*
    *为input channel 1 设置参数
    */
    STCHECKRESULT(MI_VDISP_SetInputChannelAttr(DevId, ChnId, &stInputChnAttr));

    memset(&stOutputPortAttr, 0x0, sizeof(MI_VDISP_OutputPortAttr_t));
    //设置输出的颜色格式
    stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    //设置vdisp输出帧中未使用区域的涂黑颜色,YUV 颜色空间
    stOutputPortAttr.u32BgColor = YUYV_BLACK;
    //设置vdisp输出帧率
    stOutputPortAttr.u32FrmRate = 30;
    //设置vdisp输出帧的高
    stOutputPortAttr.u32Height = DISP_OUTPUT_HEIGHT;
    //设置vdisp输出帧的宽
    stOutputPortAttr.u32Width = DISP_OUTPUT_WIDTH;
    //设置vdisp输出帧的最低PTS
    stOutputPortAttr.u64pts = 0;
    /*
    *为output port 0 设置参数
    */
    STCHECKRESULT(MI_VDISP_SetOutputPortAttr(DevId,0,&stOutputPortAttr));
    
    /*
    *在设置完input channel 参数之后，
    *激活对应的channel，以供使用
    */
    ChnId = 0;
    STCHECKRESULT(MI_VDISP_EnableInputChannel(DevId, ChnId));
    ChnId = 1;
    STCHECKRESULT(MI_VDISP_EnableInputChannel(DevId, ChnId));

    /*
    *在vdisp 的input channel&output port 都配置完参数之后，
    *让vdisp的这个设备开始工作
    */
    STCHECKRESULT(MI_VDISP_StartDev(DevId));
    
    return MI_SUCCESS;
}
/*
* 禁用所有channel
*      v
* 停止vdisp设备
*      v
* 关闭vdisp设备
*      v
* 退出vdisp模块
*/
MI_S32 ST_Vdisp_Deinit(void)
{
    MI_VDISP_DEV DevId = 0;
    MI_VDISP_CHN ChnId = 0;
    /*
    * 先禁用已经打开vdisp channel
    * <0,1>
    */
    ChnId = 0;
    STCHECKRESULT(MI_VDISP_DisableInputChannel(DevId, ChnId));
    ChnId = 1;
    STCHECKRESULT(MI_VDISP_DisableInputChannel(DevId, ChnId));

    /*
    *停止已经打开的vdisp设备
    */
    STCHECKRESULT(MI_VDISP_StopDev(DevId));
    /*
    *关闭已经打开的vdisp设备
    */
    STCHECKRESULT(MI_VDISP_CloseDevice(DevId));
    /*
    *退出vdisp模块
    */
    STCHECKRESULT(MI_VDISP_Exit());
    
    return MI_SUCCESS;
}

/*
* 绑定 vpe out0 -> vdisp in0---\
* 绑定 vpe out1 -> vdisp in1--------->--> disp in0
*/
MI_S32 ST_Vdisp_Bind(void)
{
    ST_Sys_BindInfo_T stBindInfo;

    /*
    * 绑定 vdisp out0-> disp in0
    * <chn,dev,port> : (0,0,0) -> (0,0,0)
    */
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    /*
     * 绑定 vpe out0-> vdisp 0
     * <chn,dev,port> : (0,0,0) -> (0,0,0)
     */
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    /*
     * 绑定 vpe out0-> vdisp in1
     * <chn,dev,port> : (1,0,0) -> (1,0,0)
     */
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 1;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 1;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    return MI_SUCCESS;
}

MI_S32 ST_Vdisp_UnBind(void)
{
    ST_Sys_BindInfo_T stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 1;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VDISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 1;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    return MI_SUCCESS;
}

MI_BOOL ST_DoSetIqBin(MI_VPE_CHANNEL Vpechn,char *pConfigPath)
{
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    MI_U8  u8ispreadycnt = 0;
    if (strlen(pConfigPath) == 0)
    {
        printf("IQ Bin File path NULL!\n");
        return FALSE;
    }
    printf("%s:%d,vpech: %d,iqapi:%s\n", __FUNCTION__, __LINE__,Vpechn,pConfigPath);
    do
    {
        if(u8ispreadycnt > 100)
        {
            printf("%s:%d, isp ready time out \n", __FUNCTION__, __LINE__);
            u8ispreadycnt = 0;
            break;
        }

        MI_ISP_IQ_GetParaInitStatus(Vpechn, &status);
        if(status.stParaAPI.bFlag != 1)
        {
            usleep(300*1000);
            u8ispreadycnt++;
            continue;
        }

        u8ispreadycnt = 0;

        printf("loading api bin...path:%s\n",pConfigPath);
        MI_ISP_API_CmdLoadBinFile(Vpechn, (char *)pConfigPath, 1234);

        usleep(10*1000);
    }while(!status.stParaAPI.bFlag);

    return 0;
}


MI_S32 ST_BaseModuleInit(ST_Config_S* pstConfig)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPadInfo;
    MI_SNR_PlaneInfo_t stSnrPlaneInfo;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VIF_WorkMode_e eVifWorkMode = E_MI_VIF_WORK_MODE_RGB_REALTIME;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    MI_U32 u32ChocieRes =0;
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_SYS_WindowRect_t stRect;
    MI_U32 u32SensorNum = 1;
    MI_U32 i = 0;
    MI_SNR_PAD_ID_e eSNRPad = E_MI_SNR_PAD_ID_0;
    ST_VIF_PortInfo_T stVifPortInfoInfo;
    MI_U32 u32VifDevId = 0;
    MI_U32 u32VifChnId = 0;
    MI_U32 u32VifPortId = 0;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    MI_VIF_ChnPortAttr_t stVifPortInfo;
    MI_U32 u32VpeDevId = 0;
    MI_U32 u32VpeChnId = 0;
    MI_U32 u32VpePortId = 0;
    MI_DISP_PubAttr_t stPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_RotateConfig_t stRotateConfig;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;
    MI_PANEL_IntfType_e eIntfType;
    MI_PANEL_ParamConfig_t pstParamCfg;
    MI_U32 u32DispDevId = 0;
    //MI_U32 u32DispChnId = 0;
    MI_U32 u32DispPortId = 0;
    MI_U32 u32DispLayer = 0;
    char acharIqApiPath[64]={0};
    
    memset(&stPadInfo, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlaneInfo, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stVpePortInfo, 0x0, sizeof(ST_VPE_PortInfo_T));
    memset(&stRect, 0x0, sizeof(MI_SYS_WindowRect_t));

    /************************************************
     init SYS and Sensor
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());
    
    u32SensorNum = pstConfig->u8SensorNum;
    for(i = 0; i < u32SensorNum; i++)
    {
        DBG_INFO("i[%d]\n", i);
        eSNRPad = (MI_SNR_PAD_ID_e)i;
        if(pstConfig->s32HDRtype > 0)
        {
            STCHECKRESULT(MI_SNR_SetPlaneMode(eSNRPad, TRUE));
        }
        else
        {
            STCHECKRESULT(MI_SNR_SetPlaneMode(eSNRPad, FALSE));
        }
        
        STCHECKRESULT(MI_SNR_QueryResCount(eSNRPad, &u32ResCount));
        for(u8ResIndex = 0; u8ResIndex < u32ResCount; u8ResIndex++)
        {
            MI_SNR_GetRes(eSNRPad, u8ResIndex, &stRes);
            DBG_INFO("eSNRPad[%d], index[%d], Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps[%d], minfps[%d], ResDesc[%s] \n",
            eSNRPad,
            u8ResIndex,
            stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
            stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
            stRes.u32MaxFps,stRes.u32MinFps,
            stRes.strResDesc);
        }

        DBG_INFO("eSNRPad[%d], cnt[%d]resolutions, choice[%d] \n", eSNRPad, u32ResCount, pstConfig->u8SNRChocieRes);
        u32ChocieRes = pstConfig->u8SNRChocieRes;
        //gc1054 default set mirror/flip = 1
        if(ST_Sensor_Type_GC1054 == pstConfig->enSensorType)
        {
            STCHECKRESULT(MI_SNR_SetOrien(eSNRPad, 1, 1));
        }
        STCHECKRESULT(MI_SNR_SetRes(eSNRPad,u32ChocieRes));
        STCHECKRESULT(MI_SNR_Enable(eSNRPad));

        STCHECKRESULT(MI_SNR_GetPadInfo(eSNRPad, &stPadInfo));
        STCHECKRESULT(MI_SNR_GetPlaneInfo(eSNRPad, 0, &stSnrPlaneInfo));

        g_u32CapWidth = stSnrPlaneInfo.stCapRect.u16Width;
        g_u32CapHeight = stSnrPlaneInfo.stCapRect.u16Height;
        u32CapWidth = stSnrPlaneInfo.stCapRect.u16Width;
        u32CapHeight = stSnrPlaneInfo.stCapRect.u16Height;
        eFrameRate = E_MI_VIF_FRAMERATE_FULL;
        ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlaneInfo.ePixPrecision, stSnrPlaneInfo.eBayerId);

        /************************************************
         init VIF
        *************************************************/
        u32VifDevId = i;
        u32VifChnId = i * 4;
        u32VifPortId = 0;
        eVifHdrType = (MI_VIF_HDRType_e)pstConfig->s32HDRtype;
        eVifWorkMode = u32SensorNum > 1 ? E_MI_VIF_WORK_MODE_RGB_FRAMEMODE : E_MI_VIF_WORK_MODE_RGB_REALTIME;
        DBG_INFO("VIF:DevId[%d] ChnId[%d] PortId[%d], eVifWorkMode[%d]\n", u32VifDevId, u32VifChnId, u32VifPortId, eVifWorkMode);
        STCHECKRESULT(ST_Vif_EnableDev(u32VifDevId, eVifWorkMode, eVifHdrType, &stPadInfo));

        memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
        stVifPortInfoInfo.u32RectX = stSnrPlaneInfo.stCapRect.u16X;
        stVifPortInfoInfo.u32RectY = stSnrPlaneInfo.stCapRect.u16Y;
        stVifPortInfoInfo.u32RectWidth = u32CapWidth;
        stVifPortInfoInfo.u32RectHeight = u32CapHeight;
        stVifPortInfoInfo.u32DestWidth = u32CapWidth;
        stVifPortInfoInfo.u32DestHeight = u32CapHeight;
        stVifPortInfoInfo.eFrameRate = eFrameRate;
        stVifPortInfoInfo.ePixFormat = ePixFormat;
        STCHECKRESULT(ST_Vif_CreatePort(u32VifChnId, u32VifPortId, &stVifPortInfoInfo));
        STCHECKRESULT(ST_Vif_StartPort(u32VifDevId, u32VifChnId, u32VifPortId));
        
        /************************************************
         init VPE
        *************************************************/
        u32VpeDevId = 0;
        u32VpeChnId = i;
        u32VpePortId = 0;
        memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
        eVpeHdrType = (MI_VPE_HDRType_e)pstConfig->s32HDRtype;
        stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
        stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
        stVpeChannelInfo.u32X = 0;
        stVpeChannelInfo.u32Y = 0;
        stVpeChannelInfo.u16VpeCropW = 0;
        stVpeChannelInfo.u16VpeCropH = 0;
        stVpeChannelInfo.eRunningMode = u32SensorNum > 1 ? E_MI_VPE_RUN_CAM_MODE : E_MI_VPE_RUN_REALTIME_MODE;
        stVpeChannelInfo.eFormat = ePixFormat;
        stVpeChannelInfo.e3DNRLevel = pstConfig->en3dNrLevel;
        stVpeChannelInfo.eHDRtype = eVpeHdrType;
        stVpeChannelInfo.bRotation = FALSE;
        if(stVpeChannelInfo.eRunningMode == E_MI_VPE_RUN_DVR_MODE)
        {
            stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR_INVALID;
        }
        else
        {
            stVpeChannelInfo.eBindSensorId = (MI_VPE_SensorChannel_e)(eSNRPad+1);
        }
        DBG_INFO("VPE:DevId[%d] ChnId[%d] PortId[%d],eRunningMode[%d]\n", u32VpeDevId, u32VpeChnId, u32VpePortId, stVpeChannelInfo.eRunningMode);
        STCHECKRESULT(ST_Vpe_CreateChannel(u32VpeChnId, &stVpeChannelInfo));
        STCHECKRESULT(ST_Vpe_StartChannel(u32VpeChnId));
        
        // bind VIF to VPE
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
        stBindInfo.stSrcChnPort.u32DevId = u32VifDevId;
        stBindInfo.stSrcChnPort.u32ChnId = u32VifChnId;
        stBindInfo.stSrcChnPort.u32PortId = u32VifPortId;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stDstChnPort.u32DevId = u32VpeDevId;
        stBindInfo.stDstChnPort.u32ChnId = u32VpeChnId;
        stBindInfo.stDstChnPort.u32PortId = u32VpePortId;
        stBindInfo.eBindType = u32SensorNum > 1 ? E_MI_SYS_BIND_TYPE_FRAME_BASE : E_MI_SYS_BIND_TYPE_REALTIME;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        
        memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
        stVpePortInfo.DepVpeChannel = u32VpeChnId;
        stVpePortInfo.u16OutputWidth = u32SensorNum > 1 ? DISP_INPUT_WIDTH/2 : DISP_INPUT_WIDTH;
        stVpePortInfo.u16OutputHeight = u32SensorNum > 1 ? DISP_INPUT_HEIGHT : DISP_INPUT_HEIGHT;
        stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
        if (pstConfig->stVPEPortCrop.u16Width != 0 && pstConfig->stVPEPortCrop.u16Height != 0)
        {
            stRect.u16X = pstConfig->stVPEPortCrop.u16X;
            stRect.u16Y = pstConfig->stVPEPortCrop.u16Y; 
            stRect.u16Width = pstConfig->stVPEPortCrop.u16Width;
            stRect.u16Height = pstConfig->stVPEPortCrop.u16Height;
            STCHECKRESULT(MI_VPE_SetPortCrop(u32VpeChnId, u32VpePortId, &stRect));
        }
        STCHECKRESULT(ST_Vpe_StartPort(u32VpePortId, &stVpePortInfo));

    }
    /************************************************
     init DISP
    *************************************************/

    
    memset(&stWinRect, 0, sizeof(MI_DISP_VidWinRect_t));
    
    if(u32SensorNum > 1)
    {
        ST_Vdisp_Init();
        ST_Vdisp_Bind();
    }
    else
    {
        // bind VPE to divp
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = 0;
        stBindInfo.stSrcChnPort.u32PortId = 0;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
        stBindInfo.stDstChnPort.u32DevId = 0;
        stBindInfo.stDstChnPort.u32ChnId = 0;
        stBindInfo.stDstChnPort.u32PortId = 0;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    }
    for(i = 0; i < u32SensorNum; i++)
    {
        u32VpeDevId = 0;
        u32VpeChnId = i;
        u32VpePortId = 0;
        //open ip server
        memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));
        STCHECKRESULT(MI_VIF_GetChnPortAttr(u32VpeChnId, u32VpePortId, &stVifPortInfo));
        STCHECKRESULT(MI_IQSERVER_Open(stVifPortInfo.stDestSize.u16Width, stVifPortInfo.stDestSize.u16Height, u32VpeChnId));

        //Load IQ bin
        memset(acharIqApiPath, 0x0, sizeof(acharIqApiPath));
        snprintf(acharIqApiPath, sizeof(acharIqApiPath), "/config/iqfile/iqapifile%d.bin", u32VpeChnId);
        ST_DoSetIqBin(u32VpeChnId, acharIqApiPath);
    }
    return MI_SUCCESS;
}
MI_S32 ST_BaseModuleUnInit(ST_Config_S* pstConfig)
{
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PAD_ID_e eSNRPad = E_MI_SNR_PAD_ID_0;
    MI_U32 u32SensorNum = 1;
    MI_U32 i = 0;
    MI_U32 u32VifDevId = 0;
    MI_U32 u32VifChnId = 0;
    MI_U32 u32VifPortId = 0;
    MI_U32 u32VpeDevId = 0;
    MI_U32 u32VpeChnId = 0;
    MI_U32 u32VpePortId = 0;
    MI_U32 u32DispDevId = 0;
    MI_U32 u32DispChnId = 0;
    MI_U32 u32DispPortId = 0;
    MI_U32 u32DispLayer = 0;

    u32SensorNum = pstConfig->u8SensorNum;

    /************************************************
    Step1:  destory vdisp
    *************************************************/
    if(u32SensorNum > 1)
    {
        ST_Vdisp_UnBind();
        ST_Vdisp_Deinit();
    }
    else
    {
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stSrcChnPort.u32DevId = u32VpeDevId;
        stBindInfo.stSrcChnPort.u32ChnId = u32VpeChnId;
        stBindInfo.stSrcChnPort.u32PortId = u32VpePortId;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
        stBindInfo.stDstChnPort.u32DevId = u32DispDevId;
        stBindInfo.stDstChnPort.u32ChnId = u32DispChnId;
        stBindInfo.stDstChnPort.u32PortId = u32DispPortId;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    }

    /************************************************
    Step3:  destory VPE
    *************************************************/
    for(i = 0; i < u32SensorNum; i++)
    {
        eSNRPad = (MI_SNR_PAD_ID_e)i;
        u32VifDevId = i;
        u32VifChnId = i * 4;
        u32VifPortId = 0;
        u32VpeDevId = 0;
        u32VpeChnId = i;
        u32VpePortId = 0;
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
        stBindInfo.stSrcChnPort.u32DevId = u32VifDevId;
        stBindInfo.stSrcChnPort.u32ChnId = u32VifChnId;
        stBindInfo.stSrcChnPort.u32PortId = u32VifPortId;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stDstChnPort.u32DevId = u32VpeDevId;
        stBindInfo.stDstChnPort.u32ChnId = u32VpeChnId;
        stBindInfo.stDstChnPort.u32PortId = u32VpePortId;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

        STCHECKRESULT(ST_Vpe_StopPort(u32VpeChnId, u32VpePortId));
        STCHECKRESULT(ST_Vpe_StopChannel(u32VpeChnId));
        STCHECKRESULT(ST_Vpe_DestroyChannel(u32VpeChnId));

        /************************************************
        Step4:  destory VIF
        *************************************************/
        STCHECKRESULT(ST_Vif_StopPort(u32VifChnId, u32VifPortId));
        STCHECKRESULT(ST_Vif_DisableDev(u32VifDevId));
        
        /************************************************
        Step5:  destory SENSOR
        *************************************************/
        STCHECKRESULT(MI_SNR_Disable(eSNRPad));
    }

	STCHECKRESULT(MI_IQSERVER_Close());

    STCHECKRESULT(ST_Sys_Exit());
	
    return MI_SUCCESS;
}

int SSTAR_DualSensorInit()
{
	STCHECKRESULT(ST_BaseModuleInit(&g_stConfig));
	return 0;
}

void SSTAR_DualSensorDeinit()
{
	ST_BaseModuleUnInit(&g_stConfig);
}
