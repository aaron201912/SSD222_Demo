#include "videostream.h"
#include "packet.h"
#include "frame.h"
#include "player.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define VIDEO_DECODE_TEST      0

extern AVPacket v_flush_pkt;

static struct timeval time_start, time_end;
static int64_t time0;

static int queue_picture(player_stat_t *is, AVFrame *src_frame, double pts, double duration, int64_t pos)
{
    frame_t *vp;
    frame_queue_t *f = &is->video_frm_queue;

    //if (!(vp = frame_queue_peek_writable(&is->video_frm_queue)))
    //    return -1;
    vp = &f->queue[f->windex];

    vp->sar = src_frame->sample_aspect_ratio;
    vp->uploaded = 0;

    vp->width = src_frame->width;
    vp->height = src_frame->height;
    vp->format = src_frame->format;

    vp->pts = pts;
    vp->duration = duration;
    vp->pos = pos;
    //vp->serial = serial;

    // 将AVFrame拷入队列相应位置
    av_frame_move_ref(vp->frame, src_frame);

    // 更新队列计数及写索引
    //printf("before queue ridx: %d,widx: %d,size: %d,maxsize: %d\n ",is->video_frm_queue.rindex,is->video_frm_queue.windex,is->video_frm_queue.size,is->video_frm_queue.max_size);
    frame_queue_push(&is->video_frm_queue);

    return 0;
}

// 从packet_queue中取一个packet，解码生成frame
static int video_decode_frame(AVCodecContext *p_codec_ctx, packet_queue_t *p_pkt_queue, AVFrame *frame)
{
    int ret;

    while (1)
    {
        AVPacket pkt;

        while (1)
        {
            if(p_pkt_queue->abort_request) {
                return -1;
            }
            // 3. 从解码器接收frame
            // 3.1 一个视频packet含一个视频frame
            //     解码器缓存一定数量的packet后，才有解码后的frame输出
            //     frame输出顺序是按pts的顺序，如IBBPBBP
            //     frame->pkt_pos变量是此frame对应的packet在视频文件中的偏移地址，值同pkt.pos
            ret = avcodec_receive_frame(p_codec_ctx, frame);
            if (ret < 0)
            {
                if (ret == AVERROR_EOF)
                {
                    av_log(NULL, AV_LOG_INFO, "video avcodec_receive_frame(): the decoder has been flushed\n");
                    avcodec_flush_buffers(p_codec_ctx);
                    return 0;
                }
                else if (ret == AVERROR(EAGAIN))
                {
                    //av_log(NULL, AV_LOG_ERROR, "ret : %d, cann't fetch a frame, try again!\n", ret);
                    break;
                }
                else
                {
                    av_log(NULL, AV_LOG_ERROR, "video avcodec_receive_frame(): other errors\n");
                    continue;
                }
            }
            else
            {
                frame->pts = frame->best_effort_timestamp;
                //printf("receive frame pts: %lld, frame number: %d\n", frame->pts, p_codec_ctx->frame_number);
                return 1;   // 成功解码得到一个视频帧或一个音频帧，则返回
            }
        }

        // 1. 取出一个packet。使用pkt对应的serial赋值给d->pkt_serial
        if (packet_queue_get(p_pkt_queue, &pkt, true) < 0)
        {
            printf("video packet_queue_get fail\n");
            return -1;
        }

        if (pkt.data == v_flush_pkt.data)
        {
            // 复位解码器内部状态/刷新内部缓冲区。
            avcodec_flush_buffers(p_codec_ctx);
            printf("video avcodec_flush_buffers!\n");
        }
        else
        {
            // 2. 将packet发送给解码器
            //    发送packet的顺序是按dts递增的顺序，如IPBBPBB
            //    pkt.pos变量可以标识当前packet在视频文件中的地址偏移
            ret = avcodec_send_packet(p_codec_ctx, &pkt);
            if (ret == AVERROR(EAGAIN)) {
                av_log(NULL, AV_LOG_ERROR, "receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
            }
        }
        av_packet_unref(&pkt);
    }
}

// 根据视频时钟与同步时钟(如音频时钟)的差值，校正delay值，使视频时钟追赶或等待同步时钟
// 输入参数delay是上一帧播放时长，即上一帧播放后应延时多长时间后再播放当前帧，通过调节此值来调节当前帧播放快慢
// 返回值delay是将输入参数delay经校正后得到的值
static double compute_target_delay(double delay, player_stat_t *is)
{
    double diff = 0;

    /* update delay to follow master synchronisation source */
    if (is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_idx >= 0) {
        /* if video is slave, we try to correct big delays by
           duplicating or deleting a frame */
        diff = get_clock(&is->video_clk) - get_master_clock(is);

        if (!isnan(diff) && fabs(diff) > 2 * AV_SYNC_THRESHOLD_MAX) {
            /* skip or repeat frame. We take into account the
               delay to compute the threshold. I still don't know
               if it is the best guess */
            if (!isnan(diff)) {
                /*double sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
                if (diff <= -sync_threshold) {
                    delay = FFMAX(0, delay + diff);
                }
                else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD) {
                    delay = delay + diff;
                }
                else if (diff >= sync_threshold) {
                    delay = 2 * delay;
                }*/

                if (diff < -AV_SYNC_THRESHOLD_MAX) {
                    delay = FFMAX(0, delay + diff);
                }
                else if (diff > AV_SYNC_THRESHOLD_MAX) {
                    delay = 2 * delay;
                }
                else {
                    delay = delay;
                }
            }
        }

        av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\r", delay, -diff);
    }

    return delay;
}

#if 0
static double vp_duration(player_stat_t *is, frame_t *vp, frame_t *nextvp) {
    if (vp->serial == nextvp->serial)
    {
        double duration = nextvp->pts - vp->pts;
        if (isnan(duration) || duration <= 0)
            return vp->duration;
        else
            return duration;
    } else {
        return 0.0;
    }
}
#endif

static void update_video_pts(player_stat_t *is, double pts, int64_t pos, int serial) {
    /* update current video pts */
    set_clock(&is->video_clk, pts, serial);            // 更新vidclock
    //-sync_clock_to_slave(&is->extclk, &is->vidclk);  // 将extclock同步到vidclock
}

static void video_display(player_stat_t *is)
{
    frame_t *vp;

    //av_log(NULL, AV_LOG_ERROR, "rindex : %d, shownidex : %d, size : %d\n", is->video_frm_queue.rindex, is->video_frm_queue.rindex_shown, is->video_frm_queue.size);
    vp = frame_queue_peek_last(&is->video_frm_queue);
    if (!vp->frame->width || !vp->frame->height)
    {
        av_log(NULL, AV_LOG_ERROR, "invalid frame width and height!\n");
        return;
    }

    if (is->functions.video_play) {
        is->functions.video_play(is, vp->frame);
    }

    /*gettimeofday(&time_end, NULL);
    time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
    time_start.tv_sec  = time_end.tv_sec;
    time_start.tv_usec = time_end.tv_usec;
    printf("time of video_display : %lldus\n", time0);*/
}

/* called to display each frame */
static int video_refresh(void *opaque, double *remaining_time, double duration)
{
    player_stat_t *is = (player_stat_t *)opaque;
    double time, delay;
    frame_t *vp;
    static double time_bias;

    if (is->frame_timer < 0.1) {
        is->frame_timer = av_gettime_relative() / 1000000.0;
        //av_log(NULL, AV_LOG_INFO, "is->frame_timer first value : %.3f\n", is->frame_timer);
    }

retry:
    // 暂停处理：disp will keep last picture
    if (is->paused)
        return 0;

    if (frame_queue_nb_remaining(&is->video_frm_queue) <= 0)  // 所有帧已显示
    {
        // nothing to do, no picture to display in the queue
        //printf("already last frame: %d\n",is->video_frm_queue.size);
        if (!is->video_complete && is->eof && is->video_pkt_queue.nb_packets == 0 && is->start_play) {
            is->video_complete = 1;
            if (is->video_complete && is->audio_complete) {
                is->status = 1;
                //stream_seek(is, is->p_fmt_ctx->start_time, 0, is->seek_by_bytes);
            }
            av_log(NULL, AV_LOG_INFO, "video play completely, total/left frame = [%d %d]!\n", is->p_vcodec_ctx->frame_number, is->video_frm_queue.size);
        } else {
            return 0;
        }
    }

    /* dequeue the picture */
    vp = frame_queue_peek(&is->video_frm_queue);   // 当前帧：当前待显示的帧
    //printf("refresh ridx: %d,rs:%d,widx: %d,size: %d,maxsize: %d\n",is->video_frm_queue.rindex,is->video_frm_queue.rindex_shown,is->video_frm_queue.windex,is->video_frm_queue.size,is->video_frm_queue.max_size);

    delay = compute_target_delay(vp->duration, is);    // 根据视频时钟和同步时钟的差值，计算delay值
    if (!strcmp(is->p_fmt_ctx->iformat->name, "rtsp")) {
        if (is->video_pkt_queue.nb_packets > 20) {
            time_bias = delay / 2;
        } else if (is->video_pkt_queue.nb_packets > 10) {
            time_bias = delay / 4;
        } else if (is->video_pkt_queue.nb_packets <= 3) {
            time_bias = 0;
        }
    }
    delay = delay - time_bias;

    //printf("last_duration: %lf, delay: %lf\n", duration, delay);
    time = av_gettime_relative()/1000000.0;
    // 当前帧播放时刻(is->frame_timer+delay)大于当前时刻(time)，表示播放时刻未到
    if (time < is->frame_timer + delay && is->start_play) {
        // 播放时刻未到，则更新刷新时间remaining_time为当前时刻到下一播放时刻的时间差
        *remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
        return 0;
    }
    //printf("remaining time : %f. duration : %f.\n", *remaining_time, last_duration);
    // 更新frame_timer值
    is->frame_timer += delay;
    //printf("frame_timer : %0.6lf, video pts : %0.6lf, mremaining : %0.6lf\n", is->frame_timer, vp->pts, *remaining_time);
    // 校正frame_timer值：若frame_timer落后于当前系统时间太久(超过最大同步域值)，则更新为当前系统时间
    if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
    {
        is->frame_timer = time;
    }

    pthread_mutex_lock(&is->video_frm_queue.mutex);
    if (!isnan(vp->pts))
    {
        update_video_pts(is, vp->pts, vp->pos, vp->serial); // 更新视频时钟：时间戳、时钟时间
        is->video_clock = vp->pts;
    }
    pthread_mutex_unlock(&is->video_frm_queue.mutex);

    // 是否要丢弃未能及时播放的视频帧
    if (frame_queue_nb_remaining(&is->video_frm_queue) > 1)  // 队列中未显示帧数>1(只有一帧则不考虑丢帧)
    {
        // 当前帧vp未能及时播放，即下一帧播放时刻(is->frame_timer+duration)小于当前系统时刻(time)
        if (time > is->frame_timer + duration && is->start_play)
        {
            frame_queue_next(&is->video_frm_queue);   // 删除上一帧已显示帧，即删除lastvp，读指针加1(从lastvp更新到vp)
            //av_log(NULL, AV_LOG_WARNING, "discard current frame!\n");
            goto retry;
        }
    }

    frame_queue_next(&is->video_frm_queue);
    // 删除当前读指针元素，读指针+1。若未丢帧，读指针从lastvp更新到vp；若有丢帧，读指针从vp更新到nextvp
    if (is->step && !is->paused) {
        stream_toggle_pause(is);
    }

    video_display(is);                  // 取出当前帧vp(若有丢帧是nextvp)进行播放

    if (!is->start_play) {              // 播放第一张图片后认为开始播放
        is->start_play = true;
        av_log(NULL, AV_LOG_WARNING, "play the first frame done!\n");
    }
    return 0;
}

// 将视频包解码得到视频帧，然后写入picture队列
static void * video_decode_thread(void *arg)
{
    player_stat_t *is = (player_stat_t *)arg;
    double last_pts, pts, duration;
    int ret, got_picture;
    AVRational tb = is->p_video_stream->time_base;
    AVRational frame_rate = av_guess_frame_rate(is->p_fmt_ctx, is->p_video_stream, NULL);

    AVFrame *p_frame = av_frame_alloc();
    if (p_frame == NULL) {
        av_log(NULL, AV_LOG_ERROR, "av_frame_alloc() for p_frame failed\n");
        return NULL;
    }

    last_pts = (is->p_fmt_ctx->start_time != AV_NOPTS_VALUE) ? is->p_fmt_ctx->start_time * av_q2d(AV_TIME_BASE_Q) : 0.0;
    duration = av_q2d((AVRational){frame_rate.den, frame_rate.num});
    printf("video time base : %.3fs, start time : %.3fs.\n", AV_TIME_BASE * av_q2d(tb), last_pts);
    printf("fps : %.3f, frame rate num : %d. frame rate den : %d.\n", duration, frame_rate.num, frame_rate.den);

    printf("get in video decode thread!\n");

    while (1)
    {
        if(is->abort_request)
        {
            break;
        }
        got_picture = video_decode_frame(is->p_vcodec_ctx, &is->video_pkt_queue, p_frame);
        if (got_picture < 0)
        {
            printf("video got pic fail\n");
            goto exit;
        } 
        else if (got_picture > 0)
        {
            duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);   // 当前帧播放时长

            if (p_frame->pts == AV_NOPTS_VALUE) {  // 视频中时间戳无效
                pts = last_pts;
                last_pts += duration;
            } else {
                pts = p_frame->pts * av_q2d(tb);   // 当前帧显示时间戳
                duration = (pts - last_pts < 0.0) ? 0.0 : (pts - last_pts);
                last_pts = pts;
            }

            //printf("frame number : %d, video frame duration : %f.\n", is->p_vcodec_ctx->frame_number, duration);
            //printf("frame queue num : %d, video frame clock : %f.\n", is->video_frm_queue.size, pts);
            ret = queue_picture(is, p_frame, pts, duration, p_frame->pkt_pos);          // 将当前帧压入frame_queue
            av_frame_unref(p_frame);

            if (NULL == frame_queue_peek_writable(&is->video_frm_queue)) {
                ret = -1;
                goto exit;
            }

#if VIDEO_DECODE_TEST
            frame_queue_next(&is->video_frm_queue);
            gettimeofday(&time_end, NULL);
            time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
            time_start.tv_sec  = time_end.tv_sec;
            time_start.tv_usec = time_end.tv_usec;
            printf("time of video_decode_thread : %lldus\n", time0);
#endif
        }
        if (ret < 0)
        {
            printf("queue_picture failed!\n");
            goto exit;
        }
    }

exit:
    printf("video decode thread exit\n");
    av_frame_free(&p_frame);

    return NULL;
}

static void * video_playing_thread(void *arg)
{
    int ret;
    player_stat_t *is = (player_stat_t *)arg;
    AVRational frame_rate = av_guess_frame_rate(is->p_fmt_ctx, is->p_video_stream, NULL);
    double duration = av_q2d((AVRational){frame_rate.den, frame_rate.num});
    double remaining_time = 0.0;

    printf("video fps time : %.3lf\n", duration);

    printf("video_playing_thread in\n");

    while (1)
    {
        if(is->abort_request)
        {
            break;
        }

        if (remaining_time > 0.0)
        {
            av_usleep((unsigned)(remaining_time * 1000000.0));
        }
        remaining_time = REFRESH_RATE;

        // 立即显示当前帧，或延时remaining_time后再显示
        ret = video_refresh(is, &remaining_time, duration);
        if (ret < 0) {
            break;
        }
    }

    printf("video play thread exit\n");

    return NULL;
}

static int open_video_playing(void *arg)
{
    player_stat_t *is = (player_stat_t *)arg;
    int ret;
    int dst_width, dst_height;
    const AVPixFmtDescriptor *desc;

    if (is->functions.video_init) {
        ret = is->functions.video_init(is);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "player video init failed!\n");
            return ret;
        }
        is->video_enable = true;
    }

    is->p_frm_yuv = av_frame_alloc();
    if (is->p_frm_yuv == NULL)
    {
        printf("av_frame_alloc() for p_frm_raw failed\n");
        return -1;
    }

    if (is->p_vcodec_ctx->width > is->p_vcodec_ctx->height) {
        dst_width  = FFMIN(is->p_vcodec_ctx->width, 1920);
        dst_height = FFMIN(is->p_vcodec_ctx->height, 1080);
    } else {
        dst_width  = FFMIN(is->p_vcodec_ctx->width, 1080);
        dst_height = FFMIN(is->p_vcodec_ctx->height, 1920);
    }

    //dst_width  = is->src_width;
    //dst_height = is->src_height;
    // 为AVFrame.*data[]手工分配缓冲区，用于存储sws_scale()中目的帧视频数据
    is->buf_size = av_image_get_buffer_size(AV_PIX_FMT_NV12,
                                            dst_width,
                                            dst_height,
                                            1
                                            );
    printf("alloc size: %d,width: %d,height: %d\n", is->buf_size, dst_width, dst_height);

    // buffer将作为p_frm_yuv的视频数据缓冲区
    if (is->functions.sys_malloc) {
        ret = is->functions.sys_malloc("#yuv420p", &(is->vir_addr), &(is->phy_addr), is->buf_size);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "sys_malloc for nv12 failed\n");
            return -1;
        }
    } else {
        is->vir_addr = (uint8_t *)av_malloc(is->buf_size);
        if (is->vir_addr == NULL) {
            av_log(NULL, AV_LOG_ERROR, "av_malloc for yuv buffer failed\n");
            return -1;
        }
    }

    // 使用给定参数设定p_frm_yuv->data和p_frm_yuv->linesize
    is->p_frm_yuv->width  = dst_width;
    is->p_frm_yuv->height = dst_height;
    ret = av_image_fill_arrays(is->p_frm_yuv->data,     // dst data[]
                               is->p_frm_yuv->linesize, // dst linesize[]
                               is->vir_addr,            // src buffer
                               AV_PIX_FMT_NV12,         // pixel format
                               dst_width,
                               dst_height,
                               1                        // align
                               );
    if (ret < 0)
    {
        printf("av_image_fill_arrays failed %d\n", ret);
        return -1;
    }

    // A2. 初始化SWS context，用于后续图像转换
    //     此处第6个参数使用的是FFmpeg中的像素格式，对比参考注释B3
    //     FFmpeg中的像素格式AV_PIX_FMT_YUV420P对应SDL中的像素格式SDL_PIXELFORMAT_IYUV
    //     如果解码后得到图像的不被SDL支持，不进行图像转换的话，SDL是无法正常显示图像的
    //     如果解码后得到图像的能被SDL支持，则不必进行图像转换
    //     这里为了编码简便，统一转换为SDL支持的格式AV_PIX_FMT_YUV420P==>SDL_PIXELFORMAT_IYUV
    desc = av_pix_fmt_desc_get(is->p_vcodec_ctx->pix_fmt);

    is->img_convert_ctx = sws_getContext(is->p_vcodec_ctx->width,   // src width
                                         is->p_vcodec_ctx->height,  // src height
                                         is->p_vcodec_ctx->pix_fmt, // src format
                                         dst_width,
                                         dst_height,
                                         AV_PIX_FMT_NV12,           // dst format
                                         SWS_BICUBIC,               // flags
                                         NULL,                      // src filter
                                         NULL,                      // dst filter
                                         NULL                       // param
                                         );
    if (is->img_convert_ctx == NULL)
    {
        printf("sws_getContext() failed\n");
        return -1;
    }

    ret = pthread_create(&is->video_play_tid, NULL, video_playing_thread, is);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "video_playing_thread create failed!\n");
        is->video_play_tid = 0;
        return -1;
    }

    return 0;
}

static int open_video_stream(player_stat_t *is)
{
    AVCodecParameters* p_codec_par = NULL;
    AVCodec* p_codec = NULL;
    AVCodecContext* p_codec_ctx = NULL;
    AVStream *p_stream = is->p_video_stream;
    int ret;

    // 1. 为视频流构建解码器AVCodecContext
    // 1.1 获取解码器参数AVCodecParameters
    p_codec_par = p_stream->codecpar;

    // 1.2 获取解码器
    p_codec = avcodec_find_decoder(p_codec_par->codec_id); 
    if (p_codec == NULL)
    {
        printf("cann't find video codec!\n");
        return -1;
    }
    printf("open video codec: %s\n", p_codec->name);

    // 1.3 构建解码器AVCodecContext
    // 1.3.1 p_codec_ctx初始化：分配结构体，使用p_codec初始化相应成员为默认值
    p_codec_ctx = avcodec_alloc_context3(p_codec);
    if (p_codec_ctx == NULL)
    {
        printf("avcodec_alloc_context3() failed\n");
        return -1;
    }

    // 1.3.2 p_codec_ctx初始化：p_codec_par ==> p_codec_ctx，初始化相应成员
    ret = avcodec_parameters_to_context(p_codec_ctx, p_codec_par);
    if (ret < 0)
    {
        printf("avcodec_parameters_to_context() failed\n");
        return -1;
    }

    if (p_codec_par->width <= 0 || p_codec_par->height <= 0)
    {
        printf("video w/h = [%d %d] isn't invalid!\n", p_codec_par->width, p_codec_par->height);
        return -1;
    }
    printf("video w/h = [%d %d], input w/h = [%d %d]\n", p_codec_par->width, p_codec_par->height, is->in_width, is->in_height);

    is->out_width  = is->in_width;
    is->out_height = is->in_height;
    is->src_width  = FFMIN(is->out_width , p_codec_par->width);
    is->src_height = FFMIN(is->out_height, p_codec_par->height);
    printf("scaler src w/h = [%d %d], dst w/h = [%d %d]\n", is->src_width, is->src_height, is->out_width, is->out_height);

    p_codec_ctx->thread_count = 1;
    // 1.3.3 p_codec_ctx初始化：使用p_codec初始化p_codec_ctx，初始化完成
    ret = avcodec_open2(p_codec_ctx, p_codec, NULL);
    if (ret < 0)
    {
        printf("avcodec_open2() failed %d\n", ret);
        return -1;
    }

    is->p_vcodec_ctx = p_codec_ctx;
    is->p_vcodec_ctx->debug  = true;
    printf("codec width: %d, height: %d\n", is->p_vcodec_ctx->width, is->p_vcodec_ctx->height);

    // 2. 创建视频解码线程
    ret = pthread_create(&is->video_decode_tid, NULL, video_decode_thread, (void *)is);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "video_decode_thread create failed!\n");
        is->video_decode_tid = 0;
        return -1;
    }

    return 0;
}

int open_video(player_stat_t *is)
{
    int ret;

    if (is && is->video_idx >= 0) {
        ret = open_video_stream(is);
        if (ret < 0)
            return ret;

        ret = open_video_playing(is);
        if (ret < 0)
            return ret;
    } 

    return 0;
}

int video_buffer_flush(frame_queue_t *f)
{
    int i, ret = 0;

    pthread_mutex_lock(&f->mutex);
    for (i = 0; i < f->max_size; i ++) {
        frame_t *vp = &f->queue[i];
        frame_queue_unref_item(vp);
        av_frame_free(&vp->frame);
    }
    f->rindex = 0;
    f->rindex_shown = 0;
    f->windex = 0;
    f->size   = 0;
    f->max_size = 0;
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
    return ret;
}


