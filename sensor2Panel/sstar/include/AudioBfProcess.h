/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifdef __cplusplus
extern "C" {
#endif
#ifndef _AUDIOBFPROCESS_H_
#define _AUDIOBFPROCESS_H_
#define API_VERSION             {'1','1'}

typedef struct
{
	unsigned int point_number;
	unsigned int sample_rate; // 8k or 16k
	unsigned int mic_distance; //cm
	unsigned int channel; //only support 2
}AudioBfInit;

typedef struct
{
	unsigned int temperature;
	int noise_gate_dbfs;
	int noise_supression_mode;
	int noise_estimation;
	float output_gain;
	int vad_enable;
}AudioBfConfig;

typedef enum {
	ALGO_BF_RET_SUCCESS                     = 0x00000000,
	ALGO_BF_RET_INIT_ERROR                  = 0x10000301,
	ALGO_BF_RET_INVALID_CONFIG				= 0x10000302,
	ALGO_BF_RET_INVALID_HANDLE              = 0x10000303,
	ALGO_BF_RET_INVALID_NOISE_ESTIMATION	= 0x10000304,
	ALGO_BF_RET_INVALID_VAD_ENABLE			= 0x10000305,
	ALGO_BF_RET_INVALID_OUTPUT_GAIN			= 0x10000306,
	ALGO_BF_RET_INVALID_NOISE_SUPMODE		= 0x10000307,
	ALGO_BF_RET_INVALID_INPUT_POINTER		= 0x10000308,
	ALGO_BF_RET_INVALID_SAMPLERATE          = 0x10000309,
	ALGO_BF_RET_INVALID_POINTNUMBER 		= 0x10000310,
	ALGO_BF_RET_INVALID_CHANNEL 			= 0x10000311,
	ALGO_BF_RET_INVALID_CALLING			 	= 0x10000312,
	ALGO_BF_RET_API_CONFLICT				= 0x10000313

} ALGO_BF_RET;

typedef void* BF_HANDLE;

unsigned int IaaBf_GetBufferSize(void);
BF_HANDLE IaaBf_Init(char* working_buffer,AudioBfInit* bf_init);
ALGO_BF_RET IaaBf_Config(BF_HANDLE handle,AudioBfConfig* bf_config);
ALGO_BF_RET IaaBf_Get_Config(BF_HANDLE handle,AudioBfConfig *bf_config);
ALGO_BF_RET IaaBf_Run(BF_HANDLE handle,short* microphone_input,int * microphone_doa);
BF_HANDLE IaaBf_Reset(BF_HANDLE working_buffer,AudioBfInit* bf_init);
ALGO_BF_RET IaaBf_Free(BF_HANDLE handle);

#endif
#ifdef __cplusplus
}
#endif
