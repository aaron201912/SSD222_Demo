#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <pthread.h>
#include "list.h"
#include "base_types.h"
#include "DSpotterApi.h"
#include "DSpotterApi_Const.h"
#include "appconfig.h"
#include "voicedetect.h"
#include "sstar_dynamic_load.h"
#include <dlfcn.h>


#define AUDIO_SAMPLE_PER_FRAME 		(256)
#define AUDIO_SAMPLE_RATE 			(E_MI_AUDIO_SAMPLE_RATE_16000)
#define AUDIO_SOUND_MODE 			(E_MI_AUDIO_SOUND_MODE_MONO)

#define AUDIO_AI_DEV_ID_AMIC_IN   	0
#define AUDIO_AI_DEV_ID_DMIC_IN   	1
#define AUDIO_AI_DEV_ID_I2S_IN    	2

#if USE_AMIC
#define AI_DEV_ID (AUDIO_AI_DEV_ID_AMIC_IN)
#else
#define AI_DEV_ID (AUDIO_AI_DEV_ID_DMIC_IN)
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
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)
#endif

#define COLOR_NONE          "\033[0m"
#define COLOR_BLACK         "\033[0;30m"
#define COLOR_BLUE          "\033[0;34m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_WHITE         "\033[1;37m"

#define ST_DBG(fmt, args...) \
    do { \
        printf(COLOR_GREEN "[DBG]:%s[%d]: " COLOR_NONE, __FUNCTION__,__LINE__); \
        printf(fmt, ##args); \
    }while(0)

#define ST_ERR(fmt, args...) \
	do { \
		printf(COLOR_RED "[ERR]:%s[%d]: " COLOR_NONE, __FUNCTION__,__LINE__); \
		printf(fmt, ##args); \
	}while(0)
#define AD_LOG(fmt, args...) {printf("\033[1;34m");printf("%s[%d]:", __FUNCTION__, __LINE__);printf(fmt, ##args);printf("\033[0m");}

#define MAX_LINE_LEN				1024
#define MAX_BUF_LEN					256
#define MAX_FRAME_QUEUE_DEPTH       64
#define SAMPLE_RATE					16000
#define MAX_COMMAN_NUM				31

typedef struct
{
	struct list_head    frameList;
    unsigned char*      pFrameData;
    int                 frameLen;
} VoiceFrame_t;

typedef struct
{
    list_t wordList;
    int index;
    char szWord[62];
}TrainingWordData_t;

typedef struct
{
	int exitWordNum;
	unsigned char **ppExitWord;
	int timeout;
	int result;
} ConfigSettings_t;

typedef struct
{
	bool bExit;
	pthread_t pt;
} ThreadData_t;

typedef struct
{
	HANDLE hDSpotter[2];
	VoiceAnalyzeCallback pfnCallback;
} UsrData_t;

typedef enum
{
	MODE_TRIGGER_WORD = 0,
	MODE_COMMAND
} CommandMode_e;

typedef struct
{
	void *pHandle;
	DSPDLL_API INT (*pfnDSpotterGetNumGroup)(char *lpchPackBin);
	DSPDLL_API HANDLE (*pfnDSpotterInitMultiWithPackBin)(char *lpchPackBin, BOOL *lpbEnableGroup, INT nMaxTime,
					   BYTE *lpbyState, INT nStateSize, INT *lpnErr, char *lpchLicenseFile, char *lpchServerFile);
	DSPDLL_API INT (*pfnDSpotterGetCommandNumber)(HANDLE hDSpotter);
	DSPDLL_API INT (*pfnDSpotterGetUTF8Command)(HANDLE hDSpotter, INT nCmdIdx, BYTE *lpbyCommand);
	DSPDLL_API INT (*pfnDSpotterReset)(HANDLE hDSpotter);
	DSPDLL_API INT (*pfnDSpotterAddSample)(HANDLE hDSpotter, SHORT *lpsSample, INT nNumSample);
	DSPDLL_API INT (*pfnDSpotterGetUTF8Result)(HANDLE hDSpotter, INT *lpnCmdIdx, BYTE *lpbyResult, INT *lpnWordDura,
					INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL);
	DSPDLL_API INT (*pfnDSpotterRelease)(HANDLE hDSpotter);
} DSpotterAssembly_t;

static int g_voiceFrameCnt = 0;
static list_t g_wordListHead;
static list_t g_voiceFrameListHead;
static pthread_mutex_t g_frameMutex;
static ThreadData_t g_stRecogThreadData;
static ThreadData_t g_stAudioInThreadData;
static ConfigSettings_t g_configSetting = {0};
static UsrData_t* g_pstUsrData = NULL;
//static int g_nSampleRate = SAMPLE_RATE;

static DSpotterAssembly_t g_stDSpotterAssembly;
static AudioInAssembly_t g_stAudioInAssembly;

// libDSpotter interface
static int OpenDSpotterLibrary()
{
	g_stDSpotterAssembly.pHandle = dlopen("libDSpotter.so", RTLD_NOW);
	if(NULL == g_stDSpotterAssembly.pHandle)
	{
		printf(" %s: Can not load libDSpotter.so!\n", __func__);
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterGetNumGroup = (DSPDLL_API INT(*)(char *lpchPackBin))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterGetNumGroup");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterGetNumGroup)
	{
		printf(" %s: dlsym DSpotterGetNumGroup failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterInitMultiWithPackBin = (DSPDLL_API HANDLE(*)(char *lpchPackBin, BOOL *lpbEnableGroup, INT nMaxTime,
			   BYTE *lpbyState, INT nStateSize, INT *lpnErr, char *lpchLicenseFile, char *lpchServerFile))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterInitMultiWithPackBin");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterInitMultiWithPackBin)
	{
		printf(" %s: dlsym DSpotterInitMultiWithPackBin failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterGetCommandNumber = (DSPDLL_API INT(*)(HANDLE hDSpotter))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterGetCommandNumber");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterGetCommandNumber)
	{
		printf(" %s: dlsym DSpotterGetCommandNumber failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterGetUTF8Command = (DSPDLL_API INT(*)(HANDLE hDSpotter, INT nCmdIdx, BYTE *lpbyCommand))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterGetUTF8Command");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterGetUTF8Command)
	{
		printf(" %s: dlsym DSpotterGetUTF8Command failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterRelease = (DSPDLL_API INT(*)(HANDLE hDSpotter))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterRelease");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterRelease)
	{
		printf(" %s: dlsym DSpotterRelease failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterReset = (DSPDLL_API INT(*)(HANDLE hDSpotter))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterReset");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterReset)
	{
		printf(" %s: dlsym DSpotterReset failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterAddSample = (DSPDLL_API INT(*)(HANDLE hDSpotter, SHORT *lpsSample, INT nNumSample))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterAddSample");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterAddSample)
	{
		printf(" %s: dlsym DSpotterAddSample failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stDSpotterAssembly.pfnDSpotterGetUTF8Result = (DSPDLL_API INT(*)(HANDLE hDSpotter, INT *lpnCmdIdx, BYTE *lpbyResult, INT *lpnWordDura,
			INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL))dlsym(g_stDSpotterAssembly.pHandle, "DSpotterGetUTF8Result");
	if(NULL == g_stDSpotterAssembly.pfnDSpotterGetUTF8Result)
	{
		printf(" %s: dlsym DSpotterGetUTF8Result failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

static void CloseDSpotterLibrary()
{
	if(g_stDSpotterAssembly.pHandle)
	{
		dlclose(g_stDSpotterAssembly.pHandle);
		g_stDSpotterAssembly.pHandle = NULL;
	}
	memset(&g_stDSpotterAssembly, 0, sizeof(g_stDSpotterAssembly));
}

// training word list opt
void InitTrainingWordList(list_t *listHead)
{
	INIT_LIST_HEAD(listHead);
}

void DeinitTrainingWordList(list_t *listHead)
{
	TrainingWordData_t *pos = NULL;
	TrainingWordData_t *posN = NULL;

    if (!list_empty(listHead))
    {
	    list_for_each_entry_safe(pos, posN, listHead, wordList)
	    {
		    list_del(&pos->wordList);
		    free(pos);
	    }
    }
}

void AddTrainingWord(int index, char *word, list_t *listHead)
{
	TrainingWordData_t *pTrainingWord = (TrainingWordData_t *)malloc(sizeof(TrainingWordData_t));
	memset(pTrainingWord, 0, sizeof(TrainingWordData_t));

	INIT_LIST_HEAD(&pTrainingWord->wordList);
	pTrainingWord->index = index;
	strcpy(pTrainingWord->szWord, word);
	list_add_tail(&pTrainingWord->wordList, listHead);
}

void DeleteTrainingWord(char *word, list_t *listHead)
{
	TrainingWordData_t *pos = NULL;
	TrainingWordData_t *posN = NULL;

	list_for_each_entry_safe(pos, posN, listHead, wordList)
	{
		if (!strcmp(pos->szWord, word))
		{
			list_del(&pos->wordList);
			free(pos);
			break;
		}
	}
}

int FindTrainingWord(char *word, list_t *listHead)
{
	TrainingWordData_t *pos = NULL;

	list_for_each_entry(pos, listHead, wordList)
	{
		if (!strcmp(pos->szWord, word))
		{
			return pos->index;
		}
	}

	return -1;
}

// voice frame list opt
void InitVoiceFrameQueue(void)
{
    INIT_LIST_HEAD(&g_voiceFrameListHead);
    pthread_mutex_init(&g_frameMutex, NULL);
    g_voiceFrameCnt = 0;
}

void DeinitVoiceFrameQueue(void)
{
    VoiceFrame_t *pstVoiceFrame = NULL;
	struct list_head *pListPos = NULL;
	struct list_head *pListPosN = NULL;

    pthread_mutex_lock(&g_frameMutex);
	list_for_each_safe(pListPos, pListPosN, &g_voiceFrameListHead)
	{
		pstVoiceFrame = list_entry(pListPos, VoiceFrame_t, frameList);
		list_del(pListPos);

        if (pstVoiceFrame->pFrameData)
        {
            free(pstVoiceFrame->pFrameData);
        }

		free(pstVoiceFrame);
	}
    pthread_mutex_unlock(&g_frameMutex);

    pthread_mutex_destroy(&g_frameMutex);
}

VoiceFrame_t* PopVoiceFrameFromQueue(void)
{
    VoiceFrame_t *pstVoiceFrame = NULL;
    struct list_head *pListPos = NULL;

    pthread_mutex_lock(&g_frameMutex);
    if (list_empty(&g_voiceFrameListHead))
    {
        pthread_mutex_unlock(&g_frameMutex);
        return NULL;
    }

    pListPos = g_voiceFrameListHead.next;

    g_voiceFrameListHead.next = pListPos->next;
    pListPos->next->prev = pListPos->prev;
    pthread_mutex_unlock(&g_frameMutex);

    pstVoiceFrame = list_entry(pListPos, VoiceFrame_t, frameList);

    return pstVoiceFrame;
}

void PutVoiceFrameToQueue(MI_AUDIO_Frame_t *pstAudioFrame)
{
    struct list_head *pListPos = NULL;
	struct list_head *pListPosN = NULL;
    VoiceFrame_t *pstVoiceFrame = NULL;
    int queueDepth = 0;

    if (pstAudioFrame == NULL)
    {
        return;
    }

    // calc depth
    pthread_mutex_lock(&g_frameMutex);
    list_for_each_safe(pListPos, pListPosN, &g_voiceFrameListHead)
	{
		queueDepth ++;
	}
    pthread_mutex_unlock(&g_frameMutex);

    // max depth check
    if (queueDepth >= MAX_FRAME_QUEUE_DEPTH)
    {
        // pop frame
        pstVoiceFrame = PopVoiceFrameFromQueue();
        if (pstVoiceFrame != NULL)
        {
            if (pstVoiceFrame->pFrameData != NULL)
            {
                free(pstVoiceFrame->pFrameData);
                pstVoiceFrame->pFrameData = NULL;
            }

            free(pstVoiceFrame);
            pstVoiceFrame = NULL;
        }
    }

    pstVoiceFrame = (VoiceFrame_t *)malloc(sizeof(VoiceFrame_t));
    if (pstVoiceFrame == NULL)
    {
        ST_ERR("malloc error, not enough memory\n");
        goto END;
    }
    memset(pstVoiceFrame, 0, sizeof(VoiceFrame_t));

    pstVoiceFrame->pFrameData = (unsigned char *)malloc(pstAudioFrame->u32Len[0]);
    if (pstVoiceFrame->pFrameData == NULL)
    {

        ST_ERR("malloc error, not enough memory\n");
        goto END;
    }
    memset(pstVoiceFrame->pFrameData, 0, pstAudioFrame->u32Len[0]);
    memcpy(pstVoiceFrame->pFrameData, pstAudioFrame->apVirAddr[0], pstAudioFrame->u32Len[0]);
    pstVoiceFrame->frameLen = pstAudioFrame->u32Len[0];

    // ST_DBG("pFrameData:%p, frameLen:%d\n", pstVoiceFrame->pFrameData, pstVoiceFrame->frameLen);

    pthread_mutex_lock(&g_frameMutex);
    list_add_tail(&pstVoiceFrame->frameList, &g_voiceFrameListHead);
    g_voiceFrameCnt++;
    pthread_mutex_unlock(&g_frameMutex);

    return;
END:
    if (pstVoiceFrame->pFrameData != NULL)
    {
        free(pstVoiceFrame->pFrameData);
        pstVoiceFrame->pFrameData = NULL;
    }

    if (pstVoiceFrame)
    {
        free(pstVoiceFrame);
        pstVoiceFrame = NULL;
    }
}


// audio in thread
static void *_SSTAR_AudioInGetDataProc_(void *pdata)
{
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;
    MI_AUDIO_Frame_t stAudioFrame;
	MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32ToTalSize = 0;
    MI_S32 s32Ret = 0;
    FILE *pFile = NULL;
    char szFileName[64] = {0,};

	memset(&stAudioFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    InitVoiceFrameQueue();
    ST_DBG("pid=%ld\n", syscall(SYS_gettid));

    MI_SYS_ChnPort_t stChnPort;
    MI_S32 s32Fd = -1;
    fd_set read_fds;
    struct timeval TimeoutVal;

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_AI;
    stChnPort.u32DevId = AiDevId;
    stChnPort.u32ChnId = AiChn;
    stChnPort.u32PortId = 0;
    s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);

    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_GetFd err:%x, chn:%d\n", s32Ret, AiChn);
        return NULL;
    }

    while(!g_stAudioInThreadData.bExit)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);

        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret < 0)
        {
            ST_ERR("select failed!\n");
            //  usleep(10 * 1000);
            continue;
        }
        else if(s32Ret == 0)
        {
            ST_ERR("get audio in frame time out\n");
            //usleep(10 * 1000);
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd, &read_fds))
            {
                g_stAudioInAssembly.pfnAiGetFrame(AiDevId, AiChn, &stAudioFrame, &stAecFrame, 1000 / 16);
                if (0 == stAudioFrame.u32Len)
                {
                    usleep(10 * 1000);
                    continue;
                }

                PutVoiceFrameToQueue(&stAudioFrame);		// save

                g_stAudioInAssembly.pfnAiReleaseFrame(AiDevId,  AiChn, &stAudioFrame, &stAecFrame);
            }
        }
    }

    DeinitVoiceFrameQueue();

    return NULL;
}


// aidev opt
static MI_S32 SSTAR_AudioInStart()
{
    MI_S32 s32Ret = MI_SUCCESS, i;

    //Ai
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;
    MI_AUDIO_Attr_t stAiSetAttr;
    MI_SYS_ChnPort_t stAiChn0OutputPort0;
    MI_AI_ChnParam_t stAiChnParam;

    memset(&g_stAudioInAssembly, 0, sizeof(AudioInAssembly_t));

    if (SSTAR_AI_OpenLibrary(&g_stAudioInAssembly))
	{
		printf("open libmi_ai failed\n");
		return -1;
	}

    //set ai attr
    memset(&stAiSetAttr, 0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = AUDIO_SAMPLE_RATE;
    stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAiSetAttr.u32ChnCnt = 1;
    stAiSetAttr.u32PtNumPerFrm = (int)stAiSetAttr.eSamplerate / 16;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = 1;
	stAiSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_32;
	ExecFunc(g_stAudioInAssembly.pfnAiSetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS);
    ExecFunc(g_stAudioInAssembly.pfnAiEnable(AiDevId), MI_SUCCESS);
	
    //set ai output port depth
    memset(&stAiChn0OutputPort0, 0, sizeof(MI_SYS_ChnPort_t));
    stAiChn0OutputPort0.eModId = E_MI_MODULE_ID_AI;
    stAiChn0OutputPort0.u32DevId = AiDevId;
    stAiChn0OutputPort0.u32ChnId = AiChn;
    stAiChn0OutputPort0.u32PortId = 0;

	ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChn0OutputPort0, 4, 8), MI_SUCCESS);
	
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
	stAiChnParam.stChnGain.bEnableGainSet = TRUE;
#if USE_AMIC
	stAiChnParam.stChnGain.s16FrontGain = 15;
	stAiChnParam.stChnGain.s16RearGain = 0;
#else
	stAiChnParam.stChnGain.s16FrontGain = 3;
	stAiChnParam.stChnGain.s16RearGain = 0;
#endif
	ExecFunc(g_stAudioInAssembly.pfnAiSetChnParam(AiDevId, AiChn, &stAiChnParam), MI_SUCCESS);
    ExecFunc(g_stAudioInAssembly.pfnAiEnableChn(AiDevId, AiChn), MI_SUCCESS);

    g_stAudioInThreadData.bExit = false;
    s32Ret = pthread_create(&g_stAudioInThreadData.pt, NULL, _SSTAR_AudioInGetDataProc_, NULL);
    if(0 != s32Ret)
    {
         ST_ERR("create thread failed\n");
         return -1;
    }

    return MI_SUCCESS;
}

static MI_S32 SSTAR_AudioInStop()
{
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;

	if (!g_stAudioInAssembly.pHandle)
    	return MI_SUCCESS;
    g_stAudioInThreadData.bExit = true;
    pthread_join(g_stAudioInThreadData.pt, NULL);

    ExecFunc(g_stAudioInAssembly.pfnAiDisableChn(AiDevId, AiChn), MI_SUCCESS);
    ExecFunc(g_stAudioInAssembly.pfnAiDisable(AiDevId), MI_SUCCESS);
    ExecFunc(g_stAudioInAssembly.pfnAiDeInitDev(),MI_SUCCESS);
    SSTAR_AI_CloseLibrary(&g_stAudioInAssembly);

    return MI_SUCCESS;
}

static BOOL ReadConfigSettingINI(char *lpchFilePath, ConfigSettings_t *lpSettingArg)
{
	FILE *fp = NULL;
	char *pchTokenStr;
	char pchLineStr[MAX_LINE_LEN];
	unsigned char **ppbyExitWord = NULL;
	int nNumExitWord = 0, i;

	fp = fopen(lpchFilePath, "r");
	if(fp == NULL)
	{
		printf("ReadConfigSettingINI():: Fail to open file!\n");
		goto ERR_READ_DEMO_SETTING_INI;
	}

	while(NULL != fgets(pchLineStr, MAX_LINE_LEN, fp))
	{
		pchTokenStr = strtok(pchLineStr, "\r\n");
		if(pchTokenStr == NULL || strlen(pchTokenStr) == 0)
					continue;

		if(strstr(pchTokenStr, "//Timeout value(millisecond)"))
		{
			fgets(pchLineStr, MAX_LINE_LEN, fp);
			pchTokenStr = strtok(pchLineStr, "\r\n");
			lpSettingArg->timeout = atoi(pchTokenStr);
		}
		else if(strstr(pchTokenStr, "//Exit word"))
		{
			while(NULL != fgets(pchLineStr, MAX_LINE_LEN, fp))
			{
				pchTokenStr = strtok(pchLineStr, "\r\n");
				if(pchTokenStr == NULL || strlen(pchTokenStr) == 0)
					break;
				nNumExitWord++;
			}
		}
		else
		{
			continue;
		}
	}

	if(nNumExitWord == 0)
	{
		fclose(fp);
		return TRUE;
	}

	ppbyExitWord = (BYTE **)malloc(sizeof(BYTE *) * nNumExitWord);
	if(ppbyExitWord == NULL)
	{
		printf("ReadConfigSettingINI():: Leave no memory!");
		goto ERR_READ_DEMO_SETTING_INI;
	}
	memset(ppbyExitWord, 0, sizeof(BYTE *) * nNumExitWord);

	for(i = 0; i < nNumExitWord; i++)
	{
		ppbyExitWord[i] = (BYTE *)malloc(sizeof(BYTE) * COMMAND_LEN);
		if(ppbyExitWord[i] == NULL)
		{
			printf("ReadConfigSettingINI():: Leave no memory!");
			goto ERR_READ_DEMO_SETTING_INI;
		}
	}

	nNumExitWord = 0;
	fseek(fp, 0, SEEK_SET);
	while( NULL != fgets(pchLineStr, MAX_LINE_LEN, fp))
	{
		pchTokenStr = strtok(pchLineStr, "\r\n");
		if(pchTokenStr == NULL || strlen(pchTokenStr) == 0)
			continue;
		if(strstr(pchTokenStr, "//Exit word"))
		{
			while(NULL != fgets(pchLineStr, MAX_LINE_LEN, fp))
			{
				pchTokenStr = strtok(pchLineStr, "\r\n");
				if(pchTokenStr == NULL || strlen(pchTokenStr) == 0)
					break;

				strcpy((char*)ppbyExitWord[nNumExitWord++], pchTokenStr);
			}
		}
		else
		{
			continue;
		}
	}

	fclose(fp);
	lpSettingArg->ppExitWord = ppbyExitWord;
	lpSettingArg->exitWordNum = nNumExitWord;

	return TRUE;


ERR_READ_DEMO_SETTING_INI:
	if(fp)
		fclose(fp);
	if(ppbyExitWord)
	{
		for(i = 0; i < nNumExitWord; i++)
			SAFE_FREE(ppbyExitWord[i]);
		free(ppbyExitWord);
	}

	return FALSE;
}

static void ReleaseConfigSettingData()
{
	int i;

	if(g_configSetting.ppExitWord != NULL)
	{
		for(i = 0; i < g_configSetting.exitWordNum; i++)
		{
			free(g_configSetting.ppExitWord[i]);
		}
		free(g_configSetting.ppExitWord);
		g_configSetting.ppExitWord = NULL;
	}
}

static BOOL IsExitWord(unsigned char *lpbyResult)
{
	int i;

	for(i = 0; i < g_configSetting.exitWordNum; i++)
	{
		if(strcmp((const char*)lpbyResult, (const char*)g_configSetting.ppExitWord[i]) == 0)
			return TRUE;
	}

	return FALSE;
}

int SSTAR_VoiceDetectDeinit()
{
	if (g_pstUsrData)
	{
		g_stDSpotterAssembly.pfnDSpotterRelease(g_pstUsrData->hDSpotter[0]);
		g_stDSpotterAssembly.pfnDSpotterRelease(g_pstUsrData->hDSpotter[1]);
		free(g_pstUsrData);
		g_pstUsrData = NULL;
	}

	ReleaseConfigSettingData();
	CloseDSpotterLibrary();

	return 0;
}

// init时会获取唤醒词条列表和指令词条列表，不再需要预先设置词条列表数组
int SSTAR_VoiceDetectInit(TrainedWord_t **ppTriggerCmdList, int *pnTriggerCmdCnt,
						  TrainedWord_t **ppCommonCmdList, int *pnCommonCmdCnt)
{
	int nErr = 0;
	HANDLE hDSpotter[2] = {0};
	char pchDataPath[] = "./Data";
	char pchPackBinFile[MAX_BUF_LEN];
	char pchLicenseBinFile[MAX_BUF_LEN];
	char pchDemoSettingIniFile[MAX_BUF_LEN];
	unsigned char pLastCmd[COMMAND_LEN];
	int i;
	BOOL *pbEnableGroup = NULL;

	if (OpenDSpotterLibrary())
	{
		printf("open libDSpotter failed\n");
		goto exit_init;
	}

	sprintf(pchPackBinFile, "%s/DefCmd_pack_withTxt_0804", DSPOTTER_DATA_PATH);
	sprintf(pchLicenseBinFile, "%s/CybLicense.bin", DSPOTTER_DATA_PATH);
	sprintf(pchDemoSettingIniFile, "%s/DemoSettings_0804.ini", DSPOTTER_DATA_PATH);

	printf("PackBinFile:%s\n", pchPackBinFile);
	printf("LicenseFile:%s\n", pchLicenseBinFile);
	printf("DemoSettingIniFile:%s\n", pchDemoSettingIniFile);

	memset(&g_configSetting, 0, sizeof(ConfigSettings_t));
	ReadConfigSettingINI(pchDemoSettingIniFile, &g_configSetting);

	pbEnableGroup = (BOOL *)malloc(sizeof(BOOL) * g_stDSpotterAssembly.pfnDSpotterGetNumGroup(pchPackBinFile));
	if(!pbEnableGroup)
	{
		printf("main():: Leave no memory!\n");
		goto exit_init;
	}

	pbEnableGroup[0] = TRUE;
	pbEnableGroup[1] = FALSE;
	hDSpotter[0] = g_stDSpotterAssembly.pfnDSpotterInitMultiWithPackBin(pchPackBinFile, pbEnableGroup, 500, NULL, 0, &nErr, pchLicenseBinFile, NULL);
	if(hDSpotter[0] == NULL)
	{
		printf("main():: Fail to initialize DSpotter ( %d )!\n", nErr);
		goto exit_init;
	}

	pbEnableGroup[0] = FALSE;
	pbEnableGroup[1] = TRUE;
	hDSpotter[1] = g_stDSpotterAssembly.pfnDSpotterInitMultiWithPackBin(pchPackBinFile, pbEnableGroup, 500, NULL, 0, &nErr, pchLicenseBinFile, NULL);
	if(hDSpotter[1] == NULL)
	{
		printf("main():: Fail to initialize DSpotter ( %d )!\n", nErr);
		goto exit_init;
	}

	*pnTriggerCmdCnt = g_stDSpotterAssembly.pfnDSpotterGetCommandNumber(hDSpotter[0]);
	*ppTriggerCmdList = (TrainedWord_t*)malloc(sizeof(TrainedWord_t) * (*pnTriggerCmdCnt));
	memset(*ppTriggerCmdList, 0, sizeof(TrainedWord_t) * (*pnTriggerCmdCnt));

	// filter repeat commands
	pLastCmd[0] = 0;
	printf("========== Trigger Word ==========\n");
	for(i = 0; i < (*pnTriggerCmdCnt); i++)
	{
		g_stDSpotterAssembly.pfnDSpotterGetUTF8Command(hDSpotter[0], i, (*ppTriggerCmdList)[i].cmd);
		if(strcmp((const char*)pLastCmd, (const char*)(*ppTriggerCmdList)[i].cmd) != 0)
		{
			printf("%s\n", (*ppTriggerCmdList)[i].cmd);
			strcpy((char*)pLastCmd, (const char*)(*ppTriggerCmdList)[i].cmd);
		}
	}
	printf("==================================\n");

	*pnCommonCmdCnt = g_stDSpotterAssembly.pfnDSpotterGetCommandNumber(hDSpotter[1]);
	if (*pnCommonCmdCnt > MAX_COMMAN_NUM)
		*pnCommonCmdCnt = MAX_COMMAN_NUM;

	*ppCommonCmdList = (TrainedWord_t*)malloc(sizeof(TrainedWord_t) * (*pnCommonCmdCnt));
	pLastCmd[0] = 0;
	printf("========== Command List ==========\n");
	for(i = 0; i < (*pnCommonCmdCnt); i++)
	{
		g_stDSpotterAssembly.pfnDSpotterGetUTF8Command(hDSpotter[1], i, (*ppCommonCmdList)[i].cmd);
		if(strcmp((const char*)pLastCmd, (const char*)(*ppCommonCmdList)[i].cmd) != 0)
		{
			printf("%s\n", (*ppCommonCmdList)[i].cmd);
			strcpy((char*)pLastCmd, (const char*)(*ppCommonCmdList)[i].cmd);
		}
	}
	printf("==================================\n");

	//g_nSampleRate = DSpotterGetSampleRate(hDSpotter[0]);

	g_pstUsrData = (UsrData_t*)malloc(sizeof(UsrData_t));
	if(g_pstUsrData == NULL)
	{
		printf("main():: Leave no memory!\n");
		goto exit_init;
	}

	memset(g_pstUsrData, 0, sizeof(UsrData_t));
	g_pstUsrData->hDSpotter[0] = hDSpotter[0];
	g_pstUsrData->hDSpotter[1] = hDSpotter[1];

	SAFE_FREE(pbEnableGroup);

	return 0;

exit_init:
	if (hDSpotter[0])
	{
		g_stDSpotterAssembly.pfnDSpotterRelease(hDSpotter[0]);
		hDSpotter[0] = NULL;
	}
	if (hDSpotter[1])
	{
		g_stDSpotterAssembly.pfnDSpotterRelease(hDSpotter[1]);
		hDSpotter[1] = NULL;
	}
	CloseDSpotterLibrary();
	SAFE_FREE(g_pstUsrData);
	ReleaseConfigSettingData();
	SAFE_FREE(pbEnableGroup);
	SAFE_FREE(*ppCommonCmdList);
	SAFE_FREE(*ppTriggerCmdList);
	return -1;
}

static void *_SSTAR_VoiceAnalyzeProc(void *pData)
{
    VoiceFrame_t *pstVoiceFrame = NULL;
	UsrData_t *pUsrData = (UsrData_t*)pData;
    int ret = 0;
	int nCmdIdx, nWordDura, nEndSilDura, nNetworkLatency;
	int nGMM, nSG, nFIL;
	unsigned char pbyResult[COMMAND_LEN];
	CommandMode_e eMode = MODE_TRIGGER_WORD;
	struct timespec recogTime, curTime;

    AD_LOG("Enter _SSTAR_VoiceAnalyzeProc_\n");

    if ((ret = g_stDSpotterAssembly.pfnDSpotterReset(pUsrData->hDSpotter[0])) != DSPOTTER_SUCCESS)
    {
        printf("DSpotter0 Reset: Fail to start recognition (%d)\n", ret);
        return NULL;
    }

    if((ret = g_stDSpotterAssembly.pfnDSpotterReset(pUsrData->hDSpotter[1])) != DSPOTTER_SUCCESS)
	{
		printf("DSpotter1 Reset: Fail to start recognition ( %d )!\n", ret);
		return NULL;
	}

    InitVoiceFrameQueue();

    while (!g_stRecogThreadData.bExit)
    {
        pstVoiceFrame = PopVoiceFrameFromQueue();

        if (!pstVoiceFrame)
        {
            usleep(1000*10);
            continue;
        }

        if(eMode == MODE_COMMAND)
		{
			unsigned long long timeInterval = 0;
			memset(&curTime, 0, sizeof(curTime));
			clock_gettime(CLOCK_MONOTONIC, &curTime);
			timeInterval = (unsigned long long)((curTime.tv_sec - recogTime.tv_sec) * 1000)
						   + (unsigned long long)((curTime.tv_nsec - recogTime.tv_nsec) / 1000000);
//			printf("curTime: %ld s, %ld ns\n", curTime.tv_sec, curTime.tv_nsec);
//			printf("recogTime: %ld s, %ld ns\n", recogTime.tv_sec, recogTime.tv_nsec);
//			printf("tmInterval:%lld, timeout:%d\n", timeInterval, g_configSetting.timeout);

			if (timeInterval >=  g_configSetting.timeout)
			{
				printf("Timeout!\n");
				eMode = MODE_TRIGGER_WORD;
				memset(&recogTime, 0, sizeof(recogTime));
				clock_gettime(CLOCK_MONOTONIC, &recogTime);
				pUsrData->pfnCallback(E_TRIGGER_CMD, -1);	// timeout
			}
		}

		if(eMode == MODE_TRIGGER_WORD)
		{
			ret = g_stDSpotterAssembly.pfnDSpotterAddSample(pUsrData->hDSpotter[0], (short*)pstVoiceFrame->pFrameData, pstVoiceFrame->frameLen/sizeof(short));

			if(ret == DSPOTTER_SUCCESS)
			{
				printf("Get trigger word!\n");

				g_stDSpotterAssembly.pfnDSpotterGetUTF8Result(pUsrData->hDSpotter[0], &nCmdIdx, pbyResult, &nWordDura, &nEndSilDura, &nNetworkLatency, &nGMM, &nSG, &nFIL);
				printf("Trigger word: %s \n", pbyResult);
				printf("Command index: %d\n", nCmdIdx);
				printf("Word duration: %d, End silence duration: %d, Network latency: %d\n", nWordDura, nEndSilDura, nNetworkLatency);
				printf("Score: %d\n", nGMM);
				pUsrData->pfnCallback(E_TRIGGER_CMD, nCmdIdx);

				g_stDSpotterAssembly.pfnDSpotterReset(pUsrData->hDSpotter[0]);

				eMode = MODE_COMMAND;
				memset(&recogTime, 0, sizeof(recogTime));
				clock_gettime(CLOCK_MONOTONIC, &recogTime);
			}
		}
		else
		{
			ret = g_stDSpotterAssembly.pfnDSpotterAddSample(pUsrData->hDSpotter[1], (short*)pstVoiceFrame->pFrameData, pstVoiceFrame->frameLen/sizeof(short));

			if(ret == DSPOTTER_SUCCESS)
			{
				printf("Get command!\n");

				g_stDSpotterAssembly.pfnDSpotterGetUTF8Result(pUsrData->hDSpotter[1], &nCmdIdx, pbyResult, &nWordDura, &nEndSilDura, &nNetworkLatency, &nGMM, &nSG, &nFIL);
				printf("Command: %s \n", pbyResult);
				printf("Command index: %d\n", nCmdIdx);
				printf("Word duration: %d, End silence duration: %d, Network latency: %d\n", nWordDura, nEndSilDura, nNetworkLatency);
				printf("Score: %d\n", nGMM);
				pUsrData->pfnCallback(E_COMMON_CMD, nCmdIdx);

				g_stDSpotterAssembly.pfnDSpotterReset(pUsrData->hDSpotter[1]);

				if(IsExitWord(pbyResult))
					eMode = MODE_TRIGGER_WORD;

				memset(&recogTime, 0, sizeof(recogTime));
				clock_gettime(CLOCK_MONOTONIC, &recogTime);
			}
		}

        if (pstVoiceFrame != NULL)
        {
            if (pstVoiceFrame->pFrameData != NULL)
            {
                free(pstVoiceFrame->pFrameData);
                pstVoiceFrame->pFrameData = NULL;
            }

            free(pstVoiceFrame);
            pstVoiceFrame = NULL;
        }
    }

    DeinitVoiceFrameQueue();

    return NULL;
}

int SSTAR_VoiceDetectStart(VoiceAnalyzeCallback pfnCallback)
{
	if (MI_SUCCESS != SSTAR_AudioInStart())
	{
		printf("aidev init failed\n");
		return -1;
	}

	if (!g_pstUsrData)
	{
		printf("DSpotter has not inited\n");
		return -1;
	}

	g_stRecogThreadData.bExit = false;
	g_pstUsrData->pfnCallback = pfnCallback;
	pthread_create(&g_stRecogThreadData.pt, NULL, _SSTAR_VoiceAnalyzeProc, (void*)g_pstUsrData);

	return 0;
}

void SSTAR_VoiceDetectStop()
{
	if (!g_stRecogThreadData.bExit)
	{
		g_stRecogThreadData.bExit = true;
		pthread_join(g_stRecogThreadData.pt, NULL);
	}

	SSTAR_AudioInStop();
}
