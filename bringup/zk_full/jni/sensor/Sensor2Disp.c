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

#include "panelconfig.h"
#include "Sensor2Disp.h"

#ifndef STCHECKRESULT
#define STCHECKRESULT(_func_)\
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != MI_SUCCESS)\
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
    } while(0)
#endif

#define MI_U32VALUE(pu8Data, index) (pu8Data[index]<<24)|(pu8Data[index+1]<<16)|(pu8Data[index+2]<<8)|(pu8Data[index+3])

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE              MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED                MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN              MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE               MAKE_YUYV_VALUE(29,225,107)

typedef struct SSTAR_Rect_s
{
    MI_U32 u32X;
    MI_U32 u32Y;
    MI_U16 u16PicW;
    MI_U16 u16PicH;
} SSTAR_Rect_t;

typedef struct SSTAR_BindInfo_s
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindParam;
} SSTAR_BindInfo_t;

typedef struct ST_VIF_PortInfo_s
{
    MI_U32 u32RectX;
    MI_U32 u32RectY;
    MI_U32 u32RectWidth;
    MI_U32 u32RectHeight;
    MI_U32 u32DestWidth;
    MI_U32 u32DestHeight;
    MI_U32 u32IsInterlace;
    MI_VIF_FrameRate_e eFrameRate;
    MI_SYS_PixelFormat_e ePixFormat;
} ST_VIF_PortInfo_T;

typedef struct ST_VPE_ChannelInfo_s
{
    MI_U16 u16VpeMaxW;
    MI_U16 u16VpeMaxH;
    MI_U16 u16VpeCropW;
    MI_U16 u16VpeCropH;
    MI_S32 u32X;
    MI_S32 u32Y;
    MI_SYS_PixelFormat_e eFormat;
    MI_VPE_RunningMode_e eRunningMode;
    MI_VPE_HDRType_e eHDRtype;
    MI_VPE_3DNR_Level_e e3DNRLevel;
    MI_BOOL bRotation;
    MI_VPE_SensorChannel_e eBindSensorId;
} ST_VPE_ChannelInfo_T;

typedef struct ST_VPE_PortInfo_s
{
    MI_VPE_CHANNEL DepVpeChannel;
    MI_U16 u16OutputWidth;                         // Width of target image
    MI_U16 u16OutputHeight;                        // Height of target image
    MI_SYS_PixelFormat_e  ePixelFormat;      // Pixel format of target image
    MI_SYS_CompressMode_e eCompressMode;     // Compression mode of the output
} ST_VPE_PortInfo_T;

MI_U32 g_u32CapWidth = 0;
MI_U32 g_u32CapHeight = 0;

static SensorAssembly_t g_stSensorAssembly;
static VifAssembly_t g_stVifAssembly;
static VpeAssembly_t g_stVpeAssembly;
static IspAssembly_t g_stIspAssembly;
static IqServerAssembly_t g_stIqServerAssembly;
static VdispAssembly_t g_stVdispAssembly;		// dual sensor
static DivpAssembly_t g_stDivpAssembly;			// face detect

int SSTAR_SensorFlow_OpenLibrary()
{
	memset(&g_stSensorAssembly, 0, sizeof(SensorAssembly_t));
	memset(&g_stVifAssembly, 0, sizeof(VifAssembly_t));
	memset(&g_stVpeAssembly, 0, sizeof(VpeAssembly_t));
	memset(&g_stIspAssembly, 0, sizeof(IspAssembly_t));
	memset(&g_stIqServerAssembly, 0, sizeof(IqServerAssembly_t));
	memset(&g_stVdispAssembly, 0, sizeof(VdispAssembly_t));
	memset(&g_stDivpAssembly, 0, sizeof(DivpAssembly_t));

	if (SSTAR_SNR_OpenLibrary(&g_stSensorAssembly))
	{
		printf("open libmi_sensor failed\n");
		goto exit;
	}

	if (SSTAR_VIF_OpenLibrary(&g_stVifAssembly))
	{
		printf("open libmi_vif failed\n");
		goto close_snr_lib;
	}

	if (SSTAR_VPE_OpenLibrary(&g_stVpeAssembly))
	{
		printf("open libmi_vpe failed\n");
		goto close_vif_lib;
	}

	if (SSTAR_ISP_OpenLibrary(&g_stIspAssembly))
	{
		printf("open libmi_isp failed\n");
		goto close_vpe_lib;
	}

	if (SSTAR_IQSERVER_OpenLibrary(&g_stIqServerAssembly))
	{
		printf("open libmi_iqserver failed\n");
		goto close_isp_lib;
	}

	if (SSTAR_VDISP_OpenLibrary(&g_stVdispAssembly))
	{
		printf("open libmi_vdisp failed\n");
		goto close_iqserver_lib;
	}

	if (SSTAR_DIVP_OpenLibrary(&g_stDivpAssembly))
	{
		printf("open libmi_divp failed\n");
		goto close_vdisp_lib;
	}

	return 0;

close_vdisp_lib:
	SSTAR_VDISP_CloseLibrary(&g_stVdispAssembly);

close_iqserver_lib:
	SSTAR_IQSERVER_CloseLibrary(&g_stIqServerAssembly);

close_isp_lib:
	SSTAR_ISP_CloseLibrary(&g_stIspAssembly);

close_vpe_lib:
	SSTAR_VPE_CloseLibrary(&g_stVpeAssembly);

close_vif_lib:
	SSTAR_VIF_CloseLibrary(&g_stVifAssembly);

close_snr_lib:
	SSTAR_SNR_CloseLibrary(&g_stSensorAssembly);

exit:
	return 0;
}

void SSTAR_SensorFlow_CloseLibrary()
{
	SSTAR_DIVP_CloseLibrary(&g_stDivpAssembly);
	SSTAR_VDISP_CloseLibrary(&g_stVdispAssembly);
	SSTAR_IQSERVER_CloseLibrary(&g_stIqServerAssembly);
	SSTAR_ISP_CloseLibrary(&g_stIspAssembly);
	SSTAR_VPE_CloseLibrary(&g_stVpeAssembly);
	SSTAR_VIF_CloseLibrary(&g_stVifAssembly);
	SSTAR_SNR_CloseLibrary(&g_stSensorAssembly);
}


MI_S32 SSTAR_Module_Bind(SSTAR_BindInfo_t *pstBindInfo)
{
    STCHECKRESULT(MI_SYS_BindChnPort2(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort,
        pstBindInfo->u32SrcFrmrate, pstBindInfo->u32DstFrmrate, pstBindInfo->eBindType, pstBindInfo->u32BindParam));
    printf("\n");
    printf("src(%d-%d-%d-%d)  dst(%d-%d-%d-%d)  %d...\n", pstBindInfo->stSrcChnPort.eModId, pstBindInfo->stSrcChnPort.u32DevId,
        pstBindInfo->stSrcChnPort.u32ChnId, pstBindInfo->stSrcChnPort.u32PortId,
        pstBindInfo->stDstChnPort.eModId, pstBindInfo->stDstChnPort.u32DevId, pstBindInfo->stDstChnPort.u32ChnId,
        pstBindInfo->stDstChnPort.u32PortId, pstBindInfo->eBindType);
    printf("\n");
    return MI_SUCCESS;
}

MI_S32 SSTAR_Module_UnBind(SSTAR_BindInfo_t *pstBindInfo)
{
    STCHECKRESULT(MI_SYS_UnBindChnPort(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VIF_EnableDev(MI_VIF_DEV VifDev, MI_VIF_WorkMode_e eWorkMode, MI_VIF_HDRType_e eHdrType, MI_SNR_PADInfo_t *pstSnrPadInfo)
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

    STCHECKRESULT(g_stVifAssembly.pfnVifSetDevAttr(VifDev, &stDevAttr));
    STCHECKRESULT(g_stVifAssembly.pfnVifEnableDev(VifDev));
    return MI_SUCCESS;
}
MI_S32 SSTAR_VIF_DisableDev(MI_VIF_DEV VifDev)
{
    STCHECKRESULT(g_stVifAssembly.pfnVifDisableDev(VifDev));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VIF_CreatePort(MI_VIF_CHN VifChn, MI_VIF_PORT VifPort, ST_VIF_PortInfo_T *pstPortInfoInfo)
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
    STCHECKRESULT(g_stVifAssembly.pfnVifSetChnPortAttr(VifChn, VifPort, &stChnPortAttr));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VIF_StartPort(MI_VIF_DEV VifDev, MI_VIF_CHN VifChn, MI_VIF_PORT VifPort)
{
    MI_SYS_ChnPort_t stChnPort;

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VIF;
    stChnPort.u32DevId = VifDev;
    stChnPort.u32ChnId = VifChn;
    stChnPort.u32PortId = VifPort;

    STCHECKRESULT(g_stVifAssembly.pfnVifEnableChnPort(VifChn, VifPort));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VIF_StopPort(MI_VIF_CHN VifChn, MI_VIF_PORT VifPort)
{
    STCHECKRESULT(g_stVifAssembly.pfnVifDisableChnPort(VifChn, VifPort));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_CreateChannel(MI_VPE_CHANNEL VpeChannel, ST_VPE_ChannelInfo_T *pstChannelInfo)
{
    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_SYS_WindowRect_t stCropWin;
    MI_VPE_ChannelPara_t stChannelVpeParam;

    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelAttr_t));
    memset(&stCropWin, 0, sizeof(MI_SYS_WindowRect_t));
    memset(&stChannelVpeParam, 0x00, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeParam.eHDRType = pstChannelInfo->eHDRtype;
    stChannelVpeParam.e3DNRLevel = pstChannelInfo->e3DNRLevel;
    g_stVpeAssembly.pfnVpeSetChannelParam(VpeChannel, &stChannelVpeParam);

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

    STCHECKRESULT(g_stVpeAssembly.pfnVpeCreateChannel(VpeChannel, &stChannelVpeAttr));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_StartChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(g_stVpeAssembly.pfnVpeStartChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_StopChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(g_stVpeAssembly.pfnVpeStopChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_DestroyChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(g_stVpeAssembly.pfnVpeDestroyChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_StartPort(MI_VPE_PORT VpePort, ST_VPE_PortInfo_T *pstPortInfo)
{
    MI_VPE_PortMode_t stVpeMode;
    MI_SYS_ChnPort_t stChnPort;
    printf("SSTAR_VPE_StartPort ch:%d port %d,w %d h %d\n", pstPortInfo->DepVpeChannel, VpePort,pstPortInfo->u16OutputWidth, pstPortInfo->u16OutputHeight);

    memset(&stVpeMode, 0, sizeof(stVpeMode));
    STCHECKRESULT(g_stVpeAssembly.pfnVpeGetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));

    stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stVpeMode.ePixelFormat = pstPortInfo->ePixelFormat;
    stVpeMode.u16Width = pstPortInfo->u16OutputWidth;
    stVpeMode.u16Height= pstPortInfo->u16OutputHeight;
    STCHECKRESULT(g_stVpeAssembly.pfnVpeSetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VPE;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = pstPortInfo->DepVpeChannel;
    stChnPort.u32PortId = VpePort;
    STCHECKRESULT(g_stVpeAssembly.pfnVpeEnablePort(pstPortInfo->DepVpeChannel, VpePort));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VPE_StopPort(MI_VPE_CHANNEL VpeChannel, MI_VPE_PORT VpePort)
{
    STCHECKRESULT(g_stVpeAssembly.pfnVpeDisablePort(VpeChannel, VpePort));

    return MI_SUCCESS;
}


MI_S32 SSTAR_VDISP_Init(void)
{
    MI_VDISP_InputChnAttr_t stInputChnAttr;
    MI_VDISP_OutputPortAttr_t stOutputPortAttr;
    SSTAR_Rect_t stRect;
    MI_VDISP_DEV DevId = 0;
    MI_VDISP_CHN ChnId = 0;

    STCHECKRESULT(g_stVdispAssembly.pfnVdispInit());
    STCHECKRESULT(g_stVdispAssembly.pfnVdispOpenDevice(DevId));

    // x align by 16
    ChnId = 0;
    memset(&stRect, 0x0, sizeof(SSTAR_Rect_t));
    memset(&stInputChnAttr, 0x0, sizeof(MI_VDISP_InputChnAttr_t));
    stRect.u32X = ALIGN_DOWN(0, 16);
    stRect.u32Y = 0;
    stRect.u16PicW = PANEL_WIDTH/2;
    stRect.u16PicH = PANEL_HEIGHT;
    stInputChnAttr.s32IsFreeRun = TRUE;
    stInputChnAttr.u32OutHeight = stRect.u16PicH;
    stInputChnAttr.u32OutWidth = stRect.u16PicW;
    stInputChnAttr.u32OutX = stRect.u32X;
    stInputChnAttr.u32OutY = stRect.u32Y;

    STCHECKRESULT(g_stVdispAssembly.pfnVdispSetInputChannelAttr(DevId, ChnId, &stInputChnAttr));

    // x align by 16
    ChnId = 1;
    memset(&stRect, 0x0, sizeof(SSTAR_Rect_t));
    memset(&stInputChnAttr, 0x0, sizeof(MI_VDISP_InputChnAttr_t));
    stRect.u32X = ALIGN_DOWN(PANEL_WIDTH/2, 16);
    stRect.u32Y = 0;
    stRect.u16PicW = PANEL_WIDTH/2;
    stRect.u16PicH = PANEL_HEIGHT;
    stInputChnAttr.s32IsFreeRun = TRUE;
    stInputChnAttr.u32OutHeight = stRect.u16PicH;
    stInputChnAttr.u32OutWidth = stRect.u16PicW;
    stInputChnAttr.u32OutX = stRect.u32X;
    stInputChnAttr.u32OutY = stRect.u32Y;

    STCHECKRESULT(g_stVdispAssembly.pfnVdispSetInputChannelAttr(DevId, ChnId, &stInputChnAttr));

    memset(&stOutputPortAttr, 0x0, sizeof(MI_VDISP_OutputPortAttr_t));
    //设置输出的颜色格式
    stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    //设置vdisp输出帧中未使用区域的涂黑颜色,YUV 颜色空间
    stOutputPortAttr.u32BgColor = YUYV_BLACK;
    //设置vdisp输出帧率
    stOutputPortAttr.u32FrmRate = 30;
    //设置vdisp输出帧的高
    stOutputPortAttr.u32Height = PANEL_HEIGHT;
    //设置vdisp输出帧的宽
    stOutputPortAttr.u32Width = PANEL_WIDTH;
    //设置vdisp输出帧的最低PTS
    stOutputPortAttr.u64pts = 0;
    /*
    *为output port 0 设置参数
    */
    STCHECKRESULT(g_stVdispAssembly.pfnVdispSetOutputPortAttr(DevId,0,&stOutputPortAttr));
    
    ChnId = 0;
    STCHECKRESULT(g_stVdispAssembly.pfnVdispEnableInputChannel(DevId, ChnId));
    ChnId = 1;
    STCHECKRESULT(g_stVdispAssembly.pfnVdispEnableInputChannel(DevId, ChnId));
    STCHECKRESULT(g_stVdispAssembly.pfnVdispStartDev(DevId));
    
    return MI_SUCCESS;
}

MI_S32 SSTAR_VDISP_Deinit(void)
{
    MI_VDISP_DEV DevId = 0;
    MI_VDISP_CHN ChnId = 0;

    ChnId = 0;
    STCHECKRESULT(g_stVdispAssembly.pfnVdispDisableInputChannel(DevId, ChnId));
    ChnId = 1;
    STCHECKRESULT(g_stVdispAssembly.pfnVdispDisableInputChannel(DevId, ChnId));
    STCHECKRESULT(g_stVdispAssembly.pfnVdispStopDev(DevId));
    STCHECKRESULT(g_stVdispAssembly.pfnVdispCloseDevice(DevId));
    STCHECKRESULT(g_stVdispAssembly.pfnVdispExit());
	STCHECKRESULT(g_stVdispAssembly.pfnVdispDeInitDev());
    
    return MI_SUCCESS;
}


/*
* 绑定 vpe out0 -> vdisp in0---\
* 绑定 vpe out1 -> vdisp in1--------->--> disp in0
*/
MI_S32 SSTAR_VDISP_Bind(void)
{
    SSTAR_BindInfo_t stBindInfo;

    /*
    * 绑定 vdisp out0-> disp in0
    * <chn,dev,port> : (0,0,0) -> (0,0,0)
    */
    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));

    /*
     * 绑定 vpe out0-> vdisp 0
     * <chn,dev,port> : (0,0,0) -> (0,0,0)
     */
    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));

    /*
     * 绑定 vpe out0-> vdisp in1
     * <chn,dev,port> : (1,0,0) -> (1,0,0)
     */
    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));

    return MI_SUCCESS;
}

MI_S32 SSTAR_VDISP_UnBind(void)
{
    SSTAR_BindInfo_t stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));

    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));

    memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
    STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));

    return MI_SUCCESS;
}

MI_BOOL ST_DoSetIqBin(MI_VPE_CHANNEL Vpechn,char *pConfigPath)
{
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    CUS3A_ALGO_STATUS_t stCus3AAlgoStatus;
    MI_U8  u8ispreadycnt = 0;
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

        g_stIspAssembly.pfnIspIqGetParaInitStatus(Vpechn, &status);
        CUS3A_GetAlgoStatus((CUS3A_ISP_CH_e)Vpechn, &stCus3AAlgoStatus);
        //printf("00%s,%d:bFlag:%d, Ae:%d，cnt:%d \n", __FUNCTION__, __LINE__, status.stParaAPI.bFlag, stCus3AAlgoStatus.Ae, u8ispreadycnt);
        if((status.stParaAPI.bFlag != 1) || (stCus3AAlgoStatus.Ae != E_ALGO_STATUS_RUNNING))
        {
            usleep(300*1000);
            u8ispreadycnt++;
            //printf("3%s,%d:bFlag:%d, Ae:%d，cnt:%d \n", __FUNCTION__, __LINE__, status.stParaAPI.bFlag, stCus3AAlgoStatus.Ae, u8ispreadycnt);
            continue;
        }

        u8ispreadycnt = 0;

        printf("loading api bin...path:%s\n",pConfigPath);
        g_stIspAssembly.pfnIspApiCmdLoadBinFile(Vpechn, (char *)pConfigPath, 1234);

        usleep(10*1000);
    }while((status.stParaAPI.bFlag != 1) || (stCus3AAlgoStatus.Ae != E_ALGO_STATUS_RUNNING));

    return 0;
}


MI_S32 SSTAR_SensorFlow_Init(SSTar_SensorConfig_t* pstConfig)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    SSTAR_BindInfo_t stBindInfo;
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
    MI_U8 u8SensorNum = 1;
    MI_U8 u8FaceDetect = 0;
    MI_U8 i = 0;
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
    MI_DIVP_ChnAttr_t stAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    MI_SYS_ChnPort_t stChnPort;
    MI_U32 u32DispDevId = 0;
    MI_U32 u32DispPortId = 0;
    MI_U32 u32DispLayer = 0;
    char acharIqApiPath[64]={0};
    
    memset(&stPadInfo, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlaneInfo, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stVpePortInfo, 0x0, sizeof(ST_VPE_PortInfo_T));
    memset(&stRect, 0x0, sizeof(MI_SYS_WindowRect_t));

    if (SSTAR_SensorFlow_OpenLibrary())
    {
    	printf("dlopen mi libs error\n");
    	return -1;
    }
    
    u8SensorNum = pstConfig->u8SensorNum;
    u8FaceDetect = pstConfig->u8FaceDetect;
    if(1 == u8FaceDetect)
    {
        STCHECKRESULT(ST_RGN_Init());

        printf("rgn init done\n");
    }
    for(i = 0; i < u8SensorNum; i++)
    {
        printf("i[%d]\n", i);
        eSNRPad = (MI_SNR_PAD_ID_e)i;
        if(pstConfig->s32HDRtype > 0)
        {
        	STCHECKRESULT(g_stSensorAssembly.pfnSnrSetPlaneMode(eSNRPad, TRUE));
        }
        else
        {
        	STCHECKRESULT(g_stSensorAssembly.pfnSnrSetPlaneMode(eSNRPad, FALSE));
        }
        
        STCHECKRESULT(g_stSensorAssembly.pfnSnrQueryResCount(eSNRPad, &u32ResCount));

        for(u8ResIndex = 0; u8ResIndex < u32ResCount; u8ResIndex++)
        {
        	g_stSensorAssembly.pfnSnrGetRes(eSNRPad, u8ResIndex, &stRes);
            printf("eSNRPad[%d], index[%d], Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps[%d], minfps[%d], ResDesc[%s] \n",
					eSNRPad,
					u8ResIndex,
					stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
					stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
					stRes.u32MaxFps,stRes.u32MinFps,
					stRes.strResDesc);
        }

        printf("eSNRPad[%d], cnt[%d]resolutions, choice[%d] \n", eSNRPad, u32ResCount, pstConfig->u8SNRChocieRes);

        u32ChocieRes = pstConfig->u8SNRChocieRes;

        //gc1054 default set mirror/flip = 1
        if(ST_Sensor_Type_GC1054 == pstConfig->enSensorType)
        {
        	STCHECKRESULT(g_stSensorAssembly.pfnSnrSetOrien(eSNRPad, 1, 1));
        }

        STCHECKRESULT(g_stSensorAssembly.pfnSnrSetRes(eSNRPad,u32ChocieRes));
        STCHECKRESULT(g_stSensorAssembly.pfnSnrEnable(eSNRPad));
        STCHECKRESULT(g_stSensorAssembly.pfnSnrGetPadInfo(eSNRPad, &stPadInfo));
        STCHECKRESULT(g_stSensorAssembly.pfnSnrGetPlaneInfo(eSNRPad, 0, &stSnrPlaneInfo));

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
        eVifWorkMode = u8SensorNum > 1 ? E_MI_VIF_WORK_MODE_RGB_FRAMEMODE : E_MI_VIF_WORK_MODE_RGB_REALTIME;
        printf("VIF:DevId[%d] ChnId[%d] PortId[%d], eVifWorkMode[%d]\n", u32VifDevId, u32VifChnId, u32VifPortId, eVifWorkMode);
        STCHECKRESULT(SSTAR_VIF_EnableDev(u32VifDevId, eVifWorkMode, eVifHdrType, &stPadInfo));

        memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
        stVifPortInfoInfo.u32RectX = stSnrPlaneInfo.stCapRect.u16X;
        stVifPortInfoInfo.u32RectY = stSnrPlaneInfo.stCapRect.u16Y;
        stVifPortInfoInfo.u32RectWidth = u32CapWidth;
        stVifPortInfoInfo.u32RectHeight = u32CapHeight;
        stVifPortInfoInfo.u32DestWidth = u32CapWidth;
        stVifPortInfoInfo.u32DestHeight = u32CapHeight;
        stVifPortInfoInfo.eFrameRate = eFrameRate;
        stVifPortInfoInfo.ePixFormat = ePixFormat;
        STCHECKRESULT(SSTAR_VIF_CreatePort(u32VifChnId, u32VifPortId, &stVifPortInfoInfo));
        STCHECKRESULT(SSTAR_VIF_StartPort(u32VifDevId, u32VifChnId, u32VifPortId));
        
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
        stVpeChannelInfo.eRunningMode = u8SensorNum > 1 ? E_MI_VPE_RUN_CAM_MODE : E_MI_VPE_RUN_REALTIME_MODE;
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
        printf("VPE:DevId[%d] ChnId[%d] PortId[%d],eRunningMode[%d]\n", u32VpeDevId, u32VpeChnId, u32VpePortId, stVpeChannelInfo.eRunningMode);
        STCHECKRESULT(SSTAR_VPE_CreateChannel(u32VpeChnId, &stVpeChannelInfo));
        STCHECKRESULT(SSTAR_VPE_StartChannel(u32VpeChnId));
        
        // bind VIF to VPE
        memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
        stBindInfo.stSrcChnPort.u32DevId = u32VifDevId;
        stBindInfo.stSrcChnPort.u32ChnId = u32VifChnId;
        stBindInfo.stSrcChnPort.u32PortId = u32VifPortId;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
        stBindInfo.stDstChnPort.u32DevId = u32VpeDevId;
        stBindInfo.stDstChnPort.u32ChnId = u32VpeChnId;
        stBindInfo.stDstChnPort.u32PortId = u32VpePortId;
        stBindInfo.eBindType = u8SensorNum > 1 ? E_MI_SYS_BIND_TYPE_FRAME_BASE : E_MI_SYS_BIND_TYPE_REALTIME;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));
        
        memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
        stVpePortInfo.DepVpeChannel = u32VpeChnId;
        stVpePortInfo.u16OutputWidth = u8SensorNum > 1 ? PANEL_WIDTH/2 : PANEL_WIDTH;
        stVpePortInfo.u16OutputHeight = PANEL_HEIGHT;
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
        STCHECKRESULT(SSTAR_VPE_StartPort(u32VpePortId, &stVpePortInfo));
    }

    // set disp inputport attr
    MI_DISP_InputPortAttr_t stDispInputAttr;
    MI_DISP_GetInputPortAttr(0, 0, &stDispInputAttr);
    stDispInputAttr.stDispWin.u16X = 0;
    stDispInputAttr.stDispWin.u16Y = 0;
    stDispInputAttr.stDispWin.u16Width = PANEL_WIDTH;
    stDispInputAttr.stDispWin.u16Height = PANEL_HEIGHT;
    stDispInputAttr.u16SrcWidth = stDispInputAttr.stDispWin.u16Width;
    stDispInputAttr.u16SrcHeight = stDispInputAttr.stDispWin.u16Height;
    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_SetInputPortAttr(0, 0, &stDispInputAttr);
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    if(u8SensorNum > 1)
    {
        SSTAR_VDISP_Init();
        SSTAR_VDISP_Bind();
    }
    else
    {
        if((E_MI_SYS_ROTATE_180 == pstConfig->enRotation) || (1 == u8FaceDetect))
        {
            memset(&stAttr, 0, sizeof(stAttr));
            memset(&stOutputPortAttr, 0, sizeof(stOutputPortAttr));
            stAttr.bHorMirror = FALSE;
            stAttr.bVerMirror = FALSE;
            stAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
            stAttr.eRotateType = pstConfig->enRotation;
            stAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
            stAttr.stCropRect.u16X = 0;
            stAttr.stCropRect.u16Y = 0;
            stAttr.stCropRect.u16Width = PANEL_WIDTH;
            stAttr.stCropRect.u16Height = PANEL_HEIGHT;
            stAttr.u32MaxWidth = PANEL_WIDTH;
            stAttr.u32MaxHeight = PANEL_HEIGHT;
            STCHECKRESULT(g_stDivpAssembly.pfnDivpCreateChn(0, &stAttr));

            stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
            stOutputPortAttr.u32Width = PANEL_WIDTH;
            stOutputPortAttr.u32Height = PANEL_HEIGHT;
            stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(g_stDivpAssembly.pfnDivpSetOutputPortAttr(0, &stOutputPortAttr));
            STCHECKRESULT(g_stDivpAssembly.pfnDivpStartChn(0));

            // bind VPE to divp
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = 0;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = 0;
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));

            // bind divp to disp
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
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
            STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));
        }
        else
        {
            // bind VPE to disp
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
            STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));
        }
        if(1 == u8FaceDetect)
        {
            /************************************************
             hc fd stream 
            *************************************************/
            memset(&stAttr, 0, sizeof(stAttr));
            memset(&stOutputPortAttr, 0, sizeof(stOutputPortAttr));
            stAttr.bHorMirror = FALSE;
            stAttr.bVerMirror = FALSE;
            stAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
            stAttr.eRotateType = E_MI_SYS_ROTATE_NONE;
            stAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
            stAttr.stCropRect.u16X = 0;
            stAttr.stCropRect.u16Y = 0;
            stAttr.stCropRect.u16Width = PANEL_WIDTH;
            stAttr.stCropRect.u16Height = PANEL_HEIGHT;
            stAttr.u32MaxWidth = PANEL_WIDTH;
            stAttr.u32MaxHeight = PANEL_HEIGHT;
            STCHECKRESULT(g_stDivpAssembly.pfnDivpCreateChn(1, &stAttr));

            stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
            stOutputPortAttr.u32Width = HCFD_RAW_W;
            stOutputPortAttr.u32Height = HCFD_RAW_H;
            stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(g_stDivpAssembly.pfnDivpSetOutputPortAttr(1, &stOutputPortAttr));
            STCHECKRESULT(g_stDivpAssembly.pfnDivpStartChn(1));

            stChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stChnPort.u32ChnId =  1;
            stChnPort.u32DevId = 0;
            stChnPort.u32PortId = 0;
            STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stChnPort, 3, 5));

            // bind VPE to divp
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = 0;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = 1;
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = HCFD_FRMRATE * 4; //每次HC or FD运行取2帧,相同一秒做5次算法运行
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(SSTAR_Module_Bind(&stBindInfo));
            
            ST_RGN_Create(0);
            mid_hchdfd_Initial();
        }
    }
    for(i = 0; i < u8SensorNum; i++)
    {
        u32VpeDevId = 0;
        u32VpeChnId = i;
        u32VpePortId = 0;
        //open ip server
        memset(&stVifPortInfo, 0, sizeof(MI_VIF_ChnPortAttr_t));
        STCHECKRESULT(g_stVifAssembly.pfnVifGetChnPortAttr(u32VpeChnId, u32VpePortId, &stVifPortInfo));
        STCHECKRESULT(g_stIqServerAssembly.pfnIqServerOpen(stVifPortInfo.stDestSize.u16Width, stVifPortInfo.stDestSize.u16Height, u32VpeChnId));

        //Load IQ bin
        memset(acharIqApiPath, 0x0, sizeof(acharIqApiPath));
        snprintf(acharIqApiPath, sizeof(acharIqApiPath), "/config/iqfile/iqapifile%d.bin", u32VpeChnId);
        ST_DoSetIqBin(u32VpeChnId, acharIqApiPath);
    }
    return MI_SUCCESS;
}
MI_S32 SSTAR_SensorFlow_Deinit(SSTar_SensorConfig_t* pstConfig)
{
    SSTAR_BindInfo_t stBindInfo;
    MI_SNR_PAD_ID_e eSNRPad = E_MI_SNR_PAD_ID_0;
    MI_U8 u8SensorNum = 1;
    MI_U8 u8FaceDetect = 0;
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

    u8SensorNum = pstConfig->u8SensorNum;
    u8FaceDetect = pstConfig->u8FaceDetect;
    
    /************************************************
     destory ive/rgn
    *************************************************/
    if(1 == u8FaceDetect)
    {
        mid_hchdfd_Uninitial();
        STCHECKRESULT(ST_RGN_Destroy(0));
    }

    /************************************************
     destory vdisp
    *************************************************/
    if(u8SensorNum > 1)
    {
        SSTAR_VDISP_UnBind();
        SSTAR_VDISP_Deinit();
    }
    else
    {
        if((E_MI_SYS_ROTATE_180 == pstConfig->enRotation) || (1 == u8FaceDetect))
        {
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = 0;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = 0;
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));
            
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = 0;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = 0;
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));
            STCHECKRESULT(g_stDivpAssembly.pfnDivpStopChn(0));
            STCHECKRESULT(g_stDivpAssembly.pfnDivpDestroyChn(0));
        }
        else
        {
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
            STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));
        }

        if(1 == u8FaceDetect)
        {
            memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = 0;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
            stBindInfo.stDstChnPort.u32DevId = 0;
            stBindInfo.stDstChnPort.u32ChnId = 1;
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = HCFD_FRMRATE * 4;
            STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));

			STCHECKRESULT(g_stDivpAssembly.pfnDivpStopChn(1));
			STCHECKRESULT(g_stDivpAssembly.pfnDivpDestroyChn(1));
			STCHECKRESULT(g_stDivpAssembly.pfnDivpDeInitDev());
        }
    }

    for(i = 0; i < u8SensorNum; i++)
    {
        eSNRPad = (MI_SNR_PAD_ID_e)i;
        u32VifDevId = i;
        u32VifChnId = i * 4;
        u32VifPortId = 0;
        u32VpeDevId = 0;
        u32VpeChnId = i;
        u32VpePortId = 0;
        memset(&stBindInfo, 0x0, sizeof(SSTAR_BindInfo_t));
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
        STCHECKRESULT(SSTAR_Module_UnBind(&stBindInfo));

        STCHECKRESULT(SSTAR_VPE_StopPort(u32VpeChnId, u32VpePortId));
        STCHECKRESULT(SSTAR_VPE_StopChannel(u32VpeChnId));
        STCHECKRESULT(SSTAR_VPE_DestroyChannel(u32VpeChnId));
        STCHECKRESULT(SSTAR_VIF_StopPort(u32VifChnId, u32VifPortId));
        STCHECKRESULT(SSTAR_VIF_DisableDev(u32VifDevId));
        STCHECKRESULT(g_stSensorAssembly.pfnSnrDisable(eSNRPad));
    }

    if(1 == u8FaceDetect)
    {
        STCHECKRESULT(ST_RGN_Deinit());
    }

    STCHECKRESULT(g_stVpeAssembly.pfnVpeDeInitDev());
	STCHECKRESULT(g_stVifAssembly.pfnVifDeInitDev());
	STCHECKRESULT(g_stSensorAssembly.pfnSnrDeInitDev());
	STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
	STCHECKRESULT(MI_DISP_DeInitDev());
	STCHECKRESULT(g_stIqServerAssembly.pfnIqServerClose());

	SSTAR_SensorFlow_CloseLibrary();

    return MI_SUCCESS;
}

