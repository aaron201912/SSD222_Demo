#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "sstar_dynamic_load.h"

//========================== Add play audui file ============================
#define MI_AUDIO_SAMPLE_PER_FRAME 	256
#define DMA_BUF_SIZE_8K     		(8000)
#define DMA_BUF_SIZE_16K    		(16000)
#define DMA_BUF_SIZE_32K    		(32000)
#define DMA_BUF_SIZE_48K    		(48000)

#define MI_U32VALUE(pu8Data, index) (pu8Data[index]<<24)|(pu8Data[index+1]<<16)|(pu8Data[index+2]<<8)|(pu8Data[index+3])
#define ST_DBG printf

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
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)
#endif


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

static MI_S32 g_AoReadFd = -1;
static MI_S32 g_AoDevId = 0;
static MI_S32 g_AoChn = 0;
static WaveFileHeader_t g_stWavHeaderInput;
static MI_S32 g_s32NeedSize = 0;
static pthread_t tid_playaudio;
static MI_BOOL bAoExit;
MI_U8 u8TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 2];
static AudioOutAssembly_t g_stAudioOutAssembly;

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
void* SSTAR_aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;
    if (g_AoReadFd < 0)
    {
        return NULL;
    }
    bAoExit = FALSE;
	
    while(FALSE == bAoExit)
    {
        s32Ret = read(g_AoReadFd, &u8TempBuf, g_s32NeedSize);
        if(s32Ret != g_s32NeedSize)
        {
            lseek(g_AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(g_AoReadFd, &u8TempBuf, g_s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }
        
        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        stAoSendFrame.apVirAddr[0] = u8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do{
            s32Ret = g_stAudioOutAssembly.pfnAoSendFrame(g_AoDevId, g_AoChn, &stAoSendFrame, -1);
        }while(s32Ret == MI_AO_ERR_NOBUF);

        if(s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
        }
		
    }
    if (g_AoReadFd > 0)
    {
        close(g_AoReadFd);
    }
    return NULL;
}

MI_S32 SSTAR_StopPlayAudioFile(void)
{
    bAoExit = TRUE;
    if (tid_playaudio)
    {
    	pthread_join(tid_playaudio, NULL);
    	tid_playaudio = 0;
    }

    ExecFunc(g_stAudioOutAssembly.pfnAoDisableChn(g_AoDevId, g_AoChn), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoDisable(g_AoDevId), MI_SUCCESS);
	ExecFunc(g_stAudioOutAssembly.pfnAoDeInitDev(),MI_SUCCESS);
	
    return 0;
}

MI_S32 SSTAR_StartPlayAudioFile(const char *WavAudioFile, MI_S32 s32AoVolume)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_S32 s32AoGetVolume;
    MI_AO_ChnParam_t stAoChnParam;
    MI_U32 u32DmaBufSize;

    g_AoReadFd = open((const char *)WavAudioFile, O_RDONLY, 0666);
    if(g_AoReadFd <= 0)
    {
        printf("Open input file failed:%s \n", WavAudioFile);
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
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;

    stAoSetAttr.u32ChnCnt = 1;

    if(g_stWavHeaderInput.wave.wChannels == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if(g_stWavHeaderInput.wave.wChannels == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)g_stWavHeaderInput.wave.dwSamplesPerSec;
    stAoSetAttr.u32PtNumPerFrm = (int)stAoSetAttr.eSamplerate / (int)E_MI_AUDIO_SAMPLE_RATE_8000 * MI_AUDIO_SAMPLE_PER_FRAME;

    ExecFunc(g_stAudioOutAssembly.pfnAoSetPubAttr(g_AoDevId, &stAoSetAttr), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoGetPubAttr(g_AoDevId, &stAoGetAttr), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoEnable(g_AoDevId), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoEnableChn(g_AoDevId, g_AoChn), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoSetVolume(g_AoDevId, g_AoChn, s32AoVolume, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
    ExecFunc(g_stAudioOutAssembly.pfnAoGetVolume(g_AoDevId, g_AoChn, &s32AoGetVolume), MI_SUCCESS);

    g_s32NeedSize = stAoSetAttr.u32PtNumPerFrm * 2 * stAoSetAttr.u32ChnCnt * g_stWavHeaderInput.wave.wChannels;
    g_s32NeedSize = g_s32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * g_stWavHeaderInput.wave.wChannels) * (stAoSetAttr.u32ChnCnt * 2 * g_stWavHeaderInput.wave.wChannels);

    pthread_create(&tid_playaudio, NULL, SSTAR_aoSendFrame, NULL);
    printf("create ao thread.\n");

    return 0;
}



void SSTAR_LocalCameraDisp(MI_S32 s32Disp)
{
    if (s32Disp)
    {
        #if 1
        if ((access("8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("8K_16bit_MONO.wav", 2);
            ST_DBG("open current dir wav file\n");
        }
        else if ((access("/customer/res/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("/customer/res/8K_16bit_MONO.wav", 2);
            ST_DBG("open /custome dir wav file\n");
        }
        else if ((access("/config/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("/config/8K_16bit_MONO.wav", 2);
            ST_DBG("open /config dir wav file\n");
        }
        #endif
        ST_DBG("Bind ST_LocalCameraDisp...\n");
    }
    else
    {
        #if 1
        if ((access("8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open current dir wav file\n");
        }
        else if ((access("/customer/res/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open /customer dir wav file\n");
        }
        else if ((access("/config/8K_16bit_MONO.wav", F_OK)) != -1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open /config dir wav file\n");
        }
        #endif
        ST_DBG("UnBind ST_LocalCameraDisp...\n");
    }

    return;
}

//void SSTAR_playVideo()
void SSTAR_PlayAudio()
{
	memset(&g_stAudioOutAssembly, 0, sizeof(AudioOutAssembly_t));

	if (SSTAR_AO_OpenLibrary(&g_stAudioOutAssembly))
	{
		printf("open libmi_ao failed\n");
		return;
	}

    SSTAR_LocalCameraDisp(1);
}

//void SSTAR_stopVideo()
void SSTAR_StopAudio()
{
	if (!g_stAudioOutAssembly.pHandle)
		return;

    SSTAR_LocalCameraDisp(0);

    SSTAR_AO_CloseLibrary(&g_stAudioOutAssembly);
}

void SSTAR_SetVolume(int vol) {
    g_stAudioOutAssembly.pfnAoSetVolume(0, 0, vol - 60, E_MI_AO_GAIN_FADING_OFF);
}
#ifdef __cplusplus
}
#endif // __cplusplus
