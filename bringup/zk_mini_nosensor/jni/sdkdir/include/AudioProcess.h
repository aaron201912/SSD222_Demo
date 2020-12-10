//==============================================================================
//
//  File        : AudioProcess.h
//  Description : Audio Process function source code
//  Author      : ChenChu Hsu
//  Revision    : 1.0
//
//==============================================================================
#ifndef AUDIOPROCESS_H_
#define AUDIOPROCESS_H_
typedef void* APC_HANDLE;
typedef void* ANR_HANDLE;
typedef void* EQ_HANDLE;
typedef void* AGC_HANDLE;
//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define eqCOSTDOWN_EN                   (1)
#define COSTDOWN_EN                     (0)
#define _NR_BAND_NUM                    (7)
#define _EQ_BAND_NUM                    (129)
#define _AGC_CR_NUM                  	(7)
#define API_VERSION                  	{'1','1'}
//==============================================================================
//
//                              ENUMERATIONS
//
//==============================================================================
typedef enum {
	ALGO_APC_RET_SUCCESS,
	ALGO_APC_RET_INIT_ERROR,
	ALGO_APC_RET_INVALID_HANDLE,
	ALGO_APC_RET_INVALID_SAMPLE_RATE,
	ALGO_APC_RET_INVALID_POINT_NUMBER,
	ALGO_APC_RET_INVALID_CHANNEL,
	ALGO_APC_ANR_RET_INVALID_ENABLE,
	ALGO_APC_ANR_RET_INVALID_MODE,
	ALGO_APC_ANR_RET_INVALID_INTENSITY,
	ALGO_APC_ANR_RET_INVALID_SMOOTH_LEVEL,
	ALGO_APC_ANR_RET_INVALID_CONVERGE_SPEED,
	ALGO_APC_EQ_RET_INVALID_ENABLE,
	ALGO_APC_EQ_RET_INVALID_MODE,
	ALGO_APC_EQ_RET_INVALID_TABLE,
	ALGO_APC_HPF_RET_INVALID_ENABLE,
	ALGO_APC_HPF_RET_INVALID_MODE,
	ALGO_APC_HPF_RET_INVALID_TABLE,
	ALGO_APC_AGC_RET_INVALID_ENABLE,
	ALGO_APC_AGC_RET_INVALID_MODE,
	ALGO_APC_AGC_RET_INVALID_COMPRESSION_RATIO,
	ALGO_APC_AGC_RET_INVALID_DROP_GAIN_MAX,
	ALGO_APC_AGC_RET_INVALID_RELEASE_TIME,
	ALGO_APC_AGC_RET_INVALID_ATTACK_TIME,
	ALGO_APC_AGC_RET_INVALID_NOISE_GATE,
	ALGO_APC_AGC_RET_INVALID_NOISE_ATTENU,
	ALGO_APC_AGC_RET_INVALID_DROP_GAIN_LEVEL,
	ALGO_APC_AGC_RET_INVALID_GAIN_INFO,
	ALGO_APC_RET_API_CONFLICT,
	ALGO_APC_RET_INVALID_CALLING,
	ALGO_APC_RET_FAILED
}ALGO_APC_RET;

typedef enum {
	ALGO_ANR_RET_SUCCESS,
	ALGO_ANR_RET_INIT_ERROR,
	ALGO_ANR_RET_INVALID_HANDLE,
	ALGO_ANR_RET_INVALID_SAMPLE_RATE,
	ALGO_ANR_RET_INVALID_POINT_NUMBER,
	ALGO_ANR_RET_INVALID_CHANNEL,
	ALGO_ANR_RET_INVALID_ENABLE,
	ALGO_ANR_RET_INVALID_MODE,
	ALGO_ANR_RET_INVALID_INTENSITY,
	ALGO_ANR_RET_INVALID_SMOOTH_LEVEL,
	ALGO_ANR_RET_INVALID_CONVERGE_SPEED,
	ALGO_ANR_RET_API_CONFLICT,
	ALGO_ANR_RET_INVALID_CALLING,
	ALGO_ANR_RET_FAILED
}ALGO_ANR_RET;

typedef enum {
	ALGO_EQ_RET_SUCCESS,
	ALGO_EQ_RET_INIT_ERROR,
	ALGO_EQ_RET_INVALID_HANDLE,
	ALGO_EQ_RET_INVALID_SAMPLE_RATE,
	ALGO_EQ_RET_INVALID_POINT_NUMBER,
	ALGO_EQ_RET_INVALID_CHANNEL,
	ALGO_EQ_RET_INVALID_ENABLE,
	ALGO_EQ_RET_INVALID_MODE,
	ALGO_EQ_RET_INVALID_TABLE,
	ALGO_HPF_RET_INVALID_ENABLE,
	ALGO_HPF_RET_INVALID_MODE,
	ALGO_HPF_RET_INVALID_TABLE,
	ALGO_EQ_RET_API_CONFLICT,
	ALGO_EQ_RET_INVALID_CALLING,
	ALGO_EQ_RET_FAILED
}ALGO_EQ_RET;

typedef enum {
	ALGO_AGC_RET_SUCCESS,
	ALGO_AGC_RET_INIT_ERROR,
	ALGO_AGC_RET_INVALID_HANDLE,
	ALGO_AGC_RET_INVALID_SAMPLE_RATE,
	ALGO_AGC_RET_INVALID_POINT_NUMBER,
	ALGO_AGC_RET_INVALID_CHANNEL,
	ALGO_AGC_RET_INVALID_ENABLE,
	ALGO_AGC_RET_INVALID_MODE,
	ALGO_AGC_RET_INVALID_COMPRESSION_RATIO,
	ALGO_AGC_RET_INVALID_DROP_GAIN_MAX,
	ALGO_AGC_RET_INVALID_RELEASE_TIME,
	ALGO_AGC_RET_INVALID_ATTACK_TIME,
	ALGO_AGC_RET_INVALID_NOISE_GATE,
	ALGO_AGC_RET_INVALID_NOISE_ATTENU,
	ALGO_AGC_RET_INVALID_DROP_GAIN_LEVEL,
	ALGO_AGC_RET_INVALID_GAIN_INFO,
	ALGO_AGC_RET_API_CONFLICT,
	ALGO_AGC_RET_INVALID_CALLING,
	ALGO_AGC_RET_FAILED,
}ALGO_AGC_RET;

typedef enum {
    IAA_APC_SAMPLE_RATE_8000  =  8000 ,
    IAA_APC_SAMPLE_RATE_16000 = 16000 ,
	IAA_APC_SAMPLE_RATE_48000 = 48000 ,
}IAA_APC_SAMPLE_RATE;

typedef enum {
    AUDIO_HPF_FREQ_80 , /* 80Hz*/
    AUDIO_HPF_FREQ_120, /*120Hz*/
    AUDIO_HPF_FREQ_150, /*150Hz*/
    AUDIO_HPF_FREQ_BUTT,
}IAA_HPF_FREQ;


typedef enum {
    NR_SPEED_LOW,
    NR_SPEED_MID,
    NR_SPEED_HIGH
} NR_CONVERGE_SPEED;

/*ANR config structure*/

typedef struct{
    unsigned int anr_enable;
    unsigned int user_mode;
    int anr_intensity_band[_NR_BAND_NUM-1];
    int anr_intensity[_NR_BAND_NUM];
    unsigned int anr_smooth_level; //range???
    NR_CONVERGE_SPEED anr_converge_speed; //0 1 2 higer the speed more fast
}AudioAnrConfig;

/*APC GetBufferSize and Init structure*/

typedef struct{
    unsigned int anr_enable;
    unsigned int eq_enable;
    unsigned int dr_enable;
    unsigned int vad_enable;
    unsigned int agc_enable;
}AudioApcBufferConfig;


/*EQ config structure*/
typedef struct{
    unsigned int eq_enable;
    unsigned int user_mode;
    short eq_gain_db[_EQ_BAND_NUM];
}AudioEqConfig;

/*HPF config structure*/
typedef struct{
    unsigned int hpf_enable;
    unsigned int user_mode;
    IAA_HPF_FREQ cutoff_frequency;
}AudioHpfConfig;

/*VAD config structure*/

typedef struct{
    unsigned int vad_enable;
    unsigned int user_mode;
    int vad_threshold;

}AudioVadConfig;

/*De-reverberation config structure*/
typedef struct{
    unsigned int dereverb_enable;
}AudioDereverbConfig;

/*APC init structure*/
typedef struct {
    unsigned int point_number;
    unsigned int channel;
    IAA_APC_SAMPLE_RATE sample_rate;
}AudioProcessInit;


/*AGC gain info*/
typedef struct
{
    int gain_max;  //gain maximum
    int gain_min;  //gain minimum
    int gain_init; //default gain (initial gain)
}AgcGainInfo;

/*AGC config structure*/
typedef struct
{
    unsigned int agc_enable;
    unsigned int user_mode;
    //gain setting
    AgcGainInfo gain_info;
    unsigned int drop_gain_max;

    //attack time, release time
    unsigned int attack_time;
    unsigned int release_time;

    //target level
    short compression_ratio_input[_AGC_CR_NUM];
    short compression_ratio_output[_AGC_CR_NUM];
    int drop_gain_threshold;

    // noise gate
    int noise_gate_db;
    unsigned int noise_gate_attenuation_db;

}AudioAgcConfig;


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
//Audio Process Chain
unsigned int IaaApc_GetBufferSize(AudioApcBufferConfig *apc_switch);
APC_HANDLE IaaApc_Init(char* const working_buffer_address,AudioProcessInit *audio_process_init, AudioApcBufferConfig *apc_switch);
ALGO_APC_RET IaaApc_Config(APC_HANDLE handle,
                         AudioAnrConfig *anr_config,
                         AudioEqConfig *eq_config,
                         AudioHpfConfig *hpf_config,
                         AudioVadConfig *vad_config,
                         AudioDereverbConfig *dereverb_config,
                         AudioAgcConfig *agc_config);
ALGO_APC_RET IaaApc_GetNrResult(APC_HANDLE,short* nr_audio_out);
ALGO_APC_RET IaaApc_GetNrEqResult(APC_HANDLE,short* nr_eq_audio_out);
ALGO_APC_RET IaaApc_Run(APC_HANDLE,short* pss_audio_in);
ALGO_APC_RET IaaApc_Free(APC_HANDLE);
APC_HANDLE IaaApc_Reset(char* working_buffer_address,AudioProcessInit *audio_process_init, AudioApcBufferConfig *apc_switch);

ALGO_APC_RET IaaApc_GetConfig(APC_HANDLE handle,
                         AudioProcessInit *audio_process_init,
                         AudioAnrConfig *anr_config,
                         AudioEqConfig *eq_config,
                         AudioHpfConfig *hpf_config,
                         AudioVadConfig *vad_config,
                         AudioDereverbConfig *dereverb_config,
                         AudioAgcConfig *agc_config);

//Noise Reduction
unsigned int IaaAnr_GetBufferSize(void);
ANR_HANDLE IaaAnr_Init(char* working_buffer_address, AudioProcessInit *anr_init);
ALGO_ANR_RET IaaAnr_Config(ANR_HANDLE handle, AudioAnrConfig *anr_config);
ALGO_ANR_RET IaaAnr_Run(ANR_HANDLE, short* pss_audio_in);
ALGO_ANR_RET IaaAnr_Free(ANR_HANDLE);
ANR_HANDLE IaaAnr_Reset(char* working_buffer_address, AudioProcessInit *anr_init);
ALGO_ANR_RET IaaAnr_GetConfig(ANR_HANDLE handle, AudioProcessInit *anr_init, AudioAnrConfig *anr_config);

// Equalizer
unsigned int IaaEq_GetBufferSize(void);
EQ_HANDLE IaaEq_Init(char* working_buffer_address, AudioProcessInit *eq_init);
ALGO_EQ_RET IaaEq_Config(EQ_HANDLE handle, AudioHpfConfig *hpf_config, AudioEqConfig *eq_config);
ALGO_EQ_RET IaaEq_Run(EQ_HANDLE, short* pss_audio_in);
ALGO_EQ_RET IaaEq_Free(EQ_HANDLE);
EQ_HANDLE IaaEq_Reset(char* working_buffer_address, AudioProcessInit *eq_init);
ALGO_EQ_RET IaaEq_GetConfig(EQ_HANDLE handle, AudioProcessInit *eq_init, AudioHpfConfig *hpf_config, AudioEqConfig *eq_config);

//Automatic Gain control
unsigned int IaaAgc_GetBufferSize(void);
AGC_HANDLE IaaAgc_Init(char* working_buffer_address, AudioProcessInit *agc_init);
ALGO_AGC_RET IaaAgc_Config(AGC_HANDLE handle, AudioAgcConfig *agc_config);
ALGO_AGC_RET IaaAgc_Run(AGC_HANDLE, short* pss_audio_in);
ALGO_AGC_RET IaaAgc_Free(AGC_HANDLE);
AGC_HANDLE IaaAgc_Reset(char* working_buffer_address, AudioProcessInit *agc_init);
ALGO_AGC_RET IaaAgc_GetConfig(AGC_HANDLE handle, AudioProcessInit *agc_init, AudioAgcConfig *agc_config);


#endif // #ifndef _AUDIOPROCESS_H_
