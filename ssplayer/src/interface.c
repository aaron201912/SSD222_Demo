#include <sys/time.h>

#include "audiostream.h"
#include "videostream.h"
#include "demux.h"
#include "player.h"
#include "blitutil.h"
#include "interface.h"

#define DUMP_DEBUG          0

#define LOCAL_X             0
#define LOCAL_Y             0

#define MIN_WIDTH           128
#define MIN_HEIGHT          64
#define MAX_WIDTH           1920
#define MAX_HEIGHT          1080

static player_stat_t *ssplayer = NULL;
player_stat_t *g_myplayer = NULL;

static player_opts_t g_opts;
static bool g_mute = false;
static int audio_dev = AO_DEV_DAC0;

static int sstar_audio_deinit(void *arg)
{
    MI_AUDIO_DEV AoDevId = audio_dev;
    MI_AO_CHN AoChn = AUDIO_CHN;

    CheckFuncResult(MI_AO_DisableChn(AoDevId, AoChn));
    CheckFuncResult(MI_AO_Disable(AoDevId));

    return 0;
}

static int sstar_audio_init(void *arg)
{
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AUDIO_DEV AoDevId = audio_dev;
    MI_AO_CHN AoChn = AUDIO_CHN;
    MI_S32 s32SetVolumeDb = 0;
    MI_S32 s32GetVolumeDb;

    player_stat_t *is = (player_stat_t *)arg;

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt  = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 4;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;

    stAoSetAttr.eSoundmode  = E_MI_AUDIO_SOUND_MODE_MONO;
    stAoSetAttr.u32ChnCnt   = 1;
    stAoSetAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_48000;

    CheckFuncResult(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr));

    CheckFuncResult(MI_AO_GetPubAttr(AoDevId, &stAoGetAttr));

    CheckFuncResult(MI_AO_Enable(AoDevId));

    CheckFuncResult(MI_AO_EnableChn(AoDevId, AoChn));

    CheckFuncResult(MI_AO_SetVolume(AoDevId, 0, s32SetVolumeDb, E_MI_AO_GAIN_FADING_OFF));
    CheckFuncResult(MI_AO_GetVolume(AoDevId, 0, &s32GetVolumeDb));

    av_log(NULL, AV_LOG_INFO, "audio dev [%d], channel layout [%llu]\n", audio_dev, is->audio_param_src.channel_layout);

    return 0;
}

static int sstar_audio_pause(void)
{
    MI_AO_PauseChn(audio_dev, AUDIO_CHN);
    return 0;
}

static int sstar_audio_resume(void)
{
    MI_AO_ResumeChn(audio_dev, AUDIO_CHN);
    return 0;
}

static int audio_send_stream(void *arg, char *data, int len)
{
    int data_idx = 0, data_len = len;
    char *audio_pcm = (char *)data;
    int audio_write_buf_size = 0;

    MI_AUDIO_Frame_t stAoSendFrame;
    MI_S32 s32RetSendStatus = 0;
    MI_AUDIO_DEV AoDevId = audio_dev;
    MI_AO_CHN AoChn = AUDIO_CHN;
    MI_AO_ChnState_t stState;

    MI_AO_QueryChnStat(AoDevId, AoChn, &stState);
    audio_write_buf_size = stState.u32ChnBusyNum + 1024;
    //printf("remain audio_write_buf_size = %d\n", audio_write_buf_size);

    do {
        if (data_len <= MI_AUDIO_MAX_DATA_SIZE)
        {
            stAoSendFrame.u32Len[0] = data_len;
        }
        else
        {
            stAoSendFrame.u32Len[0] = MI_AUDIO_MAX_DATA_SIZE;
        }
        stAoSendFrame.apVirAddr[0] = &audio_pcm[data_idx];
        stAoSendFrame.apVirAddr[1] = NULL;

        data_len -= MI_AUDIO_MAX_DATA_SIZE;
        data_idx += MI_AUDIO_MAX_DATA_SIZE;

        do{
            s32RetSendStatus = MI_AO_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);
        }while(s32RetSendStatus == MI_AO_ERR_NOBUF);

        if(s32RetSendStatus != MI_SUCCESS)
        {
            av_log(NULL, AV_LOG_ERROR, "audio_send_stream failed, error is 0x%x: \n", s32RetSendStatus);
        }
    }while(data_len > 0);

    return audio_write_buf_size;
}

static struct timeval time_start, time_end;
static int64_t time1;

#if 0
static void sstar_video_rotate(player_stat_t *is, MI_PHY yAddr, MI_PHY uvAddr)
{
    Surface srcY, dstY;
    Surface srcUV, dstUV;
    RECT r;
    srcY.eGFXcolorFmt   = E_MI_GFX_FMT_I8;
    srcY.h              = is->p_vcodec_ctx->height;
    srcY.phy_addr       = is->phy_addr;
    srcY.pitch          = is->p_vcodec_ctx->width;
    srcY.w              = is->p_vcodec_ctx->width;
    srcY.BytesPerPixel  = 1;

    dstY.eGFXcolorFmt   = E_MI_GFX_FMT_I8;
    dstY.h              = srcY.w;
    dstY.phy_addr       = yAddr;
    dstY.pitch          = ALIGN_UP(srcY.h, 16);
    dstY.w              = srcY.h;
    dstY.BytesPerPixel  = 1;
    r.left   = 0;
    r.top    = 0;
    r.bottom = srcY.h;
    r.right  = srcY.w;
    if (is->display_mode == E_MI_DISP_ROTATE_90) {
        SstarBlitCW(&srcY, &dstY, &r);
    }
    else if (is->display_mode == E_MI_DISP_ROTATE_270) {
        SstarBlitCCW(&srcY, &dstY, &r);
    }

    srcUV.eGFXcolorFmt  = E_MI_GFX_FMT_ARGB4444;
    srcUV.h             = is->p_vcodec_ctx->height / 2;
    srcUV.phy_addr      = is->phy_addr + is->p_vcodec_ctx->width * is->p_vcodec_ctx->height;
    srcUV.pitch         = is->p_vcodec_ctx->width;
    srcUV.w             = is->p_vcodec_ctx->width / 2;
    srcUV.BytesPerPixel = 2;

    dstUV.eGFXcolorFmt  = E_MI_GFX_FMT_ARGB4444;
    dstUV.h             = srcUV.w;
    dstUV.phy_addr      = uvAddr;
    dstUV.pitch         = ALIGN_UP(srcY.h, 16);
    dstUV.w             = srcUV.h;
    dstUV.BytesPerPixel = 2;
    r.left   = 0;
    r.top    = 0;
    r.bottom = srcUV.h;
    r.right  = srcUV.w;
    if (is->display_mode == E_MI_DISP_ROTATE_90) {
        SstarBlitCW(&srcUV, &dstUV, &r);
    }
    else if (is->display_mode == E_MI_DISP_ROTATE_270) {
        SstarBlitCCW(&srcUV, &dstUV, &r);
    }
}

static int alloc_for_frame(frame_t *vp, AVFrame *frame)
{
    int ret;

    vp->buf_size = av_image_get_buffer_size(frame->format, frame->width, frame->height, 1);
    if (vp->buf_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_image_get_buffer_size failed!\n");
        return -1;
    }
    //av_log(NULL, AV_LOG_WARNING, "malloc for frame = %d\n", vp->buf_size);

    //ret = MI_SYS_MMA_Alloc((MI_U8 *)"MMU_MMA", vp->buf_size, &vp->phy_addr);
    ret = MI_SYS_MMA_Alloc((MI_U8 *)"#frame", vp->buf_size, &vp->phy_addr);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_MMA_Alloc Falied!\n");
        return -1;
    }

    ret = MI_SYS_Mmap(vp->phy_addr, vp->buf_size, (void **)&vp->vir_addr, TRUE);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_Mmap Falied!\n");
        return -1;
    }

    ret = av_image_fill_arrays(vp->frame->data,     // dst data[]
                               vp->frame->linesize, // dst linesize[]
                               vp->vir_addr,        // src buffer
                               frame->format,       // pixel format
                               frame->width,
                               frame->height,
                               1                    // align
                               );
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "av_image_fill_arrays failed!\n");
        return -1;;
    }

    vp->frame->format = frame->format;
    vp->frame->width  = frame->width;
    vp->frame->height = frame->height;
    ret = av_frame_copy(vp->frame, frame);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "av_frame_copy failed!\n");
        return -1;
    }

    return 0;
}
#endif

static int video_load_picture(void *arg, void *frame)
{
    MI_SYS_ChnPort_t  stInputChnPort;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE bufHandle;

    player_stat_t *is = (player_stat_t *)arg;
    AVFrame *av_frame = (AVFrame *)frame;

#if DUMP_DEBUG
    FILE *jpg_in = fopen("/mnt/ffmpeg/image_in.yuv", "w+");
    fwrite(av_frame->data[0], 1, av_frame->width * av_frame->height, jpg_in);
    fwrite(av_frame->data[1], 1, av_frame->width * av_frame->height / 2, jpg_in);
    fclose(jpg_in);
#endif

    //gettimeofday(&time_start, NULL);
    // YUV格式统一转换成NV12
    sws_scale(is->img_convert_ctx,                  // sws context
              (const uint8_t *const *)av_frame->data,  // src slice
              av_frame->linesize,                      // src stride
              0,                                    // src slice y
              is->p_vcodec_ctx->height,             // src slice height
              is->p_frm_yuv->data,                  // dst planes
              is->p_frm_yuv->linesize               // dst strides
              );
    //printf("nv12 linesize value=[%d %d %d], 420p linesize [%d %d %d]\n", is->p_frm_yuv->linesize[0], is->p_frm_yuv->linesize[1],
    //is->p_frm_yuv->linesize[2], av_frame->linesize[0], av_frame->linesize[1], av_frame->linesize[2]);
    //gettimeofday(&time_end, NULL);
    //time1 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
    //printf("time of sws_scale format: %lldus\n", time1);

#if DUMP_DEBUG
    uint8_t *p_src_u = av_frame->data[1];
    uint8_t *p_src_v = av_frame->data[1] + av_frame->width * av_frame->height / 4;
    ST_I420ToNV12((unsigned char*)av_frame->data[0], av_frame->width,
                   p_src_u, av_frame->width / 2,
                   p_src_v, av_frame->width / 2,
                   is->p_frm_yuv->data[0], av_frame->width,
                   is->p_frm_yuv->data[1], av_frame->width,
                   av_frame->width, av_frame->height);
    jpg_in = fopen("/mnt/ffmpeg/image_out.yuv", "w+");
    fwrite(is->p_frm_yuv->data[0], 1, is->p_frm_yuv->width * is->p_frm_yuv->height, jpg_in);
    fwrite(is->p_frm_yuv->data[1], 1, is->p_frm_yuv->width * is->p_frm_yuv->height / 2, jpg_in);
    fclose(jpg_in);
#endif

    memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
    memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
    memset(&stInputChnPort, 0, sizeof(MI_SYS_ChnPort_t));

    stInputChnPort.eModId           = E_MI_MODULE_ID_DIVP;
    stInputChnPort.u32ChnId         = 0;
    stInputChnPort.u32DevId         = 0;
    stInputChnPort.u32PortId        = 0;

    stBufConf.eBufType              = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.stFrameCfg.eFormat    = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stBufConf.stFrameCfg.u16Height  = is->p_frm_yuv->height;
    stBufConf.stFrameCfg.u16Width   = is->p_frm_yuv->width;

    if (MI_SUCCESS == MI_SYS_ChnInputPortGetBuf(&stInputChnPort, &stBufConf, &stBufInfo, &bufHandle, -1))
    {
        stBufInfo.stFrameData.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
        stBufInfo.stFrameData.eFieldType    = E_MI_SYS_FIELDTYPE_NONE;
        stBufInfo.stFrameData.eTileMode     = E_MI_SYS_FRAME_TILE_MODE_NONE;
        stBufInfo.bEndOfStream              = FALSE;

        //gettimeofday(&time_start, NULL);

        int image_size = is->p_frm_yuv->width * is->p_frm_yuv->height;
        /*if (is->vir_addr && is->phy_addr) {
            MI_SYS_MemcpyPa(stBufInfo.stFrameData.phyAddr[0], is->phy_addr, image_size);
            MI_SYS_MemcpyPa(stBufInfo.stFrameData.phyAddr[1], is->phy_addr + image_size, image_size / 2);
        } else {
            memcpy(stBufInfo.stFrameData.pVirAddr[0], (uint8_t *)(is->p_frm_yuv->data[0]), image_size);
            memcpy(stBufInfo.stFrameData.pVirAddr[1], (uint8_t *)(is->p_frm_yuv->data[1]), image_size / 2);
        }*/
        for (int index = 0; index < stBufInfo.stFrameData.u16Height; index ++)
        {
            if (is->vir_addr && is->phy_addr) {
                MI_SYS_MemcpyPa(stBufInfo.stFrameData.phyAddr[0] + index * stBufInfo.stFrameData.u32Stride[0],
                is->phy_addr + index * is->p_frm_yuv->width, is->p_frm_yuv->width);
            } else {
                memcpy(stBufInfo.stFrameData.pVirAddr[0] + index * stBufInfo.stFrameData.u32Stride[0],
                (uint8_t *)(is->p_frm_yuv->data[0] + index * is->p_frm_yuv->width),
                is->p_frm_yuv->width);
            }
        }
        for (int index = 0; index < stBufInfo.stFrameData.u16Height / 2; index ++)
        {
            if (is->vir_addr && is->phy_addr) {
                MI_SYS_MemcpyPa(stBufInfo.stFrameData.phyAddr[1] + index * stBufInfo.stFrameData.u32Stride[1],
                is->phy_addr + image_size + index * is->p_frm_yuv->width, is->p_frm_yuv->width);
            } else {
                memcpy(stBufInfo.stFrameData.pVirAddr[1] + index * stBufInfo.stFrameData.u32Stride[1],
                (uint8_t *)(is->p_frm_yuv->data[1] + index * is->p_frm_yuv->width),
                is->p_frm_yuv->width);
            }
        }

        //gettimeofday(&time_end, NULL);
        //time1 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //printf("time of memory copy format: %lldus\n", time1);

        MI_SYS_ChnInputPortPutBuf(bufHandle, &stBufInfo, FALSE);
    }

//    gettimeofday(&time_end, NULL);
//    time1 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
//    time_start.tv_sec  = time_end.tv_sec;
//    time_start.tv_usec = time_end.tv_usec;
//    printf("time of video_display : %lldus\n", time1);

    return 0;
}

static int sstar_display_set(void *arg)
{
    MI_DISP_InputPortAttr_t stInputPortAttr;

    player_stat_t *is = (player_stat_t *)arg;

    CheckFuncResult(MI_DISP_DisableInputPort(0, 0));

    CheckFuncResult(MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr));
    stInputPortAttr.u16SrcWidth         = ALIGN_BACK(is->src_width , 32);
    stInputPortAttr.u16SrcHeight        = ALIGN_BACK(is->src_height, 32);
    stInputPortAttr.stDispWin.u16X      = is->pos_x;
    stInputPortAttr.stDispWin.u16Y      = is->pos_y;
    stInputPortAttr.stDispWin.u16Width  = is->out_width;
    stInputPortAttr.stDispWin.u16Height = is->out_height;

    CheckFuncResult(MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr));
    CheckFuncResult(MI_DISP_EnableInputPort(0, 0));

    MI_DIVP_ChnAttr_t stDivpAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;

    memset(&stDivpAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    stDivpAttr.bHorMirror           = FALSE;
    stDivpAttr.bVerMirror           = FALSE;
    stDivpAttr.eDiType              = E_MI_DIVP_DI_TYPE_OFF;
    stDivpAttr.eRotateType          = E_MI_SYS_ROTATE_NONE;
    stDivpAttr.eTnrLevel            = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpAttr.stCropRect.u16X      = 0;
    stDivpAttr.stCropRect.u16Y      = 0;
    stDivpAttr.stCropRect.u16Width  = 0;
    stDivpAttr.stCropRect.u16Height = 0;
    stDivpAttr.u32MaxWidth          = 1920;
    stDivpAttr.u32MaxHeight         = 1080;
    CheckFuncResult(MI_DIVP_CreateChn(0, &stDivpAttr));

    memset(&stOutputPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
    stOutputPortAttr.eCompMode      = E_MI_SYS_COMPRESS_MODE_NONE;
    stOutputPortAttr.u32Width       = ALIGN_BACK(is->src_width , 32);
    stOutputPortAttr.u32Height      = ALIGN_BACK(is->src_height, 32);
    stOutputPortAttr.ePixelFormat   = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    CheckFuncResult(MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr));
    CheckFuncResult(MI_DIVP_StartChn(0));

    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    memset(&stSrcChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stSrcChnPort.eModId     = E_MI_MODULE_ID_DIVP;
    stSrcChnPort.u32DevId   = 0;
    stSrcChnPort.u32ChnId   = 0;
    stSrcChnPort.u32PortId  = 0;

    memset(&stDstChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stDstChnPort.eModId     = E_MI_MODULE_ID_DISP;
    stDstChnPort.u32DevId   = 0;
    stDstChnPort.u32ChnId   = 0;
    stDstChnPort.u32PortId  = 0;

    CheckFuncResult(MI_SYS_BindChnPort2(&stSrcChnPort, &stDstChnPort, 30, 30, E_MI_SYS_BIND_TYPE_FRAME_BASE, 0));

    return 0;
}

static int sstar_display_unset(void *arg)
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    memset(&stSrcChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stSrcChnPort.eModId     = E_MI_MODULE_ID_DIVP;
    stSrcChnPort.u32DevId   = 0;
    stSrcChnPort.u32ChnId   = 0;
    stSrcChnPort.u32PortId  = 0;

    memset(&stDstChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stDstChnPort.eModId     = E_MI_MODULE_ID_DISP;
    stDstChnPort.u32DevId   = 0;
    stDstChnPort.u32ChnId   = 0;
    stDstChnPort.u32PortId  = 0;

    CheckFuncResult(MI_SYS_UnBindChnPort(&stSrcChnPort, &stDstChnPort));

    CheckFuncResult(MI_DIVP_StopChn(0));
    CheckFuncResult(MI_DIVP_DestroyChn(0));

    CheckFuncResult(MI_DISP_ClearInputPortBuffer(DISP_LAYER, DISP_INPUTPORT, TRUE));
    CheckFuncResult(MI_DISP_HideInputPort(DISP_LAYER, DISP_INPUTPORT));
    CheckFuncResult(MI_DISP_DisableInputPort(DISP_LAYER, DISP_INPUTPORT));
    return 0;
}

static int sys_mmp_malloc(void *name, void **vir_addr, void *phy_addr, int size)
{
    int ret;

    ret = MI_SYS_MMA_Alloc((MI_U8 *)name, (MI_U32)size, (MI_PHY *)phy_addr);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_MMA_Alloc Falied!\n");
        return -1;
    }

    ret = MI_SYS_Mmap(*(MI_PHY*)phy_addr, (MI_U32)size, (void **)vir_addr, TRUE);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_Mmap Falied!\n");
        return -1;
    }

    return 0;
}

static int sys_unmmp_free(void *vir_addr, unsigned long long phy_addr, int size)
{
    int ret;

    ret = MI_SYS_Munmap(vir_addr, (MI_U32)size);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_Munmap Falied!\n");
        return -1;
    }

    ret = MI_SYS_MMA_Free((MI_PHY)phy_addr);
    if (ret != MI_SUCCESS) {
        av_log(NULL, AV_LOG_ERROR, "MI_SYS_MMA_Free Falied!\n");
        return -1;
    }

    return 0;
}
/************************************************************************************************/

static void set_global_functions(void *arg)
{
    player_stat_t *is = (player_stat_t *)arg;

    is->functions.audio_init   = sstar_audio_init;
    is->functions.audio_deinit = sstar_audio_deinit;
    is->functions.audio_play   = audio_send_stream;
    is->functions.audio_pause  = sstar_audio_pause;
    is->functions.audio_resume = sstar_audio_resume;
    is->functions.video_init   = sstar_display_set;
    is->functions.video_deinit = sstar_display_unset;
    is->functions.video_play   = video_load_picture;
    //is->functions.sys_malloc   = sys_mmp_malloc;
    //is->functions.sys_free     = sys_unmmp_free;
}

static void reset_global_variable(void)
{
    memset(&g_opts, 0x0, sizeof(player_opts_t));
    g_mute = false;
    audio_dev  = AUDIO_DEV;
    ssplayer   = NULL;
    g_myplayer = NULL;
}

int sstar_player_open(const char *fp, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    int ret;

    if (ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "player has been opened!\n");
        return -1;
    }

    if (!fp || width < MIN_WIDTH || width > MAX_WIDTH || height < MIN_HEIGHT || height > MAX_HEIGHT) {
        av_log(NULL, AV_LOG_ERROR, "player input parameter invalid!\n");
        return -1;
    }

    ssplayer = player_init(fp);
    if (ssplayer == NULL) {
        av_log(NULL, AV_LOG_ERROR, "player init failed!\n");
        return -1;
    }

    memcpy(&ssplayer->options, &g_opts, sizeof(player_opts_t));
    ssplayer->display_mode = ssplayer->options.rotate_attr;
    ssplayer->in_width     = width;
    ssplayer->in_height    = height;
    ssplayer->pos_x        = x;
    ssplayer->pos_y        = y;
    av_log(NULL, AV_LOG_INFO, "set out width : %d, height : %d\n", width, height);

    set_global_functions(ssplayer);

    ret = open_demux(ssplayer);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open_demux failed!\n");
        return ret;
    }
    av_log(NULL, AV_LOG_INFO, "open_demux successful!\n");

    ret = open_video(ssplayer);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open_video failed!\n");
        return ret;
    }
    av_log(NULL, AV_LOG_INFO, "open video successful!\n");

    ret = open_audio(ssplayer);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open_audio failed!\n");
        return ret;
    }
    av_log(NULL, AV_LOG_INFO, "open audio successful!\n");

    g_myplayer = ssplayer;

    return ret;
}

int sstar_player_close(void)
{
    int ret;

    if (ssplayer == NULL) {
        av_log(NULL, AV_LOG_ERROR, "player has been closed!\n");
        return -1;
    }

    player_deinit(ssplayer);

    reset_global_variable();

    return ret;
}

//暂停播放
int sstar_player_pause(void)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_pause failed!\n");
        return -1;
    }

    if (!ssplayer->paused)
        toggle_pause(ssplayer);

    return 0;
}

//恢复播放
int sstar_player_resume(void)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_resume failed!\n");
        return -1;
    }

    if (ssplayer->paused)
        toggle_pause(ssplayer);

    return 0;
}

//从当前位置向前seek到指定时间, 单位秒
int sstar_player_seek2time(double time)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_seek2time failed!\n");
        return -1;
    }

    double target, pos, diff;

    pos = get_master_clock(ssplayer);
    if (isnan(pos))
        pos = (double)ssplayer->seek_pos / AV_TIME_BASE;

    if (time < 0.1) {
        diff = ssplayer->p_fmt_ctx->start_time - pos;
        stream_seek(ssplayer, ssplayer->p_fmt_ctx->start_time, (int64_t)(diff * AV_TIME_BASE), ssplayer->seek_by_bytes);
    } else {
        diff   = time - pos;
        target = time;
        if (ssplayer->p_fmt_ctx->start_time != AV_NOPTS_VALUE && time <= ssplayer->p_fmt_ctx->start_time / (double)AV_TIME_BASE)
            target = ssplayer->p_fmt_ctx->start_time / (double)AV_TIME_BASE;
        if (ssplayer->p_fmt_ctx->duration != AV_NOPTS_VALUE && time >= ssplayer->p_fmt_ctx->duration / (double)AV_TIME_BASE)
            target = ssplayer->p_fmt_ctx->duration / (double)AV_TIME_BASE;
        stream_seek(ssplayer, (int64_t)(target * AV_TIME_BASE), (int64_t)(diff * AV_TIME_BASE), ssplayer->seek_by_bytes);
    }

    return 0;
}

//在当前位置向前或向后seek一段时间, 正数向前负数向, 单位秒
int sstar_player_seek(double time)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_seek failed!\n");
        return -1;
    }

    double pos;
    pos = get_master_clock(ssplayer);
    if (isnan(pos))
        pos = (double)ssplayer->seek_pos / AV_TIME_BASE;
    pos += time;
    if (ssplayer->p_fmt_ctx->start_time != AV_NOPTS_VALUE && pos <= ssplayer->p_fmt_ctx->start_time / (double)AV_TIME_BASE)
        pos = ssplayer->p_fmt_ctx->start_time / (double)AV_TIME_BASE;
    if (ssplayer->p_fmt_ctx->duration != AV_NOPTS_VALUE && pos >= ssplayer->p_fmt_ctx->duration / (double)AV_TIME_BASE)
        pos = ssplayer->p_fmt_ctx->duration / (double)AV_TIME_BASE;
    stream_seek(ssplayer, (int64_t)(pos * AV_TIME_BASE), (int64_t)(time * AV_TIME_BASE), ssplayer->seek_by_bytes);

    return 0;
}

int sstar_player_status(void)
{
    if (!ssplayer) {
        return -1;
    }

    if (ssplayer->status < 0) {
        return PLAYER_ERROR;
    } else if (ssplayer->audio_complete && ssplayer->video_complete) {
        return PLAYER_DONE;
    } else {
        return PLAYER_IDLE;
    }
}

//获取视频总时长
int sstar_player_getduration(double *duration)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_getduration failed!\n");
        return -1;
    }

    if (ssplayer->p_fmt_ctx->duration != AV_NOPTS_VALUE) {
        *duration = ssplayer->p_fmt_ctx->duration * av_q2d(AV_TIME_BASE_Q);
    } else {
        *duration = NAN;
        av_log(NULL, AV_LOG_WARNING, "get invalid duration!\n");
    }

    return 0;
}

//获取视频当前播放的时间点
int sstar_player_gettime(double *time)
{
    if (!ssplayer) {
        av_log(NULL, AV_LOG_ERROR, "sstar_player_gettime failed!\n");
        return -1;
    }

    if (ssplayer->p_fmt_ctx->start_time != AV_NOPTS_VALUE) {
        if (ssplayer->av_sync_type == AV_SYNC_AUDIO_MASTER && ssplayer->audio_clock != NAN) {
            *time = ssplayer->audio_clock - (ssplayer->p_fmt_ctx->start_time * av_q2d(AV_TIME_BASE_Q));
        } else if (ssplayer->av_sync_type == AV_SYNC_VIDEO_MASTER && ssplayer->video_clock != NAN) {
            *time = ssplayer->video_clock - (ssplayer->p_fmt_ctx->start_time * av_q2d(AV_TIME_BASE_Q));
        } else {
            *time = NAN;
            av_log(NULL, AV_LOG_WARNING, "get invalid current time!\n");
        }
    } else {
        *time = NAN;
        av_log(NULL, AV_LOG_WARNING, "get invalid current time!\n");
    }

    return 0;
}

//设置音量[0~100]
int sstar_player_set_volumn(int volumn)
{
    if (ssplayer == NULL || volumn < 0 || volumn > 100) {
        printf("set volumn range : [0~100]\n");
        return -1;
    }

    if (ssplayer->audio_idx >= 0) {
        MI_S32 vol;
        MI_AO_ChnState_t stAoState;

        if (volumn) {
            //vol = volumn * (MAX_ADJUST_AO_VOLUME - MIN_ADJUST_AO_VOLUME) / 100 + MIN_ADJUST_AO_VOLUME;
            //vol = (vol > MAX_ADJUST_AO_VOLUME) ? MAX_ADJUST_AO_VOLUME : vol;
            //vol = (vol < MIN_ADJUST_AO_VOLUME) ? MIN_ADJUST_AO_VOLUME : vol;
            vol = (int)(log10(volumn * 1.0) * 45 - 60);
            vol = (vol > 30) ? 30 : vol;
        } else {
            vol = MIN_AO_VOLUME;
        }

        memset(&stAoState, 0, sizeof(MI_AO_ChnState_t));
        if (MI_SUCCESS == MI_AO_QueryChnStat(audio_dev, AUDIO_CHN, &stAoState))
        {
            MI_AO_SetVolume(audio_dev, AUDIO_CHN, vol, E_MI_AO_GAIN_FADING_OFF);
            MI_AO_SetMute(audio_dev, AUDIO_CHN, g_mute);
        }
        av_log(NULL, AV_LOG_INFO, "set volumn and dB value [%d %d]\n", volumn, vol);
    }

    return 0;
}

//设置静音
int sstar_player_set_mute(bool mute)
{
    if (ssplayer == NULL) {
        printf("sstar_player_set_mute failed!\n");
        return -1;
    }

    if (ssplayer->audio_idx >= 0) {
        g_mute = mute;
        MI_AO_SetMute(audio_dev, AUDIO_CHN, mute);
    }

    return 0;
}

//设置播放器的一些客制化属性, 包括单独音频模式\单独视频模式\旋转
int sstar_player_setopts(const char *key, const char *value, int flags)
{
    if (!strcmp(key, "audio_only")) {
        sscanf(value, "%d", &g_opts.audio_only);
        printf("player options audio_only = %d\n", g_opts.audio_only);
    }

    if (!strcmp(key, "video_only")) {
        sscanf(value, "%d", &g_opts.video_only);
        printf("player options video_only = %d\n", g_opts.video_only);
    }

    if (!strcmp(key, "rotate")) {
        int rotate_attr;
        sscanf(value, "%d", &rotate_attr);
        if (rotate_attr == 90) {
            g_opts.rotate_attr = E_MI_DISP_ROTATE_90;
        } else if (rotate_attr == 180) {
            g_opts.rotate_attr = E_MI_DISP_ROTATE_180;
        } else if (rotate_attr == 270) {
            g_opts.rotate_attr = E_MI_DISP_ROTATE_270;
        } else {
            g_opts.rotate_attr = E_MI_DISP_ROTATE_NONE;
        }
        printf("player options rotate_attr = %d\n", g_opts.rotate_attr);
    }

    if (!strcmp(key, "displayer")) {
        if (!strcmp(value, "hdmi")) {
            g_opts.audio_dev = 3;
        } else {
            g_opts.audio_dev = 0;
        }
        printf("player options audio_dev = %d\n", g_opts.audio_dev);
    }

    return 0;
}


