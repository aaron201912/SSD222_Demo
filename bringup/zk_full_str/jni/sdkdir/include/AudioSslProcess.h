#ifdef __cplusplus
extern "C" {
#endif
#ifndef _AUDIOSLLPROCESS_H_
#define _AUDIOSLLPROCESS_H_
#define API_VERSION             {'1','1'}

typedef struct
{
	unsigned int point_number;
	unsigned int sample_rate;
	unsigned int mic_distance;
	unsigned int bf_mode;
}AudioSslInit;

typedef struct
{
	unsigned int temperature;
	int noise_gate_dbfs;
	int direction_frame_num;
}AudioSslConfig;

typedef enum {
	ALGO_SSL_RET_SUCCESS                     = 0x00000000,
	ALGO_SSL_RET_INIT_ERROR                  = 0x10000101,
	ALGO_SSL_RET_INVALID_CONFIG				 = 0x10000102,
	ALGO_SSL_RET_INVALID_HANDLE              = 0x10000103,
	ALGO_SSL_RET_INVALID_SAMPLERATE          = 0x10000104,
	ALGO_SSL_RET_INVALID_POINTNUMBER 		 = 0x10000105,
	ALGO_SSL_RET_INVALID_BFMODE		 		 = 0x10000106,
	ALGO_SSL_RET_DELAY_SAMPLE_TOO_LARGE		 = 0x10000107,
	ALGO_SSL_RET_INVALID_CALLING			 = 0x10000108,
	ALGO_SSL_RET_API_CONFLICT				 = 0x10000109

} ALGO_SSL_RET;

typedef void* SSL_HANDLE;

unsigned int IaaSsl_GetBufferSize(void);
SSL_HANDLE IaaSsl_Init(char* working_buffer,AudioSslInit* ssl_init);
ALGO_SSL_RET IaaSsl_Config(SSL_HANDLE handle,AudioSslConfig* ssl_config);
ALGO_SSL_RET IaaSsl_Get_Config(SSL_HANDLE handle,AudioSslConfig *ssl_config);
ALGO_SSL_RET IaaSsl_Run(SSL_HANDLE handle,short* microphone_input,int *delay_sample);
ALGO_SSL_RET IaaSsl_Get_Direction(SSL_HANDLE handle,int* direction);
SSL_HANDLE IaaSsl_Reset(SSL_HANDLE working_buffer,AudioSslInit* ssl_init);
ALGO_SSL_RET IaaSsl_Free(SSL_HANDLE handle);

#endif
#ifdef __cplusplus
}
#endif
