/*
 * audio.h
 *
 *  Created on: 2020年11月2日
 *      Author: sigma
 */

#ifndef JNI_AUDIO_AUDIO_H_
#define JNI_AUDIO_AUDIO_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "sstar_dynamic_load.h"

#define AMIC_DEV_ID				5		// 5:ADC0/1/2 chn=4
#define DMIC_DEV_ID				1		// 1:DMIC 4 physical chn; MONO mode, chn=4
#define AUDIO_IN_RECORD_DIR		"/customer/res/record"

int SSTAR_AI_SetSampleRate(MI_AUDIO_SampleRate_e eSampleRate);

int SSTAR_AI_StartRecord(MI_AUDIO_DEV AiDevId, int gain, bool bEnableAec);	// 录制音频
int SSTAR_AI_StopRecord(MI_AUDIO_DEV AiDevId, bool bEnableAec);
int SSTRR_AO_StartPlayRecord(int adcIdx);
int SSTRR_AO_StopPlayRecord();

int SSTAR_AO_StartTestStereo();	// 测试立体声
int SSTAR_AO_StopTestStereo();

int SSTAR_AO_StartTestHeadPhone();	// 测试耳机
int SSTAR_AO_StopTestHeadPhone();

#ifdef  __cplusplus
}
#endif
#endif /* JNI_AUDIO_AUDIO_H_ */
