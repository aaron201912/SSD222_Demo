#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <gcc/stdatomic.h>
#include <libv4l/libv4l2.h>

#include "v4l2.h"

DEMO_DBG_LEVEL_e demo_debug_level = DEMO_DBG_ALL;
bool demo_func_trace = true;

static const int desired_video_buffers = 4;

#define V4L_ALLFORMATS  3
#define V4L_RAWFORMATS  1
#define V4L_COMPFORMATS 2

struct video_data {
    int fd;
    int pixelformat; /* V4L2_PIX_FMT_* */
    int width, height;
    int frame_size;
    int interlaced;

    int buffers;
    atomic_int buffers_queued;
    void **buf_start;
    unsigned int *buf_len;
    char *standard;
    v4l2_std_id std_id;
    int channel;
    int list_format;    /**< Set by a private option. */
    int list_standard;  /**< Set by a private option. */
    char *framerate;    /**< Set by a private option. */

    int use_libv4l2;
    int (*open_f)(const char *file, int oflag, ...);
    int (*close_f)(int fd);
    int (*dup_f)(int fd);
    int (*ioctl_f)(int fd, unsigned long int request, ...);
    ssize_t (*read_f)(int fd, void *buffer, size_t n);
    void *(*mmap_f)(void *start, size_t length, int prot, int flags, int fd, int64_t offset);
    int (*munmap_f)(void *_start, size_t length);
};

void v4l2_free(void *ptr)
{
    free(ptr);
}

void *v4l2_malloc(size_t size)
{
    void *ptr = NULL;

    if (!size)
        return NULL;

    ptr = malloc(size);
    if(!ptr)
        return NULL;
    memset(ptr, 0, size);

    return ptr;
}

static int new_packet(Packet *pkt, int size)
{
    assert(pkt);

    BufferRef *buf = NULL;

    buf = (BufferRef *)v4l2_malloc(sizeof(BufferRef));
    if(!buf)
        return -ENOMEM;

    buf->size = size;
    buf->data = (uint8_t *)v4l2_malloc(size * sizeof(int));
    if(!buf->data) {
        v4l2_free(buf);
        return -ENOMEM;
    }
    pkt->buf      = buf;
    pkt->data     = buf->data;
    pkt->size     = buf->size;

    return 0;
}

static void free_packet(Packet *pkt)
{
    BufferRef *buf = pkt->buf;

    assert(buf->data);
    assert(buf);

    v4l2_free(buf->data);
    v4l2_free(buf);

    pkt->data = NULL;
    pkt->buf  = NULL;
    pkt->size = 0;
}

static int64_t gettime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

static int device_open(DeviceContex_t *ctx, const char* device_path)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_capability cap;
    int fd;
    int err;
    int flags = O_RDWR | O_NONBLOCK;

#define SET_WRAPPERS(prefix) do {       \
    s->open_f   = prefix ## open;       \
    s->close_f  = prefix ## close;      \
    s->dup_f    = prefix ## dup;        \
    s->ioctl_f  = prefix ## ioctl;      \
    s->read_f   = prefix ## read;       \
    s->mmap_f   = prefix ## mmap;       \
    s->munmap_f = prefix ## munmap;     \
} while (0)

    SET_WRAPPERS(v4l2_);

#define v4l2_open   s->open_f
#define v4l2_close  s->close_f
#define v4l2_dup    s->dup_f
#define v4l2_ioctl  s->ioctl_f
#define v4l2_read   s->read_f
#define v4l2_mmap   s->mmap_f
#define v4l2_munmap s->munmap_f

    fd = v4l2_open(device_path, flags, 0);
    if (fd < 0) {
        DEMO_ERR("Cannot open video device %s: %s\n",
            device_path, strerror(errno));
        err = fd;
        goto fail;
    }

    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        err = errno;
        DEMO_ERR("ioctl(VIDIOC_QUERYCAP): %s\n",
               strerror(errno));
        goto fail;
    }

    DEMO_INFO("fd:%d capabilities:%x\n", fd, cap.capabilities);

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        DEMO_ERR("Not a video capture device.\n");
        err = -ENODEV;
        goto fail;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        DEMO_ERR("The device does not support the streaming I/O method.\n");
        err = -ENOSYS;
        goto fail;
    }

    return fd;

fail:
    v4l2_close(fd);
    return err;
}

static int device_init(DeviceContex_t *ctx, int *width, int *height,
                       int pixelformat)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };
    int res = 0;

    fmt.fmt.pix.width = *width;
    fmt.fmt.pix.height = *height;
    fmt.fmt.pix.pixelformat = pixelformat;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    /* Some drivers will fail and return EINVAL when the pixelformat
       is not supported (even if type field is valid and supported) */
    if (v4l2_ioctl(s->fd, VIDIOC_S_FMT, &fmt) < 0)
        res = errno;

    if ((*width != fmt.fmt.pix.width) || (*height != fmt.fmt.pix.height)) {
        DEMO_INFO("The V4L2 driver changed the video from %dx%d to %dx%d\n",
               *width, *height, fmt.fmt.pix.width, fmt.fmt.pix.height);
        *width = fmt.fmt.pix.width;
        *height = fmt.fmt.pix.height;
    }

    if (pixelformat != fmt.fmt.pix.pixelformat) {
        DEMO_DEBUG("The V4L2 driver changed the pixel format "
               "from 0x%08X to 0x%08X\n",
               pixelformat, fmt.fmt.pix.pixelformat);
        res = -EINVAL;
    }

    if (fmt.fmt.pix.field == V4L2_FIELD_INTERLACED) {
        DEMO_DEBUG("The V4L2 driver is using the interlaced mode\n");
        s->interlaced = 1;
    }

    return res;
}

static void list_formats(DeviceContex_t *ctx, int type)
{
    const struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_fmtdesc vfd = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };
    int ret;
    while(! (ret = v4l2_ioctl(s->fd, VIDIOC_ENUM_FMT, &vfd))) {

        vfd.index++;
        if (!(vfd.flags & V4L2_FMT_FLAG_COMPRESSED) &&
            type & V4L_RAWFORMATS) {
            DEMO_INFO("Raw       : %15s\n",vfd.description);
        } else
        if (vfd.flags & V4L2_FMT_FLAG_COMPRESSED &&
            type & V4L_COMPFORMATS) {
            DEMO_INFO("Compressed: %15s\n",vfd.description);
        } else {
            continue;
        }
    }
}

static void list_standards(DeviceContex_t *ctx)
{
    int ret;
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_standard standard;

    if (s->std_id == 0)
        return;

    for (standard.index = 0; ; standard.index++) {
        if (v4l2_ioctl(s->fd, VIDIOC_ENUMSTD, &standard) < 0) {
            ret = errno;
            if (ret == EINVAL) {
                break;
            } else {
                DEMO_ERR("ioctl(VIDIOC_ENUMSTD): %s\n", strerror(ret));
                return;
            }
        }
        DEMO_INFO("%2d, %16llu, %s\n",
               standard.index, (uint64_t)standard.id, standard.name);
    }
}

static int mmap_init(DeviceContex_t *ctx)
{
    int i, res;
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_requestbuffers req = {
    	.count  = desired_video_buffers,
        .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP
    };

    if (v4l2_ioctl(s->fd, VIDIOC_REQBUFS, &req) < 0) {
        res = errno;
        DEMO_ERR("ioctl(VIDIOC_REQBUFS): %s\n", strerror(res));
        return res;
    }

    if (req.count < 2) {
        DEMO_ERR("Insufficient buffer memory\n");
        return -ENOMEM;
    }
    s->buffers = req.count;
    s->buf_start = (void **)v4l2_malloc(s->buffers * sizeof(void *));
    if (!s->buf_start) {
        DEMO_ERR("Cannot allocate buffer pointers\n");
        return -ENOMEM;
    }
    s->buf_len = (unsigned int *)v4l2_malloc(s->buffers * sizeof(unsigned int));
    if (!s->buf_len) {
        DEMO_ERR("Cannot allocate buffer sizes\n");
        v4l2_free(&s->buf_start);
        return -ENOMEM;
    }
    DEMO_INFO("buf_start0x%p buf_len0x%p\n", s->buf_start, s->buf_len);

    for (i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {
        	.index  = (__u32)i,
            .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP
        };
        if (v4l2_ioctl(s->fd, VIDIOC_QUERYBUF, &buf) < 0) {
            res = errno;
            DEMO_ERR("ioctl(VIDIOC_QUERYBUF): %s\n", strerror(res));
            return res;
        }

        s->buf_len[i] = buf.length;
        if (s->frame_size > 0 && s->buf_len[i] < s->frame_size) {
            DEMO_ERR("buf_len[%d] = %d < expected frame size %d\n",
                   i, s->buf_len[i], s->frame_size);
            return -ENOMEM;
        }
        s->buf_start[i] = v4l2_mmap(NULL, buf.length,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               s->fd, buf.m.offset);

        if (s->buf_start[i] == MAP_FAILED) {
            res = errno;
            DEMO_ERR("mmap: %s\n", strerror(res));
            return res;
        }
        DEMO_INFO("mmap: index%d start0x%p\n", i, s->buf_start[i]);
    }

    return 0;
}

static int enqueue_buffer(struct video_data *s, struct v4l2_buffer *buf)
{
    int res = 0;

    if (v4l2_ioctl(s->fd, VIDIOC_QBUF, buf) < 0) {
        res = errno;
        DEMO_ERR("ioctl(VIDIOC_QBUF): %s\n", strerror(res));
    } else {
        atomic_fetch_add(&s->buffers_queued, 1);
    }

    return res;
}

static int mmap_read_frame(DeviceContex_t *ctx, Packet *pkt)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_buffer buf = {
        .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP
    };
    struct timeval buf_ts;
    int ret;

    pkt->size = 0;

    /* FIXME: Some special treatment might be needed in case of loss of signal... */
    while ((ret = v4l2_ioctl(s->fd, VIDIOC_DQBUF, &buf)) < 0 && (errno == EINTR));
    if (ret < 0) {

        if (errno == EAGAIN)
            return -EAGAIN;

        DEMO_ERR("ioctl(VIDIOC_DQBUF): %s\n", strerror(ret));
        return ret;
    }

    buf_ts = buf.timestamp;

    if (buf.index >= s->buffers) {
        DEMO_ERR("Invalid buffer index received.\n");
        return -EINVAL;
    }
    atomic_fetch_add(&s->buffers_queued, -1);
    // always keep at least one buffer queued
    assert(atomic_load(&s->buffers_queued) >= 1);

    if (buf.flags & V4L2_BUF_FLAG_ERROR) {
        DEMO_WRN("Dequeued v4l2 buffer contains corrupted data (%d bytes).\n",
               buf.bytesused);
        buf.bytesused = 0;
    } else {
        if (!buf.bytesused ||(s->frame_size > 0 && buf.bytesused > s->frame_size)) {
            DEMO_WRN("Dequeued v4l2 buffer contains %d bytes, but %d were expected. Flags: 0x%08X.\n",
                   buf.bytesused, s->frame_size, buf.flags);
            enqueue_buffer(s, &buf);
            return -EINVAL;
        }
    }

    /* Image is at s->buff_start[buf.index] */
    ret = new_packet(pkt, buf.bytesused);
    if (ret < 0) {
        DEMO_ERR("Error allocating a packet.\n");
        enqueue_buffer(s, &buf);
        return ret;
    }
    memcpy(pkt->data, s->buf_start[buf.index], buf.bytesused);

    ret = enqueue_buffer(s, &buf);
    if (ret) {
        free_packet(pkt);
        return ret;
    }
    pkt->pts = buf_ts.tv_sec * INT64_C(1000000) + buf_ts.tv_usec;

    return pkt->size;
}

static int mmap_start(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    enum v4l2_buf_type type;
    int i, res;

    for (i = 0; i < s->buffers; i++) {
        struct v4l2_buffer buf = {
        	.index  = (__u32)i,
            .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP
        };

        if (v4l2_ioctl(s->fd, VIDIOC_QBUF, &buf) < 0) {
            res = errno;
            DEMO_ERR("ioctl(VIDIOC_QBUF): %s\n",
                   strerror(res));
            return res;
        }
    }
    atomic_store(&s->buffers_queued, s->buffers);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(s->fd, VIDIOC_STREAMON, &type) < 0) {
        res = errno;
        DEMO_ERR("ioctl(VIDIOC_STREAMON): %s\n",
               strerror(res));
        return res;
    }

    return 0;
}

static void mmap_close(struct video_data *s)
{
    enum v4l2_buf_type type;
    int i;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /* We do not check for the result, because we could
     * not do anything about it anyway...
     */
    v4l2_ioctl(s->fd, VIDIOC_STREAMOFF, &type);
    for (i = 0; i < s->buffers; i++) {
        v4l2_munmap(s->buf_start[i], s->buf_len[i]);
    }
    v4l2_free(s->buf_start);
    v4l2_free(s->buf_len);
}

typedef struct VideoRational {
    int num; ///< Numerator
    int den; ///< Denominator
} VideoRational;

static int parse_video_rate(VideoRational *r,  char *f)
{
    int framerate = atoi(f);
    r->num = 1;
    r->den = framerate;
    return 0;
}

static int v4l2_set_parameters(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_standard standard = { 0 };
    struct v4l2_streamparm streamparm = { 0 };
    struct v4l2_fract *tpf;
    VideoRational framerate_q = { 0 };
    int i, ret;

    if (s->framerate &&
        (ret = parse_video_rate(&framerate_q, s->framerate)) < 0) {
        DEMO_ERR("Could not parse framerate '%s'.\n", s->framerate);
        return ret;
    }

    if (s->standard) {
        if (s->std_id) {
            ret = 0;
            DEMO_DEBUG("Setting standard: %s\n", s->standard);
            /* set tv standard */
            for (i = 0; ; i++) {
                standard.index = i;
                if (v4l2_ioctl(s->fd, VIDIOC_ENUMSTD, &standard) < 0) {
                    ret = -1 * errno;
                    break;
                }
                if (!strcmp((char*)standard.name, s->standard))
                    break;
            }
            if (ret < 0) {
                DEMO_ERR("Unknown or unsupported standard '%s'\n", s->standard);
                return ret;
            }

            if (v4l2_ioctl(s->fd, VIDIOC_S_STD, &standard.id) < 0) {
                ret = errno;
                DEMO_ERR("ioctl(VIDIOC_S_STD): %s\n", strerror(errno));
                return ret;
            }
        } else {
            DEMO_WRN("This device does not support any standard\n");
        }
    }

    /* get standard */
    if (v4l2_ioctl(s->fd, VIDIOC_G_STD, &s->std_id) == 0) {
        tpf = &standard.frameperiod;
        for (i = 0; ; i++) {
            standard.index = i;
            if (v4l2_ioctl(s->fd, VIDIOC_ENUMSTD, &standard) < 0) {
                ret = errno;
                if (ret == EINVAL) {
                    tpf = &streamparm.parm.capture.timeperframe;
                    break;
                }
                DEMO_ERR("ioctl(VIDIOC_ENUMSTD): %s\n", strerror(ret));
                return ret;
            }
            if (standard.id == s->std_id) {
                DEMO_DEBUG("Current standard: %s, id: %llu, frameperiod: %d/%d\n",
                       standard.name, (uint64_t)standard.id, tpf->numerator, tpf->denominator);
                break;
            }
        }
    } else {
        tpf = &streamparm.parm.capture.timeperframe;
    }

    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(s->fd, VIDIOC_G_PARM, &streamparm) < 0) {
        ret = errno;
        DEMO_WRN("ioctl(VIDIOC_G_PARM): %s\n", strerror(ret));
    } else if (framerate_q.num && framerate_q.den) {
        if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
            tpf = &streamparm.parm.capture.timeperframe;

            DEMO_DEBUG("Setting time per frame to %d/%d\n",
                   framerate_q.den, framerate_q.num);
            tpf->numerator   = framerate_q.den;
            tpf->denominator = framerate_q.num;

            if (v4l2_ioctl(s->fd, VIDIOC_S_PARM, &streamparm) < 0) {
                ret = errno;
                DEMO_ERR("ioctl(VIDIOC_S_PARM): %s\n",
                       strerror(ret));
                return ret;
            }

            if (framerate_q.num != tpf->denominator ||
                framerate_q.den != tpf->numerator) {
                DEMO_INFO("The driver changed the time per frame from "
                       "%d/%d to %d/%d\n",
                       framerate_q.den, framerate_q.num,
                       tpf->numerator, tpf->denominator);
            }
        } else {
            DEMO_WRN("The driver does not permit changing the time per frame\n");
        }
    }

    return 0;
}

static bool format_is_support(int *fmt, int *width, int *height)
{
    DEMO_DEBUG("fmt 0x%x width%d height%d\n", *fmt, *width, *height);
    return true;
}

static int device_try_init(DeviceContex_t *ctx,
                           int *width,
                           int *height,
                           int *desired_format)
{
    int ret = -EINVAL;

    if (format_is_support(desired_format, width, height)) {
        ret = device_init(ctx, width, height, *desired_format);
        if (ret < 0) {
            *desired_format = 0;
            return -EINVAL;
        }
    }
    format_is_support(desired_format, width, height);
    return ret;
}

static int image_check_size(unsigned int w, unsigned int h, void *log_ctx)
{
    return 0; //todo
}

static unsigned int image_get_buffer_size(int fmt, int width, int height, int align)
{
    unsigned int buffer_size = 0;

    switch(fmt) {
    case V4L2_PIX_FMT_YUYV:
        buffer_size= width * height * 2.0; 
        break;
    case V4L2_PIX_FMT_NV12:
        buffer_size= width * height * 1.5; 
        break;
    case V4L2_PIX_FMT_MJPEG:
        buffer_size= width * height * 2.0 / 6; 
        break;
    case V4L2_PIX_FMT_H264:
        buffer_size= width * height * 2.0 / 7; 
        break;
    case V4L2_PIX_FMT_H265:
        buffer_size= width * height * 2.0 / 8; 
        break;
    } 
    return buffer_size;
}

static char* format_fcc_to_str(uint fcc)
{
    switch(fcc)
    {
    case V4L2_PIX_FMT_YUYV:    
        return "YUYV";         
    case V4L2_PIX_FMT_NV12:    
        return "NV12";         
    case V4L2_PIX_FMT_MJPEG:   
        return "MJPEG";        
    case V4L2_PIX_FMT_H264:    
        return "H264";
    case V4L2_PIX_FMT_H265:
        return "H265";
    default:
        return "unkonown";
    }
}

int v4l2_read_header(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    int res = 0;
    struct v4l2_input input = { 0 };
 
    /* silence libv4l2 logging. if fopen() fails v4l2_log_file will be NULL
       and errors will get sent to stderr */
    if (s->use_libv4l2)
        v4l2_log_file = fopen("/dev/null", "w");

    s->fd = device_open(ctx, ctx->url);
    if (s->fd < 0) {
        DEMO_ERR("device_open error\n");
        return s->fd;
    }
    DEMO_INFO("v4l2_read_header done\n");

    if (s->channel != -1) {
        /* set video input */
        DEMO_INFO("Selecting input_channel: %d\n", s->channel);
        if (v4l2_ioctl(s->fd, VIDIOC_S_INPUT, &s->channel) < 0) {
            DEMO_ERR("ioctl(VIDIOC_S_INPUT): %s\n", strerror(errno));
            goto fail;
        }
    } else {
        /* get current video input */
        if (v4l2_ioctl(s->fd, VIDIOC_G_INPUT, &s->channel) < 0) {
            DEMO_ERR("ioctl(VIDIOC_G_INPUT): %s\n", strerror(errno));
            goto fail;
        }
    }

    /* enum input */
    input.index = s->channel;
    if (v4l2_ioctl(s->fd, VIDIOC_ENUMINPUT, &input) < 0) {
        DEMO_ERR("ioctl(VIDIOC_ENUMINPUT): %s\n", strerror(errno));
        goto fail;
    }
    s->std_id = input.std;
    DEMO_DEBUG("Current input_channel: %d, input_name: %s, input_std: 0x%llx\n",
           s->channel, input.name, (uint64_t)input.std);

    if (s->list_format) {
        list_formats(ctx, s->list_format);
    }

    if (s->list_standard) {
        list_standards(ctx);
    }

    if (!s->width && !s->height) {
        struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };

        DEMO_INFO("Querying the device for the current frame size\n");
        if (v4l2_ioctl(s->fd, VIDIOC_G_FMT, &fmt) < 0) {
            DEMO_ERR("ioctl(VIDIOC_G_FMT): %s\n",
                   strerror(errno));
            goto fail;
        }

        s->width  = fmt.fmt.pix.width;
        s->height = fmt.fmt.pix.height;
    }
    s->pixelformat = s->pixelformat ? s->pixelformat : V4L2_PIX_FMT_MJPEG;

    res = device_try_init(ctx, &s->width, &s->height, &s->pixelformat);
    if (res < 0)
        goto fail;

    DEMO_INFO("Setting format %s frame size to %dx%d\n", 
        format_fcc_to_str(s->pixelformat),s->width, s->height);

    if ((res = image_check_size(s->width, s->height, ctx)) < 0)
        goto fail;

    if ((res = v4l2_set_parameters(ctx)) < 0)
        goto fail;

    s->frame_size = image_get_buffer_size(s->pixelformat,
                                         s->width, s->height, 1);

    if ((res = mmap_init(ctx)) ||
        (res = mmap_start(ctx)) < 0)
            goto fail;

    return 0;

fail:
    v4l2_close(s->fd);
    return res;
}

int v4l2_read_packet_end(DeviceContex_t *ctx, Packet *pkt)
{
    free_packet(pkt);
    return 0;
}

int v4l2_read_packet(DeviceContex_t *ctx, Packet *pkt)
{
    int ret;

    if ((ret = mmap_read_frame(ctx, pkt)) < 0) {
        return ret;
    }

    return pkt->size;
}

int v4l2_read_close(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;

    if (atomic_load(&s->buffers_queued) != s->buffers)
        DEMO_WRN("Some buffers are still owned by the caller on close.\n");

    mmap_close(s);

    if (s->fd >= 0)
    v4l2_close(s->fd);
    return 0;
}

int v4l2_dev_init(DeviceContex_t **ctx, char *path)
{
    assert(ctx);
    assert(path);

    DeviceContex_t *c = 
       (DeviceContex_t *)v4l2_malloc(sizeof(DeviceContex_t));
    struct video_data *s = 
       (struct video_data *)v4l2_malloc(sizeof(struct video_data));

    assert(c);
    assert(s);

    s->use_libv4l2 = true; //for libv4l2 debug
    s->list_format = V4L_ALLFORMATS;
    s->list_standard = true;

    c->priv_data = s;
    c->inited = true;
    strcpy(c->url, path);
    *ctx = c;
    return 0;
}

void v4l2_dev_set_fmt(DeviceContex_t *ctx, int v4l2_fmt, int width, int height)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;

    assert(ctx);
    assert(s);

    s->pixelformat = v4l2_fmt;
    s->width = width;
    s->height = height;
}
void v4l2_dev_deinit(DeviceContex_t *ctx)
{
    assert(ctx);
    assert(ctx->inited==true);

    v4l2_free(ctx->priv_data);
    v4l2_free(ctx);
}

void save_file(void *buf, int length,char type)
{
    if(NULL == buf || 0 >= length)
       return;

    FILE *DEMOFile = NULL;
    char FileName[120] = {0};

    memset(FileName, 0x0, sizeof(FileName));
    
    switch(type){
    case 0:
        snprintf(FileName, sizeof(FileName) - 1, "myusb.img");
        DEMOFile = fopen(FileName, "w+");
       break;
    case 1:
        snprintf(FileName, sizeof(FileName) - 1, "720p.h264");
        DEMOFile = fopen(FileName, "a+");
       break;
    case 2:
        //snprintf(FileName, sizeof(FileName) - 1, "yuyv640x480.yuv");
        DEMOFile = fopen("mjpeg1280x720_normal.jpg", "w+");
        break;
    case 3:
        //snprintf(FileName, sizeof(FileName) - 1, "yuyv640x480.yuv");
        //DEMOFile = fopen("libjpeg1280x720.yuv", "w+");
        DEMOFile = fopen("mjpeg1280x720_error.jpg", "w+");
        break;
    case 4:
        //snprintf(FileName, sizeof(FileName) - 1, "yuyv640x480.yuv");
        DEMOFile = fopen("libyuv1280x720.yuy2", "w+");
        break;
    default:
       break;
    }    
    fwrite(buf,length,1, DEMOFile);
    fclose(DEMOFile);
}
