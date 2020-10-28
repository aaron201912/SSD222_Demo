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

#include "Sensor2Panel.h"
#include "panelconfig.h"


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

ST_DBG_LEVEL_e g_eSTDbgLevel = ST_DBG_ALL;
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
#if 0
    STCHECKRESULT(MI_SYS_Init());
#endif
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
    MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 5);

    STCHECKRESULT(MI_VPE_EnablePort(pstPortInfo->DepVpeChannel, VpePort));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StopPort(MI_VPE_CHANNEL VpeChannel, MI_VPE_PORT VpePort)
{
    STCHECKRESULT(MI_VPE_DisablePort(VpeChannel, VpePort));

    return MI_SUCCESS;
}

MI_S32 ST_BaseModuleInit(ST_Config_S* pstConfig)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VIF_WorkMode_e eVifWorkMode = E_MI_VIF_WORK_MODE_RGB_REALTIME;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    MI_U32 u32ChocieRes =0;
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_SYS_WindowRect_t stRect;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    memset(&stVpeChannelInfo, 0x0, sizeof(ST_VPE_ChannelInfo_T));
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stVpePortInfo, 0x0, sizeof(ST_VPE_PortInfo_T));
    memset(&stRect, 0x0, sizeof(MI_SYS_WindowRect_t));

    /************************************************
    Step1:  init SYS and Sensor
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());

    if(pstConfig->s32HDRtype > 0)
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(E_MI_SNR_PAD_ID_0, TRUE));
    }
    else
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(E_MI_SNR_PAD_ID_0, FALSE));
    }
    
    STCHECKRESULT(MI_SNR_QueryResCount(E_MI_SNR_PAD_ID_0, &u32ResCount));
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        MI_SNR_GetRes(E_MI_SNR_PAD_ID_0, u8ResIndex, &stRes);
        DBG_INFO("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }

    DBG_INFO("cnt [%d] resolutions, choice [%d] \n", u32ResCount, pstConfig->u8SNRChocieRes);
    u32ChocieRes = pstConfig->u8SNRChocieRes;
    
    STCHECKRESULT(MI_SNR_SetRes(E_MI_SNR_PAD_ID_0,u32ChocieRes));
    STCHECKRESULT(MI_SNR_Enable(E_MI_SNR_PAD_ID_0));

    STCHECKRESULT(MI_SNR_GetPadInfo(E_MI_SNR_PAD_ID_0, &stPad0Info));
    STCHECKRESULT(MI_SNR_GetPlaneInfo(E_MI_SNR_PAD_ID_0, 0, &stSnrPlane0Info));

    g_u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    g_u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

    /************************************************
    Step2:  init VIF
    *************************************************/
    eVifHdrType = (MI_VIF_HDRType_e)pstConfig->s32HDRtype;

    STCHECKRESULT(ST_Vif_EnableDev(0, eVifWorkMode, eVifHdrType, &stPad0Info));

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
    STCHECKRESULT(ST_Vif_CreatePort(0, 0, &stVifPortInfoInfo));
    STCHECKRESULT(ST_Vif_StartPort(0, 0, 0));
    
    memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
    eVpeHdrType = (MI_VPE_HDRType_e)pstConfig->s32HDRtype;
    stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
    stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
    stVpeChannelInfo.u32X = 0;
    stVpeChannelInfo.u32Y = 0;
    stVpeChannelInfo.u16VpeCropW = 0;
    stVpeChannelInfo.u16VpeCropH = 0;
    stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
    stVpeChannelInfo.eFormat = ePixFormat;
    stVpeChannelInfo.e3DNRLevel = pstConfig->en3dNrLevel;
    stVpeChannelInfo.eHDRtype = eVpeHdrType;
    stVpeChannelInfo.bRotation = FALSE;
    STCHECKRESULT(ST_Vpe_CreateChannel(0, &stVpeChannelInfo));
    STCHECKRESULT(ST_Vpe_StartChannel(0));
    // bind VIF to VPE
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    
    /************************************************
    Step3:  init VPE
    *************************************************/
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = 0;
    stVpePortInfo.u16OutputWidth = PANEL_WIDTH;
    stVpePortInfo.u16OutputHeight = PANEL_HEIGHT;
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    if (pstConfig->stVPEPortCrop.u16Width != 0 && pstConfig->stVPEPortCrop.u16Height != 0)
    {
        stRect.u16X = pstConfig->stVPEPortCrop.u16X;
        stRect.u16Y = pstConfig->stVPEPortCrop.u16Y; 
        stRect.u16Width = pstConfig->stVPEPortCrop.u16Width;
        stRect.u16Height = pstConfig->stVPEPortCrop.u16Height;
        STCHECKRESULT(MI_VPE_SetPortCrop(0, 0, &stRect));
    }
    STCHECKRESULT(ST_Vpe_StartPort(0, &stVpePortInfo));

#if 0
    /************************************************
    Step4:  destory DISP and PANEL
    *************************************************/
    MI_DISP_PubAttr_t stPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_RotateConfig_t stRotateConfig;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;
    MI_PANEL_IntfType_e eIntfType;
    MI_PANEL_ParamConfig_t pstParamCfg;
    
    memset(&stWinRect, 0, sizeof(MI_DISP_VidWinRect_t));
    memset(&pstParamCfg, 0, sizeof(MI_PANEL_ParamConfig_t));
    memset(&eIntfType, 0, sizeof(MI_PANEL_IntfType_e));
    //init panel
    eIntfType = E_MI_PNL_INTF_TTL;
    STCHECKRESULT(MI_PANEL_Init(eIntfType));
    STCHECKRESULT(MI_PANEL_GetPanelParam(eIntfType, &pstParamCfg));
    
    //config disp
    memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
    stPubAttr.u32BgColor = YUYV_BLACK;
    stPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    STCHECKRESULT(MI_DISP_SetPubAttr(0, &stPubAttr));
    STCHECKRESULT(MI_DISP_Enable(0));

    memset(&stLayerAttr, 0, sizeof(MI_DISP_VideoLayerAttr_t));
    memset(&stRotateConfig, 0, sizeof(MI_DISP_RotateConfig_t));
    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width = pstParamCfg.u16Width;
    stLayerAttr.stVidLayerDispWin.u16Height = pstParamCfg.u16Height;
    STCHECKRESULT(MI_DISP_BindVideoLayer(0, 0));
    STCHECKRESULT(MI_DISP_SetVideoLayerAttr(0, &stLayerAttr));
    STCHECKRESULT(MI_DISP_EnableVideoLayer(0));
    
    stRotateConfig.eRotateMode = E_MI_DISP_ROTATE_NONE;
    STCHECKRESULT(MI_DISP_SetVideoLayerRotateMode(0, &stRotateConfig));

    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    stInputPortAttr.u16SrcWidth = PANEL_WIDTH;
    stInputPortAttr.u16SrcHeight = PANEL_HEIGHT;
    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width = pstParamCfg.u16Width;
    stInputPortAttr.stDispWin.u16Height = pstParamCfg.u16Height;
    STCHECKRESULT(MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr));
    STCHECKRESULT(MI_DISP_EnableInputPort(0, 0));
#endif

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
    
    /************************************************
    Step7:  //hc fd stream 
    *************************************************/
    MI_DIVP_ChnAttr_t stAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    MI_SYS_ChnPort_t stChnPort;
    
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
    STCHECKRESULT(MI_DIVP_CreateChn(0, &stAttr));
    

    stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stOutputPortAttr.u32Width = HCFD_RAW_W;
    stOutputPortAttr.u32Height = HCFD_RAW_H;
    stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    STCHECKRESULT(MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr));
    STCHECKRESULT(MI_DIVP_StartChn(0));
    stChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stChnPort.u32ChnId =  0;
    stChnPort.u32DevId = 0;
    stChnPort.u32PortId = 0;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stChnPort, 3, 5));

    // bind VPE to divp
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = HCFD_FRMRATE * 4; //每次HC or FD运行取2帧,相同一秒做5次算法运行
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    
    ST_RGN_Init(0);

    mid_hchdfd_Initial();

    return MI_SUCCESS;
}
MI_S32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;
    
    /************************************************
    Step1:  destory DISP and PANEL
    *************************************************/
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
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

#if 0
    STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
    STCHECKRESULT(MI_DISP_DisableVideoLayer(0));
    STCHECKRESULT(MI_DISP_UnBindVideoLayer(0, 0));
    STCHECKRESULT(MI_DISP_Disable(0));
    STCHECKRESULT(MI_PANEL_DeInit());
#endif
    /************************************************
    Step2:  destory DIVP & RGN
    *************************************************/
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = HCFD_FRMRATE * 4;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    
    STCHECKRESULT(MI_DIVP_StopChn(0));
    STCHECKRESULT(MI_DIVP_DestroyChn(0));

    mid_hchdfd_Uninitial();

    /************************************************
    Step2:  destory VPE
    *************************************************/
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    STCHECKRESULT(ST_Vpe_StopPort(0, 0));
    STCHECKRESULT(ST_Vpe_StopChannel(0));
    STCHECKRESULT(ST_Vpe_DestroyChannel(0));

    /************************************************
    Step3:  destory VIF
    *************************************************/
    STCHECKRESULT(ST_Vif_StopPort(0, 0));
    STCHECKRESULT(ST_Vif_DisableDev(0));
    
    /************************************************
    Step4:  destory SENSOR
    *************************************************/
    STCHECKRESULT(MI_SNR_Disable(E_MI_SNR_PAD_ID_0));

    //STCHECKRESULT(MI_RGN_Destroy(0));
    //STCHECKRESULT(MI_RGN_DeInit());
    ST_RGN_Deinit(0);

#if 0
    /************************************************
    Step5:  destory SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Exit());
#endif
    return MI_SUCCESS;
}

int SSTAR_FaceDetectInit()
{
	return ST_BaseModuleInit(&g_stConfig);
}

void SSTAR_FaceDetectDeinit()
{
	ST_BaseModuleUnInit();
}
