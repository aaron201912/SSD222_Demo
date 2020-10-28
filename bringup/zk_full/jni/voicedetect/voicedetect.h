#ifndef __AUDIODETECT_H__
#define __AUDIODETECT_H__
#include "base_types.h"


#ifdef __cplusplus
extern "C" {
#endif


#define COMMAND_LEN					64

typedef enum
{
	E_TRIGGER_CMD = 0,
	E_COMMON_CMD
} CommandType_e;

typedef struct
{
	unsigned char cmd[COMMAND_LEN];
} TrainedWord_t;

typedef void* (*VoiceAnalyzeCallback)(CommandType_e, int);

int SSTAR_VoiceDetectInit(TrainedWord_t **ppTriggerCmdList, int *pnTriggerCmdCnt, TrainedWord_t **ppCommonCmdList, int *pnCommonCmdCnt);
int SSTAR_VoiceDetectDeinit();
int SSTAR_VoiceDetectStart(VoiceAnalyzeCallback pfnCallback);
void SSTAR_VoiceDetectStop();

#ifdef __cplusplus
}
#endif

#endif
