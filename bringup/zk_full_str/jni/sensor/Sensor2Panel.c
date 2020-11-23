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
#include "Sensor2Panel.h"

ST_Config_S g_stConfig =
{
    .s32UseOnvif = 0,
    .s32UseVdf = 0,
    .s32UseAudio = 0,
    .s32LoadIQ = 0,
    .s32UsePanel = 0,
    .u8SensorNum = 1,
    .u8FaceDetect = 0,
    .enPixelFormat = E_MI_SYS_PIXEL_FRAME_FORMAT_MAX,
    .s32HDRtype = 0,
    .enSensorType = ST_Sensor_Type_GC1054,
    .enRotation = E_MI_SYS_ROTATE_NONE,
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

MI_U32 g_u32CapWidth = 0;
MI_U32 g_u32CapHeight = 0;
static MI_BOOL g_bExit = FALSE;

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
    stRect.u16PicW = PANEL_WIDTH/2;
    stRect.u16PicH = PANEL_HEIGHT;
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
    stRect.u32X = ALIGN_DOWN(PANEL_WIDTH/2, 16);
    stRect.u32Y = 0;
    stRect.u16PicW = PANEL_WIDTH/2;
    stRect.u16PicH = PANEL_HEIGHT;
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
    stOutputPortAttr.u32Height = PANEL_HEIGHT;
    //设置vdisp输出帧的宽
    stOutputPortAttr.u32Width = PANEL_WIDTH;
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
	STCHECKRESULT(MI_VDISP_DeInitDev());
    
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

MI_S32 ST_Sensor2PanelInit(ST_Config_S* pstConfig)
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
    MI_U8 u8SensorNum = 1;
    MI_U8 u8FaceDetect = 0;
    MI_U8 i = 0;
    MI_SNR_PAD_ID_e eSNRPad = E_MI_SNR_PAD_ID_0;
    ST_VIF_PortInfo_T stVifPortInfoInfo;
    MI_U32 u32VifDevId = 0;
    MI_U32 u32VifChnId = 0;
    MI_U32 u32VifPortId = 0;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
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
    MI_DIVP_ChnAttr_t stAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    MI_SYS_ChnPort_t stChnPort;
    MI_U32 u32DispDevId = 0;
    //MI_U32 u32DispChnId = 0;
    MI_U32 u32DispPortId = 0;
    MI_U32 u32DispLayer = 0;
    
    memset(&stPadInfo, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlaneInfo, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stVpePortInfo, 0x0, sizeof(ST_VPE_PortInfo_T));
    memset(&stRect, 0x0, sizeof(MI_SYS_WindowRect_t));

    /************************************************
    Step1:  init SYS and Sensor
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());
    
    u8SensorNum = pstConfig->u8SensorNum;
    u8FaceDetect = pstConfig->u8FaceDetect;
    for(i = 0; i < u8SensorNum; i++)
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
        Step2:  init VIF
        *************************************************/
        u32VifDevId = i;
        u32VifChnId = i * 4;
        u32VifPortId = 0;
        eVifHdrType = (MI_VIF_HDRType_e)pstConfig->s32HDRtype;
        eVifWorkMode = u8SensorNum > 1 ? E_MI_VIF_WORK_MODE_RGB_FRAMEMODE : E_MI_VIF_WORK_MODE_RGB_REALTIME;
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
        Step3:  init VPE
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

        if (stVpeChannelInfo.eRunningMode == E_MI_VPE_RUN_DVR_MODE)
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
        stBindInfo.eBindType = u8SensorNum > 1 ? E_MI_SYS_BIND_TYPE_FRAME_BASE : E_MI_SYS_BIND_TYPE_REALTIME;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        
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
        STCHECKRESULT(ST_Vpe_StartPort(u32VpePortId, &stVpePortInfo));
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
        ST_Vdisp_Init();
        ST_Vdisp_Bind();
    }
    else
    {
        if(E_MI_SYS_ROTATE_180 == pstConfig->enRotation)
        {
            /************************************************
             init DIVP
            *************************************************/
            memset(&stAttr, 0, sizeof(stAttr));
            memset(&stOutputPortAttr, 0, sizeof(stOutputPortAttr));
            stAttr.bHorMirror = FALSE;
            stAttr.bVerMirror = FALSE;
            stAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
            stAttr.eRotateType = pstConfig->enRotation;
            stAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
            stAttr.stCropRect.u16X = 0;
            stAttr.stCropRect.u16Y = 0;
            stAttr.stCropRect.u16Width = pstParamCfg.u16Width;;
            stAttr.stCropRect.u16Height = pstParamCfg.u16Height;;
            stAttr.u32MaxWidth = pstParamCfg.u16Width;;
            stAttr.u32MaxHeight = pstParamCfg.u16Height;;
            STCHECKRESULT(MI_DIVP_CreateChn(0, &stAttr));
            

            stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
            stOutputPortAttr.u32Width = HCFD_RAW_W;
            stOutputPortAttr.u32Height = HCFD_RAW_H;
            stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            STCHECKRESULT(MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr));
            STCHECKRESULT(MI_DIVP_StartChn(0));

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
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

            // bind divp to disp
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
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
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
        else
        {
        // bind VPE to disp
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
        if(1 == u8FaceDetect)
        {
            /************************************************
            Step7:  //hc fd stream 
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
        }
    }
    return MI_SUCCESS;
}
MI_S32 ST_Sensor2PanelDeinit(ST_Config_S* pstConfig)
{
    ST_Sys_BindInfo_T stBindInfo;
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
    Step1:  destory vdisp
    *************************************************/
    if(u8SensorNum > 1)
    {
        ST_Vdisp_UnBind();
        ST_Vdisp_Deinit();
    }
    else
    {
        if(E_MI_SYS_ROTATE_180 == pstConfig->enRotation)
        {
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
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
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
            
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
            stBindInfo.u32DstFrmrate = 30;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
            
            STCHECKRESULT(MI_DIVP_StopChn(0));
            STCHECKRESULT(MI_DIVP_DestroyChn(0));
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
		Step2:  destory DIVP & RGN
		*************************************************/
		if(1 == u8FaceDetect)
		{
			STCHECKRESULT(ST_RGN_Deinit(0));
			mid_hchdfd_Uninitial();
			
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
			STCHECKRESULT(MI_DIVP_DeInitDev());
		}
    }

    /************************************************
    Step4:  destory VPE
    *************************************************/
    for(i = 0; i < u8SensorNum; i++)
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
        Step5:  destory VIF
        *************************************************/
        STCHECKRESULT(ST_Vif_StopPort(u32VifChnId, u32VifPortId));
        STCHECKRESULT(ST_Vif_DisableDev(u32VifDevId));
        
        /************************************************
        Step6:  destory SENSOR
        *************************************************/
        STCHECKRESULT(MI_SNR_Disable(eSNRPad));
    }
	
	STCHECKRESULT(MI_VPE_DeInitDev());
	STCHECKRESULT(MI_VIF_DeInitDev());
	STCHECKRESULT(MI_SNR_DeInitDev());
	STCHECKRESULT(MI_DISP_DeInitDev());
	STCHECKRESULT(MI_PANEL_DeInitDev());
	
    /************************************************
    Step7:  destory SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Exit());

    return MI_SUCCESS;
}

