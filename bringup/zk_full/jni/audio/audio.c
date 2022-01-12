/*
 * audio.c
 *
 *  Created on: 2020年11月2日
 *      Author: sigma
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <pthread.h>
#include <errno.h>
#include "audio.h"
#include "AudioAecProcess.h"
#include "AudioBfProcess.h"

#define AO_DEV_ID			0		// 0:DAC0/1 chn=2; 2:DAC0 chn=1; 3:DAC1 chn=1;
#define AO_DEV_HEADPHONE	4		// 4:HeadPhone chn=2
#define SAVE_WAV			1

#define AI_MAX_CHN_CNT		4
#define MI_AUDIO_SAMPLE_PER_FRAME 	256		//1024
#define CHECK_AI_DEV_ID(devID)	{if ((devID != AMIC_DEV_ID) && (devID != DMIC_DEV_ID)) return -1;}
#define ALIGN_UP(v, align)	((v+align-1)/ align * align)

#define AUDIO_IN_RECORD_PREFIX	AUDIO_IN_RECORD_DIR"/AudioIn_record_"
#define AUDIO_IN_RECORD(idx)	AUDIO_IN_RECORD_PREFIX#idx
#define AUDIO_OUT_MONO_FILE		"/customer/res/8K_16bit_MONO.wav"
#define AUDIO_OUT_STEREO_FILE	"/customer/res/8K_16bit_STERO_30s.wav"

#define AUDIO_AEC_SAMPLERATE(eSampleRate)	{	\
	IAA_AEC_SAMPLE_RATE eAecSampleRate = IAA_AEC_SAMPLE_RATE_16000;	\
\
	switch (eSampleRate)	\
	{	\
		case E_MI_AUDIO_SAMPLE_RATE_8000:	\
			eAecSampleRate = IAA_AEC_SAMPLE_RATE_8000;	\
			break;	\
		case E_MI_AUDIO_SAMPLE_RATE_16000:	\
			eAecSampleRate = IAA_AEC_SAMPLE_RATE_16000;	\
			break;	\
	}	\
	eAecSampleRate;	\
}

typedef enum
{
  E_AENC_TYPE_G711A = 0,
  E_AENC_TYPE_G711U,
  E_AENC_TYPE_G726_16,
  E_AENC_TYPE_G726_24,
  E_AENC_TYPE_G726_32,
  E_AENC_TYPE_G726_40,
  PCM,
} AencType_e;

typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;

typedef struct
{
	bool bExit;
	bool bDoBf;
	bool bDoAec;
	unsigned int ptNumPerFrm;
	int phyChnCnt;
	pthread_t pt;
	int devId;
	int chnId;
	int fd;
} ThreadData_t;

// Ai
static ThreadData_t g_stAudioInThreadData[AI_MAX_CHN_CNT];
static MI_AUDIO_SampleRate_e g_eSampleRate = E_MI_AUDIO_SAMPLE_RATE_8000;
static AudioInAssembly_t g_stAudioInAssembly;

// Ao
static MI_S32 g_AoReadFd = -1;
static WaveFileHeader_t g_stWavHeaderInput;
static MI_S32 g_s32NeedSize = 0;
static ThreadData_t g_stAudioOutPlayFile;
static AudioOutAssembly_t g_stAudioOutAssembly;

static int addWaveHeader(WaveFileHeader_t *pWaveHeader,
						AencType_e eAencType,
						MI_AUDIO_SoundMode_e eSoundMode,
						MI_AUDIO_SampleRate_e eSampleRate,
						int rawLen)
{
	pWaveHeader->chRIFF[0] = 'R';
	pWaveHeader->chRIFF[1] = 'I';
	pWaveHeader->chRIFF[2] = 'F';
	pWaveHeader->chRIFF[3] = 'F';

	pWaveHeader->chWAVE[0] = 'W';
	pWaveHeader->chWAVE[1] = 'A';
	pWaveHeader->chWAVE[2] = 'V';
	pWaveHeader->chWAVE[3] = 'E';

	pWaveHeader->chFMT[0] = 'f';
	pWaveHeader->chFMT[1] = 'm';
	pWaveHeader->chFMT[2] = 't';
	pWaveHeader->chFMT[3] = 0x20;
	pWaveHeader->dwFMTLen = 0x10;

	if (eAencType == PCM)
	{
		if(eSoundMode == E_MI_AUDIO_SOUND_MODE_MONO)
			pWaveHeader->wave.wChannels = 0x01;
		else
			pWaveHeader->wave.wChannels = 0x02;

		pWaveHeader->wave.wFormatTag = 0x1;
		pWaveHeader->wave.wBitsPerSample = 16; //16bit
		pWaveHeader->wave.dwSamplesPerSec = eSampleRate;
		pWaveHeader->wave.dwAvgBytesPerSec = (pWaveHeader->wave.wBitsPerSample  * pWaveHeader->wave.dwSamplesPerSec * pWaveHeader->wave.wChannels) / 8;
		pWaveHeader->wave.wBlockAlign = 1024;
	}

	pWaveHeader->chDATA[0] = 'd';
	pWaveHeader->chDATA[1] = 'a';
	pWaveHeader->chDATA[2] = 't';
	pWaveHeader->chDATA[3] = 'a';
	pWaveHeader->dwDATALen = rawLen;
	pWaveHeader->dwRIFFLen = rawLen + sizeof(WaveFileHeader_t) - 8;

	return 0;
}

static IAA_AEC_SAMPLE_RATE _SSTAR_AI_GetAecSampleRate(MI_AUDIO_SampleRate_e eSampleRate)
{
	IAA_AEC_SAMPLE_RATE eAecSampleRate = IAA_AEC_SAMPLE_RATE_16000;

	switch (eSampleRate)
	{
		case E_MI_AUDIO_SAMPLE_RATE_8000:
			eAecSampleRate = IAA_AEC_SAMPLE_RATE_8000;
			break;
		case E_MI_AUDIO_SAMPLE_RATE_16000:
			eAecSampleRate = IAA_AEC_SAMPLE_RATE_16000;
			break;
	}
	return eAecSampleRate;
}

int SSTAR_AI_SetSampleRate(MI_AUDIO_SampleRate_e eSampleRate)
{
	g_eSampleRate = eSampleRate;
	return 0;
}

static int _SSTAR_AI_InitAec(AEC_HANDLE *pHandle, AudioAecInit *pAecInit, char *pWorkingBuffer, MI_AUDIO_SampleRate_e eSampleRate)
{
	AudioAecConfig aec_config;
	unsigned int supMode_band[6] = {20,40,60,80,100,120};
	unsigned int supMode[7] = {4,4,4,4,4,4,4};
	unsigned int workingBufferSize;
	int ret = 0;

	memset(&aec_config, 0, sizeof(AudioAecConfig));
	aec_config.delay_sample = 0;
	aec_config.comfort_noise_enable = IAA_AEC_FALSE;
	memcpy(&(aec_config.suppression_mode_freq[0]), supMode_band, sizeof(supMode_band));
	memcpy(&(aec_config.suppression_mode_intensity[0]), supMode, sizeof(supMode));

	workingBufferSize = IaaAec_GetBufferSize();
	pWorkingBuffer = (char*)malloc(workingBufferSize);
	if (!pWorkingBuffer)
	{
		printf("AEC working buf malloc failed\n");
		return -1;
	}

	*pHandle = IaaAec_Init(pWorkingBuffer, pAecInit);
	if (!(*pHandle))
	{
		printf("AEC init failed\n");
		return -1;
	}

	ret = IaaAec_Config(*pHandle, &aec_config);
	if (ret)
	{
		printf("AEC config failed\n");
		return -1;
	}

	return 0;
}

static int _SSTAR_AI_InitBf(AEC_HANDLE *pHandle, AudioBfInit *pBfInit, char *pWorkingBuffer, MI_AUDIO_SampleRate_e eSampleRate)
{
	AudioBfConfig bf_config;
	unsigned int workingBufferSize;
	int ret = 0;

	memset(&bf_config, 0, sizeof(AudioBfConfig));
	bf_config.noise_gate_dbfs = -40;
	bf_config.temperature = 25;
	bf_config.noise_supression_mode = 8;
	bf_config.noise_estimation = 1;
	bf_config.output_gain = 0.7;
	bf_config.vad_enable = 0;

	workingBufferSize = IaaBf_GetBufferSize();
	pWorkingBuffer = (char*)malloc(workingBufferSize);
	if (!pWorkingBuffer)
	{
		printf("BF working buf malloc failed\n");
		return -1;
	}

	*pHandle = IaaBf_Init(pWorkingBuffer, pBfInit);
	if (!(*pHandle))
	{
		printf("BF init failed\n");
		return -1;
	}

	ret = IaaBf_Config(*pHandle, &bf_config);
	if (ret)
	{
		printf("BF config failed, ret=%d\n", ret);
		return -1;
	}

	return 0;
}
// audio in thread
// AMIC：get data(3chn mono) -> do aec(3ch mono) -> save wav(3ch mono)
// DMIC： get data(1chn stereo) -> do aec(1chn stereo) -> do bf(1chn mono) -> save wav(1ch mono)
static void *_SSTAR_AudioInGetDataProc_(void *pData)
{
	ThreadData_t *pThreadData = (ThreadData_t*)pData;
    MI_AUDIO_Frame_t stAudioFrame;
	MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32ToTalSize = 0;
    MI_S32 s32Ret = 0;
    FILE *pFile = NULL;
    char szFileName[64] = {0,};
    MI_SYS_ChnPort_t stChnPort;
	MI_S32 s32Fd = -1;
	fd_set read_fds;
	struct timeval TimeoutVal;
	WaveFileHeader_t waveHeader;
	int totalSize = 0;
	MI_AUDIO_SampleRate_e eSampleRate = g_eSampleRate;
	//int waitTime = 1000 * (int)g_eSampleRate / (int)E_MI_AUDIO_SAMPLE_RATE_8000 * MI_AUDIO_SAMPLE_PER_FRAME / (int)g_eSampleRate;
	int waitTime = 1000 / 16;

	// aec
	AEC_HANDLE handleAec;
	char *pAecWorkingBuffer = NULL;
	AudioAecInit aec_init;
	// bf
	BF_HANDLE handleBf;
	AudioBfInit bf_init;
	char *pBfWorkingBuffer = NULL;
	int delay_sample = 0;

	if (pThreadData->bDoAec)
	{
		memset(&aec_init, 0, sizeof(AudioAecInit));
		aec_init.point_number= 128;
		aec_init.nearend_channel = pThreadData->phyChnCnt;
		aec_init.farend_channel = pThreadData->phyChnCnt;
		aec_init.sample_rate = _SSTAR_AI_GetAecSampleRate(eSampleRate);

		if (_SSTAR_AI_InitAec(&handleAec, &aec_init, pAecWorkingBuffer, eSampleRate))
		{
			printf("AEC alg init error!\n");
			return NULL;
		}
	}

	if (pThreadData->bDoBf)
	{
		memset(&bf_init, 0, sizeof(AudioBfInit));
		bf_init.mic_distance = 12;
		bf_init.point_number = 128;
		bf_init.sample_rate = (unsigned int)eSampleRate;
		bf_init.channel = pThreadData->phyChnCnt;

		if (_SSTAR_AI_InitBf(&handleBf, &bf_init, pBfWorkingBuffer, eSampleRate))
		{
			printf("BF alg init error!\n");
			return NULL;
		}
	}


	memset(&stAudioFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    printf("pid=%ld\n", syscall(SYS_gettid));

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_AI;
    stChnPort.u32DevId = pThreadData->devId;
    stChnPort.u32ChnId = pThreadData->chnId;
    stChnPort.u32PortId = 0;
    s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);

    if(MI_SUCCESS != s32Ret)
    {
        printf("MI_SYS_GetFd err:%x, chn:%d\n", s32Ret, pThreadData->chnId);
        return NULL;
    }

    // write waveHeader struct to file
    if (pThreadData->fd > 0)
    {
#if SAVE_WAV
    	memset(&waveHeader, 0, sizeof(WaveFileHeader_t));
    	write(pThreadData->fd, &waveHeader, sizeof(WaveFileHeader_t));
#endif
    }

    while(!pThreadData->bExit)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);

        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret < 0)
        {
            printf("select failed!\n");
            continue;
        }
        else if(s32Ret == 0)
        {
            printf("get audio in frame time out\n");
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd, &read_fds))
            {
            	g_stAudioInAssembly.pfnAiGetFrame(pThreadData->devId, pThreadData->chnId, &stAudioFrame, &stAecFrame, waitTime);//1024 / 8000 = 128ms
                if (0 == stAudioFrame.u32Len)
                {
                    usleep(10 * 1000);
                    continue;
                }

                // save stAudioFrame to file
                if (pThreadData->fd > 0)
                {
                	if (pThreadData->bDoAec)
                	{
                		int ret = 0;
						char *pNearBuf = NULL, *pFarBuf = NULL;
						int nearBufSize = aec_init.point_number * aec_init.nearend_channel;	// short点数
						int farBufSize = aec_init.point_number * aec_init.farend_channel;	// short点数

                		for (int i = 0; i < (pThreadData->ptNumPerFrm*pThreadData->phyChnCnt/nearBufSize); i++)
                		{
                			pNearBuf = (char*)stAudioFrame.apVirAddr[0] + i*nearBufSize*2;	// 计算buffer字节偏移
                			pFarBuf = (char*)stAecFrame.stRefFrame.apVirAddr[0] + i*farBufSize*2;
                			ret = IaaAec_Run(handleAec, (short*)pNearBuf, (short*)pFarBuf);
							if (ret < 0)
							{
								printf("IaaAec run failed\n");
								break;
							}
                		}
                		//printf("AEC audioFrmLen=%d, aecFrmLen=%d\n", stAudioFrame.u32Len[0], stAecFrame.stRefFrame.u32Len[0]);
                	}

					if (pThreadData->bDoBf)
					{
						int ret = 0;
						char *pInBuf = NULL;
						int tempSize = bf_init.point_number * bf_init.channel;	// short点数

						for (int i = 0; i < (pThreadData->ptNumPerFrm*pThreadData->phyChnCnt/tempSize); i++)
						{
							pInBuf = (char*)stAudioFrame.apVirAddr[0] + i*tempSize*2;	// 计算buffer字节偏移
							ret = IaaBf_Run(handleBf, (short*)pInBuf, &delay_sample);
							if (ret < 0)
							{
								printf("IaaBf run failed\n");
								break;
							}

							write(pThreadData->fd, pInBuf, tempSize*2/2);	// shorts to bytes, the actual length is half of inputdata length
						}
					}
					else
					{
						write(pThreadData->fd, stAudioFrame.apVirAddr[0], stAudioFrame.u32Len[0]);
					}
                }

                if (pThreadData->bDoBf)
                	totalSize += stAudioFrame.u32Len[0]/2;
                else
                	totalSize += stAudioFrame.u32Len[0];

                g_stAudioInAssembly.pfnAiReleaseFrame(pThreadData->devId,  pThreadData->chnId, &stAudioFrame, &stAecFrame);
            }
        }
    }

    if (pThreadData->fd > 0)
    {
#if SAVE_WAV
    	memset(&waveHeader, 0, sizeof(WaveFileHeader_t));
    	if (pThreadData->devId == DMIC_DEV_ID && !pThreadData->bDoBf)
    	{
    		addWaveHeader(&waveHeader, PCM, E_MI_AUDIO_SOUND_MODE_STEREO, eSampleRate, totalSize);
    		printf("save Dmic stereo file, samplerate=%d, totalSize=%d\n", (int)eSampleRate, totalSize);
    	}
    	else
    	{
    		addWaveHeader(&waveHeader, PCM, E_MI_AUDIO_SOUND_MODE_MONO, eSampleRate, totalSize);
    		printf("save %s mono file, samplerate=%d, totalSize=%d\n", (pThreadData->devId == DMIC_DEV_ID)?"Dmic":"Amic", (int)eSampleRate, totalSize);
    	}
    	lseek(pThreadData->fd, 0, SEEK_SET);
    	write(pThreadData->fd, &waveHeader, sizeof(WaveFileHeader_t));
#endif
    	close(pThreadData->fd);
    }

    if (pThreadData->bDoBf)
    {
    	IaaBf_Free(handleBf);
    	free(pBfWorkingBuffer);
    }

    if (pThreadData->bDoAec)
    {
    	IaaAec_Free(handleAec);
    	free(pAecWorkingBuffer);
    }

    return NULL;
}

int SSTAR_AI_StartRecord(MI_AUDIO_DEV AiDevId, int gain, bool bEnableAec)
{
	MI_AI_CHN AiChn = 0;
	MI_AUDIO_Attr_t stAiSetAttr;
	MI_SYS_ChnPort_t stAiChn0OutputPort0;
	MI_AI_ChnParam_t stAiChnParam;
	int i = 0;
	char micTypePrefixe[8] = {0};

	CHECK_AI_DEV_ID(AiDevId);

	if (SSTAR_AI_OpenLibrary(&g_stAudioInAssembly))
	{
		printf("open libmi_ai failed\n");
		return -1;
	}

	memset(g_stAudioInThreadData, 0, sizeof(g_stAudioInThreadData));

	//set ai attr
	memset(&stAiSetAttr, 0, sizeof(MI_AUDIO_Attr_t));
	stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
	stAiSetAttr.eSamplerate = g_eSampleRate;
	if (AiDevId == AMIC_DEV_ID)
	{
		stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
		stAiSetAttr.u32ChnCnt = 4;
		strcpy(micTypePrefixe, "AMIC_");
	}
	else
	{
		stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
		stAiSetAttr.u32ChnCnt = 1;
		strcpy(micTypePrefixe, "DMIC_");
	}
	stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
	stAiSetAttr.u32PtNumPerFrm = ALIGN_UP(((int)g_eSampleRate / 64), 128);
	stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = 1;
	stAiSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_32;
	g_stAudioInAssembly.pfnAiSetPubAttr(AiDevId, &stAiSetAttr);
	g_stAudioInAssembly.pfnAiEnable(AiDevId);

	for (i = 0; i < stAiSetAttr.u32ChnCnt; i++)
    {
		//set ai output port depth
		memset(&stAiChn0OutputPort0, 0, sizeof(MI_SYS_ChnPort_t));
		stAiChn0OutputPort0.eModId = E_MI_MODULE_ID_AI;
		stAiChn0OutputPort0.u32DevId = AiDevId;
		stAiChn0OutputPort0.u32ChnId = i;
		stAiChn0OutputPort0.u32PortId = 0;
        MI_SYS_SetChnOutputPortDepth(&stAiChn0OutputPort0, 4, 8);

        memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
		stAiChnParam.stChnGain.bEnableGainSet = TRUE;

		stAiChnParam.stChnGain.s16FrontGain = gain;
		stAiChnParam.stChnGain.s16RearGain = 0;
		g_stAudioInAssembly.pfnAiSetChnParam(AiDevId, i, &stAiChnParam);
		g_stAudioInAssembly.pfnAiEnableChn(AiDevId, i);

		g_stAudioInThreadData[i].bExit = false;
		g_stAudioInThreadData[i].devId = AiDevId;
		g_stAudioInThreadData[i].chnId = i;
		g_stAudioInThreadData[i].bDoAec = bEnableAec;
		g_stAudioInThreadData[i].ptNumPerFrm = stAiSetAttr.u32PtNumPerFrm;
		if (stAiSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_MONO)
			g_stAudioInThreadData[i].phyChnCnt = 1;
		else
			g_stAudioInThreadData[i].phyChnCnt = 2;

		if ((AiDevId == DMIC_DEV_ID) && ((stAiSetAttr.eSamplerate == E_MI_AUDIO_SAMPLE_RATE_8000)
				|| (stAiSetAttr.eSamplerate == E_MI_AUDIO_SAMPLE_RATE_16000)))
			g_stAudioInThreadData[i].bDoBf = true;
		else
			g_stAudioInThreadData[i].bDoBf = false;

		// create record files
		char recordPath[256] = {0};
		sprintf(recordPath, "%s%s%d", AUDIO_IN_RECORD_PREFIX, micTypePrefixe, i);
		g_stAudioInThreadData[i].fd = open(recordPath, O_RDWR | O_CREAT | O_TRUNC, 0777);

		printf("thread %d, fd=%d\n", i, g_stAudioInThreadData[i].fd);
		pthread_create(&g_stAudioInThreadData[i].pt, NULL, _SSTAR_AudioInGetDataProc_, (void*)&g_stAudioInThreadData[i]);
    }

	return 0;
}

int SSTAR_AI_StopRecord(MI_AUDIO_DEV AiDevId, bool bEnableAec)
{
	if (!g_stAudioInAssembly.pHandle)
		return 0;

	for (int i = 0; i < AI_MAX_CHN_CNT; i++)
	{
		g_stAudioInThreadData[i].bExit = true;

		if (g_stAudioInThreadData[i].pt)
		{
			pthread_join(g_stAudioInThreadData[i].pt, NULL);
			g_stAudioInThreadData[i].pt = 0;
			g_stAudioInAssembly.pfnAiDisableChn(AiDevId, i);
		}
	}

	g_stAudioInAssembly.pfnAiDisable(AiDevId);
	g_stAudioInAssembly.pfnAiDeInitDev();

	SSTAR_AI_CloseLibrary(&g_stAudioInAssembly);

	return 0;
}

static void* SSTAR_aoSendFrame(void* pData)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;
    MI_U8* pu8TempBuf = NULL;
    ThreadData_t *pThreadData = (ThreadData_t*)pData;

    if (g_AoReadFd < 0)
    {
        return NULL;
    }

    pu8TempBuf = (MI_U8*)malloc(g_s32NeedSize);
	if (NULL == pu8TempBuf)
	{
		printf("Dev%d failed to alloc buffer.\n", AO_DEV_ID);
	}
	else
	{
		memset(pu8TempBuf, 0, sizeof(g_s32NeedSize));
	}

    while(FALSE == g_stAudioOutPlayFile.bExit)
    {
        s32Ret = read(g_AoReadFd, pu8TempBuf, g_s32NeedSize);
        if(s32Ret != g_s32NeedSize)
        {
            lseek(g_AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(g_AoReadFd, pu8TempBuf, g_s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }

        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        stAoSendFrame.apVirAddr[0] = pu8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do{
        	s32Ret = g_stAudioOutAssembly.pfnAoSendFrame(pThreadData->devId, pThreadData->chnId, &stAoSendFrame, 30);
        }while((s32Ret == MI_AO_ERR_NOBUF) && (FALSE == g_stAudioOutPlayFile.bExit));

        if(s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
        }
    }

	if (pu8TempBuf)
	{
		free(pu8TempBuf);
		pu8TempBuf = NULL;
	}

    return NULL;
}

static int SSTAR_AO_StartPlayFile(MI_AUDIO_DEV aoDevId, char *pPcmFile, int volume)
{
	MI_S32 s32Ret = MI_SUCCESS;
	MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
	MI_S32 s32AoGetVolume;
	MI_AO_ChnParam_t stAoChnParam;

	if (SSTAR_AO_OpenLibrary(&g_stAudioOutAssembly))
	{
		printf("open libmi_ao failed\n");
		return -1;
	}

	g_AoReadFd = open((const char *)pPcmFile, O_RDONLY, 0666);
	if(g_AoReadFd < 0)
	{
		printf("Open input file failed:%s \n", pPcmFile);
		printf("error:%s", strerror(errno));
		return -1;
	}

	s32Ret = read(g_AoReadFd, &g_stWavHeaderInput, sizeof(WaveFileHeader_t));
	if (s32Ret < 0)
	{
		printf("Read wav header failed!!!\n");
		return -1;
	}

	memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
	stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)g_stWavHeaderInput.wave.dwSamplesPerSec;
	stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
	stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
	stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;	//FALSE;
	stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
	stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
	stAoSetAttr.u32PtNumPerFrm = (int)stAoSetAttr.eSamplerate / (int)E_MI_AUDIO_SAMPLE_RATE_8000 * MI_AUDIO_SAMPLE_PER_FRAME;
	stAoSetAttr.u32ChnCnt = 1;		// max chn count is 1

	if(g_stWavHeaderInput.wave.wChannels == 2)
	{
		stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
	}
	else if(g_stWavHeaderInput.wave.wChannels == 1)
	{
		stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
	}

	printf("%s: samplerate=%d, u32PtNumPerFrm=%d, setChnCnt=%d, phyChnCnt=%d\n", pPcmFile, (int)stAoSetAttr.eSamplerate, stAoSetAttr.u32PtNumPerFrm,
			stAoSetAttr.u32ChnCnt, g_stWavHeaderInput.wave.wChannels);

	g_stAudioOutAssembly.pfnAoSetPubAttr(aoDevId, &stAoSetAttr);
	g_stAudioOutAssembly.pfnAoGetPubAttr(aoDevId, &stAoGetAttr);
	g_stAudioOutAssembly.pfnAoEnable(aoDevId);
	g_stAudioOutAssembly.pfnAoEnableChn(aoDevId, 0);
	g_stAudioOutAssembly.pfnAoSetVolume(aoDevId, 0, volume, E_MI_AO_GAIN_FADING_OFF);
	g_stAudioOutAssembly.pfnAoGetVolume(aoDevId, 0, &s32AoGetVolume);

	g_s32NeedSize = stAoSetAttr.u32PtNumPerFrm * 2 * stAoSetAttr.u32ChnCnt * g_stWavHeaderInput.wave.wChannels;
	g_s32NeedSize = g_s32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * g_stWavHeaderInput.wave.wChannels) * (stAoSetAttr.u32ChnCnt * 2 * g_stWavHeaderInput.wave.wChannels);

	g_stAudioOutPlayFile.bExit = FALSE;
	g_stAudioOutPlayFile.devId = aoDevId;
	g_stAudioOutPlayFile.chnId = 0;
	pthread_create(&g_stAudioOutPlayFile.pt, NULL, SSTAR_aoSendFrame, (void*)&g_stAudioOutPlayFile);
	printf("create ao thread.\n");

	return 0;
}

static int SSTAR_AO_StopPlayFile(MI_AUDIO_DEV aoDevId)
{
	if (!g_stAudioOutPlayFile.bExit)
	{
		g_stAudioOutPlayFile.bExit = TRUE;

		if (!g_stAudioOutAssembly.pHandle)
			return 0;

		if (g_stAudioOutPlayFile.pt)
		{
			pthread_join(g_stAudioOutPlayFile.pt, NULL);
			g_stAudioOutPlayFile.pt = 0;
		}

		 if (g_AoReadFd > 0)
		{
			close(g_AoReadFd);
			g_AoReadFd = -1;
		}

		g_stAudioOutAssembly.pfnAoDisableChn(aoDevId, 0);
		g_stAudioOutAssembly.pfnAoDisable(aoDevId);
		g_stAudioOutAssembly.pfnAoDeInitDev();

		SSTAR_AO_CloseLibrary(&g_stAudioOutAssembly);
	}

	return 0;
}

int SSTRR_AO_StartPlayRecord(int adcIdx)
{
	char recordPath[256] = {0};
	char micTypePrefixe[8] = {0};

	if (adcIdx < 3)
	{
		strcpy(micTypePrefixe, "AMIC_");
		sprintf(recordPath, "%s%s%d", AUDIO_IN_RECORD_PREFIX, micTypePrefixe, adcIdx);
	}
	else
	{
		strcpy(micTypePrefixe, "DMIC_");
		sprintf(recordPath, "%s%s%d", AUDIO_IN_RECORD_PREFIX, micTypePrefixe, (adcIdx-3));
	}

	return SSTAR_AO_StartPlayFile(AO_DEV_ID, recordPath, -5);
}

int SSTRR_AO_StopPlayRecord()
{
	return SSTAR_AO_StopPlayFile(AO_DEV_ID);
}

int SSTAR_AO_StartTestStereo()
{
	return SSTAR_AO_StartPlayFile(AO_DEV_ID, AUDIO_OUT_STEREO_FILE, -5);
}

int SSTAR_AO_StopTestStereo()
{
	return SSTAR_AO_StopPlayFile(AO_DEV_ID);
}

int SSTAR_AO_StartTestHeadPhone()
{
	return SSTAR_AO_StartPlayFile(AO_DEV_HEADPHONE, AUDIO_OUT_MONO_FILE, -5);
}

int SSTAR_AO_StopTestHeadPhone()
{
	return SSTAR_AO_StopPlayFile(AO_DEV_HEADPHONE);
}
