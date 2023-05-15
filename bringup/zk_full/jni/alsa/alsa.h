/*
 * alsa.h
 *
 *  Created on: 2023年5月8日
 *      Author: koda.xu
 */

#ifndef JNI_ALSA_ALSA_H_
#define JNI_ALSA_ALSA_H_
#ifdef  __cplusplus
extern "C"
{
#endif

#define ALSA_RECORD_FILE		"/customer/alsa_record.wav"
#define ALSA_RECORD_SHELL		"/customer/alsa_record.sh"
#define ALSA_PLAY_SHELL			"/customer/alsa_play.sh"

//int SSTAR_ALSA_SetSampleRate(MI_AUDIO_SampleRate_e eSampleRate);

int SSTAR_ALSA_StartRecord(char *pRecordFile);	// 录制音频
int SSTAR_ALSA_StopRecord();
int SSTRR_ALSA_StartPlayRecord(char *pRecordFile);	// 播放录音文件
int SSTRR_ALSA_StopPlayRecord();


#ifdef  __cplusplus
}
#endif




#endif /* JNI_ALSA_ALSA_H_ */
