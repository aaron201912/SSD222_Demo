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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "libCodeSdk.h"
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <unistd.h>

#include "mi_disp.h"
#include "isp_cus3a_if.h"
#include "scanner.h"

#include <dlfcn.h>
#include "sstar_dynamic_load.h"



#define USE_SNR_PAD_ID_1  	(1)
#define SENSOR_RGB_FPS 		30

#define BARCODE_IN_WIDTH    (1280)
#define BARCODE_IN_HIGH    	(720)

#define DIVP_CHN_ZXING		0
#define DIVP_CHN_DISP		2
#define ZXING_SCAN_CODE		"QR_CODE"

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

typedef struct SSTAR_BindInfo_s
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindParam;
} SSTAR_Module_BindInfo_t;

typedef struct
{
	GetScanResultCallback pfnCallback;
} ScanUsrData_t;

static ScanUsrData_t *g_pstScanUsrData = NULL;
static bool g_bThreadExit = false;
static pthread_t g_scanThread = 0;

static SensorAssembly_t g_stSensorAssembly;
static VifAssembly_t g_stVifAssembly;
static VpeAssembly_t g_stVpeAssembly;
static DivpAssembly_t g_stDivpAssembly;
static int g_loadLibFaid = 1;

MI_S32 SSTAR_SYS_Bind(SSTAR_Module_BindInfo_t *pstBindInfo)
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

MI_S32 SSTAR_SYS_UnBind(SSTAR_Module_BindInfo_t *pstBindInfo)
{
    STCHECKRESULT(MI_SYS_UnBindChnPort(&pstBindInfo->stSrcChnPort, &pstBindInfo->stDstChnPort));

    return MI_SUCCESS;
}

MI_U32 SSTAR_VideoProcess_Bind(int dispWidth, int dispHeight)
{
    MI_VPE_PortMode_t stVpeMode;
	MI_DIVP_ChnAttr_t stDivpChnAttr;
	MI_DIVP_OutputPortAttr_t stDivpPortAttr;
	MI_SYS_ChnPort_t stChnOutputPort;
	SSTAR_Module_BindInfo_t stBindInfo;

	printf("SSTAR_VideoProcess_Bind start...\n");

	memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
	memset(&stVpeMode, 0, sizeof(MI_VPE_PortMode_t));
	memset(&stDivpPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
	memset(&stBindInfo, 0, sizeof(SSTAR_Module_BindInfo_t));
	memset(&stChnOutputPort, 0, sizeof(MI_SYS_ChnPort_t));
	ExecFunc(g_stVpeAssembly.pfnVpeGetPortMode(0, 3, &stVpeMode), MI_VPE_OK);

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
    printf("divp start to createChn...\n");
    ExecFunc(g_stDivpAssembly.pfnDivpCreateChn(DIVP_CHN_ZXING, &stDivpChnAttr), MI_SUCCESS);
    printf("divp createChn done...\n");

    stDivpPortAttr.u32Width = BARCODE_IN_WIDTH; //stVpeMode.u16Width;
	stDivpPortAttr.u32Height = BARCODE_IN_HIGH; //stVpeMode.u16Height;
	stDivpPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
	stDivpPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_ARGB8888;
	ExecFunc(g_stDivpAssembly.pfnDivpSetOutputPortAttr(DIVP_CHN_ZXING, &stDivpPortAttr), MI_SUCCESS);
	ExecFunc(g_stDivpAssembly.pfnDivpStartChn(DIVP_CHN_ZXING), MI_SUCCESS);
	
	//Create divp channel 2 for disp
	ExecFunc(g_stDivpAssembly.pfnDivpCreateChn(DIVP_CHN_DISP, &stDivpChnAttr), MI_SUCCESS);
    stDivpPortAttr.u32Width = dispWidth; 		// set to the width of disp area
	stDivpPortAttr.u32Height = dispHeight;		// set to the height of disp area
	stDivpPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
	stDivpPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	ExecFunc(g_stDivpAssembly.pfnDivpSetOutputPortAttr(DIVP_CHN_DISP, &stDivpPortAttr), MI_SUCCESS);
	ExecFunc(g_stDivpAssembly.pfnDivpStartChn(DIVP_CHN_DISP), MI_SUCCESS);
	
	//Bind vpe to divp channel 0 for zxing
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_ZXING;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 2, 4), MI_SUCCESS);
	STCHECKRESULT(SSTAR_SYS_Bind(&stBindInfo));

	//Bind vpe to divp channel 2 for disp
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_DISP;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 2, 4), MI_SUCCESS);
	STCHECKRESULT(SSTAR_SYS_Bind(&stBindInfo));
	
	printf("SSTAR_VideoProcess_Bind end...\n");

	return MI_SUCCESS;
}

MI_U32 SSTAR_VideoProcess_UnBind()
{
	SSTAR_Module_BindInfo_t stBindInfo;

	memset(&stBindInfo, 0, sizeof(SSTAR_Module_BindInfo_t));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_ZXING;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	STCHECKRESULT(SSTAR_SYS_UnBind(&stBindInfo));
	
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_DISP;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	STCHECKRESULT(SSTAR_SYS_UnBind(&stBindInfo));

	//deinit divp channel
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 0, 4), MI_SUCCESS);
    ExecFunc(g_stDivpAssembly.pfnDivpStopChn(DIVP_CHN_ZXING), MI_SUCCESS);
    ExecFunc(g_stDivpAssembly.pfnDivpDestroyChn(DIVP_CHN_ZXING), MI_SUCCESS);

	//deinit divp channel
	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stBindInfo.stDstChnPort, 0, 4), MI_SUCCESS);
    ExecFunc(g_stDivpAssembly.pfnDivpStopChn(DIVP_CHN_DISP), MI_SUCCESS);
    ExecFunc(g_stDivpAssembly.pfnDivpDestroyChn(DIVP_CHN_DISP), MI_SUCCESS);
	
	return MI_SUCCESS;
}

MI_U32 SSTAR_DispArea_Init(int x, int y, int dispWidth, int dispHeight)
{
    // set disp inputport attr
	MI_DISP_InputPortAttr_t stDispInputAttr;
	MI_DISP_GetInputPortAttr(0, 0, &stDispInputAttr);
	stDispInputAttr.stDispWin.u16X = x;
	stDispInputAttr.stDispWin.u16Y = y;
	stDispInputAttr.stDispWin.u16Width = dispWidth;					// set to the width of disp area
	stDispInputAttr.stDispWin.u16Height = dispHeight;				// set to the height of disp area
	stDispInputAttr.u16SrcWidth = dispWidth;						// set to the width of disp area
	stDispInputAttr.u16SrcHeight = dispHeight;						// set to the height of disp area
	MI_DISP_DisableInputPort(0, 0);
	MI_DISP_SetInputPortAttr(0, 0, &stDispInputAttr);
	MI_DISP_EnableInputPort(0, 0);
	MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

	// bind divp to disp
	SSTAR_Module_BindInfo_t stBindInfo;
	memset(&stBindInfo, 0x0, sizeof(SSTAR_Module_BindInfo_t));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
	stBindInfo.stSrcChnPort.u32DevId = 0;
	stBindInfo.stSrcChnPort.u32ChnId = DIVP_CHN_DISP;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
	stBindInfo.stDstChnPort.u32DevId =  0;
	stBindInfo.stDstChnPort.u32ChnId = 0;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
	stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;

	STCHECKRESULT(SSTAR_SYS_Bind(&stBindInfo));

    return MI_SUCCESS;
}

MI_U32 SSTAR_DispArea_Deinit(void)
{
    MI_U32 DispLayer = 0;
    MI_U32 DispInport = 0;
    MI_U32 DispDev = 0;

    SSTAR_Module_BindInfo_t stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(SSTAR_Module_BindInfo_t));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = DIVP_CHN_DISP;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    stBindInfo.u32SrcFrmrate = SENSOR_RGB_FPS;
    stBindInfo.u32DstFrmrate = SENSOR_RGB_FPS;
    STCHECKRESULT(SSTAR_SYS_UnBind(&stBindInfo));
    
    STCHECKRESULT(MI_DISP_DisableInputPort(DispLayer, DispInport));
    STCHECKRESULT(MI_DISP_DeInitDev());

    return 0;
}

MI_U32 SSTAR_VideoInput_Init(void)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_SYS_PixelFormat_e ePixFormat;
    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_VPE_ChannelPara_t stChannelVpeParam;
    MI_SYS_WindowRect_t stVpeCropWin;
    MI_VPE_PortMode_t stVpeMode;
    MI_SYS_ChnPort_t stVpeChnPort;
    
    SSTAR_Module_BindInfo_t stBindInfo;
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
    
    MI_SNR_PAD_ID_e eSnrPad = E_MI_SNR_PAD_ID_0;	//pstPanelAttr->eSnrPad;
    MI_VIF_DEV s32vifDev = 0;		//pstPanelAttr->s32vifDev;
    MI_VIF_CHN s32vifChn = s32vifDev*4;

    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    if(eVifHdrType > 0)
    	g_stSensorAssembly.pfnSnrSetPlaneMode(eSnrPad, TRUE);
    else
    	g_stSensorAssembly.pfnSnrSetPlaneMode(eSnrPad, FALSE);

    g_stSensorAssembly.pfnSnrQueryResCount(eSnrPad, &u32ResCount);
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
    	g_stSensorAssembly.pfnSnrGetRes(eSnrPad, u8ResIndex, &stRes);
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
				u8ResIndex,
				stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
				stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
				stRes.u32MaxFps,stRes.u32MinFps,
				stRes.strResDesc);
    }

    printf("eSNRPad[%d], cnt[%d]resolutions \n", eSnrPad, u32ResCount);
    STCHECKRESULT(g_stSensorAssembly.pfnSnrSetRes(eSnrPad, 0));
    STCHECKRESULT(g_stSensorAssembly.pfnSnrEnable(eSnrPad));
    STCHECKRESULT(g_stSensorAssembly.pfnSnrGetPadInfo(eSnrPad, &stPad0Info));
    STCHECKRESULT(g_stSensorAssembly.pfnSnrGetPlaneInfo(eSnrPad, 0, &stSnrPlane0Info));
    STCHECKRESULT(g_stSensorAssembly.pfnSnrSetFps(eSnrPad,SENSOR_RGB_FPS));

    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
    printf("ePixFormat is %d, ePixPrecision is %d, eBayerId is %d============================\n", (int)ePixFormat,
    		(int)stSnrPlane0Info.ePixPrecision, (int)stSnrPlane0Info.eBayerId);
    printf("sensor init done...\n");
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

    ExecFunc(g_stVifAssembly.pfnVifSetDevAttr(s32vifDev, &stDevAttr), MI_SUCCESS);
    ExecFunc(g_stVifAssembly.pfnVifEnableDev(s32vifDev), MI_SUCCESS);

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
    STCHECKRESULT(g_stVifAssembly.pfnVifSetChnPortAttr(s32vifChn, 0, &stVifChnPortAttr));

    MI_SYS_ChnPort_t stVifChnPort;
    memset(&stVifChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stVifChnPort.eModId = E_MI_MODULE_ID_VIF;
    stVifChnPort.u32DevId = 0;
    stVifChnPort.u32ChnId = s32vifChn;
    stVifChnPort.u32PortId = 0;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stVifChnPort, 0, 6));
    STCHECKRESULT(g_stVifAssembly.pfnVifEnableChnPort(s32vifChn, 0));
    
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

        g_stSensorAssembly.pfnSnrSetOrien(eSnrPad, bMirror, bFlip);
    }

    // create vpe
    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelPara_t));
    memset(&stVpeCropWin, 0, sizeof(MI_SYS_WindowRect_t));
    memset(&stChannelVpeParam, 0, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeParam.e3DNRLevel = en3dNrLevel;
    stChannelVpeParam.eHDRType = eVpeHdrType;
    STCHECKRESULT(g_stVpeAssembly.pfnVpeSetChannelParam(0, &stChannelVpeParam));

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

    STCHECKRESULT(g_stVpeAssembly.pfnVpeCreateChannel(0, &stChannelVpeAttr));
    STCHECKRESULT(g_stVpeAssembly.pfnVpeStartChannel(0));

    memset(&stBindInfo, 0x0, sizeof(SSTAR_Module_BindInfo_t));
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
    STCHECKRESULT(SSTAR_SYS_Bind(&stBindInfo));

    memset(&stVpeMode, 0, sizeof(stVpeMode));
    STCHECKRESULT(g_stVpeAssembly.pfnVpeGetPortMode(0, 0, &stVpeMode));

    stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stVpeMode.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpeMode.u16Width = 1280;
    stVpeMode.u16Height = 720;
    STCHECKRESULT(g_stVpeAssembly.pfnVpeSetPortMode(0, 0, &stVpeMode));

    memset(&stVpeChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stVpeChnPort.eModId = E_MI_MODULE_ID_VPE;
    stVpeChnPort.u32DevId = 0;
    stVpeChnPort.u32ChnId = 0;
    stVpeChnPort.u32PortId = 0;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stVpeChnPort, 0, 5));
    STCHECKRESULT(g_stVpeAssembly.pfnVpeEnablePort(0, 0));
    
    return MI_SUCCESS;
}


MI_U32 SSTAR_VideoInput_Deinit(void)
{
    SSTAR_Module_BindInfo_t stBindInfo;
    MI_U32 u32VifDev = 0;
    MI_U32 u32VifChn = 0;

    memset(&stBindInfo, 0x0, sizeof(SSTAR_Module_BindInfo_t));
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
    STCHECKRESULT(SSTAR_SYS_UnBind(&stBindInfo));

    STCHECKRESULT(g_stVpeAssembly.pfnVpeStopChannel(0));
    STCHECKRESULT(g_stVpeAssembly.pfnVpeDestroyChannel(0));

	STCHECKRESULT(g_stVifAssembly.pfnVifDisableChnPort(u32VifChn, 0));
	STCHECKRESULT(g_stVifAssembly.pfnVifDisableDev(u32VifDev));
	STCHECKRESULT(g_stSensorAssembly.pfnSnrDisable(E_MI_SNR_PAD_ID_0));
	STCHECKRESULT(g_stVpeAssembly.pfnVpeDeInitDev( ));
	STCHECKRESULT(g_stVifAssembly.pfnVifDeInitDev());
	STCHECKRESULT(g_stSensorAssembly.pfnSnrDeInitDev());

    return MI_SUCCESS;
}

static char scanResult[512] = {0};
static char scanResultCmp[512] = {0};

static volatile int drawonce = 0;
MI_S32  ST_Do_Barcode(const MI_SYS_BufInfo_t *stBufInfo,MI_SYS_BUF_HANDLE hBufHandle, void* pData)
{
	ScanUsrData_t *pScanUsrData = (ScanUsrData_t*)pData;

    if (stBufInfo->eBufType == E_MI_SYS_BUFDATA_RAW)
    {
        printf("BUFDATA TYEP: E_MI_SYS_BUFDATA_RAW\n");
    }
    else if (stBufInfo->eBufType == E_MI_SYS_BUFDATA_FRAME)
    {
		// dump buff
		static FILE *pFile = fopen("buf", "wr+");
		if (pFile)
		{
			fwrite((uint8_t *)(stBufInfo->stFrameData.pVirAddr[0]), BARCODE_IN_WIDTH*BARCODE_IN_HIGH*4, 1, pFile);
			fclose(pFile);
			pFile = NULL;
		}

//		printf("beigin to read barcode ...................\n");

		int ret = CodeDeCode::ImageDecode((const char*)(stBufInfo->stFrameData.pVirAddr[0]), BARCODE_IN_WIDTH, BARCODE_IN_HIGH,
											ImageFormats::BGRX, /*(char*)ZXING_SCAN_CODE*/"", scanResult, false, false, false);
//		printf("read barcode done, result: %s\n", scanResult);

		if (!ret)
		{
			if(memcmp(scanResult, scanResultCmp, 512) == 0)
			{
				return MI_SUCCESS;
			}

			memcpy(scanResultCmp, scanResult, 512);
			//printf("begin to exec callback\n");
			if (pScanUsrData && pScanUsrData->pfnCallback)
				pScanUsrData->pfnCallback(scanResult);
			//printf("exec callback done\n");
		}
    }

    return MI_SUCCESS;
}

void *ST_Barcode_thread(void *pData)
{
    printf("scan thread start\n");
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
	ScanUsrData_t *pScanUsrData = (ScanUsrData_t*)pData;
	
	stChnOutputPort.eModId      = E_MI_MODULE_ID_DIVP;
    stChnOutputPort.u32DevId    = 0;
    stChnOutputPort.u32ChnId    = 0;
    stChnOutputPort.u32PortId   = 0;
    s32Ret = MI_SYS_GetFd(&stChnOutputPort, &s32DivpFd);

    if (s32Ret < 0)
    {
        printf("divp ch0, get fd. err\n");
        g_bThreadExit = true;
    }

    printf("====>[Line:%d] Start to get divp buf in thread\n", __LINE__);
    while(!g_bThreadExit)
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

//	            printf("bufInfo: type:%d, w=%d, h=%d, virAddr=%p\n", stBufInfo.eBufType, stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height,
//	            		stBufInfo.stFrameData.pVirAddr[0]);

				ST_Do_Barcode(&stBufInfo, stBufHandle, (void*)pScanUsrData);
				MI_SYS_ChnOutputPortPutBuf(stBufHandle);
	        }
        }
   }

   return NULL;
}

int SSTAR_Scanner_OpenLibrary()
{
	memset(&g_stSensorAssembly, 0, sizeof(SensorAssembly_t));
	memset(&g_stVifAssembly, 0, sizeof(VifAssembly_t));
	memset(&g_stVpeAssembly, 0, sizeof(VpeAssembly_t));
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

	if (SSTAR_DIVP_OpenLibrary(&g_stDivpAssembly))
	{
		printf("open libmi_divp failed\n");
		goto close_vpe_lib;
	}

	return 0;

close_vpe_lib:
	SSTAR_VPE_CloseLibrary(&g_stVpeAssembly);

close_vif_lib:
	SSTAR_VIF_CloseLibrary(&g_stVifAssembly);

close_snr_lib:
	SSTAR_SNR_CloseLibrary(&g_stSensorAssembly);

exit:
	return -1;
}

void SSTAR_Scanner_CloseLibrary()
{
	SSTAR_DIVP_CloseLibrary(&g_stDivpAssembly);
	SSTAR_VPE_CloseLibrary(&g_stVpeAssembly);
	SSTAR_VIF_CloseLibrary(&g_stVifAssembly);
	SSTAR_SNR_CloseLibrary(&g_stSensorAssembly);
}

int SSTAR_Scanner_Init(int x, int y, int dispWidth, int dispHeight, GetScanResultCallback pfnGetScanResultCallback)
{
	if (g_loadLibFaid = SSTAR_Scanner_OpenLibrary())
		return -1;

	// init scanner
	STCHECKRESULT(SSTAR_VideoInput_Init());
	STCHECKRESULT(SSTAR_VideoProcess_Bind(dispWidth, dispHeight));
	STCHECKRESULT(SSTAR_DispArea_Init(x, y, dispWidth, dispHeight));

	g_pstScanUsrData = (ScanUsrData_t*)malloc(sizeof(ScanUsrData_t));
	g_pstScanUsrData->pfnCallback = pfnGetScanResultCallback;

	// create scan thread
	printf("create scan thread\n");
	g_bThreadExit = false;
	pthread_create(&g_scanThread, NULL, ST_Barcode_thread, (void*)g_pstScanUsrData);
	pthread_setname_np(g_scanThread, "Detect_Task");

	return 0;
}

void SSTAR_Scanner_Deinit()
{
	if (g_loadLibFaid)
		goto unload_lib;

	g_bThreadExit = true;
	if (g_scanThread)
	{
		pthread_join(g_scanThread, NULL);
		g_scanThread = 0;
	}

	if (g_pstScanUsrData)
	{
		free(g_pstScanUsrData);
		g_pstScanUsrData = NULL;
	}

unload_lib:
	SSTAR_VideoProcess_UnBind();
	SSTAR_DispArea_Deinit();
	SSTAR_VideoInput_Deinit();
}

