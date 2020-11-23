#include "mi_ao.h"
//#include "mi_vdec.h"
#include "mi_disp.h"
#include "mi_sys.h"
//#include "mi_vdec_datatype.h"
#include "mi_disp_datatype.h"
#include "mi_sys_datatype.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "panelconfig.h"

//========================== Add play audui file ============================
#define MI_AUDIO_SAMPLE_PER_FRAME 	256
#define DMA_BUF_SIZE_8K     		(8000)
#define DMA_BUF_SIZE_16K    		(16000)
#define DMA_BUF_SIZE_32K    		(32000)
#define DMA_BUF_SIZE_48K    		(48000)

//static int g_VdecRun = FALSE;
//static pthread_t g_VdeStream_tid = 0;

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
            s32Ret = MI_AO_SendFrame(g_AoDevId, g_AoChn, &stAoSendFrame, -1);
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

    ExecFunc(MI_AO_DisableChn(g_AoDevId, g_AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(g_AoDevId), MI_SUCCESS);
	ExecFunc(MI_AO_DeInitDev(),MI_SUCCESS);
	
    return 0;
}

MI_S32 SSTAR_StartPlayAudioFile(const char *WavAudioFile, MI_S32 s32AoVolume)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AO_AdecConfig_t stAoSetAdecConfig, stAoGetAdecConfig;
    MI_AO_VqeConfig_t stAoSetVqeConfig, stAoGetVqeConfig;
    MI_S32 s32AoGetVolume;
    MI_AO_ChnParam_t stAoChnParam;
    MI_U32 u32DmaBufSize;
    MI_AUDIO_SampleRate_e eAoInSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;

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
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
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
    eAoInSampleRate = (MI_AUDIO_SampleRate_e)g_stWavHeaderInput.wave.dwSamplesPerSec;

    stAoSetVqeConfig.bAgcOpen = FALSE;
    stAoSetVqeConfig.bAnrOpen = FALSE;
    stAoSetVqeConfig.bEqOpen = FALSE;
    stAoSetVqeConfig.bHpfOpen = FALSE;
    stAoSetVqeConfig.s32FrameSample = 128;
    stAoSetVqeConfig.s32WorkSampleRate = eAoInSampleRate;
    //memcpy(&stAoSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));
    //memcpy(&stAoSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));
    //memcpy(&stAoSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));
    //memcpy(&stAoSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));

    ExecFunc(MI_AO_SetPubAttr(g_AoDevId, &stAoSetAttr), MI_SUCCESS);
    ExecFunc(MI_AO_GetPubAttr(g_AoDevId, &stAoGetAttr), MI_SUCCESS);

    ExecFunc(MI_AO_Enable(g_AoDevId), MI_SUCCESS);

    ExecFunc(MI_AO_EnableChn(g_AoDevId, g_AoChn), MI_SUCCESS);

    if(FALSE)
    {
        ExecFunc(MI_AO_SetVqeAttr(g_AoDevId, g_AoChn, &stAoSetVqeConfig), MI_SUCCESS);
        ExecFunc(MI_AO_GetVqeAttr(g_AoDevId, g_AoChn, &stAoGetVqeConfig), MI_SUCCESS);
        ExecFunc(MI_AO_EnableVqe(g_AoDevId, g_AoChn), MI_SUCCESS);
    }

    ExecFunc(MI_AO_SetVolume(g_AoDevId, g_AoChn, s32AoVolume, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
    ExecFunc(MI_AO_GetVolume(g_AoDevId, g_AoChn, &s32AoGetVolume), MI_SUCCESS);

    g_s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * (stAoSetAttr.u32ChnCnt);
    if (E_MI_AUDIO_SAMPLE_RATE_8000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_8K;;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_16000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_16K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_32000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_32K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_48000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_48K;
    }

    if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_STEREO)
    {
        if (g_s32NeedSize > (u32DmaBufSize / 4))
        {
            g_s32NeedSize = u32DmaBufSize / 4;
        }
    }
    else if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_MONO)
    {
        if (g_s32NeedSize > (u32DmaBufSize / 8))
        {
            g_s32NeedSize = u32DmaBufSize / 8;
        }
    }

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
    SSTAR_LocalCameraDisp(1);
}

//void SSTAR_stopVideo()
void SSTAR_StopAudio()
{
    SSTAR_LocalCameraDisp(0);
}

void SSTAR_SetVolume(int vol) {
    MI_AO_SetVolume(0, 0, vol - 60, E_MI_AO_GAIN_FADING_OFF);
}
#ifdef __cplusplus
}
#endif // __cplusplus
