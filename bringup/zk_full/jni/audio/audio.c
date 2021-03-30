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

#define AI_DEV_ID			5		// 0:ADC0/1 chn=2; 4:ADC2 chn=1; 5:ADC0/1/2 chn=4
#define AO_DEV_ID			0		// 0:DAC0/1 chn=2; 2:DAC0 chn=1; 3:DAC1 chn=1;
#define AO_DEV_HEADPHONE	4		// 4:HeadPhone chn=2

#define AI_MAX_CHN_CNT		4
#define MI_AUDIO_SAMPLE_PER_FRAME 	256		//1024

#define AUDIO_IN_RECORD_PREFIX	AUDIO_IN_RECORD_DIR"/AudioIn_record_"
#define AUDIO_IN_RECORD(idx)	AUDIO_IN_RECORD_PREFIX#idx
#define AUDIO_OUT_MONO_FILE		"/customer/res/8K_16bit_MONO.wav"
#define AUDIO_OUT_STEREO_FILE	"/customer/res/8K_16bit_STERO_30s.wav"

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



int SSTAR_AI_SetSampleRate(MI_AUDIO_SampleRate_e eSampleRate)
{
	g_eSampleRate = eSampleRate;
	return 0;
}

// audio in thread
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
    	memset(&waveHeader, 0, sizeof(WaveFileHeader_t));
    	write(pThreadData->fd, &waveHeader, sizeof(WaveFileHeader_t));
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
                	write(pThreadData->fd, stAudioFrame.apVirAddr[0], stAudioFrame.u32Len[0]);
                }

                totalSize += stAudioFrame.u32Len[0];
                g_stAudioInAssembly.pfnAiReleaseFrame(pThreadData->devId,  pThreadData->chnId, &stAudioFrame, &stAecFrame);
            }
        }
    }

    if (pThreadData->fd > 0)
    {
    	memset(&waveHeader, 0, sizeof(WaveFileHeader_t));
    	addWaveHeader(&waveHeader, PCM, E_MI_AUDIO_SOUND_MODE_MONO, eSampleRate, totalSize);
    	lseek(pThreadData->fd, 0, SEEK_SET);
    	write(pThreadData->fd, &waveHeader, sizeof(WaveFileHeader_t));
    	close(pThreadData->fd);
    }

    return NULL;
}

int SSTAR_AI_StartRecord()
{
	MI_AUDIO_DEV AiDevId = AI_DEV_ID;
	MI_AI_CHN AiChn = 0;
	MI_AUDIO_Attr_t stAiSetAttr;
	MI_SYS_ChnPort_t stAiChn0OutputPort0;
	MI_AI_ChnParam_t stAiChnParam;
	int i = 0;

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
	stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
	stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
	stAiSetAttr.u32ChnCnt = 4;
	stAiSetAttr.u32PtNumPerFrm = (int)g_eSampleRate / 16;
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
		stAiChnParam.stChnGain.s16FrontGain = 15;
		stAiChnParam.stChnGain.s16RearGain = 0;
		g_stAudioInAssembly.pfnAiSetChnParam(AiDevId, i, &stAiChnParam);
		g_stAudioInAssembly.pfnAiEnableChn(AiDevId, i);

		g_stAudioInThreadData[i].bExit = false;
		g_stAudioInThreadData[i].devId = AiDevId;
		g_stAudioInThreadData[i].chnId = i;

		// create record files
		char recordPath[256] = {0};
		sprintf(recordPath, "%s%d", AUDIO_IN_RECORD_PREFIX, i);
		g_stAudioInThreadData[i].fd = open(recordPath, O_RDWR | O_CREAT | O_TRUNC, 0777);

		printf("thread %d, fd=%d\n", i, g_stAudioInThreadData[i].fd);
		pthread_create(&g_stAudioInThreadData[i].pt, NULL, _SSTAR_AudioInGetDataProc_, (void*)&g_stAudioInThreadData[i]);
    }

	return 0;
}

int SSTAR_AI_StopRecord()
{
	MI_AUDIO_DEV AiDevId = AI_DEV_ID;

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

	return 0;
}

int SSTRR_AO_StartPlayRecord(int adcIdx)
{
	char recordPath[256] = {0};
	sprintf(recordPath, "%s%d", AUDIO_IN_RECORD_PREFIX, adcIdx);

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
