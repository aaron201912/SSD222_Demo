#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "player.h"

#ifdef SUPPORT_HDMI
#define AUDIO_DEV               3
#else
#define AUDIO_DEV               0
#endif

#define AO_DEV_DAC0             2
#define AO_DEV_DAC1             3
#define AO_DEV_ID_HEADPHONE     4

#define AUDIO_CHN               0

#define AUDIO_INPUT_SAMPRATE            48000
#define AUDIO_INPUT_CHLAYOUT            AV_CH_LAYOUT_MONO
#define AUDIO_INPUT_SAMPFMT             AV_SAMPLE_FMT_S16

#define AUDIO_OUTPUT_SAMPRATE           E_MI_AUDIO_SAMPLE_RATE_48000
#define AUDIO_OUTPUT_CHLAYOUT           E_MI_AUDIO_SOUND_MODE_MONO
#define AUDIO_OUTPUT_SAMPFMT            E_MI_AUDIO_BIT_WIDTH_16

#define MI_AUDIO_SAMPLE_PER_FRAME       1024
#define MI_AUDIO_MAX_SAMPLES_PER_FRAME  2048
#define MI_AUDIO_MAX_FRAME_NUM          6
//#define MI_AO_PCM_BUF_SIZE_BYTE         (MI_AUDIO_SAMPLE_PER_FRAME * MI_AUDIO_MAX_FRAME_NUM * 2 * 4)
#define MI_AO_PCM_BUF_SIZE_BYTE         65536

#define MI_AUDIO_MAX_DATA_SIZE          25000

#define MIN_AO_VOLUME           -60
#define MAX_AO_VOLUME           30
#define MIN_ADJUST_AO_VOLUME    -10
#define MAX_ADJUST_AO_VOLUME    20

int open_audio(player_stat_t *is);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif
