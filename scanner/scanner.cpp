/* Copyright (c) 2020-2021 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "ReadBarcode.h"
#include "TextUtfEncoding.h"

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>


#include "mi_common.h"
#include "mi_vif.h"
#include "mi_vpe.h"
#include "mi_venc.h"
#include "mi_sys.h"
#include "mi_disp.h"
#include "mi_divp.h"
#include "mi_panel.h"
#include "mi_sensor.h"
#include "mi_isp.h"
#include "isp_cus3a_if.h"

using namespace ZXing;


#define USE_SNR_PAD_ID_1  (1)
#define BARCODE_IN_WIDTH    640	//(1280)
#define BARCODE_IN_HIGH    	480	//	(720)

#define ST_MAX_VENC_NUM (16)

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)

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

#ifndef ExecFunc
#define ExecFunc(_func_, _ret_) \
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != _ret_) \
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
    } while(0)
#endif

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

struct ST_Panel_Attr_T
{
    MI_U32              u32Width;
    MI_U32              u32Height;
    MI_SNR_PAD_ID_e     eSnrPad;
    MI_VIF_DEV          s32vifDev;
};

typedef struct SSTAR_BindInfo_s
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindParam;
} ST_Sys_BindInfo_T;

static ST_VencAttr_t gstVencattr[ST_MAX_VENC_NUM];

static struct ST_Panel_Attr_T g_stPanelAttr[]=
{

#if USE_SNR_PAD_ID_1
    {
            .u32Width = 1024,
            .u32Height = 600,
            .eSnrPad   = E_MI_SNR_PAD_ID_0,
            .s32vifDev = 0,
    },
#else
    {
        .u32Width = 1024,
        .u32Height = 600,
        .eSnrPad   = E_MI_SNR_PAD_ID_2,
        .s32vifDev = 1,
    },
#endif
};

#define SENSOR_RGB_FPS 30


MI_S32 ST_Sys_Bind(ST_Sys_BindInfo_T *pstBindInfo)
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

MI_S32 ST_Sys_UnBind(ST_Sys_BindInfo_T *pstBindInfo)
{
    STCHECKRESULT(MI_SYS_UnBindChnPort(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort));

    return MI_SUCCESS;
}


void ST_Flush(void)
{
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

MI_U32 ST_Vpe_Divp_Bind()
{
    MI_VPE_PortMode_t stVpeMode;
	MI_DIVP_ChnAttr_t stDivpChnAttr;
	MI_DIVP_OutputPortAttr_t stDivpPortAttr;
	MI_SYS_ChnPort_t stChnOutputPort;
	ST_Sys_BindInfo_T stBindInfo;
	ST_Panel_Attr_T *pstPanelAttr = g_stPanelAttr;
	memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
	memset(&stVpeMode, 0, sizeof(MI_VPE_PortMode_t));
	memset(&stDivpPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
	memset(&stBindInfo, 0, sizeof(ST_Sys_BindInfo_T));
	memset(&stChnOutputPort, 0, sizeof(MI_SYS_ChnPort_t));
	ExecFunc(MI_VPE_GetPortMode(0, 3, &stVpeMode), MI_VPE_OK);

	// init divp channel
	stDivpChnAttr.bHorMirror = FALSE;
    stDivpChnAttr.bVerMirror = FALSE;
    stDivpChnAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X = 0;
    stDivpChnAttr.stCropRect.u16Y = 0;
    stDivpChnAttr.u32MaxWidth = stVpeMode.u16Width;
    stDivpChnAttr.u32MaxHeight = stVpeMode.u16Height;
	
	//Create divp channel 0 for zxing
    ExecFunc(MI_DIVP_CreateChn(0, &stDivpChnAttr), MI_SUCCESS);
    stDivpPortAttr.u32Width = BARCODE_IN_WIDTH; //stVpeMode.u16Width;
	stDivpPortAttr.u32Height = BARCODE_IN_HIGH; //stVpeMode.u16Height;
	stDivpPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
	stDivpPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_ARGB8888;
	ExecFunc(MI_DIVP_SetOutputPortAttr(0, &stDivpPortAttr), MI_SUCCESS);
	ExecFunc(MI_DIVP_StartChn(0), MI_SUCCESS);
	
	//Create divp channel 2 for disp
    ExecFunc(MI_DIVP_CreateChn(2, &stDivpChnAttr), MI_SUCCESS);
    stDivpPortAttr.u32Width = pstPanelAttr->u32Width; //stVpeMode.u16Width;
	stDivpPortAttr.u32Height = pstPanelAttr->u32Height; //stVpeMode.u16Height;
	stDivpPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
	stDivpPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	ExecFunc(MI_DIVP_SetOutputPortAttr(2, &stDivpPortAttr), MI_SUCCESS);
	ExecFunc(MI_DIVP_StartChn(2), MI_SUCCESS);

	//Bind vpe to divp channel 0 for zxing
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 0;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 2, 4), MI_SUCCESS);
	STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

	//Bind vpe to divp channel 2 for disp
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 2;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 2, 4), MI_SUCCESS);
	STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
	
	return MI_SUCCESS;
}

MI_U32 ST_Vpe_Divp_UnBind()
{
	ST_Sys_BindInfo_T stBindInfo;

	memset(&stBindInfo, 0, sizeof(ST_Sys_BindInfo_T));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 0;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 2;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));


	//deinit divp channel
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 0, 4), MI_SUCCESS);
	ExecFunc(MI_DIVP_StopChn(0), MI_SUCCESS);
    ExecFunc(MI_DIVP_DestroyChn(0), MI_SUCCESS);

	//deinit divp channel
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 0, 4), MI_SUCCESS);
	ExecFunc(MI_DIVP_StopChn(2), MI_SUCCESS);
    ExecFunc(MI_DIVP_DestroyChn(2), MI_SUCCESS);
	
	return MI_SUCCESS;
}

MI_U32 ST_PanelStart(void)
{
    ST_Panel_Attr_T *pstPanelAttr = g_stPanelAttr;

    MI_DISP_DEV DispDev = 0;
    MI_U32 u32DispWidth = pstPanelAttr->u32Width;
    MI_U32 u32DispHeight = pstPanelAttr->u32Height;

    MI_PANEL_IntfType_e eIntfType = E_MI_PNL_INTF_TTL;

    /************************************************
    Step1:  panel init
    *************************************************/
    STCHECKRESULT(MI_PANEL_Init(eIntfType));

    /************************************************
    Step2:  set disp pub
    *************************************************/
    MI_DISP_PubAttr_t stPubAttr;
    memset(&stPubAttr, 0x0, sizeof(MI_DISP_PubAttr_t));

    stPubAttr.u32BgColor = YUYV_BLACK;
    stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    stPubAttr.eIntfSync =  E_MI_DISP_OUTPUT_USER;
    STCHECKRESULT(MI_DISP_SetPubAttr(DispDev,  &stPubAttr));
    STCHECKRESULT(MI_DISP_Enable(DispDev));

    /************************************************
    Step3:  set layer
    *************************************************/
    MI_DISP_LAYER            DispLayer = 0;
    MI_SYS_PixelFormat_e     ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;  
    MI_DISP_VideoLayerAttr_t stLayerAttr;


    memset(&stLayerAttr, 0x0, sizeof(MI_DISP_VideoLayerAttr_t));

    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width = u32DispWidth;
    stLayerAttr.stVidLayerDispWin.u16Height = u32DispHeight;

    stLayerAttr.stVidLayerSize.u16Width = u32DispWidth;
    stLayerAttr.stVidLayerSize.u16Height = u32DispHeight;

    stLayerAttr.ePixFormat = ePixFormat;

    STCHECKRESULT(MI_DISP_BindVideoLayer(DispLayer,DispDev));
    STCHECKRESULT(MI_DISP_SetVideoLayerAttr(DispLayer, &stLayerAttr));
    STCHECKRESULT(MI_DISP_EnableVideoLayer(DispLayer));

    /************************************************
    Step4:  set inputport
    *************************************************/
    MI_U8 u8DispInport = 0;

    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;

    memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
    memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));

    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width =u32DispWidth;
    stInputPortAttr.stDispWin.u16Height = u32DispHeight;

    stInputPortAttr.u16SrcWidth =u32DispWidth;
    stInputPortAttr.u16SrcHeight = u32DispHeight;

    stWinRect.u16Width = u32DispWidth;
    stWinRect.u16Height = u32DispHeight;


    printf("%s:%d layer:%d port:%d srcwidth:%d srcheight:%d x:%d y:%d outwidth:%d outheight:%d\n",__FUNCTION__,__LINE__,
        DispLayer,u8DispInport,
        stInputPortAttr.u16SrcWidth,stInputPortAttr.u16SrcHeight,
        stInputPortAttr.stDispWin.u16X,stInputPortAttr.stDispWin.u16Y,
        stInputPortAttr.stDispWin.u16Width,stInputPortAttr.stDispWin.u16Height);

    STCHECKRESULT(MI_DISP_SetInputPortAttr(DispLayer, u8DispInport, &stInputPortAttr));
    STCHECKRESULT(MI_DISP_EnableInputPort(DispLayer, u8DispInport));
    STCHECKRESULT(MI_DISP_SetInputPortSyncMode(DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));

#if 1
	/************************************************
    Step5:  bind divp -> disp
    *************************************************/
    ST_Sys_BindInfo_T stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 2;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId =  0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;

    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

#else
    /************************************************
    Step5:  bind vpe->disp
    *************************************************/
    ST_Sys_BindInfo_T stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId =  0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;

    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
#endif

    return MI_SUCCESS;
}

MI_U32 ST_PanelStop(void)
{
    MI_U32 DispLayer = 0;
    MI_U32 DispInport = 0;
    MI_U32 DispDev = 0;

    ST_Sys_BindInfo_T stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 2;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    STCHECKRESULT(MI_DISP_DisableInputPort(DispLayer, DispInport));

    STCHECKRESULT(MI_DISP_DisableVideoLayer(DispLayer));
    STCHECKRESULT(MI_DISP_UnBindVideoLayer(DispLayer, DispDev));

    STCHECKRESULT(MI_DISP_Disable(DispLayer));
    STCHECKRESULT(MI_PANEL_DeInit());

    return 0;
}

MI_U32 ST_BaseModuleInit(void)
{
    ST_Panel_Attr_T *pstPanelAttr = g_stPanelAttr;

    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_SYS_PixelFormat_e ePixFormat;

    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_VPE_ChannelPara_t stChannelVpeParam;
    MI_SYS_WindowRect_t stVpeCropWin;
    MI_VPE_PortMode_t stVpeMode;
    MI_SYS_ChnPort_t stVpeChnPort;

    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;

    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_VPE_3DNR_Level_e en3dNrLevel = E_MI_VPE_3DNR_LEVEL_OFF;
    MI_SYS_Rotate_e enRotation = E_MI_SYS_ROTATE_NONE;

    MI_SNR_PAD_ID_e eSnrPad = pstPanelAttr->eSnrPad;
    MI_VIF_DEV s32vifDev = pstPanelAttr->s32vifDev;
    MI_VIF_CHN s32vifChn = s32vifDev*4;

    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    /************************************************
    Step1:  init SYS
    *************************************************/
    STCHECKRESULT(MI_SYS_Init());

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

    MI_SNR_SetRes(eSnrPad, 0);
    MI_SNR_Enable(eSnrPad);

    MI_SNR_GetPadInfo(eSnrPad, &stPad0Info);
    MI_SNR_GetPlaneInfo(eSnrPad, 0, &stSnrPlane0Info);
	MI_SNR_SetFps(eSnrPad,SENSOR_RGB_FPS);

    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

    /************************************************
    Step2:  init VIF(for IPC, only one dev)
    *************************************************/
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

    MI_VIF_ChnPortAttr_t stVifChnPortAttr;
    memset(&stVifChnPortAttr, 0, sizeof(MI_VIF_ChnPortAttr_t));
    stVifChnPortAttr.stCapRect.u16X = 0;
    stVifChnPortAttr.stCapRect.u16Y = 0;
    stVifChnPortAttr.stCapRect.u16Width = u32CapWidth;
    stVifChnPortAttr.stCapRect.u16Height = u32CapHeight;
    stVifChnPortAttr.stDestSize.u16Width = u32CapWidth;
    stVifChnPortAttr.stDestSize.u16Height = u32CapHeight;
    stVifChnPortAttr.eCapSel = E_MI_SYS_FIELDTYPE_BOTH;
    stVifChnPortAttr.eScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
    stVifChnPortAttr.ePixFormat = ePixFormat;
    stVifChnPortAttr.eFrameRate = eFrameRate;
    STCHECKRESULT(MI_VIF_SetChnPortAttr(s32vifChn, 0, &stVifChnPortAttr));

    MI_SYS_ChnPort_t stVifChnPort;
    memset(&stVifChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stVifChnPort.eModId = E_MI_MODULE_ID_VIF;
    stVifChnPort.u32DevId = 0;
    stVifChnPort.u32ChnId = s32vifChn;
    stVifChnPort.u32PortId = 0;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stVifChnPort, 0, 6));
    STCHECKRESULT(MI_VIF_EnableChnPort(s32vifChn, 0));

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

    // create vpe
    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelPara_t));
    memset(&stVpeCropWin, 0, sizeof(MI_SYS_WindowRect_t));
    memset(&stChannelVpeParam, 0, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeParam.e3DNRLevel = en3dNrLevel;
    stChannelVpeParam.eHDRType = eVpeHdrType;
    STCHECKRESULT(MI_VPE_SetChannelParam(0, &stChannelVpeParam));

	if(eSnrPad == E_MI_SNR_PAD_ID_0)
		stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR0;
	else if(eSnrPad == E_MI_SNR_PAD_ID_1)
		stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR1;
	else if(eSnrPad == E_MI_SNR_PAD_ID_2)
		stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR2;
	else if(eSnrPad == E_MI_SNR_PAD_ID_3)
		stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR3;
	else
		stChannelVpeAttr.eSensorBindId = E_MI_VPE_SENSOR_INVALID;

    stChannelVpeAttr.u16MaxW = (MI_U16)u32CapWidth;
    stChannelVpeAttr.u16MaxH = (MI_U16)u32CapHeight;
    stChannelVpeAttr.bNrEn= FALSE;
    stChannelVpeAttr.bEdgeEn= FALSE;
    stChannelVpeAttr.bEsEn= FALSE;
    stChannelVpeAttr.bContrastEn= FALSE;
    stChannelVpeAttr.bUvInvert= FALSE;
    stChannelVpeAttr.eRunningMode = E_MI_VPE_RUN_CAM_MODE;//E_MI_VPE_RUN_REALTIME_MODE;
    stChannelVpeAttr.ePixFmt = ePixFormat;
    stChannelVpeAttr.bRotation = FALSE;
    stChannelVpeAttr.eHDRType  = eVpeHdrType;
    stChannelVpeAttr.bEnLdc =  FALSE;
    stChannelVpeAttr.u32ChnPortMode = E_MI_VPE_ZOOM_LDC_NULL;   //0;
    printf("--------------vpe chnportMode is %d\n", stChannelVpeAttr.u32ChnPortMode);
    STCHECKRESULT(MI_VPE_CreateChannel(0, &stChannelVpeAttr));
    STCHECKRESULT(MI_VPE_StartChannel(0));

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = s32vifDev;
    stBindInfo.stSrcChnPort.u32ChnId = s32vifChn;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;//E_MI_SYS_BIND_TYPE_REALTIME;
    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    memset(&stVpeMode, 0, sizeof(stVpeMode));
    STCHECKRESULT(MI_VPE_GetPortMode(0, 0, &stVpeMode));

    stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stVpeMode.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpeMode.u16Width = 1280;
    stVpeMode.u16Height = 720;
    STCHECKRESULT(MI_VPE_SetPortMode(0, 0, &stVpeMode));

    memset(&stVpeChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stVpeChnPort.eModId = E_MI_MODULE_ID_VPE;
    stVpeChnPort.u32DevId = 0;
    stVpeChnPort.u32ChnId = 0;
    stVpeChnPort.u32PortId = 0;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stVpeChnPort, 0, 5));
    STCHECKRESULT(MI_VPE_EnablePort(0, 0));

    return MI_SUCCESS;
}


MI_U32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;

 #if USE_SNR_PAD_ID_1
    MI_U32 u32VifDev = 0;
    MI_U32 u32VifChn = 0;
 #else
    MI_U32 u32VifDev = 1;
    MI_U32 u32VifChn = 4;
#endif
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

    /************************************************
    Step1:  destory VPE
    *************************************************/
    STCHECKRESULT(MI_VPE_StopChannel(0));
    STCHECKRESULT(MI_VPE_DestroyChannel(0));

    /************************************************
    Step2:  destory VIF
    *************************************************/
    STCHECKRESULT(MI_VIF_DisableChnPort(u32VifChn, 0));
    STCHECKRESULT(MI_VIF_DisableDev(u32VifDev));

    /************************************************
    Step3:  destory SYS
    *************************************************/
    STCHECKRESULT(MI_SYS_Exit());

    return MI_SUCCESS;
}

#define DLA_PERSON_COLOR    RGB2PIXEL1555(255, 0, 0)
#define DLA_ANIMAL_COLOR    RGB2PIXEL1555(0, 255, 0)
#define DLA_MOBILE_COLOR    RGB2PIXEL1555(0, 0, 255)
#define DLA_OTHERS_COLOR    RGB2PIXEL1555(0, 0, 0)

static volatile int drawonce = 0;
MI_S32 ST_Do_Barcode(const MI_SYS_BufInfo_t *stBufInfo,MI_SYS_BUF_HANDLE hBufHandle)
{
	DecodeHints hints;
	const char *pOsdChar = NULL;
	char osdChar[2048] = {0};
	static char osdCharCmp[2048] = {0};
	std::string osdStr;
		
    if (stBufInfo->eBufType == E_MI_SYS_BUFDATA_RAW)
    {
        printf("BUFDATA TYEP: E_MI_SYS_BUFDATA_RAW\n");
    }
    else if (stBufInfo->eBufType == E_MI_SYS_BUFDATA_FRAME)
    {

		ImageView img = ImageView{(uint8_t *)(stBufInfo->stFrameData.pVirAddr[0]), BARCODE_IN_WIDTH, BARCODE_IN_HIGH, ImageFormat::BGRX};

		// dump buff
		static FILE *pFile = fopen("buf", "wr+");
		if (pFile)
		{
			fwrite((uint8_t *)(stBufInfo->stFrameData.pVirAddr[0]), BARCODE_IN_WIDTH*BARCODE_IN_HIGH*4, 1, pFile);
			fclose(pFile);
			pFile = NULL;
		}

		hints.setTryHarder(FALSE);
		auto result = ReadBarcode(img, hints);

		if(result.status() == DecodeStatus::NoError)
		{
			std::cout << "Text-2: 	\"" << TextUtfEncoding::ToUtf8(result.text()) << "\"\n"
					  << "Format:	" << ToString(result.format()) << "\n"
					  << "Rotation: " << result.orientation() << " deg\n";


	        osdStr = TextUtfEncoding::ToUtf8(result.text());
			osdStr.copy(osdChar, osdStr.length(), 0);
			printf("len:%d, DrawText:%s\n", osdStr.length(), osdChar);
			if(memcmp(osdChar,osdCharCmp, 1024) == 0)
			{
				return MI_SUCCESS;
			}
			memcpy(osdCharCmp, osdChar, 1024);
		}
    }

    return MI_SUCCESS;
}

static MI_BOOL gBarcode_Run = FALSE;
void *ST_Barcode_thread(void *argc)
{
    printf("detect thread start\n");
	sleep(2);
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
	
	stChnOutputPort.eModId      = E_MI_MODULE_ID_DIVP;
    stChnOutputPort.u32DevId    = 0;
    stChnOutputPort.u32ChnId    = 0;
    stChnOutputPort.u32PortId   = 0;
    s32Ret = MI_SYS_GetFd(&stChnOutputPort, &s32DivpFd);
	gBarcode_Run = TRUE;
    if (s32Ret < 0)
    {
        printf("divp ch0, get fd. err\n");
		gBarcode_Run = FALSE;
    }

    printf("====>[Line:%d] Start to get divp buf in thread\n", __LINE__);
    while(gBarcode_Run)
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
				ST_Do_Barcode(&stBufInfo,stBufHandle);
				MI_SYS_ChnOutputPortPutBuf(stBufHandle);
	        }
        }
   }

   return NULL;
}

int main(int argc, char **argv)
{
	void *status;		
	pthread_t     g_detectThread;
    STCHECKRESULT(ST_BaseModuleInit());
	ST_Vpe_Divp_Bind();
    STCHECKRESULT(ST_PanelStart());

	printf("StreamInit success\n");
	pthread_create(&g_detectThread, NULL, ST_Barcode_thread, NULL);
	pthread_setname_np(g_detectThread, "Detect_Task");
	
    for(;;)
    {
        char cmd = 0xff;
        printf("input 'q' exit\n");
        scanf("%c", &cmd);
        ST_Flush();
        if('q' == cmd || 'Q' == cmd)
        {
        	gBarcode_Run = FALSE;
        	pthread_join(g_detectThread, &status);
            break;
        }
    }

	ST_Vpe_Divp_UnBind();
    STCHECKRESULT(ST_PanelStop());
    STCHECKRESULT(ST_BaseModuleUnInit());

    return MI_SUCCESS;
}
