#include "mi_ao.h"
//#include "mi_vdec.h"
#include "mi_disp.h"
#include "mi_sys.h"
//#include "mi_vdec_datatype.h"
#include "mi_disp_datatype.h"
#include "mi_sys_datatype.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "panelconfig.h"

//========================== Add play audui file ============================
#define MI_AUDIO_SAMPLE_PER_FRAME 	256		//1024
#define DMA_BUF_SIZE_8K     		(8000)
#define DMA_BUF_SIZE_16K    		(16000)
#define DMA_BUF_SIZE_32K    		(32000)
#define DMA_BUF_SIZE_48K    		(48000)

//static int g_VdecRun = FALSE;
//static pthread_t g_VdeStream_tid = 0;

#define MI_U32VALUE(pu8Data, index) (pu8Data[index]<<24)|(pu8Data[index+1]<<16)|(pu8Data[index+2]<<8)|(pu8Data[index+3])
#define ST_DBG printf

#ifndef ExecFunc
#define ExecFunc(_func_, _ret_) \
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != _ret_) \
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)
#endif


typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;

static MI_S32 g_AoReadFd = -1;
static MI_S32 g_AoDevId = 0;
static MI_S32 g_AoChn = 0;
static WaveFileHeader_t g_stWavHeaderInput;
static MI_S32 g_s32NeedSize = 0;
static pthread_t tid_playaudio;
static MI_BOOL bAoExit;
MI_U8 u8TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 2];

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
void* SSTAR_aoSendFrame(void* data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;
    if (g_AoReadFd < 0)
    {
        return NULL;
    }
    bAoExit = FALSE;

    while(FALSE == bAoExit)
    {
        s32Ret = read(g_AoReadFd, &u8TempBuf, g_s32NeedSize);
        if(s32Ret != g_s32NeedSize)
        {
            lseek(g_AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(g_AoReadFd, &u8TempBuf, g_s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }
        
        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len[0] = s32Ret;
        stAoSendFrame.apVirAddr[0] = u8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do{
            s32Ret = MI_AO_SendFrame(g_AoDevId, g_AoChn, &stAoSendFrame, -1);
        }while(s32Ret == MI_AO_ERR_NOBUF);

        if(s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
        }
		
    }
    if (g_AoReadFd > 0)
    {
        close(g_AoReadFd);
    }
    return NULL;
}

MI_S32 SSTAR_StopPlayAudioFile(void)
{
    bAoExit = TRUE;
    if (tid_playaudio)
    {
    	pthread_join(tid_playaudio, NULL);
    	tid_playaudio = 0;
    }

    ExecFunc(MI_AO_DisableChn(g_AoDevId, g_AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(g_AoDevId), MI_SUCCESS);

    return 0;
}

MI_S32 SSTAR_StartPlayAudioFile(const char *WavAudioFile, MI_S32 s32AoVolume)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AO_AdecConfig_t stAoSetAdecConfig, stAoGetAdecConfig;
    MI_AO_VqeConfig_t stAoSetVqeConfig, stAoGetVqeConfig;
    MI_S32 s32AoGetVolume;
    MI_AO_ChnParam_t stAoChnParam;
    MI_U32 u32DmaBufSize;
    MI_AUDIO_SampleRate_e eAoInSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;

    g_AoReadFd = open((const char *)WavAudioFile, O_RDONLY, 0666);
    if(g_AoReadFd <= 0)
    {
        printf("Open input file failed:%s \n", WavAudioFile);
        printf("error:%s", strerror(errno));
        return -1;
    }

    s32Ret = read(g_AoReadFd, &g_stWavHeaderInput, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        printf("Read wav header failed!!!\n");
        return -1;
    }

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;	//FALSE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
    //stAoSetAttr.u32ChnCnt = g_stWavHeaderInput.wave.wChannels;
    stAoSetAttr.u32ChnCnt = 1;

    if(g_stWavHeaderInput.wave.wChannels == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if(g_stWavHeaderInput.wave.wChannels == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)g_stWavHeaderInput.wave.dwSamplesPerSec;
    eAoInSampleRate = (MI_AUDIO_SampleRate_e)g_stWavHeaderInput.wave.dwSamplesPerSec;

    stAoSetVqeConfig.bAgcOpen = FALSE;
    stAoSetVqeConfig.bAnrOpen = FALSE;
    stAoSetVqeConfig.bEqOpen = FALSE;
    stAoSetVqeConfig.bHpfOpen = FALSE;
    stAoSetVqeConfig.s32FrameSample = 128;
    stAoSetVqeConfig.s32WorkSampleRate = eAoInSampleRate;
    //memcpy(&stAoSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));
    //memcpy(&stAoSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));
    //memcpy(&stAoSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));
    //memcpy(&stAoSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));

    ExecFunc(MI_AO_SetPubAttr(g_AoDevId, &stAoSetAttr), MI_SUCCESS);
    ExecFunc(MI_AO_GetPubAttr(g_AoDevId, &stAoGetAttr), MI_SUCCESS);

    ExecFunc(MI_AO_Enable(g_AoDevId), MI_SUCCESS);

    ExecFunc(MI_AO_EnableChn(g_AoDevId, g_AoChn), MI_SUCCESS);

    if(FALSE)
    {
        ExecFunc(MI_AO_SetVqeAttr(g_AoDevId, g_AoChn, &stAoSetVqeConfig), MI_SUCCESS);
        ExecFunc(MI_AO_GetVqeAttr(g_AoDevId, g_AoChn, &stAoGetVqeConfig), MI_SUCCESS);
        ExecFunc(MI_AO_EnableVqe(g_AoDevId, g_AoChn), MI_SUCCESS);
    }

    ExecFunc(MI_AO_SetVolume(g_AoDevId, g_AoChn, s32AoVolume, E_MI_AO_GAIN_FADING_OFF), MI_SUCCESS);
    ExecFunc(MI_AO_GetVolume(g_AoDevId, g_AoChn, &s32AoGetVolume), MI_SUCCESS);

    g_s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * (stAoSetAttr.u32ChnCnt);
    if (E_MI_AUDIO_SAMPLE_RATE_8000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_8K;;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_16000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_16K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_32000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_32K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_48000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_48K;
    }

    if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_STEREO)
    {
        if (g_s32NeedSize > (u32DmaBufSize / 4))
        {
            g_s32NeedSize = u32DmaBufSize / 4;
        }
    }
    else if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_MONO)
    {
        if (g_s32NeedSize > (u32DmaBufSize / 8))
        {
            g_s32NeedSize = u32DmaBufSize / 8;
        }
    }

    pthread_create(&tid_playaudio, NULL, SSTAR_aoSendFrame, NULL);
    printf("create ao thread.\n");

    return 0;
}

//static int FindStartCode2 (unsigned char *Buf)
//{
//    if((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 1))
//        return 0;
//    else
//        return 1;
//}
//
//static int FindStartCode3 (unsigned char *Buf)
//{
//    if((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 0) || (Buf[3] != 1))
//        return 0;
//    else
//        return 1;
//}

//NALU_t *AllocNALU(int buffersize)
//{
//    NALU_t *n;
//    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
//    {
//        printf("AllocNALU: n");
//        exit(0);
//    }
//    n->max_size=buffersize;
//    if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
//    {
//        free (n);
//        printf ("AllocNALU: n->buf");
//        exit(0);
//    }
//    return n;
//}
//
//void FreeNALU(NALU_t *n)
//{
//    if (n)
//    {
//        if (n->buf)
//        {
//            free(n->buf);
//            n->buf=NULL;
//        }
//        free (n);
//    }
//}

//int GetAnnexbNALU (NALU_t *nalu, MI_S32 chn)
//{
//    int pos = 0;
//    int StartCodeFound, rewind;
//    unsigned char *Buf;
//
//    if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
//        printf ("GetAnnexbNALU: Could not allocate Buf memory\n");
//    nalu->startcodeprefix_len=3;
//    if (3 != fread (Buf, 1, 3, g_pStreamFile[chn]))
//    {
//        free(Buf);
//        return 0;
//    }
//    info2 = FindStartCode2 (Buf);
//    if(info2 != 1)
//    {
//        if(1 != fread(Buf+3, 1, 1, g_pStreamFile[chn]))
//        {
//            free(Buf);
//            return 0;
//        }
//        info3 = FindStartCode3 (Buf);
//        if (info3 != 1)
//        {
//            free(Buf);
//            return -1;
//        }
//        else
//        {
//            pos = 4;
//            nalu->startcodeprefix_len = 4;
//        }
//    }
//    else
//    {
//        nalu->startcodeprefix_len = 3;
//        pos = 3;
//    }
//    StartCodeFound = 0;
//    info2 = 0;
//    info3 = 0;
//    while (!StartCodeFound)
//    {
//        if (feof (g_pStreamFile[chn]))
//        {
//            nalu->len = (pos-1)-nalu->startcodeprefix_len;
//            memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
//            free(Buf);
//            fseek(g_pStreamFile[chn], 0, 0);
//            return pos-1;
//        }
//        Buf[pos++] = fgetc (g_pStreamFile[chn]);
//        info3 = FindStartCode3(&Buf[pos-4]);
//        if(info3 != 1)
//            info2 = FindStartCode2(&Buf[pos-3]);
//        StartCodeFound = (info2 == 1 || info3 == 1);
//    }
//    rewind = (info3 == 1) ? -4 : -3;
//    if (0 != fseek (g_pStreamFile[chn], rewind, SEEK_CUR))
//    {
//        free(Buf);
//        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
//    }
//    nalu->len = (pos+rewind);
//    memcpy (nalu->buf, &Buf[0], nalu->len);
//    free(Buf);
//    return (pos+rewind);
//}
//
//void dump(NALU_t *n)
//{
//    if (!n)
//        return;
//    //printf(" len: %d  ", n->len);
//    //printf("nal_unit_type: %x\n", n->nal_unit_type);
//}
//
//MI_S32 SSTAR_ModuleBind(MI_S32 s32SrcMod, MI_S32 s32SrcDev, MI_S32 s32SrcChn,MI_S32 s32SrcPort,
//    MI_S32 s32DstMod,MI_S32 s32DstDev,MI_S32 s32DstChn,MI_S32 s32DstPort)
//{
//    ST_Sys_BindInfo_T stBindInfo;
//    memset(&stBindInfo, 0x0, sizeof(stBindInfo));
//
//    stBindInfo.stSrcChnPort.eModId = (MI_ModuleId_e)s32SrcMod;
//    stBindInfo.stSrcChnPort.u32DevId = s32SrcDev;
//    stBindInfo.stSrcChnPort.u32ChnId = s32SrcChn;
//    stBindInfo.stSrcChnPort.u32PortId = s32SrcPort;
//
//    stBindInfo.stDstChnPort.eModId = (MI_ModuleId_e)s32DstMod;
//    stBindInfo.stDstChnPort.u32DevId = s32DstDev;
//    stBindInfo.stDstChnPort.u32ChnId = s32DstChn;
//    stBindInfo.stDstChnPort.u32PortId = s32DstPort;
//
//    stBindInfo.u32SrcFrmrate = 0;
//    stBindInfo.u32DstFrmrate = 0;
//    ExecFunc(MI_SYS_BindChnPort(&stBindInfo.stSrcChnPort, &stBindInfo.stDstChnPort, \
//        stBindInfo.u32SrcFrmrate, stBindInfo.u32DstFrmrate), MI_SUCCESS);
//
//    return 0;
//}
//
//MI_S32 SSTAR_ModuleUnBind(MI_S32 s32SrcMod, MI_S32 s32SrcDev, MI_S32 s32SrcChn,MI_S32 s32SrcPort,
//    MI_S32 s32DstMod,MI_S32 s32DstDev,MI_S32 s32DstChn,MI_S32 s32DstPort)
//{
//    ST_Sys_BindInfo_T stBindInfo;
//    memset(&stBindInfo, 0x0, sizeof(stBindInfo));
//
//    stBindInfo.stSrcChnPort.eModId = (MI_ModuleId_e)s32SrcMod;
//    stBindInfo.stSrcChnPort.u32DevId = s32SrcDev;
//    stBindInfo.stSrcChnPort.u32ChnId = s32SrcChn;
//    stBindInfo.stSrcChnPort.u32PortId = s32SrcPort;
//
//    stBindInfo.stDstChnPort.eModId = (MI_ModuleId_e)s32DstMod;
//    stBindInfo.stDstChnPort.u32DevId = s32DstDev;
//    stBindInfo.stDstChnPort.u32ChnId = s32DstChn;
//    stBindInfo.stDstChnPort.u32PortId = s32DstPort;
//
//    stBindInfo.u32SrcFrmrate = 0;
//    stBindInfo.u32DstFrmrate = 0;
//    printf("xxxxxxxModule Unbind src(%d-%d-%d-%d) dst(%d-%d-%d-%d)...\n", s32SrcMod, s32SrcDev, s32SrcChn, s32SrcPort,
//        s32DstMod, s32DstDev, s32DstChn, s32DstPort);
//    ExecFunc(MI_SYS_UnBindChnPort(&stBindInfo.stSrcChnPort, &stBindInfo.stDstChnPort), MI_SUCCESS);
//
//    return 0;
//}

//void *SSTAR_VdecSendStream(void *args)
//{
//    MI_SYS_ChnPort_t stChnPort;
//    MI_SYS_BufConf_t stBufConf;
//    MI_SYS_BufInfo_t stBufInfo;
//    MI_SYS_BUF_HANDLE hSysBuf;
//    MI_S32 s32Ret = MI_SUCCESS;
//    MI_VDEC_CHN vdecChn = 0;
//    MI_S32 s32TimeOutMs = 20, s32ChannelId = 0, s32TempHeight = 0;
//    MI_S32 s32Ms = 30;
//    MI_BOOL bVdecChnEnable;
//    MI_U16 u16Times = 20000;
//
//    MI_S32 s32ReadCnt = 0;
//    FILE *readfp = NULL;
//    MI_U8 *pu8Buf = NULL;
//    MI_S32 s32Len = 0;
//    MI_U32 u32FrameLen = 0;
//    MI_U64 u64Pts = 0;
//    MI_U8 au8Header[32] = {0};
//    MI_U32 u32Pos = 0;
//    MI_VDEC_ChnStat_t stChnStat;
//    MI_VDEC_VideoStream_t stVdecStream;
//
//    MI_U32 u32FpBackLen = 0; // if send stream failed, file pointer back length
//
//    char tname[32];
//    memset(tname, 0, 32);
//
//#ifndef ADD_HEADER_ES
//    NALU_t *n;
//    n = AllocNALU(2000000);
//#endif
//
//    vdecChn = 0;
//    snprintf(tname, 32, "push_t_%u", vdecChn);
//
//    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
//    stChnPort.eModId = E_MI_MODULE_ID_VDEC;
//    stChnPort.u32DevId = 0;
//    stChnPort.u32ChnId = vdecChn;//0 1 2 3
//    stChnPort.u32PortId = 0;
//    if ((access("720P25.h264", F_OK))!=-1)
//    {
//        readfp = fopen("720P25.h264", "rb"); //ES
//        ST_DBG("open current dir es file\n");
//    }
//    else if ((access("/customer/res/720P25.h264", F_OK))!=-1)
//    {
//        readfp = fopen("/customer/res/720P25.h264", "rb"); //ES
//        ST_DBG("open /customer dir es file\n");
//    }
//	else if ((access("/config/720P25.h264", F_OK)) != -1)
//    {
//        readfp = fopen("/config/720P25.h264", "rb"); //ES
//        ST_DBG("open /config dir es file\n");
//    }
//    if (!readfp)
//    {
//        ST_ERR("Open es file failed!\n");
//        return NULL;
//    }
//    else
//    {
//        g_pStreamFile[vdecChn] = readfp;
//    }
//
//    // s32Ms = _stTestParam.stChannelInfo[s32VoChannel].s32PushDataMs;
//    // bVdecChnEnable = _stTestParam.stChannelInfo[0].bVdecChnEnable;
//
//    memset(&stBufConf, 0x0, sizeof(MI_SYS_BufConf_t));
//    stBufConf.eBufType = E_MI_SYS_BUFDATA_RAW;
//    stBufConf.u64TargetPts = 0;
//    pu8Buf = (MI_U8 *)malloc(NALU_PACKET_SIZE);
//
//    s32Ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 3);
//    if (MI_SUCCESS != s32Ret)
//    {
//        ST_ERR("MI_SYS_SetChnOutputPortDepth error, %X\n", s32Ret);
//        return NULL;
//    }
//
//    s32Ms = 30;
//
//    printf("----------------------%d----------g_VdecRun %d--------\n", stChnPort.u32ChnId, g_VdecRun);
//    while (g_VdecRun)
//    {
//#ifdef ADD_HEADER_ES
//        memset(au8Header, 0, 16);
//        u32Pos = fseek(readfp, 0, SEEK_CUR);
//        s32Len = fread(au8Header, 1, 16, readfp);
//        if(s32Len <= 0)
//        {
//            fseek(readfp, 0, SEEK_SET);
//            continue;
//        }
//        u32FrameLen = MI_U32VALUE(au8Header, 4);
//        //printf("vdecChn:%d, u32FrameLen:%d, %d\n", vdecChn, u32FrameLen, NALU_PACKET_SIZE);
//        if(u32FrameLen > NALU_PACKET_SIZE)
//        {
//            fseek(readfp, 0, SEEK_SET);
//            continue;
//        }
//        s32Len = fread(pu8Buf, 1, u32FrameLen, readfp);
//        if(s32Len <= 0)
//        {
//            fseek(readfp, 0, SEEK_SET);
//            continue;
//        }
//
//        stVdecStream.pu8Addr = pu8Buf;
//        stVdecStream.u32Len = s32Len;
//        stVdecStream.u64PTS = u64Pts;
//        stVdecStream.bEndOfFrame = 1;
//        stVdecStream.bEndOfStream = 0;
//
//        u32FpBackLen = stVdecStream.u32Len + 16; //back length
//#else
//        GetAnnexbNALU(n, vdecChn);
//        dump(n);
//        stVdecStream.pu8Addr = (MI_U8 *)n->buf;
//        stVdecStream.u32Len = n->len;
//        stVdecStream.u64PTS = u64Pts;
//        stVdecStream.bEndOfFrame = 1;
//        stVdecStream.bEndOfStream = 0;
//
//        u32FpBackLen = stVdecStream.u32Len; //back length
//#endif
//        if(0x00 == stVdecStream.pu8Addr[0] && 0x00 == stVdecStream.pu8Addr[1]
//            && 0x00 == stVdecStream.pu8Addr[2] && 0x01 == stVdecStream.pu8Addr[3]
//            && 0x65 == stVdecStream.pu8Addr[4] || 0x61 == stVdecStream.pu8Addr[4]
//            || 0x26 == stVdecStream.pu8Addr[4] || 0x02 == stVdecStream.pu8Addr[4]
//            || 0x41 == stVdecStream.pu8Addr[4])
//        {
//            usleep(s32Ms * 1000);
//        }
//        if (MI_SUCCESS != (s32Ret = MI_VDEC_SendStream(vdecChn, &stVdecStream, s32TimeOutMs)))
//        {
//            //ST_ERR("MI_VDEC_SendStream fail, chn:%d, 0x%X\n", vdecChn, s32Ret);
//            fseek(readfp, - u32FpBackLen, SEEK_CUR);
//        }
//        u64Pts = u64Pts + 33;
//        if (0 == (s32ReadCnt++ % 30))
//            ;// printf("vdec(%d) push buf cnt (%d)...\n", s32VoChannel, s32ReadCnt)
//            ;//printf("###### ==> Chn(%d) push frame(%d) Frame Dec:%d  Len:%d\n", s32VoChannel, s32ReadCnt, stChnStat.u32DecodeStreamFrames, u32Len);
//    }
//    printf("\n\n");
//    usleep(300000);
//    free(pu8Buf);
//    printf("End----------------------%d------------------End\n", stChnPort.u32ChnId);
//
//    return NULL;
//}
//
//MI_S32 SSTAR_CreateVdecChannel(MI_S32 s32VdecChn, MI_S32 s32CodecType,
//    MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32OutWidth, MI_U32 u32OutHeight)
//{
//    MI_VDEC_ChnAttr_t stVdecChnAttr;
//    MI_VDEC_OutputPortAttr_t stOutputPortAttr;
//    MI_VDEC_InitParam_t stVdecInitParam;
//
//    memset(&stVdecInitParam, 0, sizeof(MI_VDEC_InitParam_t));
//    stVdecInitParam.bDisableLowLatency = false;
//    MI_VDEC_InitDev(&stVdecInitParam);
//
//    memset(&stVdecChnAttr, 0, sizeof(MI_VDEC_ChnAttr_t));
//    stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 2;
//    stVdecChnAttr.eVideoMode    = E_MI_VDEC_VIDEO_MODE_FRAME;
//    stVdecChnAttr.u32BufSize    = 1 * 1024 * 1024;
//    stVdecChnAttr.u32PicWidth   = u32Width;
//    stVdecChnAttr.u32PicHeight  = u32Height;
//    stVdecChnAttr.u32Priority   = 0;
//    stVdecChnAttr.eCodecType    = (MI_VDEC_CodecType_e)s32CodecType;
//    stVdecChnAttr.eDpbBufMode = (MI_VDEC_DPB_BufMode_e)0;
//
//    STCHECKRESULT(MI_VDEC_CreateChn(s32VdecChn, &stVdecChnAttr));
//    STCHECKRESULT(MI_VDEC_StartChn(s32VdecChn));
//
//    if (u32OutWidth > u32Width)
//    {
//        u32OutWidth = u32Width;
//    }
//    if (u32OutHeight > u32Height)
//    {
//        u32OutHeight = u32Height;
//    }
//    stOutputPortAttr.u16Width = u32OutWidth;
//    stOutputPortAttr.u16Height = u32OutHeight;
//    MI_VDEC_SetOutputPortAttr(s32VdecChn, &stOutputPortAttr);
//
//    return MI_SUCCESS;
//}
//
//MI_S32 SSTAR_DestroyVdecChannel(MI_S32 s32VdecChn)
//{
//    MI_S32 s32Ret = MI_SUCCESS;
//
//    s32Ret = MI_VDEC_StopChn(s32VdecChn);
//    if (MI_SUCCESS != s32Ret)
//    {
//        printf("%s %d, MI_VENC_StopRecvPic %d error, %X\n", __func__, __LINE__, s32VdecChn, s32Ret);
//    }
//    s32Ret |= MI_VDEC_DestroyChn(s32VdecChn);
//    if (MI_SUCCESS != s32Ret)
//    {
//        printf("%s %d, MI_VENC_StopRecvPic %d error, %X\n", __func__, __LINE__, s32VdecChn, s32Ret);
//    }
//
//    return s32Ret;
//}

//MI_S32 SSTAR_CreateVdec2DispPipe(MI_S32 s32VdecChn, MI_S32 s32DivpChn, MI_U32 u32VdecW, MI_U32 u32VdecH, MI_S32 s32CodecType)
//{
//    ST_Rect_T stCrop= {0, 0, 0, 0};
//    MI_DISP_InputPortAttr_t stInputPortAttr;
//    SSTAR_CreateVdecChannel(s32VdecChn, s32CodecType, u32VdecW, u32VdecH, PANEL_MAX_WIDTH, PANEL_MAX_HEIGHT);
//
//    MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
//	stInputPortAttr.stDispWin.u16X      = 0;
//	stInputPortAttr.stDispWin.u16Y      = 0;
//	stInputPortAttr.stDispWin.u16Width  = PANEL_MAX_WIDTH;
//	stInputPortAttr.stDispWin.u16Height = PANEL_MAX_HEIGHT;
//	stInputPortAttr.u16SrcWidth = PANEL_MAX_WIDTH;
//	stInputPortAttr.u16SrcHeight = PANEL_MAX_HEIGHT;
//
//	printf("disp input: w=%d, h=%d\n", stInputPortAttr.u16SrcWidth, stInputPortAttr.u16SrcHeight);
//	MI_DISP_DisableInputPort(0, 0);
//	MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
//	MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
//	MI_DISP_EnableInputPort(0, 0);
//	MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);
//
//    SSTAR_ModuleBind(E_MI_MODULE_ID_VDEC, 0, s32VdecChn, 0,
//                    E_MI_MODULE_ID_DISP, 0, 0, 0); //DIVP->DISP
//
//    return MI_SUCCESS;
//}
//
//MI_S32 SSTAR_DestroyVdec2DispPipe(MI_S32 s32VdecChn, MI_S32 s32DivpChn)
//{
//    SSTAR_ModuleUnBind(E_MI_MODULE_ID_VDEC, 0, s32VdecChn, 0,
//                    E_MI_MODULE_ID_DISP, 0, 0, 0); //DIVP->DISP
//    SSTAR_DestroyVdecChannel(s32VdecChn);
//    MI_DISP_DisableInputPort(0, 0);
//
//    MI_VDEC_DeInitDev();
//
//    return MI_SUCCESS;
//}

void SSTAR_LocalCameraDisp(MI_S32 s32Disp)
{
    if (s32Disp)
    {
//        SSTAR_CreateVdec2DispPipe(0, 0, 1280, 720, E_MI_VDEC_CODEC_TYPE_H264);
//        g_VdecRun = TRUE;
//        pthread_create(&g_VdeStream_tid, NULL, SSTAR_VdecSendStream, NULL);
        #if 1
        if ((access("8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("8K_16bit_MONO.wav", 2);
            ST_DBG("open current dir wav file\n");
        }
        else if ((access("/customer/res/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("/customer/res/8K_16bit_MONO.wav", 2);
            ST_DBG("open /custome dir wav file\n");
        }
        else if ((access("/config/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StartPlayAudioFile("/config/8K_16bit_MONO.wav", 2);
            ST_DBG("open /config dir wav file\n");
        }
        #endif
        ST_DBG("Bind ST_LocalCameraDisp...\n");
    }
    else
    {
//        g_VdecRun = FALSE;
//        pthread_join(g_VdeStream_tid, NULL);
//        SSTAR_DestroyVdec2DispPipe(0, 0);
        #if 1
        if ((access("8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open current dir wav file\n");
        }
        else if ((access("/customer/res/8K_16bit_MONO.wav", F_OK))!=-1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open /customer dir wav file\n");
        }
        else if ((access("/config/8K_16bit_MONO.wav", F_OK)) != -1)
        {
            SSTAR_StopPlayAudioFile();
            ST_DBG("open /config dir wav file\n");
        }
        #endif
        ST_DBG("UnBind ST_LocalCameraDisp...\n");
    }

    return;
}

//void SSTAR_playVideo()
void SSTAR_PlayAudio()
{
    SSTAR_LocalCameraDisp(1);
}

//void SSTAR_stopVideo()
void SSTAR_StopAudio()
{
    SSTAR_LocalCameraDisp(0);
}

void SSTAR_SetVolume(int vol) {
    MI_AO_SetVolume(0, 0, vol - 60, E_MI_AO_GAIN_FADING_OFF);
    //MI_AO_SetVolume(0, 0, vol - 60, E_MI_AO_GAIN_FADING_64_SAMPLE);
}
#ifdef __cplusplus
}
#endif // __cplusplus
