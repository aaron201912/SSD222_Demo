//#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <libavutil/avutil.h>
#include <libavutil/attributes.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include <libswresample/swresample.h>

#include "mi_common.h"
#include "mi_sys.h"
#include "mi_ao.h"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

//#define DUMP_FILE
#define WAV_SAMPLERATE			    44100
#define MI_AUDIO_SAMPLE_PER_FRAME 	256

#define ExecFunc(result, value)\
    if (result != value)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__);\
        return -1;\
    }\


typedef struct {
    int videoindex;
    int sndindex;
    AVFormatContext* pFormatCtx;
    AVCodecContext* sndCodecCtx;
    AVCodec* sndCodec;
    SwrContext *swr_ctx;
    DECLARE_ALIGNED(16,uint8_t,audio_buf) [AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
}AudioState;

 //下面这四个结构体是为了分析wav头的
typedef struct {
    u_int magic;      /* 'RIFF' */
    u_int length;     /* filelen */
    u_int type;       /* 'WAVE' */
} WaveHeader;

typedef struct {
    u_short format;       /* see WAV_FMT_* */
    u_short channels;
    u_int sample_fq;      /* frequence of sample */
    u_int byte_p_sec;
    u_short byte_p_spl;   /* samplesize; 1 or 2 bytes */
    u_short bit_p_spl;    /* 8, 12 or 16 bit */
} WaveFmtBody;

typedef struct {
    u_int type;        /* 'data' */
    u_int length;      /* samplecount */
} WaveChunkHeader;

#define COMPOSE_ID(a,b,c,d) ((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
#define WAV_RIFF COMPOSE_ID('R','I','F','F')
#define WAV_WAVE COMPOSE_ID('W','A','V','E')
#define WAV_FMT COMPOSE_ID('f','m','t',' ')
#define WAV_DATA COMPOSE_ID('d','a','t','a')

static MI_S32 AoDevId = 0;
static MI_S32 AoChn = 0;

static bool g_bExit = false;
static bool g_bPlayThreadRun = false;
static bool g_bDecodeDone = false;
static pthread_t g_playThread = 0;

#ifdef DUMP_FILE
static FILE *fp = NULL;
#endif

int insert_wave_header(FILE* fp, long data_len, int sampleRate, int chnCnt)
{
    int len;
    WaveHeader* header;
    WaveChunkHeader* chunk;
    WaveFmtBody* body;

    fseek(fp, 0, SEEK_SET);        //写到wav文件的开始处

    len = sizeof(WaveHeader)+sizeof(WaveFmtBody)+sizeof(WaveChunkHeader)*2;
    char* buf = (char*)malloc(len);
    header = (WaveHeader*)buf;
    header->magic = WAV_RIFF;
    header->length = data_len + sizeof(WaveFmtBody)+sizeof(WaveChunkHeader)*2 + 4;
    header->type = WAV_WAVE;

    chunk = buf+sizeof(WaveHeader);
    chunk->type = WAV_FMT;
    chunk->length = 16;

    body = (WaveFmtBody*)(buf+sizeof(WaveHeader)+sizeof(WaveChunkHeader));
    body->format = (u_short)0x0001;      		//编码方式为pcm
    body->channels = (u_short)chnCnt;	 		//(u_short)0x02;      //声道数为2
    body->sample_fq = sampleRate;		 		//44100;             //采样频率为44.1k
    body->byte_p_sec = sampleRate * chnCnt * 2;	//176400;           //每秒所需字节数 44100*2*2=采样频率*声道*采样位数 
    body->byte_p_spl = (u_short)0x4;     		//对齐无意义
    body->bit_p_spl = (u_short)16;       		//采样位数16bit=2Byte


    chunk = (WaveChunkHeader*)(buf+sizeof(WaveHeader)+sizeof(WaveChunkHeader)+sizeof(WaveFmtBody));
    chunk->type = WAV_DATA;
    chunk->length = data_len;

    fwrite(buf, 1, len, fp);
    free(buf);
    return 0;
}

void *mp3DecodeProc(void *pParam)
{
    AudioState *is = (AudioState *)pParam;
    AVPacket *packet = av_mallocz(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();
    uint8_t *out[] = { is->audio_buf };
    int data_size = 0, got_frame = 0;
    int wavDataLen = 0;
#ifdef DUMP_FILE    
    int file_data_size = 0;
#endif

    while(g_bPlayThreadRun)    //1.2 循环读取mp3文件中的数据帧
    {
        if (av_read_frame(is->pFormatCtx, packet) < 0)
            break;

        if(packet->stream_index != is->sndindex)
            continue;
        if(avcodec_decode_audio4(is->sndCodecCtx, frame, &got_frame, packet) < 0) //1.3 解码数据帧
        {
            printf("file eof");
            break;
        }

        if(got_frame <= 0) /* No data yet, get more frames */
            continue;
        data_size = av_samples_get_buffer_size(NULL, is->sndCodecCtx->channels, frame->nb_samples, is->sndCodecCtx->sample_fmt, 1);
        //1.4下面将ffmpeg解码后的数据帧转为我们需要的数据(关于"需要的数据"下面有解释)
        if(NULL==is->swr_ctx)
        {
            if(is->swr_ctx != NULL)
                swr_free(&is->swr_ctx);
            printf("frame: channnels=%d,format=%d, sample_rate=%d", frame->channels, frame->format, frame->sample_rate);
            is->swr_ctx = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, WAV_SAMPLERATE, av_get_default_channel_layout(frame->channels), frame->format, frame->sample_rate, 0, NULL);
            if(is->swr_ctx == NULL)
            {
                printf("swr_ctx == NULL");
            }
            swr_init(is->swr_ctx);
        }
        wavDataLen = swr_convert(is->swr_ctx, out, WAV_SAMPLERATE, (const uint8_t **)frame->extended_data, frame->nb_samples);

        MI_AUDIO_Frame_t stAoSendFrame;
        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = wavDataLen * 2 * 2;
        stAoSendFrame.apVirAddr[0] = (unsigned char*)is->audio_buf;
        stAoSendFrame.apVirAddr[1] = NULL;
        int s32Ret = MI_SUCCESS;
        do
        {
            s32Ret = MI_AO_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);

        }
        while (s32Ret == MI_AO_ERR_NOBUF);

        MI_AO_ChnState_t stChnState;
        do
        {
            s32Ret = MI_AO_QueryChnStat(AoDevId, AoChn, &stChnState);

            if (stChnState.u32ChnFreeNum >= MI_AUDIO_SAMPLE_PER_FRAME * WAV_SAMPLERATE / (int)E_MI_AUDIO_SAMPLE_RATE_8000)
                break;
        }
        while (1);
#ifdef DUMP_FILE
		file_data_size += wavDataLen;

        //1.5 数据格式转换完成后就写到文件中
		if (fp)
			fwrite((short *)is->audio_buf, sizeof(short), (size_t) wavDataLen * 2, fp);
#endif
    }

#ifdef DUMP_FILE
	file_data_size *= 2*2;			// 计算字节数,2chn & 16bits
    printf("file_data_size=%d", file_data_size);
    //第2步添加上wav的头
    insert_wave_header(fp, file_data_size, WAV_SAMPLERATE, 2);
    fclose(fp);
#endif

	if (is->swr_ctx != NULL)
		swr_free(&is->swr_ctx);
    av_free_packet(packet);
    av_frame_free(&frame);
    g_bDecodeDone = true;

    return NULL;
}

int init_ffplayer(AudioState* is, char* filepath)
{
    int i=0;
    int ret;
    is->sndindex = -1;

#ifdef DUMP_FILE
    int wavHeaderLen = 0;
#endif

    if(NULL == filepath)
    {
        printf("input file is NULL");
        return -1;
    }

    avcodec_register_all();
    //avfilter_register_all();
    av_register_all();

    is->pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&is->pFormatCtx, filepath, NULL, NULL)!=0)
        return -1;

    if(avformat_find_stream_info(is->pFormatCtx, NULL)<0)
        return -1;
    av_dump_format(is->pFormatCtx,0, 0, 0);
    is->videoindex = av_find_best_stream(is->pFormatCtx, AVMEDIA_TYPE_VIDEO, is->videoindex, -1, NULL, 0); 
    is->sndindex = av_find_best_stream(is->pFormatCtx, AVMEDIA_TYPE_AUDIO,is->sndindex, is->videoindex, NULL, 0);
    printf("videoindex=%d, sndindex=%d", is->videoindex, is->sndindex);
    if(is->sndindex != -1)
    {
        is->sndCodecCtx = is->pFormatCtx->streams[is->sndindex]->codec;
        is->sndCodec = avcodec_find_decoder(is->sndCodecCtx->codec_id);
        if(is->sndCodec == NULL)
        {
            printf("Codec not found");
            return -1;
        }
        if(avcodec_open2(is->sndCodecCtx, is->sndCodec, NULL) < 0)
            return -1;
    }

#ifdef DUMP_FILE
	fp = fopen("./test.wav", "wb+");
	if (fp)
	{
		wavHeaderLen = sizeof(WaveHeader) + sizeof(WaveFmtBody) + sizeof(WaveChunkHeader) * 2;
		fseek(fp, wavHeaderLen, SEEK_SET);
		printf("wavHeaderLen=%d", wavHeaderLen);
	}
#endif

    g_bDecodeDone = false;
    g_bPlayThreadRun = true;
    ret = pthread_create(&g_playThread, NULL, mp3DecodeProc, (void *)is);
    if (ret != 0) 
    {
        printf("pthread_create mp3DecodeProc failed!\n");
    }
    return 0;
}

void deinit_ffplayer(AudioState* is)
{
    g_bPlayThreadRun = false;

    if (g_playThread)
    {
        pthread_join(g_playThread, NULL);
        g_playThread = NULL;
    }
    
    avcodec_close(is->sndCodecCtx);
    avformat_close_input(&is->pFormatCtx);
}

void signalHandler(int signo)
{
    switch (signo)
    {
        case SIGINT:
            printf("catch exit signal\n");
            g_bExit = true;
            break;
        default:
            break;
    }
}

int init_ao(signed short channels, unsigned int dwSamplesPerSec, int s32AoVolume)
{
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AO_AdecConfig_t stAoSetAdecConfig, stAoGetAdecConfig;
    MI_AO_VqeConfig_t stAoSetVqeConfig, stAoGetVqeConfig;
    MI_AO_ChnParam_t stAoChnParam;
    MI_U32 u32DmaBufSize;

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)dwSamplesPerSec;
    //stAoSetAttr.u32PtNumPerFrm = 1024;//MI_AUDIO_SAMPLE_PER_FRAME;
    stAoSetAttr.u32PtNumPerFrm = dwSamplesPerSec / (int)E_MI_AUDIO_SAMPLE_RATE_8000 * MI_AUDIO_SAMPLE_PER_FRAME;//MI_AUDIO_SAMPLE_PER_FRAME;
    stAoSetAttr.u32ChnCnt = 1;

    if (channels == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if (channels == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    ExecFunc(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS);
    ExecFunc(MI_AO_GetPubAttr(AoDevId, &stAoGetAttr), MI_SUCCESS);
    ExecFunc(MI_AO_Enable(AoDevId), MI_SUCCESS);
    ExecFunc(MI_AO_EnableChn(AoDevId, AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_SetVolume(AoDevId, 0, s32AoVolume, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);

    printf("ao init, volume value is %d\n", s32AoVolume);

    return 0;
}

void deinit_ao()
{
    MI_AO_ChnState_t stChnState;
    do
    {
        MI_AO_QueryChnStat(AoDevId, AoChn, &stChnState);

        //printf("busyNum = %d\n", stChnState.u32ChnBusyNum);

        if (stChnState.u32ChnBusyNum <= MI_AUDIO_SAMPLE_PER_FRAME)
            break;
	    
	usleep(10000);
    }
    while (1);

    MI_AO_DisableChn(AoDevId, AoChn);
    MI_AO_Disable(AoDevId);
}

int main(int argc, char **argv)
{
    int ret = 0;
    int volume = -10;
    AudioState* is = (AudioState*) av_mallocz(sizeof(AudioState));

    signal(SIGINT, signalHandler);

    if (argc < 2)
    {
        printf("please input a mp3 file!\n");
        printf("eg: ./mp3Player [file] [volume] , the default volume is -10\n");
        return -1;
    }

    if (argc > 2)
    {
        volume = atoi(argv[2]);
    }

    if (init_ao(2, WAV_SAMPLERATE, volume) != 0)
    {
        printf("init ao error\n");
        return -1;
    }
    
    if( (ret=init_ffplayer(is, argv[1])) != 0)
    {
        printf("init_ffmpeg error");
        goto ao_deinit;
    }

    while (1)
    {
        if (g_bExit || g_bDecodeDone)
        {
            break;
        }

        usleep(30000);
    }

    deinit_ffplayer(is);

ao_deinit:    
    deinit_ao();

    av_free(is);

    return 0;
}
