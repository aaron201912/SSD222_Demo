/*
 * sstar_dynamic_load.c
 *
 *  Created on: 2021年2月8日
 *      Author: koda.xu
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include "sstar_dynamic_load.h"

int SSTAR_AI_OpenLibrary(AudioInAssembly_t *pstAudioInAssembly)
{
	pstAudioInAssembly->pHandle = dlopen("libmi_ai.so", RTLD_NOW);
	if (NULL == pstAudioInAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_ai.so!\n", __func__);
		return -1;
	}

	pstAudioInAssembly->pfnAiSetPubAttr = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t *pstAttr))dlsym(pstAudioInAssembly->pHandle, "MI_AI_SetPubAttr");
	if(NULL == pstAudioInAssembly->pfnAiSetPubAttr)
	{
		printf(" %s: dlsym MI_AI_SetPubAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiEnable = (MI_S32(*)(MI_AUDIO_DEV AiDevId))dlsym(pstAudioInAssembly->pHandle, "MI_AI_Enable");
	if(NULL == pstAudioInAssembly->pfnAiEnable)
	{
		printf(" %s: dlsym MI_AI_Enable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiDisable = (MI_S32(*)(MI_AUDIO_DEV AiDevId))dlsym(pstAudioInAssembly->pHandle, "MI_AI_Disable");
	if(NULL == pstAudioInAssembly->pfnAiDisable)
	{
		printf(" %s: dlsym MI_AI_Disable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiEnableChn = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn))dlsym(pstAudioInAssembly->pHandle, "MI_AI_EnableChn");
	if(NULL == pstAudioInAssembly->pfnAiEnableChn)
	{
		printf(" %s: dlsym MI_AI_EnableChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiDisableChn = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn))dlsym(pstAudioInAssembly->pHandle, "MI_AI_DisableChn");
	if(NULL == pstAudioInAssembly->pfnAiDisableChn)
	{
		printf(" %s: dlsym MI_AI_DisableChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiGetFrame = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t*pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm , MI_S32 s32MilliSec))dlsym(pstAudioInAssembly->pHandle, "MI_AI_GetFrame");
	if(NULL == pstAudioInAssembly->pfnAiGetFrame)
	{
		printf(" %s: dlsym MI_AI_GetFrame failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiReleaseFrame = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t *pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm))dlsym(pstAudioInAssembly->pHandle, "MI_AI_ReleaseFrame");
	if(NULL == pstAudioInAssembly->pfnAiReleaseFrame)
	{
		printf(" %s: dlsym MI_AI_ReleaseFrame failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiSetChnParam = (MI_S32(*)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam))dlsym(pstAudioInAssembly->pHandle, "MI_AI_SetChnParam");
	if(NULL == pstAudioInAssembly->pfnAiSetChnParam)
	{
		printf(" %s: dlsym MI_AI_SetChnParam failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiInitDev = (MI_S32(*)(MI_AI_InitParam_t *pstInitParam))dlsym(pstAudioInAssembly->pHandle, "MI_AI_InitDev");
	if(NULL == pstAudioInAssembly->pfnAiInitDev)
	{
		printf(" %s: dlsym MI_AI_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioInAssembly->pfnAiDeInitDev = (MI_S32(*)())dlsym(pstAudioInAssembly->pHandle, "MI_AI_DeInitDev");
	if(NULL == pstAudioInAssembly->pfnAiDeInitDev)
	{
		printf(" %s: dlsym MI_AI_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_AI_CloseLibrary(AudioInAssembly_t *pstAudioInAssembly)
{
	if(pstAudioInAssembly->pHandle)
	{
		dlclose(pstAudioInAssembly->pHandle);
		pstAudioInAssembly->pHandle = NULL;
	}
	memset(pstAudioInAssembly, 0, sizeof(*pstAudioInAssembly));

}

int SSTAR_AO_OpenLibrary(AudioOutAssembly_t *pstAudioOutAssembly)
{
	pstAudioOutAssembly->pHandle = dlopen("libmi_ao.so", RTLD_NOW);
	if (NULL == pstAudioOutAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_ao.so!\n", __func__);
		return -1;
	}

	pstAudioOutAssembly->pfnAoSetPubAttr = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SetPubAttr");
	if(NULL == pstAudioOutAssembly->pfnAoSetPubAttr)
	{
		printf(" %s: dlsym MI_AO_SetPubAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoGetPubAttr = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_GetPubAttr");
	if(NULL == pstAudioOutAssembly->pfnAoGetPubAttr)
	{
		printf(" %s: dlsym MI_AO_GetPubAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoEnable = (MI_S32(*)(MI_AUDIO_DEV AoDevId))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_Enable");
	if(NULL == pstAudioOutAssembly->pfnAoEnable)
	{
		printf(" %s: dlsym MI_AO_Enable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoDisable = (MI_S32(*)(MI_AUDIO_DEV AoDevId))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_Disable");
	if(NULL == pstAudioOutAssembly->pfnAoDisable)
	{
		printf(" %s: dlsym MI_AO_Disable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoEnableChn = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_EnableChn");
	if(NULL == pstAudioOutAssembly->pfnAoEnableChn)
	{
		printf(" %s: dlsym MI_AO_EnableChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoDisableChn = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_DisableChn");
	if(NULL == pstAudioOutAssembly->pfnAoDisableChn)
	{
		printf(" %s: dlsym MI_AO_DisableChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoSendFrame = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_Frame_t *pstData, MI_S32 s32MilliSec))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SendFrame");
	if(NULL == pstAudioOutAssembly->pfnAoSendFrame)
	{
		printf(" %s: dlsym MI_AO_SendFrame failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoPauseChn = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_PauseChn");
	if(NULL == pstAudioOutAssembly->pfnAoPauseChn)
	{
		printf(" %s: dlsym MI_AO_PauseChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoResumeChn = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_ResumeChn");
	if(NULL == pstAudioOutAssembly->pfnAoResumeChn)
	{
		printf(" %s: dlsym MI_AO_ResumeChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoSetVolume = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 s32VolumeDb, MI_AO_GainFading_e eFading))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SetVolume");
	if(NULL == pstAudioOutAssembly->pfnAoSetVolume)
	{
		printf(" %s: dlsym MI_AO_SetVolume failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoGetVolume = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 *ps32VolumeDb))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_GetVolume");
	if(NULL == pstAudioOutAssembly->pfnAoGetVolume)
	{
		printf(" %s: dlsym MI_AO_GetVolume failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoSetMute = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL bEnable))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SetMute");
	if(NULL == pstAudioOutAssembly->pfnAoSetMute)
	{
		printf(" %s: dlsym MI_AO_SetMute failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoGetMute = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL *pbEnable))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_GetMute");
	if(NULL == pstAudioOutAssembly->pfnAoGetMute)
	{
		printf(" %s: dlsym MI_AO_GetMute failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoSetChnParam = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SetChnParam");
	if(NULL == pstAudioOutAssembly->pfnAoSetChnParam)
	{
		printf(" %s: dlsym MI_AO_SetChnParam failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoGetChnParam = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_GetChnParam");
	if(NULL == pstAudioOutAssembly->pfnAoGetChnParam)
	{
		printf(" %s: dlsym MI_AO_GetChnParam failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoSetSrcGain = (MI_S32(*)(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_SetSrcGain");
	if(NULL == pstAudioOutAssembly->pfnAoSetSrcGain)
	{
		printf(" %s: dlsym MI_AO_SetSrcGain failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoInitDev = (MI_S32(*)(MI_AO_InitParam_t *pstInitParam))dlsym(pstAudioOutAssembly->pHandle, "MI_AO_InitDev");
	if(NULL == pstAudioOutAssembly->pfnAoInitDev)
	{
		printf(" %s: dlsym MI_AO_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstAudioOutAssembly->pfnAoDeInitDev = (MI_S32(*)())dlsym(pstAudioOutAssembly->pHandle, "MI_AO_DeInitDev");
	if(NULL == pstAudioOutAssembly->pfnAoDeInitDev)
	{
		printf(" %s: dlsym MI_AO_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_AO_CloseLibrary(AudioOutAssembly_t *pstAudioOutAssembly)
{
	if(pstAudioOutAssembly->pHandle)
	{
		dlclose(pstAudioOutAssembly->pHandle);
		pstAudioOutAssembly->pHandle = NULL;
	}
	memset(pstAudioOutAssembly, 0, sizeof(*pstAudioOutAssembly));
}

int SSTAR_RGN_OpenLibrary(RgnAssembly_t *pstRgnAssembly)
{
	pstRgnAssembly->pHandle = dlopen("libmi_rgn.so", RTLD_NOW);
	if (NULL == pstRgnAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_rgn.so!\n", __func__);
		return -1;
	}

	pstRgnAssembly->pfnRgnInit = (MI_S32(*)(MI_RGN_PaletteTable_t *pstPaletteTable))dlsym(pstRgnAssembly->pHandle, "MI_RGN_Init");
	if(NULL == pstRgnAssembly->pfnRgnInit)
	{
		printf(" %s: dlsym MI_RGN_Init failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnDeInit = (MI_S32(*)())dlsym(pstRgnAssembly->pHandle, "MI_RGN_DeInit");
	if(NULL == pstRgnAssembly->pfnRgnDeInit)
	{
		printf(" %s: dlsym MI_RGN_DeInit failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnCreate = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion))dlsym(pstRgnAssembly->pHandle, "MI_RGN_Create");
	if(NULL == pstRgnAssembly->pfnRgnCreate)
	{
		printf(" %s: dlsym MI_RGN_Create failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnDestroy = (MI_S32(*)(MI_RGN_HANDLE hHandle))dlsym(pstRgnAssembly->pHandle, "MI_RGN_Destroy");
	if(NULL == pstRgnAssembly->pfnRgnDestroy)
	{
		printf(" %s: dlsym MI_RGN_Destroy failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnGetAttr = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion))dlsym(pstRgnAssembly->pHandle, "MI_RGN_GetAttr");
	if(NULL == pstRgnAssembly->pfnRgnGetAttr)
	{
		printf(" %s: dlsym MI_RGN_GetAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnSetBitMap = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_Bitmap_t *pstBitmap))dlsym(pstRgnAssembly->pHandle, "MI_RGN_SetBitMap");
	if(NULL == pstRgnAssembly->pfnRgnSetBitMap)
	{
		printf(" %s: dlsym MI_RGN_SetBitMap failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnAttachToChn = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t* pstChnPort, MI_RGN_ChnPortParam_t *pstChnAttr))dlsym(pstRgnAssembly->pHandle, "MI_RGN_AttachToChn");
	if(NULL == pstRgnAssembly->pfnRgnAttachToChn)
	{
		printf(" %s: dlsym MI_RGN_AttachToChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnDetachFromChn = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort))dlsym(pstRgnAssembly->pHandle, "MI_RGN_DetachFromChn");
	if(NULL == pstRgnAssembly->pfnRgnDetachFromChn)
	{
		printf(" %s: dlsym MI_RGN_DetachFromChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnSetDisplayAttr = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr))dlsym(pstRgnAssembly->pHandle, "MI_RGN_SetDisplayAttr");
	if(NULL == pstRgnAssembly->pfnRgnSetDisplayAttr)
	{
		printf(" %s: dlsym MI_RGN_SetDisplayAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnGetDisplayAttr = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr))dlsym(pstRgnAssembly->pHandle, "MI_RGN_GetDisplayAttr");
	if(NULL == pstRgnAssembly->pfnRgnGetDisplayAttr)
	{
		printf(" %s: dlsym MI_RGN_GetDisplayAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnGetCanvasInfo = (MI_S32(*)(MI_RGN_HANDLE hHandle, MI_RGN_CanvasInfo_t* pstCanvasInfo))dlsym(pstRgnAssembly->pHandle, "MI_RGN_GetCanvasInfo");
	if(NULL == pstRgnAssembly->pfnRgnGetCanvasInfo)
	{
		printf(" %s: dlsym MI_RGN_GetCanvasInfo failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnUpdateCanvas = (MI_S32(*)(MI_RGN_HANDLE hHandle))dlsym(pstRgnAssembly->pHandle, "MI_RGN_UpdateCanvas");
	if(NULL == pstRgnAssembly->pfnRgnUpdateCanvas)
	{
		printf(" %s: dlsym MI_RGN_UpdateCanvas failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnScaleRect = (MI_S32(*)(MI_RGN_ChnPort_t *pstChnPort, MI_RGN_Size_t *pstCanvasSize, MI_RGN_Size_t *pstScreenSize))dlsym(pstRgnAssembly->pHandle, "MI_RGN_ScaleRect");
	if(NULL == pstRgnAssembly->pfnRgnScaleRect)
	{
		printf(" %s: dlsym MI_RGN_ScaleRect failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnInitDev = (MI_S32(*)(MI_RGN_InitParam_t *pstInitParam))dlsym(pstRgnAssembly->pHandle, "MI_RGN_InitDev");
	if(NULL == pstRgnAssembly->pfnRgnInitDev)
	{
		printf(" %s: dlsym MI_RGN_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstRgnAssembly->pfnRgnDeInitDev = (MI_S32(*)())dlsym(pstRgnAssembly->pHandle, "MI_RGN_DeInitDev");
	if(NULL == pstRgnAssembly->pfnRgnDeInitDev)
	{
		printf(" %s: dlsym MI_RGN_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_RGN_CloseLibrary(RgnAssembly_t *pstRgnAssembly)
{
	if(pstRgnAssembly->pHandle)
	{
		dlclose(pstRgnAssembly->pHandle);
		pstRgnAssembly->pHandle = NULL;
	}
	memset(pstRgnAssembly, 0, sizeof(*pstRgnAssembly));
}

int SSTAR_IVE_OpenLibrary(IveAssembly_t *pstIveAssembly)
{
	pstIveAssembly->pHandle = dlopen("libmi_ive.so", RTLD_NOW);
	if (NULL == pstIveAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_ive.so!\n", __func__);
		return -1;
	}

	pstIveAssembly->pfnIveCreate = (MI_S32(*)(MI_IVE_HANDLE hHandle))dlsym(pstIveAssembly->pHandle, "MI_IVE_Create");
	if(NULL == pstIveAssembly->pfnIveCreate)
	{
		printf(" %s: dlsym MI_IVE_Create failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIveAssembly->pfnIveDestroy = (MI_S32(*)(MI_IVE_HANDLE hHandle))dlsym(pstIveAssembly->pHandle, "MI_IVE_Destroy");
	if(NULL == pstIveAssembly->pfnIveDestroy)
	{
		printf(" %s: dlsym MI_IVE_Destroy failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIveAssembly->pfnIveCsc = (MI_S32(*)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstDst, MI_IVE_CscCtrl_t *pstCscCtrl, MI_BOOL bInstant))dlsym(pstIveAssembly->pHandle, "MI_IVE_Csc");
	if(NULL == pstIveAssembly->pfnIveCsc)
	{
		printf(" %s: dlsym MI_IVE_Csc failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIveAssembly->pfnIveSad = (MI_S32(*)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstSad, MI_IVE_DstImage_t *pstThr, MI_IVE_SadCtrl_t *pstSadCtrl, MI_BOOL bInstant))dlsym(pstIveAssembly->pHandle, "MI_IVE_Sad");
	if(NULL == pstIveAssembly->pfnIveSad)
	{
		printf(" %s: dlsym MI_IVE_Sad failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_IVE_CloseLibrary(IveAssembly_t *pstIveAssembly)
{
	if(pstIveAssembly->pHandle)
	{
		dlclose(pstIveAssembly->pHandle);
		pstIveAssembly->pHandle = NULL;
	}
	memset(pstIveAssembly, 0, sizeof(*pstIveAssembly));
}

int SSTAR_SNR_OpenLibrary(SensorAssembly_t *pstSnrAssembly)
{
	pstSnrAssembly->pHandle = dlopen("libmi_sensor.so", RTLD_NOW);
	if (NULL == pstSnrAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_sensor.so!\n", __func__);
		return -1;
	}

	pstSnrAssembly->pfnSnrEnable = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId))dlsym(pstSnrAssembly->pHandle, "MI_SNR_Enable");
	if(NULL == pstSnrAssembly->pfnSnrEnable)
	{
		printf(" %s: dlsym MI_SNR_Enable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrDisable = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId))dlsym(pstSnrAssembly->pHandle, "MI_SNR_Disable");
	if(NULL == pstSnrAssembly->pfnSnrDisable)
	{
		printf(" %s: dlsym MI_SNR_Disable failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetPadInfo = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_SNR_PADInfo_t  *pstPadInfo))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetPadInfo");
	if(NULL == pstSnrAssembly->pfnSnrGetPadInfo)
	{
		printf(" %s: dlsym MI_SNR_GetPadInfo failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetPlaneInfo = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32  u32PlaneID, MI_SNR_PlaneInfo_t *pstPlaneInfo))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetPlaneInfo");
	if(NULL == pstSnrAssembly->pfnSnrGetPlaneInfo)
	{
		printf(" %s: dlsym MI_SNR_GetPlaneInfo failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetFps = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32 *pFps))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetFps");
	if(NULL == pstSnrAssembly->pfnSnrGetFps)
	{
		printf(" %s: dlsym MI_SNR_GetFps failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrSetFps = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32 u32Fps))dlsym(pstSnrAssembly->pHandle, "MI_SNR_SetFps");
	if(NULL == pstSnrAssembly->pfnSnrSetFps)
	{
		printf(" %s: dlsym MI_SNR_SetFps failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetBT656SrcType = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcType_e *psttype))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetBT656SrcType");
	if(NULL == pstSnrAssembly->pfnSnrGetBT656SrcType)
	{
		printf(" %s: dlsym MI_SNR_GetBT656SrcType failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrQueryResCount = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32 *pu32ResCount))dlsym(pstSnrAssembly->pHandle, "MI_SNR_QueryResCount");
	if(NULL == pstSnrAssembly->pfnSnrQueryResCount)
	{
		printf(" %s: dlsym MI_SNR_QueryResCount failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetRes = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U8 u8ResIdx, MI_SNR_Res_t *pstRes))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetRes");
	if(NULL == pstSnrAssembly->pfnSnrGetRes)
	{
		printf(" %s: dlsym MI_SNR_GetRes failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetCurRes = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U8 *pu8CurResIdx, MI_SNR_Res_t  *pstCurRes))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetCurRes");
	if(NULL == pstSnrAssembly->pfnSnrGetCurRes)
	{
		printf(" %s: dlsym MI_SNR_GetCurRes failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrSetRes = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U8 u8ResIdx))dlsym(pstSnrAssembly->pHandle, "MI_SNR_SetRes");
	if(NULL == pstSnrAssembly->pfnSnrSetRes)
	{
		printf(" %s: dlsym MI_SNR_SetRes failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrSetOrien = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_BOOL bMirror, MI_BOOL bFlip))dlsym(pstSnrAssembly->pHandle, "MI_SNR_SetOrien");
	if(NULL == pstSnrAssembly->pfnSnrSetOrien)
	{
		printf(" %s: dlsym MI_SNR_SetOrien failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetOrien = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_BOOL *pbMirror, MI_BOOL *pbFlip))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetOrien");
	if(NULL == pstSnrAssembly->pfnSnrGetOrien)
	{
		printf(" %s: dlsym MI_SNR_GetOrien failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrSetPlaneMode = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_BOOL bEnable))dlsym(pstSnrAssembly->pHandle, "MI_SNR_SetPlaneMode");
	if(NULL == pstSnrAssembly->pfnSnrSetPlaneMode)
	{
		printf(" %s: dlsym MI_SNR_SetPlaneMode failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrGetPlaneMode = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_BOOL *pbEnable))dlsym(pstSnrAssembly->pHandle, "MI_SNR_GetPlaneMode");
	if(NULL == pstSnrAssembly->pfnSnrGetPlaneMode)
	{
		printf(" %s: dlsym MI_SNR_GetPlaneMode failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrCustFunction = (MI_S32(*)(MI_SNR_PAD_ID_e ePADId, MI_U32 u32CmdId, MI_U32 u32DataSize, void *pCustData, MI_SNR_CUST_DIR_e eDir))dlsym(pstSnrAssembly->pHandle, "MI_SNR_CustFunction");
	if(NULL == pstSnrAssembly->pfnSnrCustFunction)
	{
		printf(" %s: dlsym MI_SNR_CustFunction failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrInitDev = (MI_S32(*)(MI_SNR_InitParam_t *pstInitParam))dlsym(pstSnrAssembly->pHandle, "MI_SNR_InitDev");
	if(NULL == pstSnrAssembly->pfnSnrInitDev)
	{
		printf(" %s: dlsym MI_SNR_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstSnrAssembly->pfnSnrDeInitDev = (MI_S32(*)())dlsym(pstSnrAssembly->pHandle, "MI_SNR_DeInitDev");
	if(NULL == pstSnrAssembly->pfnSnrDeInitDev)
	{
		printf(" %s: dlsym MI_SNR_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_SNR_CloseLibrary(SensorAssembly_t *pstSnrAssembly)
{
	if(pstSnrAssembly->pHandle)
	{
		dlclose(pstSnrAssembly->pHandle);
		pstSnrAssembly->pHandle = NULL;
	}
	memset(pstSnrAssembly, 0, sizeof(*pstSnrAssembly));
}

int SSTAR_VIF_OpenLibrary(VifAssembly_t *pstVifAssembly)
{
	pstVifAssembly->pHandle = dlopen("libmi_vif.so", RTLD_NOW);
	if (NULL == pstVifAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_vif.so!\n", __func__);
		return -1;
	}

	pstVifAssembly->pfnVifSetDevAttr = (MI_S32(*)(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr))dlsym(pstVifAssembly->pHandle, "MI_VIF_SetDevAttr");
	if(NULL == pstVifAssembly->pfnVifSetDevAttr)
	{
		printf(" %s: dlsym MI_VIF_SetDevAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifGetDevAttr = (MI_S32(*)(MI_VIF_DEV u32VifDev, MI_VIF_DevAttr_t *pstDevAttr))dlsym(pstVifAssembly->pHandle, "MI_VIF_GetDevAttr");
	if(NULL == pstVifAssembly->pfnVifGetDevAttr)
	{
		printf(" %s: dlsym MI_VIF_GetDevAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifEnableDev = (MI_S32(*)(MI_VIF_DEV u32VifDev))dlsym(pstVifAssembly->pHandle, "MI_VIF_EnableDev");
	if(NULL == pstVifAssembly->pfnVifEnableDev)
	{
		printf(" %s: dlsym MI_VIF_EnableDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifDisableDev = (MI_S32(*)(MI_VIF_DEV u32VifDev))dlsym(pstVifAssembly->pHandle, "MI_VIF_DisableDev");
	if(NULL == pstVifAssembly->pfnVifDisableDev)
	{
		printf(" %s: dlsym MI_VIF_DisableDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifSetChnPortAttr = (MI_S32(*)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr))dlsym(pstVifAssembly->pHandle, "MI_VIF_SetChnPortAttr");
	if(NULL == pstVifAssembly->pfnVifSetChnPortAttr)
	{
		printf(" %s: dlsym MI_VIF_SetChnPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifGetChnPortAttr = (MI_S32(*)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortAttr_t *pstAttr))dlsym(pstVifAssembly->pHandle, "MI_VIF_GetChnPortAttr");
	if(NULL == pstVifAssembly->pfnVifGetChnPortAttr)
	{
		printf(" %s: dlsym MI_VIF_GetChnPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifEnableChnPort = (MI_S32(*)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort))dlsym(pstVifAssembly->pHandle, "MI_VIF_EnableChnPort");
	if(NULL == pstVifAssembly->pfnVifEnableChnPort)
	{
		printf(" %s: dlsym MI_VIF_EnableChnPort failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifDisableChnPort = (MI_S32(*)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort))dlsym(pstVifAssembly->pHandle, "MI_VIF_DisableChnPort");
	if(NULL == pstVifAssembly->pfnVifDisableChnPort)
	{
		printf(" %s: dlsym MI_VIF_DisableChnPort failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifQuery = (MI_S32(*)(MI_VIF_CHN u32VifChn, MI_VIF_PORT u32ChnPort, MI_VIF_ChnPortStat_t *pstStat))dlsym(pstVifAssembly->pHandle, "MI_VIF_Query");
	if(NULL == pstVifAssembly->pfnVifQuery)
	{
		printf(" %s: dlsym MI_VIF_Query failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifSetDev2SnrPadMux = (MI_S32(*)(MI_VIF_Dev2SnrPadMuxCfg_t *pstVifDevMap, MI_U8 u8Length))dlsym(pstVifAssembly->pHandle, "MI_VIF_SetDev2SnrPadMux");
	if(NULL == pstVifAssembly->pfnVifSetDev2SnrPadMux)
	{
		printf(" %s: dlsym MI_VIF_SetDev2SnrPadMux failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifInitDev = (MI_S32(*)(MI_VIF_InitParam_t *pstInitParam))dlsym(pstVifAssembly->pHandle, "MI_VIF_InitDev");
	if(NULL == pstVifAssembly->pfnVifInitDev)
	{
		printf(" %s: dlsym MI_VIF_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVifAssembly->pfnVifDeInitDev = (MI_S32(*)())dlsym(pstVifAssembly->pHandle, "MI_VIF_DeInitDev");
	if(NULL == pstVifAssembly->pfnVifDeInitDev)
	{
		printf(" %s: dlsym MI_VIF_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_VIF_CloseLibrary(VifAssembly_t *pstVifAssembly)
{
	if(pstVifAssembly->pHandle)
	{
		dlclose(pstVifAssembly->pHandle);
		pstVifAssembly->pHandle = NULL;
	}
	memset(pstVifAssembly, 0, sizeof(*pstVifAssembly));
}

int SSTAR_VPE_OpenLibrary(VpeAssembly_t *pstVpeAssembly)
{
	pstVpeAssembly->pHandle = dlopen("libmi_vpe.so", RTLD_NOW);
	if (NULL == pstVpeAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_vpe.so!\n", __func__);
		return -1;
	}

	pstVpeAssembly->pfnVpeCreateChannel = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr))dlsym(pstVpeAssembly->pHandle, "MI_VPE_CreateChannel");
	if(NULL == pstVpeAssembly->pfnVpeCreateChannel)
	{
		printf(" %s: dlsym MI_VPE_CreateChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeDestroyChannel = (MI_S32(*)(MI_VPE_CHANNEL VpeCh))dlsym(pstVpeAssembly->pHandle, "MI_VPE_DestroyChannel");
	if(NULL == pstVpeAssembly->pfnVpeDestroyChannel)
	{
		printf(" %s: dlsym MI_VPE_DestroyChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetChannelAttr = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetChannelAttr");
	if(NULL == pstVpeAssembly->pfnVpeGetChannelAttr)
	{
		printf(" %s: dlsym MI_VPE_GetChannelAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetChannelAttr = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetChannelAttr");
	if(NULL == pstVpeAssembly->pfnVpeSetChannelAttr)
	{
		printf(" %s: dlsym MI_VPE_SetChannelAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeStartChannel = (MI_S32(*)(MI_VPE_CHANNEL VpeCh))dlsym(pstVpeAssembly->pHandle, "MI_VPE_StartChannel");
	if(NULL == pstVpeAssembly->pfnVpeStartChannel)
	{
		printf(" %s: dlsym MI_VPE_StartChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeStopChannel = (MI_S32(*)(MI_VPE_CHANNEL VpeCh))dlsym(pstVpeAssembly->pHandle, "MI_VPE_StopChannel");
	if(NULL == pstVpeAssembly->pfnVpeStopChannel)
	{
		printf(" %s: dlsym MI_VPE_StopChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetChannelParam = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetChannelParam");
	if(NULL == pstVpeAssembly->pfnVpeSetChannelParam)
	{
		printf(" %s: dlsym MI_VPE_SetChannelParam failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetChannelParam = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetChannelParam");
	if(NULL == pstVpeAssembly->pfnVpeGetChannelParam)
	{
		printf(" %s: dlsym MI_VPE_GetChannelParam failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetChannelCrop = (MI_S32(*)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetChannelCrop");
	if(NULL == pstVpeAssembly->pfnVpeSetChannelCrop)
	{
		printf(" %s: dlsym MI_VPE_SetChannelCrop failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetChannelCrop = (MI_S32(*)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetChannelCrop");
	if(NULL == pstVpeAssembly->pfnVpeGetChannelCrop)
	{
		printf(" %s: dlsym MI_VPE_GetChannelCrop failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetChannelRegionLuma = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_RegionInfo_t *pstRegionInfo, MI_U32 *pu32LumaData,MI_S32 s32MilliSec))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetChannelRegionLuma");
	if(NULL == pstVpeAssembly->pfnVpeGetChannelRegionLuma)
	{
		printf(" %s: dlsym MI_VPE_GetChannelRegionLuma failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetChannelRotation = (MI_S32(*)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e eType))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetChannelRotation");
	if(NULL == pstVpeAssembly->pfnVpeSetChannelRotation)
	{
		printf(" %s: dlsym MI_VPE_SetChannelRotation failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetChannelRotation = (MI_S32(*)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e *pType))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetChannelRotation");
	if(NULL == pstVpeAssembly->pfnVpeGetChannelRotation)
	{
		printf(" %s: dlsym MI_VPE_GetChannelRotation failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeEnablePort = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort))dlsym(pstVpeAssembly->pHandle, "MI_VPE_EnablePort");
	if(NULL == pstVpeAssembly->pfnVpeEnablePort)
	{
		printf(" %s: dlsym MI_VPE_EnablePort failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeDisablePort = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort))dlsym(pstVpeAssembly->pHandle, "MI_VPE_DisablePort");
	if(NULL == pstVpeAssembly->pfnVpeDisablePort)
	{
		printf(" %s: dlsym MI_VPE_DisablePort failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetPortMode = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetPortMode");
	if(NULL == pstVpeAssembly->pfnVpeSetPortMode)
	{
		printf(" %s: dlsym MI_VPE_SetPortMode failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetPortMode = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetPortMode");
	if(NULL == pstVpeAssembly->pfnVpeGetPortMode)
	{
		printf(" %s: dlsym MI_VPE_GetPortMode failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetPortCrop = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetPortCrop");
	if(NULL == pstVpeAssembly->pfnVpeSetPortCrop)
	{
		printf(" %s: dlsym MI_VPE_SetPortCrop failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetPortCrop = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetPortCrop");
	if(NULL == pstVpeAssembly->pfnVpeGetPortCrop)
	{
		printf(" %s: dlsym MI_VPE_GetPortCrop failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSetPortShowPosition = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SetPortShowPosition");
	if(NULL == pstVpeAssembly->pfnVpeSetPortShowPosition)
	{
		printf(" %s: dlsym MI_VPE_SetPortShowPosition failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeGetPortShowPosition = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo))dlsym(pstVpeAssembly->pHandle, "MI_VPE_GetPortShowPosition");
	if(NULL == pstVpeAssembly->pfnVpeGetPortShowPosition)
	{
		printf(" %s: dlsym MI_VPE_GetPortShowPosition failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeSkipFrame = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_U32 u32FrameNum))dlsym(pstVpeAssembly->pHandle, "MI_VPE_SkipFrame");
	if(NULL == pstVpeAssembly->pfnVpeSkipFrame)
	{
		printf(" %s: dlsym MI_VPE_SkipFrame failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeLDCBegViewConfig = (MI_S32(*)(MI_VPE_CHANNEL VpeCh))dlsym(pstVpeAssembly->pHandle, "MI_VPE_LDCBegViewConfig");
	if(NULL == pstVpeAssembly->pfnVpeLDCBegViewConfig)
	{
		printf(" %s: dlsym MI_VPE_LDCBegViewConfig failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeLDCEndViewConfig = (MI_S32(*)(MI_VPE_CHANNEL VpeCh))dlsym(pstVpeAssembly->pHandle, "MI_VPE_LDCEndViewConfig");
	if(NULL == pstVpeAssembly->pfnVpeLDCEndViewConfig)
	{
		printf(" %s: dlsym MI_VPE_LDCEndViewConfig failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeLDCSetViewConfig = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, void *pConfigAddr, MI_U32 u32ConfigSize))dlsym(pstVpeAssembly->pHandle, "MI_VPE_LDCSetViewConfig");
	if(NULL == pstVpeAssembly->pfnVpeLDCSetViewConfig)
	{
		printf(" %s: dlsym MI_VPE_LDCSetViewConfig failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeAlloc_IspDataBuf = (MI_S32(*)(MI_U32 u32Size,void **pUserVirAddr))dlsym(pstVpeAssembly->pHandle, "MI_VPE_Alloc_IspDataBuf");
	if(NULL == pstVpeAssembly->pfnVpeAlloc_IspDataBuf)
	{
		printf(" %s: dlsym MI_VPE_Alloc_IspDataBuf failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeFree_IspDataBuf = (MI_S32(*)(void *pUserBuf))dlsym(pstVpeAssembly->pHandle, "MI_VPE_Free_IspDataBuf");
	if(NULL == pstVpeAssembly->pfnVpeFree_IspDataBuf)
	{
		printf(" %s: dlsym MI_VPE_Free_IspDataBuf failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeCreateMultiChannelStitch = (MI_S32(*)(MI_VPE_CHANNEL VpeCh, MI_VPE_MultiChannelStitchAttr_t *pstVpeChAttr))dlsym(pstVpeAssembly->pHandle, "MI_VPE_CreateMultiChannelStitch");
	if(NULL == pstVpeAssembly->pfnVpeCreateMultiChannelStitch)
	{
		printf(" %s: dlsym MI_VPE_CreateMultiChannelStitch failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeInitDev = (MI_S32(*)(MI_VPE_InitParam_t *pstInitParam))dlsym(pstVpeAssembly->pHandle, "MI_VPE_InitDev");
	if(NULL == pstVpeAssembly->pfnVpeInitDev)
	{
		printf(" %s: dlsym MI_VPE_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVpeAssembly->pfnVpeDeInitDev = (MI_S32(*)())dlsym(pstVpeAssembly->pHandle, "MI_VPE_DeInitDev");
	if(NULL == pstVpeAssembly->pfnVpeDeInitDev)
	{
		printf(" %s: dlsym MI_VPE_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_VPE_CloseLibrary(VpeAssembly_t *pstVpeAssembly)
{
	if(pstVpeAssembly->pHandle)
	{
		dlclose(pstVpeAssembly->pHandle);
		pstVpeAssembly->pHandle = NULL;
	}
	memset(pstVpeAssembly, 0, sizeof(*pstVpeAssembly));
}

int SSTAR_DIVP_OpenLibrary(DivpAssembly_t *pstDivpAssembly)
{
	pstDivpAssembly->pHandle = dlopen("libmi_divp.so", RTLD_NOW);
	if (NULL == pstDivpAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_divp.so!\n", __func__);
		return -1;
	}

	pstDivpAssembly->pfnDivpCreateChn = (MI_S32(*)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_CreateChn");
	if(NULL == pstDivpAssembly->pfnDivpCreateChn)
	{
		printf(" %s: dlsym MI_DIVP_CreateChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpDestroyChn = (MI_S32(*)(MI_DIVP_CHN DivpChn))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_DestroyChn");
	if(NULL == pstDivpAssembly->pfnDivpDestroyChn)
	{
		printf(" %s: dlsym MI_DIVP_DestroyChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpSetChnAttr = (MI_S32(*)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_SetChnAttr");
	if(NULL == pstDivpAssembly->pfnDivpSetChnAttr)
	{
		printf(" %s: dlsym MI_DIVP_SetChnAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpGetChnAttr = (MI_S32(*)(MI_DIVP_CHN DivpChn, MI_DIVP_ChnAttr_t* pstAttr))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_GetChnAttr");
	if(NULL == pstDivpAssembly->pfnDivpGetChnAttr)
	{
		printf(" %s: dlsym MI_DIVP_GetChnAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpStartChn = (MI_S32(*)(MI_DIVP_CHN DivpChn))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_StartChn");
	if(NULL == pstDivpAssembly->pfnDivpStartChn)
	{
		printf(" %s: dlsym MI_DIVP_StartChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpStopChn = (MI_S32(*)(MI_DIVP_CHN DivpChn))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_StopChn");
	if(NULL == pstDivpAssembly->pfnDivpStopChn)
	{
		printf(" %s: dlsym MI_DIVP_StopChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpSetOutputPortAttr = (MI_S32(*)(MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_SetOutputPortAttr");
	if(NULL == pstDivpAssembly->pfnDivpSetOutputPortAttr)
	{
		printf(" %s: dlsym MI_DIVP_SetOutputPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpGetOutputPortAttr = (MI_S32(*)(MI_DIVP_CHN DivpChn, MI_DIVP_OutputPortAttr_t * pstOutputPortAttr))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_GetOutputPortAttr");
	if(NULL == pstDivpAssembly->pfnDivpGetOutputPortAttr)
	{
		printf(" %s: dlsym MI_DIVP_GetOutputPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpRefreshChn = (MI_S32(*)(MI_DIVP_CHN DivpChn))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_RefreshChn");
	if(NULL == pstDivpAssembly->pfnDivpRefreshChn)
	{
		printf(" %s: dlsym MI_DIVP_RefreshChn failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpStretchBuf = (MI_S32(*)(MI_DIVP_DirectBuf_t *pstSrcBuf, MI_SYS_WindowRect_t *pstSrcCrop, MI_DIVP_DirectBuf_t *pstDstBuf))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_StretchBuf");
	if(NULL == pstDivpAssembly->pfnDivpStretchBuf)
	{
		printf(" %s: dlsym MI_DIVP_StretchBuf failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpInitDev = (MI_S32(*)(MI_DIVP_InitParam_t *pstInitParam))dlsym(pstDivpAssembly->pHandle, "MI_DIVP_InitDev");
	if(NULL == pstDivpAssembly->pfnDivpInitDev)
	{
		printf(" %s: dlsym MI_DIVP_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstDivpAssembly->pfnDivpDeInitDev = (MI_S32(*)())dlsym(pstDivpAssembly->pHandle, "MI_DIVP_DeInitDev");
	if(NULL == pstDivpAssembly->pfnDivpDeInitDev)
	{
		printf(" %s: dlsym MI_DIVP_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_DIVP_CloseLibrary(DivpAssembly_t *pstDivpAssembly)
{
	if(pstDivpAssembly->pHandle)
	{
		dlclose(pstDivpAssembly->pHandle);
		pstDivpAssembly->pHandle = NULL;
	}
	memset(pstDivpAssembly, 0, sizeof(*pstDivpAssembly));
}

int SSTAR_ISP_OpenLibrary(IspAssembly_t *pstIspAssembly)
{
	pstIspAssembly->pHandle = dlopen("libmi_isp.so", RTLD_NOW);		// libcus3a.so  ?
	if (NULL == pstIspAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_isp.so!\n", __func__);
		return -1;
	}

	pstIspAssembly->pfnIspApiCmdLoadBinFile = (MI_S32(*)(MI_U32 Channel, char* filepath, MI_U32 user_key))dlsym(pstIspAssembly->pHandle, "MI_ISP_API_CmdLoadBinFile");
	if(NULL == pstIspAssembly->pfnIspApiCmdLoadBinFile)
	{
		printf(" %s: dlsym MI_ISP_API_CmdLoadBinFile failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIspAssembly->pfnIspIqGetParaInitStatus = (MI_S32(*)(MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data))dlsym(pstIspAssembly->pHandle, "MI_ISP_IQ_GetParaInitStatus");
	if(NULL == pstIspAssembly->pfnIspIqGetParaInitStatus)
	{
		printf(" %s: dlsym MI_ISP_IQ_GetParaInitStatus failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_ISP_CloseLibrary(IspAssembly_t *pstIspAssembly)
{
	if(pstIspAssembly->pHandle)
	{
		dlclose(pstIspAssembly->pHandle);
		pstIspAssembly->pHandle = NULL;
	}
	memset(pstIspAssembly, 0, sizeof(*pstIspAssembly));
}

int SSTAR_IQSERVER_OpenLibrary(IqServerAssembly_t *pstIqServerAssembly)
{
	pstIqServerAssembly->pHandle = dlopen("libmi_iqserver.so", RTLD_NOW);		// libcus3a.so  ?
	if (NULL == pstIqServerAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_iqserver.so!\n", __func__);
		return -1;
	}

	pstIqServerAssembly->pfnIqServerOpen = (MI_S32(*)(MI_U16 width, MI_U16 height, MI_S32 vpeChn))dlsym(pstIqServerAssembly->pHandle, "MI_IQSERVER_Open");
	if(NULL == pstIqServerAssembly->pfnIqServerOpen)
	{
		printf(" %s: dlsym MI_IQSERVER_Open failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIqServerAssembly->pfnIqServerSetDataPath = (MI_S32(*)(char* path))dlsym(pstIqServerAssembly->pHandle, "MI_IQSERVER_SetDataPath");
	if(NULL == pstIqServerAssembly->pfnIqServerSetDataPath)
	{
		printf(" %s: dlsym MI_IQSERVER_SetDataPath failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIqServerAssembly->pfnIqServerSetCustFunc = (MI_S32(*)(MI_S32(* func)(MI_U16 data_type, MI_U32 length, MI_U8 * data)))dlsym(pstIqServerAssembly->pHandle, "MI_IQSERVER_SetCustFunc");
	if(NULL == pstIqServerAssembly->pfnIqServerSetCustFunc)
	{
		printf(" %s: dlsym MI_IQSERVER_SetCustFunc failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstIqServerAssembly->pfnIqServerClose = (MI_S32(*)())dlsym(pstIqServerAssembly->pHandle, "MI_IQSERVER_Close");
	if(NULL == pstIqServerAssembly->pfnIqServerClose)
	{
		printf(" %s: dlsym MI_IQSERVER_Close failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_IQSERVER_CloseLibrary(IqServerAssembly_t *pstIqServerAssembly)
{
	if(pstIqServerAssembly->pHandle)
	{
		dlclose(pstIqServerAssembly->pHandle);
		pstIqServerAssembly->pHandle = NULL;
	}
	memset(pstIqServerAssembly, 0, sizeof(*pstIqServerAssembly));
}

int SSTAR_VDISP_OpenLibrary(VdispAssembly_t *pstVdispAssembly)
{
	pstVdispAssembly->pHandle = dlopen("libmi_vdisp.so", RTLD_NOW);		// libcus3a.so  ?
	if (NULL == pstVdispAssembly->pHandle)
	{
		printf(" %s: Can not load libmi_vdisp.so!\n", __func__);
		return -1;
	}

	pstVdispAssembly->pfnVdispInit = (MI_S32(*)())dlsym(pstVdispAssembly->pHandle, "MI_VDISP_Init");
	if(NULL == pstVdispAssembly->pfnVdispInit)
	{
		printf(" %s: dlsym MI_VDISP_Init failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispExit = (MI_S32(*)())dlsym(pstVdispAssembly->pHandle, "MI_VDISP_Exit");
	if(NULL == pstVdispAssembly->pfnVdispExit)
	{
		printf(" %s: dlsym MI_VDISP_Exit failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispOpenDevice = (MI_S32(*)(MI_VDISP_DEV DevId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_OpenDevice");
	if(NULL == pstVdispAssembly->pfnVdispOpenDevice)
	{
		printf(" %s: dlsym MI_VDISP_OpenDevice failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispCloseDevice = (MI_S32(*)(MI_VDISP_DEV DevId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_CloseDevice");
	if(NULL == pstVdispAssembly->pfnVdispCloseDevice)
	{
		printf(" %s: dlsym MI_VDISP_CloseDevice failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispSetOutputPortAttr = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_SetOutputPortAttr");
	if(NULL == pstVdispAssembly->pfnVdispSetOutputPortAttr)
	{
		printf(" %s: dlsym MI_VDISP_SetOutputPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispGetOutputPortAttr = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_GetOutputPortAttr");
	if(NULL == pstVdispAssembly->pfnVdispGetOutputPortAttr)
	{
		printf(" %s: dlsym MI_VDISP_GetOutputPortAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispSetInputChannelAttr = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_SetInputChannelAttr");
	if(NULL == pstVdispAssembly->pfnVdispSetInputChannelAttr)
	{
		printf(" %s: dlsym MI_VDISP_SetInputChannelAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispGetInputChannelAttr = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_GetInputChannelAttr");
	if(NULL == pstVdispAssembly->pfnVdispGetInputChannelAttr)
	{
		printf(" %s: dlsym MI_VDISP_GetInputChannelAttr failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispEnableInputChannel = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_EnableInputChannel");
	if(NULL == pstVdispAssembly->pfnVdispEnableInputChannel)
	{
		printf(" %s: dlsym MI_VDISP_EnableInputChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispDisableInputChannel = (MI_S32(*)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_DisableInputChannel");
	if(NULL == pstVdispAssembly->pfnVdispDisableInputChannel)
	{
		printf(" %s: dlsym MI_VDISP_DisableInputChannel failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispStartDev = (MI_S32(*)(MI_VDISP_DEV DevId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_StartDev");
	if(NULL == pstVdispAssembly->pfnVdispStartDev)
	{
		printf(" %s: dlsym MI_VDISP_StartDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispStopDev = (MI_S32(*)(MI_VDISP_DEV DevId))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_StopDev");
	if(NULL == pstVdispAssembly->pfnVdispStopDev)
	{
		printf(" %s: dlsym MI_VDISP_StopDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispInitDev = (MI_S32(*)(MI_VDISP_InitParam_t *pstInitParam))dlsym(pstVdispAssembly->pHandle, "MI_VDISP_InitDev");
	if(NULL == pstVdispAssembly->pfnVdispInitDev)
	{
		printf(" %s: dlsym MI_VDISP_InitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstVdispAssembly->pfnVdispDeInitDev = (MI_S32(*)())dlsym(pstVdispAssembly->pHandle, "MI_VDISP_DeInitDev");
	if(NULL == pstVdispAssembly->pfnVdispDeInitDev)
	{
		printf(" %s: dlsym MI_VDISP_DeInitDev failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_VDISP_CloseLibrary(VdispAssembly_t *pstVdispAssembly)
{
	if(pstVdispAssembly->pHandle)
	{
		dlclose(pstVdispAssembly->pHandle);
		pstVdispAssembly->pHandle = NULL;
	}
	memset(pstVdispAssembly, 0, sizeof(*pstVdispAssembly));
}
