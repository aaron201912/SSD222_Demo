#include "demux.h"
#include "packet.h"
#include "frame.h"
#include "player.h"
#include <sys/time.h>

#define HTTP_PATCH      1

extern AVPacket a_flush_pkt, v_flush_pkt;

AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
                                           AVDictionary *codec_opts)
{
    AVDictionary **opts;

    if (!s || !s->nb_streams)
        return NULL;

    opts = av_mallocz_array(s->nb_streams, sizeof(*opts));
    if (!opts) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not alloc memory for stream options.\n");
        return NULL;
    }

    return opts;
}

static int decode_interrupt_cb(void *ctx)
{
    player_stat_t *is = (player_stat_t *)ctx;
    return is->abort_request;
}

static int demux_init(player_stat_t *is)
{
    AVFormatContext *p_fmt_ctx = NULL;
    int err, i, ret;
    int a_idx;
    int v_idx;
    double totle_seconds, start_seconds;

    int st_index[AVMEDIA_TYPE_NB];

    avformat_network_init();

    memset(st_index, -1, sizeof(st_index));

    p_fmt_ctx = avformat_alloc_context();
    if (!p_fmt_ctx)
    {
        printf("Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    is->p_fmt_ctx = p_fmt_ctx;

    // 中断回调机制。为底层I/O层提供一个处理接口，比如中止IO操作。
    p_fmt_ctx->interrupt_callback.callback = decode_interrupt_cb;
    p_fmt_ctx->interrupt_callback.opaque = is;

#if HTTP_PATCH
    if (strstr(is->filename,"qiyi.com") != NULL){
        printf("tp1 qiyi.com user_agent \n");
        av_dict_set(&is->p_dict, "user_agent", "AppleCoreMedia/1.0.0.9A405 (iPad; U; CPU OS 5_0_1 like Mac OS X; zh_cn)", 0);
    } else {
        av_dict_set(&is->p_dict, "user_agent", "stagefright/1.2 (Linux;Android 9)", 0);
    }
#endif

    // 1. 构建AVFormatContext
    // 1.1 打开视频文件：读取文件头，将文件格式信息存储在"fmt context"中
    av_dict_set(&is->p_dict, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
    av_dict_set(&is->p_dict, "max_resolution", "921600", 0);//设置最大分辨率1280x720
    av_dict_set(&is->p_dict, "buffer_size", "1024000", 0);
    av_dict_set(&is->p_dict, "stimeout", "10000000", 0);  //设置超时断开连接时间
    av_dict_set(&is->p_dict, "rtsp_transport", "tcp", 0);
    //av_dict_set(&is->p_dict, "max_delay", "10000000", 0);//设置超时10秒
    //av_dict_set(&is->p_dict, "probesize", "102400", 0);  //探测长度设置为100K
    //av_dict_set(&is->p_dict, "fflags", "nobuffer", 0);
    //p_fmt_ctx->max_analyze_duration = 3 * AV_TIME_BASE;
    err = avformat_open_input(&p_fmt_ctx, is->filename, is->p_iformat, &is->p_dict);
    if (err < 0)
    {
        printf("avformat_open_input() failed %d\n", err);
        ret = err;
        goto fail;
    }

    av_format_inject_global_side_data(p_fmt_ctx);

    AVDictionary **opts = setup_find_stream_info_opts(p_fmt_ctx, NULL);

    for (i = 0; i < p_fmt_ctx->nb_streams; i ++) {
        if (p_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_dict_set(&opts[i], "max_resolution", "921600", 0);
        }
    }

    // 1.2 搜索流信息：读取一段视频文件数据，尝试解码，将取到的流信息填入p_fmt_ctx->streams
    //     ic->streams是一个指针数组，数组大小是pFormatCtx->nb_streams
    err = avformat_find_stream_info(p_fmt_ctx, opts);

    if (opts) {
        for (i = 0; i < p_fmt_ctx->nb_streams; i ++)
            av_dict_free(&opts[i]);
        av_freep(&opts);
    }

    if (err < 0)
    {
        printf("avformat_find_stream_info() failed %d\n", err);
        ret = err;
        goto fail;
    }
    av_log(NULL, AV_LOG_INFO, "avformat demuxer name : %s\n", p_fmt_ctx->iformat->name);

    is->seek_by_bytes = !!(p_fmt_ctx->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", p_fmt_ctx->iformat->name);

    st_index[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(p_fmt_ctx,
                                   AVMEDIA_TYPE_VIDEO,
                                   st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);

    st_index[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(p_fmt_ctx,
                                   AVMEDIA_TYPE_AUDIO,
                                   st_index[AVMEDIA_TYPE_AUDIO], st_index[AVMEDIA_TYPE_VIDEO], NULL, 0);

    // 2. 查找第一个音频流/视频流
    a_idx = st_index[AVMEDIA_TYPE_AUDIO];
    v_idx = st_index[AVMEDIA_TYPE_VIDEO];
    if (a_idx == -1 && v_idx == -1)
    {
        printf("Cann't find any audio/video stream\n");
        ret = -1;
        goto fail;
    }
    printf("audio idx: %d, video idx: %d\n", a_idx, v_idx);

    start_seconds = (p_fmt_ctx->start_time != AV_NOPTS_VALUE) ? p_fmt_ctx->start_time * av_q2d(AV_TIME_BASE_Q) : 0.0;
    totle_seconds = (p_fmt_ctx->duration != AV_NOPTS_VALUE) ? p_fmt_ctx->duration * av_q2d(AV_TIME_BASE_Q) : 0.0;
    printf("start time : %0.3f, total time of input file : %0.3f\n", start_seconds, totle_seconds);
    av_dump_format(p_fmt_ctx, 0, p_fmt_ctx->url, 0);

    if (is->options.audio_only) {
        v_idx = -1;
        is->av_sync_type = AV_SYNC_AUDIO_MASTER;
    }

    if (is->options.video_only) {
        a_idx = -1;
        is->av_sync_type = AV_SYNC_VIDEO_MASTER;
    }

    if (a_idx >= 0) {
        is->p_audio_stream = p_fmt_ctx->streams[a_idx];
        is->audio_complete = 0;
    }

    if (v_idx >= 0) {
        is->p_video_stream = p_fmt_ctx->streams[v_idx];

        if ((is->p_video_stream->codecpar->width * is->p_video_stream->codecpar->height < 64) ||
            (is->p_video_stream->codecpar->width * is->p_video_stream->codecpar->height > 1280 * 720)) {
            av_log(NULL, AV_LOG_ERROR, "video size is not match min/max resolution!\n");
            ret = -2;
            goto fail;
        }
        is->video_complete = 0;
    }

    is->audio_idx = a_idx;
    is->video_idx = v_idx;

    return 0;
fail:
    if (is->p_dict) {
        av_dict_free(&is->p_dict);
    }
    if (p_fmt_ctx != NULL) {
        avformat_close_input(&p_fmt_ctx);
        is->p_fmt_ctx = NULL;
    }
    avformat_network_deinit();
    return ret;
}

int demux_deinit(player_stat_t *is)
{
    if (is->p_fmt_ctx != NULL) {
        avformat_close_input(&is->p_fmt_ctx);
        is->p_fmt_ctx = NULL;
    }

    return 0;
}

#if 0
static int stream_has_enough_packets(AVStream *st, int stream_id, packet_queue_t *queue, player_stat_t *is)
{
    //printf("id: %d,disposition: %d,nb_packets: %d,duration: %d\n",stream_id,st->disposition,queue->nb_packets,queue->duration);
    if (stream_id == is->audio_idx)
    {
        return (stream_id < 0) || queue->abort_request ||
               (st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
               (queue->nb_packets > MIN_AUDIO_FRAMES && (!queue->duration || av_q2d(st->time_base) * queue->duration > 1.0));
    }
    if (stream_id == is->video_idx)
    {
        return (stream_id < 0) || queue->abort_request ||
               (st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
               (queue->nb_packets > MIN_VIDEO_FRAMES && (!queue->duration || av_q2d(st->time_base) * queue->duration > 1.0));
    }
    return 1;
}
#endif

static void step_to_next_frame(player_stat_t *is)
{
    /* if the stream is paused unpause it, then step */
    if (is->paused)
        stream_toggle_pause(is);                                                                                                      
    is->step = 1;
}


/* this thread gets the stream from the disk or the network */
static void * demux_thread(void *arg)
{
    int ret;
    player_stat_t *is = (player_stat_t *)arg;
    AVPacket pkt1, *pkt = &pkt1;

    struct timeval now;
    struct timespec outtime;

    pthread_mutex_t wait_mutex;
    ret = pthread_mutex_init(&wait_mutex, NULL);
    if (ret != 0) {
        av_log(is, AV_LOG_ERROR, "pthread_mutex_init error!\n");
        return NULL;
    }

    is->eof = 0;

    // 4. 解复用处理
    while (1)
    {
        if (is->abort_request)
        {
            printf("demux thread exit\n");
            break;
        }

        //printf("loop start paused: %d,last_paused: %d\n",is->paused,is->last_paused);
        if (is->paused != is->last_paused) {
            is->last_paused = is->paused;
            if (is->paused)
            {
                is->read_pause_return = av_read_pause(is->p_fmt_ctx);
            }
            else
            {
                av_read_play(is->p_fmt_ctx);
            }
        }

        if (is->seek_req) {
            int64_t seek_target = is->seek_pos;
            int64_t seek_min    = is->seek_rel > 0 ? seek_target - is->seek_rel + 2: INT64_MIN;
            int64_t seek_max    = is->seek_rel < 0 ? seek_target - is->seek_rel - 2: INT64_MAX;

            // FIXME the +-2 is due to rounding being not done in the correct direction in generation
            // of the seek_pos/seek_rel variables
            is->seek_flags |= AVSEEK_FLAG_BACKWARD;
            //ret = av_seek_frame(is->p_fmt_ctx, is->video_idx, seek_target, is->seek_flags);
            ret = avformat_seek_file(is->p_fmt_ctx, -1, seek_min, seek_target, seek_max, is->seek_flags);

            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR,
                       "%s: error while seeking\n", is->p_fmt_ctx->url);
            } else {
                if (is->audio_idx >= 0) {
                    packet_queue_flush(&is->audio_pkt_queue);
                    packet_queue_put(&is->audio_pkt_queue, &a_flush_pkt);
                }
 
                if (is->video_idx >= 0) {
                    packet_queue_flush(&is->video_pkt_queue);
                    packet_queue_put(&is->video_pkt_queue, &v_flush_pkt);
                }
                /*if (is->seek_flags & AVSEEK_FLAG_BYTE) {
                   set_clock(&is->extclk, NAN, 0);
                } else {
                   set_clock(&is->extclk, seek_target / (double)AV_TIME_BASE, 0);
                }*/
            }
            is->seek_req = 0;
            is->eof = 0;
            is->audio_complete = (is->audio_idx >= 0) ? 0 : 1;
            is->video_complete = (is->video_idx >= 0) ? 0 : 1;
            if (is->paused)
                step_to_next_frame(is);
        }

        if(is->audio_pkt_queue.size + is->video_pkt_queue.size >= MAX_QUEUE_SIZE)
        {
            /* wait 10 ms */
            pthread_mutex_lock(&wait_mutex);
            gettimeofday(&now, NULL);
            outtime.tv_sec = now.tv_sec;
            outtime.tv_nsec = now.tv_usec * 1000 + 10 * 1000 * 1000;//timeout 10ms
            pthread_cond_timedwait(&is->continue_read_thread,&wait_mutex,&outtime);
            pthread_mutex_unlock(&wait_mutex);

            continue;
        }

        // 4.1 从输入文件中读取一个packet
        ret = av_read_frame(is->p_fmt_ctx, pkt);
        if (ret < 0)
        {
            if (((ret == AVERROR_EOF) || avio_feof(is->p_fmt_ctx->pb)) && !is->eof)
            {
                // 输入文件已读完，则往packet队列中发送NULL packet，以冲洗(flush)解码器，否则解码器中缓存的帧取不出来
                if (is->video_idx >= 0)
                {
                    packet_queue_put_nullpacket(&is->video_pkt_queue, is->video_idx);
                }

                if (is->audio_idx >= 0)
                {
                    packet_queue_put_nullpacket(&is->audio_pkt_queue, is->audio_idx);
                }

                is->eof = 1;
                av_log(NULL, AV_LOG_INFO, "ret : %d, feof : %d\n", ret, avio_feof(is->p_fmt_ctx->pb));
            }

            pthread_mutex_lock(&wait_mutex);
            gettimeofday(&now, NULL);
            outtime.tv_sec = now.tv_sec;
            outtime.tv_nsec = now.tv_usec * 1000 + 10 * 1000 * 1000;//timeout 10ms
            pthread_cond_timedwait(&is->continue_read_thread,&wait_mutex,&outtime);
            pthread_mutex_unlock(&wait_mutex);

            continue;
        }
        else
        {
            is->eof = 0;
        }

        // 4.3 根据当前packet类型(音频、视频、字幕)，将其存入对应的packet队列
        if (pkt->stream_index == is->audio_idx && !is->options.video_only)
        {
            packet_queue_put(&is->audio_pkt_queue, pkt);
            //printf("\033[32;2mpkt num : %d. put audio pkt end\033[0m\n", is->audio_pkt_queue.nb_packets);
        }
        else if (pkt->stream_index == is->video_idx && !is->options.audio_only)
        {
            packet_queue_put(&is->video_pkt_queue, pkt);
            if (is->video_pkt_queue.size > 2 * 1024 * 1024) {
                printf("\033[32;2mpkt num : %d. put video pkt end\033[0m\n", is->video_pkt_queue.nb_packets);
            }
        }
        else
        {
            av_packet_unref(pkt);
        }
    }

    pthread_mutex_destroy(&wait_mutex);

    return NULL;
}

int open_demux(player_stat_t *is)
{
    int ret;

    is->demux_status = false;

    if ((ret = demux_init(is)) != 0)
    {
        printf("demux_init() failed\n");
        return ret;
    }

    ret = pthread_create(&is->read_tid, NULL, demux_thread, is);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "demux_thread create failed! ret = %d\n", ret);
        is->read_tid = 0;
        demux_deinit(is);
        return -1;
    }

    is->demux_status = true;

    return 0;
}

