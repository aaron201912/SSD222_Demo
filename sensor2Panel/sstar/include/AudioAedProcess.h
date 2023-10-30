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
//  File        : AudioAedProcess.h
//  Description : Audio AED Process function source code
//  Author      : ChenChu Hsu
//  Revision    : 1.0
//
//==============================================================================

#ifndef AUDIOAEDPROCESS_H_
#define AUDIOAEDPROCESS_H_
#define API_VERSION             {'1','1'}

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct {
	unsigned int point_number;
    unsigned int channel;
} AedProcessStruct;

//==============================================================================
//
//                              ENUMERATIONS
//
//==============================================================================

typedef enum {
    AED_SRATE_8K  = 8000,
    AED_SRATE_16K = 16000,
    AED_SRATE_32K = 32000,
	AED_SRATE_48K = 48000
} AedSampleRate;

typedef enum {
    AED_SEN_LOW,
    AED_SEN_MID,
    AED_SEN_HIGH
} AedSensitivity;

typedef enum {
	ALGO_AED_RET_SUCCESS                     = 0x00000000,
	ALGO_AED_RET_INIT_ERROR                  = 0x10000201,   /*BCRY init error*/
	ALGO_AED_RET_INVALID_CONFIG				 = 0x10000202,	 /*Invalid Config of BCRY*/
	ALGO_AED_RET_INVALID_HANDLE              = 0x10000203,   /*Invalid BCRY handle*/
	ALGO_AED_RET_INVALID_SAMPLERATE          = 0x10000204,   /*Invalid Sample rate of BCRY*/
	ALGO_AED_RET_INVALID_POINTNUMBER 		 = 0x10000205,
	ALGO_AED_RET_INVALID_CHANNEL			 = 0x10000206,
	ALGO_AED_RET_INVALID_SENSITIVITY		 = 0x10000207,
	ALGO_AED_RET_INVALID_CALLING			 = 0x10000208,
	ALGO_AED_RET_API_CONFLICT				 = 0x10000209

} ALGO_AED_RET;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
typedef void* AedHandle;
unsigned int IaaAed_GetBufferSize();
AedHandle IaaAed_Init(char *working_buf_ptr, AedProcessStruct *aed_struct);
ALGO_AED_RET IaaAed_Config(AedHandle handle);
ALGO_AED_RET IaaAed_Run(AedHandle handle,short *audio_io);
ALGO_AED_RET IaaAed_GetResult(AedHandle handle,int *aed_result);
ALGO_AED_RET IaaAed_SetSensitivity(AedHandle handle,AedSensitivity sensitivity);
ALGO_AED_RET IaaAed_SetSampleRate(AedHandle handle,AedSampleRate srate);
ALGO_AED_RET IaaAed_SetOperatingPoint(AedHandle handle,int operating_point);
ALGO_AED_RET IaaAed_SetVadThreshold(AedHandle handle,int threshold_db);
ALGO_AED_RET IaaAed_SetLsdThreshold(AedHandle handle,int threshold_db);
ALGO_AED_RET IaaAed_RunLsd(AedHandle handle,short *audio_input, int *lsd_db);
ALGO_AED_RET IaaAed_GetLsdResult(AedHandle handle, int* lsd_result);
ALGO_AED_RET IaaAed_Release(AedHandle handle);
#endif // #ifndef AUDIOAEDPROCESS_H_
