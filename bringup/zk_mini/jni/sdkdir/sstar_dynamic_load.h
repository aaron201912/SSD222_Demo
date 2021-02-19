/*
 * sstar_dynamic_load.h
 *
 *  Created on: 2021年2月8日
 *      Author: koda.xu
 */

#ifndef JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_
#define JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_

#include "mi_sys.h"
#include "mi_ai.h"
#include "mi_ao.h"
#include "mi_rgn.h"
#include "mi_ive.h"
#include "mi_sensor.h"
#include "mi_vif.h"
#include "mi_vpe.h"
#include "mi_divp.h"
#include "mi_isp.h"
#include "mi_iqserver.h"
#include "mi_vdisp.h"

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnAiSetPubAttr)(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAiGetPubAttr)(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t*pstAttr);
	MI_S32 (*pfnAiEnable)(MI_AUDIO_DEV AiDevId);
	MI_S32 (*pfnAiDisable)(MI_AUDIO_DEV AiDevId);
	MI_S32 (*pfnAiEnableChn)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 (*pfnAiDisableChn)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 (*pfnAiGetFrame)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t*pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm , MI_S32 s32MilliSec);
	MI_S32 (*pfnAiReleaseFrame)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t *pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm);
	MI_S32 (*pfnAiSetChnParam)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAiGetChnParam)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAiSetMute)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL bMute);
	MI_S32 (*pfnAiGetMute)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL *pbMute);
	MI_S32 (*pfnAiInitDev)(MI_AI_InitParam_t *pstInitParam);
	MI_S32 (*pfnAiDeInitDev)(void);

#if 0
	MI_S32 MI_AI_SetPubAttr(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 MI_AI_GetPubAttr(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t*pstAttr);
	MI_S32 MI_AI_Enable(MI_AUDIO_DEV AiDevId);
	MI_S32 MI_AI_Disable(MI_AUDIO_DEV AiDevId);
	MI_S32 MI_AI_EnableChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 MI_AI_DisableChn(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 MI_AI_GetFrame(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t*pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm , MI_S32 s32MilliSec);
	MI_S32 MI_AI_ReleaseFrame(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t *pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm);
	MI_S32 MI_AI_SetChnParam(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 MI_AI_GetChnParam(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 MI_AI_SetMute(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL bMute);
	MI_S32 MI_AI_GetMute(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL *pbMute);
	MI_S32 MI_AI_InitDev(MI_AI_InitParam_t *pstInitParam);
	MI_S32 MI_AI_DeInitDev(void);
#endif
} AudioInAssembly_t;


typedef struct
{
	void *pHandle;
	MI_S32 (*pfnAoSetPubAttr)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAoGetPubAttr)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAoEnable)(MI_AUDIO_DEV AoDevId);
	MI_S32 (*pfnAoDisable)(MI_AUDIO_DEV AoDevId);
	MI_S32 (*pfnAoEnableChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoDisableChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoSendFrame)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_Frame_t *pstData, MI_S32 s32MilliSec);
	MI_S32 (*pfnAoPauseChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoResumeChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoSetVolume)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 s32VolumeDb, MI_AO_GainFading_e eFading);
	MI_S32 (*pfnAoGetVolume)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 *ps32VolumeDb);
	MI_S32 (*pfnAoSetMute)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL bEnable);
	MI_S32 (*pfnAoGetMute)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL *pbEnable);
	MI_S32 (*pfnAoSetChnParam)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAoGetChnParam)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAoSetSrcGain)(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb);
	MI_S32 (*pfnAoInitDev)(MI_AO_InitParam_t *pstInitParam);
	MI_S32 (*pfnAoDeInitDev)(void);

#if 0
	MI_S32 MI_AO_SetPubAttr(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 MI_AO_GetPubAttr(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 MI_AO_Enable(MI_AUDIO_DEV AoDevId);
	MI_S32 MI_AO_Disable(MI_AUDIO_DEV AoDevId);
	MI_S32 MI_AO_EnableChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 MI_AO_DisableChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 MI_AO_SendFrame(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_Frame_t *pstData, MI_S32 s32MilliSec);
	MI_S32 MI_AO_PauseChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 MI_AO_ResumeChn(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 MI_AO_SetVolume(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 s32VolumeDb, MI_AO_GainFading_e eFading);
	MI_S32 MI_AO_GetVolume(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 *ps32VolumeDb);
	MI_S32 MI_AO_SetMute(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL bEnable);
	MI_S32 MI_AO_GetMute(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL *pbEnable);
	MI_S32 MI_AO_SetChnParam(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 MI_AO_GetChnParam(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 MI_AO_SetSrcGain(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb);
	MI_S32 MI_AO_InitDev(MI_AO_InitParam_t *pstInitParam);
	MI_S32 MI_AO_DeInitDev(void);
#endif
} AudioOutAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnRgnInit)(MI_RGN_PaletteTable_t *pstPaletteTable);
	MI_S32 (*pfnRgnDeInit)(void);
	MI_S32 (*pfnRgnCreate)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 (*pfnRgnDestroy) (MI_RGN_HANDLE hHandle);
	MI_S32 (*pfnRgnGetAttr)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 (*pfnRgnSetBitMap)(MI_RGN_HANDLE hHandle, MI_RGN_Bitmap_t *pstBitmap);
	MI_S32 (*pfnRgnAttachToChn)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t* pstChnPort, MI_RGN_ChnPortParam_t *pstChnAttr);
	MI_S32 (*pfnRgnDetachFromChn)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort);
	MI_S32 (*pfnRgnSetDisplayAttr)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 (*pfnRgnGetDisplayAttr)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 (*pfnRgnGetCanvasInfo)(MI_RGN_HANDLE hHandle, MI_RGN_CanvasInfo_t* pstCanvasInfo);
	MI_S32 (*pfnRgnUpdateCanvas)(MI_RGN_HANDLE hHandle);
	MI_S32 (*pfnRgnScaleRect)(MI_RGN_ChnPort_t *pstChnPort, MI_RGN_Size_t *pstCanvasSize, MI_RGN_Size_t *pstScreenSize);
	MI_S32 (*pfnRgnInitDev)(MI_RGN_InitParam_t *pstInitParam);
	MI_S32 (*pfnRgnDeInitDev)(void);

#if 0
	MI_S32 MI_RGN_Init(MI_RGN_PaletteTable_t *pstPaletteTable);
	MI_S32 MI_RGN_DeInit(void);
	MI_S32 MI_RGN_Create(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 MI_RGN_Destroy (MI_RGN_HANDLE hHandle);
	MI_S32 MI_RGN_GetAttr(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 MI_RGN_SetBitMap(MI_RGN_HANDLE hHandle, MI_RGN_Bitmap_t *pstBitmap);
	MI_S32 MI_RGN_AttachToChn(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t* pstChnPort, MI_RGN_ChnPortParam_t *pstChnAttr);
	MI_S32 MI_RGN_DetachFromChn(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort);
	MI_S32 MI_RGN_SetDisplayAttr(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 MI_RGN_GetDisplayAttr(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 MI_RGN_GetCanvasInfo(MI_RGN_HANDLE hHandle, MI_RGN_CanvasInfo_t* pstCanvasInfo);
	MI_S32 MI_RGN_UpdateCanvas(MI_RGN_HANDLE hHandle);
	MI_S32 MI_RGN_ScaleRect(MI_RGN_ChnPort_t *pstChnPort, MI_RGN_Size_t *pstCanvasSize, MI_RGN_Size_t *pstScreenSize);
	MI_S32 MI_RGN_InitDev(MI_RGN_InitParam_t *pstInitParam);
	MI_S32 MI_RGN_DeInitDev(void);
#endif
} RgnAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIveCreate)(MI_IVE_HANDLE hHandle);
	MI_S32 (*pfnIveDestroy)(MI_IVE_HANDLE hHandle);
	MI_S32 (*pfnIveCsc)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstDst, MI_IVE_CscCtrl_t *pstCscCtrl, MI_BOOL bInstant);
	MI_S32 (*pfnIveSad)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstSad, MI_IVE_DstImage_t *pstThr,
					  MI_IVE_SadCtrl_t *pstSadCtrl, MI_BOOL bInstant);

#if 0
	MI_S32 MI_IVE_Create(MI_IVE_HANDLE hHandle);
	MI_S32 MI_IVE_Destroy(MI_IVE_HANDLE hHandle);
	MI_S32 MI_IVE_Csc(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstDst, MI_IVE_CscCtrl_t *pstCscCtrl, MI_BOOL bInstant);
	MI_S32 MI_IVE_Sad(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstSad, MI_IVE_DstImage_t *pstThr,
					  MI_IVE_SadCtrl_t *pstSadCtrl, MI_BOOL bInstant);
#endif
} IveAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnSnrEnable)(MI_SNR_PAD_ID_e ePADId);
	MI_S32 (*pfnSnrDisable)(MI_SNR_PAD_ID_e ePADId);  // Stop the sensor operation, streaming, mclk
	MI_S32 (*pfnSnrGetPadInfo)(MI_SNR_PAD_ID_e ePADId, MI_SNR_PADInfo_t  *pstPadInfo);
	MI_S32 (*pfnSnrGetPlaneInfo)(MI_SNR_PAD_ID_e ePADId, MI_U32  u32PlaneID, MI_SNR_PlaneInfo_t *pstPlaneInfo);
	MI_S32 (*pfnSnrGetFps)(MI_SNR_PAD_ID_e ePADId, MI_U32 *pFps);
	MI_S32 (*pfnSnrSetFps)(MI_SNR_PAD_ID_e ePADId, MI_U32  u32Fps);
	MI_S32 (*pfnSnrGetBT656SrcType)(MI_SNR_PAD_ID_e ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcType_e *psttype);
	MI_S32 (*pfnSnrQueryResCount)(MI_SNR_PAD_ID_e ePADId, MI_U32 *pu32ResCount);
	MI_S32 (*pfnSnrGetRes)(MI_SNR_PAD_ID_e ePADId, MI_U8 u8ResIdx, MI_SNR_Res_t *pstRes);
	MI_S32 (*pfnSnrGetCurRes)(MI_SNR_PAD_ID_e ePADId, MI_U8 *pu8CurResIdx, MI_SNR_Res_t  *pstCurRes);
	MI_S32 (*pfnSnrSetRes)(MI_SNR_PAD_ID_e ePADId, MI_U8 u8ResIdx);
	MI_S32 (*pfnSnrSetOrien)(MI_SNR_PAD_ID_e ePADId, MI_BOOL bMirror, MI_BOOL bFlip);
	MI_S32 (*pfnSnrGetOrien)(MI_SNR_PAD_ID_e ePADId, MI_BOOL *pbMirror, MI_BOOL *pbFlip);
	MI_S32 (*pfnSnrSetPlaneMode)(MI_SNR_PAD_ID_e ePADId, MI_BOOL bEnable);
	MI_S32 (*pfnSnrGetPlaneMode)(MI_SNR_PAD_ID_e ePADId, MI_BOOL *pbEnable);
	MI_S32 (*pfnSnrCustFunction)(MI_SNR_PAD_ID_e ePADId, MI_U32 u32CmdId, MI_U32 u32DataSize, void *pCustData, MI_SNR_CUST_DIR_e eDir);
	MI_S32 (*pfnSnrInitDev)(MI_SNR_InitParam_t *pstInitParam);
	MI_S32 (*pfnSnrDeInitDev)(void);

#if 0
	MI_S32 MI_SNR_Enable(MI_SNR_PAD_ID_e     ePADId);
	MI_S32 MI_SNR_Disable(MI_SNR_PAD_ID_e      ePADId);  // Stop the sensor operation, streaming, mclk
	MI_S32 MI_SNR_GetPadInfo(MI_SNR_PAD_ID_e       ePADId, MI_SNR_PADInfo_t  *pstPadInfo);
	MI_S32 MI_SNR_GetPlaneInfo(MI_SNR_PAD_ID_e       ePADId, MI_U32  u32PlaneID, MI_SNR_PlaneInfo_t *pstPlaneInfo);
	MI_S32 MI_SNR_GetFps(MI_SNR_PAD_ID_e      ePADId, MI_U32 *pFps);
	MI_S32 MI_SNR_SetFps(MI_SNR_PAD_ID_e      ePADId, MI_U32  u32Fps);
	MI_S32 MI_SNR_GetBT656SrcType(MI_SNR_PAD_ID_e        ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcType_e *psttype);
	MI_S32 MI_SNR_QueryResCount(MI_SNR_PAD_ID_e        ePADId, MI_U32 *pu32ResCount);
	MI_S32 MI_SNR_GetRes(MI_SNR_PAD_ID_e      ePADId, MI_U8 u8ResIdx, MI_SNR_Res_t *pstRes);
	MI_S32 MI_SNR_GetCurRes(MI_SNR_PAD_ID_e       ePADId, MI_U8 *pu8CurResIdx, MI_SNR_Res_t  *pstCurRes);
	MI_S32 MI_SNR_SetRes(MI_SNR_PAD_ID_e      ePADId, MI_U8 u8ResIdx);
	MI_S32 MI_SNR_SetOrien(MI_SNR_PAD_ID_e      ePADId, MI_BOOL bMirror, MI_BOOL bFlip);
	MI_S32 MI_SNR_GetOrien(MI_SNR_PAD_ID_e      ePADId, MI_BOOL *pbMirror, MI_BOOL *pbFlip);
	MI_S32 MI_SNR_SetPlaneMode(MI_SNR_PAD_ID_e      ePADId, MI_BOOL bEnable);
	MI_S32 MI_SNR_GetPlaneMode(MI_SNR_PAD_ID_e      ePADId, MI_BOOL *pbEnable);
	MI_S32 MI_SNR_CustFunction(MI_SNR_PAD_ID_e ePADId, MI_U32 u32CmdId, MI_U32 u32DataSize, void *pCustData, MI_SNR_CUST_DIR_e eDir);
	MI_S32 MI_SNR_InitDev(MI_SNR_InitParam_t *pstInitParam);
	MI_S32 MI_SNR_DeInitDev(void);
#endif
} SensorAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVifSetDevAttr)(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 (*pfnVifGetDevAttr)(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 (*pfnVifEnableDev)(MI_VIF_DEV u32VifDev);
	MI_S32 (*pfnVifDisableDev)(MI_VIF_DEV u32VifDev);
	MI_S32 (*pfnVifSetChnPortAttr)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr);
	MI_S32 (*pfnVifGetChnPortAttr)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr);
	MI_S32 (*pfnVifEnableChnPort)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort);
	MI_S32 (*pfnVifDisableChnPort)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort);
	MI_S32 (*pfnVifQuery)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortStat_t *pstStat);
	MI_S32 (*pfnVifSetDev2SnrPadMux)(MI_VIF_Dev2SnrPadMuxCfg_t *pstVifDevMap, MI_U8 u8Length);
	MI_S32 (*pfnVifInitDev)(MI_VIF_InitParam_t *pstInitParam);
	MI_S32 (*pfnVifDeInitDev)(void);

#if 0
	MI_S32 MI_VIF_SetDevAttr(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 MI_VIF_GetDevAttr(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 MI_VIF_EnableDev(MI_VIF_DEV u32VifDev);
	MI_S32 MI_VIF_DisableDev(MI_VIF_DEV u32VifDev);
	MI_S32 MI_VIF_SetChnPortAttr(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr);
	MI_S32 MI_VIF_GetChnPortAttr(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr);
	MI_S32 MI_VIF_EnableChnPort(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort);
	MI_S32 MI_VIF_DisableChnPort(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort);
	MI_S32 MI_VIF_Query(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortStat_t *pstStat);
	MI_S32 MI_VIF_SetDev2SnrPadMux(MI_VIF_Dev2SnrPadMuxCfg_t *pstVifDevMap, MI_U8 u8Length);
	MI_S32 MI_VIF_InitDev(MI_VIF_InitParam_t *pstInitParam);
	MI_S32 MI_VIF_DeInitDev(void);
#endif
} VifAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVpeCreateChannel)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeDestroyChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeGetChannelAttr)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeSetChannelAttr)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeStartChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeStopChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeSetChannelParam)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 (*pfnVpeGetChannelParam)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 (*pfnVpeSetChannelCrop)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 (*pfnVpeGetChannelCrop)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 (*pfnVpeGetChannelRegionLuma)(MI_VPE_CHANNEL VpeCh, MI_VPE_RegionInfo_t *pstRegionInfo, MI_U32 *pu32LumaData,MI_S32 s32MilliSec);
	MI_S32 (*pfnVpeSetChannelRotation)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e eType);
	MI_S32 (*pfnVpeGetChannelRotation)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e *pType);
	MI_S32 (*pfnVpeEnablePort)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 (*pfnVpeDisablePort)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 (*pfnVpeSetPortMode)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 (*pfnVpeGetPortMode)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 (*pfnVpeSetPortCrop)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 (*pfnVpeGetPortCrop)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 (*pfnVpeSetPortShowPosition)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 (*pfnVpeGetPortShowPosition)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 (*pfnVpeSkipFrame)(MI_VPE_CHANNEL VpeCh, MI_U32 u32FrameNum);
	MI_S32 (*pfnVpeLDCBegViewConfig)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeLDCEndViewConfig)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeLDCSetViewConfig)(MI_VPE_CHANNEL VpeCh, void *pConfigAddr, MI_U32 u32ConfigSize);
	MI_S32 (*pfnVpeAlloc_IspDataBuf)(MI_U32 u32Size,void **pUserVirAddr);
	MI_S32 (*pfnVpeFree_IspDataBuf)(void *pUserBuf);
	MI_S32 (*pfnVpeCreateMultiChannelStitch)(MI_VPE_CHANNEL VpeCh, MI_VPE_MultiChannelStitchAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeInitDev)(MI_VPE_InitParam_t *pstInitParam);
	MI_S32 (*pfnVpeDeInitDev)(void);

#if 0
	MI_S32 MI_VPE_CreateChannel(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 MI_VPE_DestroyChannel(MI_VPE_CHANNEL VpeCh);
	MI_S32 MI_VPE_GetChannelAttr(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 MI_VPE_SetChannelAttr(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 MI_VPE_StartChannel(MI_VPE_CHANNEL VpeCh);
	MI_S32 MI_VPE_StopChannel(MI_VPE_CHANNEL VpeCh);
	MI_S32 MI_VPE_SetChannelParam(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 MI_VPE_GetChannelParam(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 MI_VPE_SetChannelCrop(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 MI_VPE_GetChannelCrop(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 MI_VPE_GetChannelRegionLuma(MI_VPE_CHANNEL VpeCh, MI_VPE_RegionInfo_t *pstRegionInfo, MI_U32 *pu32LumaData,MI_S32 s32MilliSec);
	MI_S32 MI_VPE_SetChannelRotation(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e eType);
	MI_S32 MI_VPE_GetChannelRotation(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e *pType);
	MI_S32 MI_VPE_EnablePort(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 MI_VPE_DisablePort(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 MI_VPE_SetPortMode(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 MI_VPE_GetPortMode(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 MI_VPE_SetPortCrop(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 MI_VPE_GetPortCrop(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 MI_VPE_SetPortShowPosition(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 MI_VPE_GetPortShowPosition(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 MI_VPE_SkipFrame(MI_VPE_CHANNEL VpeCh, MI_U32 u32FrameNum);
	MI_S32 MI_VPE_LDCBegViewConfig(MI_VPE_CHANNEL VpeCh);
	MI_S32 MI_VPE_LDCEndViewConfig(MI_VPE_CHANNEL VpeCh);
	MI_S32 MI_VPE_LDCSetViewConfig(MI_VPE_CHANNEL VpeCh, void *pConfigAddr, MI_U32 u32ConfigSize);
	MI_S32 MI_VPE_Alloc_IspDataBuf(MI_U32 u32Size,void **pUserVirAddr);
	MI_S32 MI_VPE_Free_IspDataBuf(void *pUserBuf);
	MI_S32 MI_VPE_CreateMultiChannelStitch(MI_VPE_CHANNEL VpeCh, MI_VPE_MultiChannelStitchAttr_t *pstVpeChAttr);
	MI_S32 MI_VPE_InitDev(MI_VPE_InitParam_t *pstInitParam);
	MI_S32 MI_VPE_DeInitDev(void);
#endif
} VpeAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnDivpCreateChn)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 (*pfnDivpDestroyChn)(MI_DIVP_CHN DivpChn);
	MI_S32 (*pfnDivpSetChnAttr)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 (*pfnDivpGetChnAttr)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 (*pfnDivpStartChn)(MI_DIVP_CHN DivpChn);
	MI_S32 (*pfnDivpStopChn)(MI_DIVP_CHN DivpChn);
	MI_S32 (*pfnDivpSetOutputPortAttr)(MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);
	MI_S32 (*pfnDivpGetOutputPortAttr)(MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);
	MI_S32 (*pfnDivpRefreshChn)(MI_DIVP_CHN DivpChn);
	MI_S32 (*pfnDivpStretchBuf)(MI_DIVP_DirectBuf_t *pstSrcBuf, MI_SYS_WindowRect_t *pstSrcCrop, MI_DIVP_DirectBuf_t *pstDstBuf);
	MI_S32 (*pfnDivpInitDev)(MI_DIVP_InitParam_t *pstInitParam);
	MI_S32 (*pfnDivpDeInitDev)(void);

#if 0
	MI_S32 MI_DIVP_CreateChn (MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 MI_DIVP_DestroyChn(MI_DIVP_CHN DivpChn);
	MI_S32 MI_DIVP_SetChnAttr(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 MI_DIVP_GetChnAttr(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr);
	MI_S32 MI_DIVP_StartChn(MI_DIVP_CHN DivpChn);
	MI_S32 MI_DIVP_StopChn(MI_DIVP_CHN DivpChn);
	MI_S32 MI_DIVP_SetOutputPortAttr (MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);
	MI_S32 MI_DIVP_GetOutputPortAttr (MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr);
	MI_S32 MI_DIVP_RefreshChn(MI_DIVP_CHN DivpChn);
	MI_S32 MI_DIVP_StretchBuf(MI_DIVP_DirectBuf_t *pstSrcBuf, MI_SYS_WindowRect_t *pstSrcCrop, MI_DIVP_DirectBuf_t *pstDstBuf);
	MI_S32 MI_DIVP_InitDev(MI_DIVP_InitParam_t *pstInitParam);
	MI_S32 MI_DIVP_DeInitDev(void);
#endif
} DivpAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIspApiCmdLoadBinFile)(MI_U32 Channel, char* filepath, MI_U32 user_key);
	MI_S32 (*pfnIspIqGetParaInitStatus)(MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data);

#if 0
	MI_S32 MI_ISP_API_CmdLoadBinFile(MI_U32 Channel, char* filepath, MI_U32 user_key);				// libmi_isp.so	依赖 libcus3a.so
	MI_S32 MI_ISP_IQ_GetParaInitStatus(MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data);
#endif
} IspAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIqServerOpen)(MI_U16 width, MI_U16 height, MI_S32 vpeChn);
	MI_S32 (*pfnIqServerSetDataPath)(char* path);
	MI_S32 (*pfnIqServerSetCustFunc)(MI_S32(* func)(MI_U16 data_type, MI_U32 length, MI_U8 * data));
	MI_S32 (*pfnIqServerClose)();

#if 0
	MI_S32 MI_IQSERVER_Open(MI_U16 width, MI_U16 height, MI_S32 vpeChn);				// libmi_iqserver.so
	MI_S32 MI_IQSERVER_SetDataPath(char* path);
	MI_S32 MI_IQSERVER_SetCustFunc(MI_S32(* func)(MI_U16 data_type, MI_U32 length, MI_U8 * data));
	MI_S32 MI_IQSERVER_Close();
#endif
} IqServerAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVdispInit)(void);
	MI_S32 (*pfnVdispExit)(void);
	MI_S32 (*pfnVdispOpenDevice)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispCloseDevice)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispSetOutputPortAttr)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdispGetOutputPortAttr)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdispSetInputChannelAttr)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 (*pfnVdispGetInputChannelAttr)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 (*pfnVdispEnableInputChannel)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 (*pfnVdispDisableInputChannel)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 (*pfnVdispStartDev)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispStopDev)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispInitDev)(MI_VDISP_InitParam_t *pstInitParam);
	MI_S32 (*pfnVdispDeInitDev)(void);

#if 0
	MI_S32 MI_VDISP_Init(void);
	MI_S32 MI_VDISP_Exit(void);
	MI_S32 MI_VDISP_OpenDevice(MI_VDISP_DEV DevId);
	MI_S32 MI_VDISP_CloseDevice(MI_VDISP_DEV DevId);
	MI_S32 MI_VDISP_SetOutputPortAttr(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 MI_VDISP_GetOutputPortAttr(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 MI_VDISP_SetInputChannelAttr(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 MI_VDISP_GetInputChannelAttr(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 MI_VDISP_EnableInputChannel(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 MI_VDISP_DisableInputChannel(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 MI_VDISP_StartDev(MI_VDISP_DEV DevId);
	MI_S32 MI_VDISP_StopDev(MI_VDISP_DEV DevId);
	MI_S32 MI_VDISP_InitDev(MI_VDISP_InitParam_t *pstInitParam);
	MI_S32 MI_VDISP_DeInitDev(void);
#endif
} VdispAssembly_t;


int SSTAR_AI_OpenLibrary(AudioInAssembly_t *pstAudioInAssembly);
void SSTAR_AI_CloseLibrary(AudioInAssembly_t *pstAudioInAssembly);

int SSTAR_AO_OpenLibrary(AudioOutAssembly_t *pstAudioOutAssembly);
void SSTAR_AO_CloseLibrary(AudioOutAssembly_t *pstAudioOutAssembly);

int SSTAR_RGN_OpenLibrary(RgnAssembly_t *pstRgnAssembly);
void SSTAR_RGN_CloseLibrary(RgnAssembly_t *pstRgnAssembly);

int SSTAR_IVE_OpenLibrary(IveAssembly_t *pstIveAssembly);
void SSTAR_IVE_CloseLibrary(IveAssembly_t *pstIveAssembly);

int SSTAR_SNR_OpenLibrary(SensorAssembly_t *pstSnrAssembly);
void SSTAR_SNR_CloseLibrary(SensorAssembly_t *pstSnrAssembly);

int SSTAR_VIF_OpenLibrary(VifAssembly_t *pstVifAssembly);
void SSTAR_VIF_CloseLibrary(VifAssembly_t *pstVifAssembly);

int SSTAR_VPE_OpenLibrary(VpeAssembly_t *pstVpeAssembly);
void SSTAR_VPE_CloseLibrary(VpeAssembly_t *pstVpeAssembly);

int SSTAR_DIVP_OpenLibrary(DivpAssembly_t *pstDivpAssembly);
void SSTAR_DIVP_CloseLibrary(DivpAssembly_t *pstDivpAssembly);

int SSTAR_ISP_OpenLibrary(IspAssembly_t *pstIspAssembly);
void SSTAR_ISP_CloseLibrary(IspAssembly_t *pstIspAssembly);

int SSTAR_IQSERVER_OpenLibrary(IqServerAssembly_t *pstIqServerAssembly);
void SSTAR_IQSERVER_CloseLibrary(IqServerAssembly_t *pstIqServerAssembly);

int SSTAR_VDISP_OpenLibrary(VdispAssembly_t *pstVdispAssembly);
void SSTAR_VDISP_CloseLibrary(VdispAssembly_t *pstVdispAssembly);


#endif /* JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_ */
