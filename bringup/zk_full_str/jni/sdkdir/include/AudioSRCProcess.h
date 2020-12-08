#ifndef AUDIOSRCPROCESS_H_
#define AUDIOSRCPROCESS_H_
#define API_VERSION                  	{'1','1'}

typedef enum {
	ALGO_SRC_RET_SUCCESS				= 0x00000000,
	ALGO_SRC_RET_INIT_ERROR				= 0x10000601,
	ALGO_SRC_RET_INVALID_MODE			= 0x10000602,
	ALGO_SRC_RET_INVALID_HANDLE			= 0x10000603,
	ALGO_SRC_RET_INVALID_CHANNEL		= 0x10000604,
	ALGO_SRC_RET_INVALID_POINT_NUMBER	= 0x10000605,
	ALGO_SRC_RET_INVALID_SAMPLE_RATE	= 0x10000606,
	ALGO_SRC_RET_API_CONFLICT			= 0x10000607,
	ALGO_SRC_RET_INVALID_CALLING		= 0x10000608
} ALGO_SRC_RET;

typedef enum{
    SRC_8k_to_16k,
    SRC_8k_to_32k,
    SRC_8k_to_48k,
    SRC_16k_to_8k,
    SRC_16k_to_32k,
    SRC_16k_to_48k,
    SRC_32k_to_8k,
    SRC_32k_to_16k,
    SRC_32k_to_48k,
    SRC_48k_to_8k,
    SRC_48k_to_16k,
    SRC_48k_to_32k
}SrcConversionMode;

typedef enum{
    SRATE_8K  =  8,
    SRATE_16K =  16,
    SRATE_32K =  32,
    SRATE_48K =  48
}SrcInSrate;

typedef void* SRC_HANDLE;

typedef struct{
    SrcInSrate WaveIn_srate;
    SrcConversionMode mode;
    unsigned int channel;
    unsigned int point_number;
}SRCStructProcess;

unsigned int IaaSrc_GetBufferSize(SrcConversionMode mode);
SRC_HANDLE IaaSrc_Init(char *workingBufferAddress, SRCStructProcess *src_struct);
ALGO_SRC_RET IaaSrc_Run(SRC_HANDLE handle, short *audio_input, short *audio_output, int* output_size);
ALGO_SRC_RET IaaSrc_Release(SRC_HANDLE handle);

#endif /* AUDIOSRCPROCESS_H_ */
