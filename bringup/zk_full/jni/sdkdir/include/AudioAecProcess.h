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


//==============================================================================
//
//  File        : AudioAecProcess.h
//  Description : Aduio AEC Process function source code
//  Author      : ChenChu Hsu
//  Revision    : 1.0
//
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _AUDIOAECPROCESS_H_
#define _AUDIOAECPROCESS_H_

#define API_VERSION                  	{'1','1'}

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef enum
{
	ALGO_AEC_RET_SUCCESS					=0x00000000,
	ALGO_AEC_RET_INIT_ERROR					=0x10000401,
	ALGO_AEC_RET_INVALID_HANDLE				=0x10000402,
	ALGO_AEC_RET_INVALID_SAMPLE_RATE		=0x10000403,
	ALGO_AEC_RET_INVALID_POINT_NUMBER		=0x10000404,
	ALGO_AEC_RET_INVALID_CHANNEL			=0x10000405,
	ALGO_AEC_RET_INVALID_ENABLE				=0x10000406,
	ALGO_AEC_RET_INVALID_SUP_BAND			=0x10000407,
	ALGO_AEC_RET_INVALID_SUP_MODE			=0x10000408,
	ALGO_AEC_RET_INVALID_COMFORT_NOISE		=0x10000409,
	ALGO_AEC_RET_INVALID_DELAY_SAMPLE		=0x10000410,
	ALGO_AEC_RET_API_CONFLICT				=0x10000411,
	ALGO_AEC_RET_INVALID_CALLING			=0x10000412,
}ALGO_AEC_RET;

typedef enum
{
	IAA_AEC_SAMPLE_RATE_8000  =  8000 ,
	IAA_AEC_SAMPLE_RATE_16000 = 16000 ,
}IAA_AEC_SAMPLE_RATE;

typedef enum
{
	IAA_AEC_TRUE = 1,
	IAA_AEC_FALSE = 0,
}IAA_AEC_BOOL;

typedef struct
{
	unsigned int point_number;
	unsigned int nearend_channel;
	unsigned int farend_channel;
	IAA_AEC_SAMPLE_RATE sample_rate; //8000 or 16000
}AudioAecInit;

typedef struct
{
	IAA_AEC_BOOL comfort_noise_enable;
	short delay_sample;
	unsigned int suppression_mode_freq[6]; //[6]
	unsigned int suppression_mode_intensity[7]; //every element 0~15 [7]
}AudioAecConfig;

typedef void* AEC_HANDLE;
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
unsigned int IaaAec_GetBufferSize(void);
AEC_HANDLE IaaAec_Init(char* working_buffer_address, AudioAecInit * aec_init);
ALGO_AEC_RET IaaAec_Config(AEC_HANDLE handle, AudioAecConfig *aec_config);
AEC_HANDLE IaaAec_Reset(char* working_buffer_address, AudioAecInit * aec_init);
ALGO_AEC_RET IaaAec_Free(AEC_HANDLE handle);
ALGO_AEC_RET IaaAec_Run(AEC_HANDLE handle, short* pss_aduio_near_end, short* pss_aduio_far_end);
ALGO_AEC_RET IaaAec_GetLibVersion(unsigned short *ver_year,
						 unsigned short *ver_date,
						 unsigned short *ver_time);
void IaaAec_GenKey(char* code_out);
void IaaAec_VerifyKey(char* code_in);
int IaaAec_GetKeyLen(void);

#endif // #ifndef _AUDIOAECPROCESS_H_
#ifdef __cplusplus
}
#endif
