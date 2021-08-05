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
#include "list.h"
#include "v4l2.h"

DEMO_DBG_LEVEL_e demo_debug_level = DEMO_DBG_ALL;
bool demo_func_trace = true;

static const int desired_video_buffers = 4;

#define V4L_ALLFORMATS  3
#define V4L_RAWFORMATS  1
#define V4L_COMPFORMATS 2

#define CLEAR_LIST(list)    \
{   \
    FmtListData_t *pos = NULL;  \
	FmtListData_t *posN = NULL; \
    \
	list_for_each_entry_safe(pos, posN, &list, fmtList) \
	{   \
		list_del(&pos->fmtList);    \
		free(pos);  \
	}   \
}

#define AUTO_MATCH(s, list, pixelFmt)  \
/*if (s->pixelformat == pixelFmt) */ \
{   \
    if (!list_empty(&list)) \
    {   \
        FmtListData_t *pos = NULL;  \
        int i = 0, matchIndex = 0;  \
        int matchW = 0, matchH = 0, matchFps = 0; \
        int minDeltRes = 4096 * 2160;   \
        char strFmt[8] = {0};   \
        \
        list_for_each_entry(pos, &list, fmtList)    \
        {   \
            int deltRes = abs((pos->supportFmt.width * pos->supportFmt.height) - (s->width * s->height));   \
        \
            if (deltRes < minDeltRes)   \
            {   \
                matchIndex = i; \
                matchW = pos->supportFmt.width; \
                matchH = pos->supportFmt.height;    \
                matchFps = pos->supportFmt.fps; \
                minDeltRes = deltRes;   \
            }   \
        \
            i++;    \
        }   \
        \
        printf("set current format: %s, %d * %d, fps %d\n", format_fcc_to_str2(pixelFmt, strFmt, sizeof(strFmt)), matchW, matchH, matchFps); \
        \
        s->pixelformat = pixelFmt;  \
        s->width = matchW;  \
        s->height = matchH; \
        \
        return 0;   \
    }   \
}

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

typedef struct {
	int fmt;
	int width;
	int height;
	int fps;
} v4l2_fmt_support;

typedef struct {
	list_t fmtList;
	v4l2_fmt_support supportFmt;
}FmtListData_t;

static list_t g_supportFmtList;
static int g_formatCnt = 0;

static list_t g_mjpegFmtList;
static int g_mjpegFmtCnt = 0;
static list_t g_yuyvFmtList;
static int g_yuyvFmtCnt = 0;
static list_t g_nv12FmtList;
static int g_nv12FmtCnt = 0;

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

        DEMO_WRN("ioctl(VIDIOC_DQBUF): %s\n", strerror(ret));
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

static char* format_fcc_to_str2(uint fcc, char *str, int size)
{
    if (size < 5)
        return NULL;

    sprintf(str, "%c%c%c%c", fcc & 0xFF, (fcc >> 8) & 0xFF, (fcc >> 16) & 0xFF, (fcc >> 24) & 0xFF);
    return str;
}

static bool format_is_support(int *fmt, int *width, int *height)
{
    char strFmt[8] = {0};
    memset(strFmt, 0, sizeof(strFmt));

    DEMO_DEBUG("fmt %s width%d height%d\n", format_fcc_to_str2(*fmt, strFmt, sizeof(strFmt)), *width, *height);
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

static int query_cap(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_capability cap;

    memset(&cap, 0, sizeof(cap));

    if (ioctl(s->fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        printf("ERR(%s):VIDIOC_QUERYCAP failed\n", __func__);
        return -1;
    }

	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		printf("dev support capture\n");

	if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
		printf("dev support output\n");

	if(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
		printf("dev support overlay\n");

	if(cap.capabilities & V4L2_CAP_STREAMING)
		printf("dev support streaming\n");

	if(cap.capabilities & V4L2_CAP_READWRITE)
		printf("dev support read write\n");

    return 0;
}

static int enum_input(DeviceContex_t *ctx)
{
    struct v4l2_input input;
    char name[128];
    struct video_data *s = (struct video_data *)ctx->priv_data;
    int found = 0;
    int index = 0;

    memset(name, 0, sizeof(name));
    input.index = 0;

    while(!ioctl(s->fd, VIDIOC_ENUMINPUT, &input))
    {
        printf("input:%s\n", input.name);

        if(input.index == index)
        {
            found = 1;
            strcpy(name, (const char*)input.name);
        }

        ++input.index;
    }

    if(!found)
    {
        printf("%s:can't find input dev\n", __func__);
        return -1;
    }

    printf("input device name:%s\n", name);

    return 0;
}

static int set_input(DeviceContex_t *ctx, int index)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    struct v4l2_input input;

    input.index = index;

    if (ioctl(s->fd, VIDIOC_S_INPUT, &input) < 0)
    {
        printf("ERR(%s):VIDIOC_S_INPUT failed\n", __func__);
        return -1;
    }

    return 0;
}

static int v4l2_enum_frameInterval(DeviceContex_t *ctx, int pixfmt, int width, int height)
{
	struct v4l2_frmivalenum fival;
	struct video_data *s = (struct video_data *)ctx->priv_data;

	// 设置参数
	memset(&fival, 0, sizeof(fival));
	fival.index = 0;
	fival.pixel_format = pixfmt;
	fival.width = width;
	fival.height = height;

	printf("\tTime interval between frame: \n");
	// 遍历的调用ioctl获取所有支持的fps
	while (ioctl(s->fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival) >= 0)
	{
		fival.index++;
		// 同样只认DISCRETE
		if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
		{
			printf("\t\t%u/%u\n", fival.discrete.numerator, fival.discrete.denominator);
            FmtListData_t *pFmtListData = (FmtListData_t *)malloc(sizeof(FmtListData_t));
			memset(pFmtListData, 0, sizeof(FmtListData_t));
			INIT_LIST_HEAD(&pFmtListData->fmtList);
			pFmtListData->supportFmt.fmt = pixfmt;
			pFmtListData->supportFmt.width = width;
			pFmtListData->supportFmt.height = height;
			pFmtListData->supportFmt.fps = fival.discrete.denominator / fival.discrete.numerator;
			// list_add_tail(&pFmtListData->fmtList, &g_supportFmtList);
			// g_formatCnt++;

            if (pixfmt == V4L2_PIX_FMT_MJPEG)
            {
                list_add_tail(&pFmtListData->fmtList, &g_mjpegFmtList);
                g_mjpegFmtCnt++;
            }
            else if (pixfmt == V4L2_PIX_FMT_YUYV)
            {
                list_add_tail(&pFmtListData->fmtList, &g_yuyvFmtList);
                g_yuyvFmtCnt++;
            }
            else if (pixfmt == V4L2_PIX_FMT_NV12)
            {
                list_add_tail(&pFmtListData->fmtList, &g_nv12FmtList);
                g_nv12FmtCnt++;
            }
		}
		else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS)
		{
			printf("\t\t{min { %u/%u } .. max { %u/%u } }\n",
				fival.stepwise.min.numerator, fival.stepwise.min.numerator,
				fival.stepwise.max.denominator, fival.stepwise.max.denominator);
			break;
		}
		else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE)
		{
			printf("\t\t{min { %u/%u } .. max { %u/%u }, stepsize { %u/%u } }\n",
				fival.stepwise.min.numerator, fival.stepwise.min.denominator,
				fival.stepwise.max.numerator, fival.stepwise.max.denominator,
				fival.stepwise.step.numerator, fival.stepwise.step.denominator);
			break;
		}
	}

	return 0;
}

static int v4l2_enum_frameSize(DeviceContex_t *ctx, int pixfmt)
{
	int ret=0;
	int fsizeind = 0;
	struct v4l2_frmsizeenum fsize;
    struct video_data *s = (struct video_data *)ctx->priv_data;

	// 设置好v4l2_frmsizeenum
	memset(&fsize, 0, sizeof(fsize));
	fsize.index = 0;
	fsize.pixel_format = pixfmt;
	// 循环调用VIDIOC_ENUM_FRAMESIZES ioctl查询所有支持的分辨率
	while (ioctl(s->fd, VIDIOC_ENUM_FRAMESIZES, &fsize) >= 0)
	{
		fsize.index++;
		// 根据输出结果的type分情况讨论
		if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)       // 离散帧size
		{
			printf("{ \tdiscrete: width = %u, height = %u }\n", fsize.discrete.width, fsize.discrete.height);
			fsizeind++;

			// 过滤超过1080p的设置
			if (fsize.discrete.width > 1920 || fsize.discrete.height > 1080)
				continue;

			// enum supported fps under this size
			v4l2_enum_frameInterval(ctx, pixfmt, fsize.discrete.width, fsize.discrete.height);
		}
		else if (fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)    // 连续帧size
		{
			// 如果type是CONTINUOUS或STEPWISE, 则不做任何事
			printf("{ continuous: min { width = %u, height = %u }, max { width = %u, height = %u } }\n",
				fsize.stepwise.min_width, fsize.stepwise.min_height,
				fsize.stepwise.max_width, fsize.stepwise.max_height);
			printf("  will not enumerate frame intervals.\n");
		}
		else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)      // 逐步定义帧size
		{
			printf("{ stepwise: min { width = %u, height = %u }, max { width = %u, height = %u }, stepsize { width = %u, height = %u } }\n",
				fsize.stepwise.min_width, fsize.stepwise.min_height,
				fsize.stepwise.max_width, fsize.stepwise.max_height,
				fsize.stepwise.step_width, fsize.stepwise.step_height);
			printf("  will not enumerate frame intervals.");
		}
		else
		{
			printf("  fsize.type not supported: %d\n", fsize.type);
			printf("     (Discrete: %d   Continuous: %d  Stepwise: %d)\n",
				V4L2_FRMSIZE_TYPE_DISCRETE,
				V4L2_FRMSIZE_TYPE_CONTINUOUS,
				V4L2_FRMSIZE_TYPE_STEPWISE);
		}
	}

	// 如果设备不支持任何DISCRETE类型的分辨率, 尝试通过VIDIOC_TRY_FMT对设备设置分辨率, 如果设置成功, 也认为
	// 这个摄像头设备支持这种分辨率
	if (fsizeind == 0)
	{
		/* ------ gspca doesn't enumerate frame sizes ------ */
		/*       negotiate with VIDIOC_TRY_FMT instead       */
		static const struct {
			int w,h;
		} defMode[] = {
			{800,600},
			{768,576},
			{768,480},
			{720,576},
			{720,480},
			{704,576},
			{704,480},
			{640,480},
			{352,288},
			{320,240}
		};

		unsigned int i;
		for (i = 0 ; i < (sizeof(defMode) / sizeof(defMode[0])); i++)
		{
			fsizeind++;
			struct v4l2_format fmt;
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width = defMode[i].w;
			fmt.fmt.pix.height = defMode[i].h;
			fmt.fmt.pix.pixelformat = pixfmt;
			fmt.fmt.pix.field = V4L2_FIELD_ANY;

			if (ioctl(s->fd,VIDIOC_TRY_FMT, &fmt) >= 0)
			{
				printf("{ ?GSPCA? : width = %u, height = %u }\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
			}
		}
	}

	return 0;
}

static int v4l2_enum_frameFormat(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
	struct v4l2_fmtdesc vfd;

	memset(&vfd, 0, sizeof(vfd));
	vfd.index = 0;
	vfd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	// enum supported fmt of this dev
	while (ioctl(s->fd, VIDIOC_ENUM_FMT, &vfd) >= 0)
	{
        char strFmt[8] = {0};
		vfd.index++;

        if (!(vfd.flags & V4L2_FMT_FLAG_COMPRESSED) &&
            s->list_format & V4L_RAWFORMATS) {
            DEMO_INFO("Raw\t\t: pixelformat = %s, description = %s\n", format_fcc_to_str2(vfd.pixelformat, strFmt, sizeof(strFmt)), vfd.description);
        } else
        if (vfd.flags & V4L2_FMT_FLAG_COMPRESSED &&
            s->list_format & V4L_COMPFORMATS) {
            DEMO_INFO("Compressed\t: pixelformat = %s, description = %s\n", format_fcc_to_str2(vfd.pixelformat, strFmt, sizeof(strFmt)), vfd.description);
        } else {
            continue;
        }

		//enumerate frame sizes & fps for this pixel format
		v4l2_enum_frameSize(ctx, vfd.pixelformat);
	}

    return 0;
}

static int get_commended_frameFormat(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;

    AUTO_MATCH(s, g_mjpegFmtList, V4L2_PIX_FMT_MJPEG);
    AUTO_MATCH(s, g_yuyvFmtList, V4L2_PIX_FMT_YUYV);
    AUTO_MATCH(s, g_nv12FmtList, V4L2_PIX_FMT_NV12);

    printf("Not supported format!\n");
    return -1;
}

int v4l2_read_header(DeviceContex_t *ctx)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    int res = 0;
    struct v4l2_input input = { 0 };
    char strFmt[8] = {0};
    
    /* silence libv4l2 logging. if fopen() fails v4l2_log_file will be NULL
       and errors will get sent to stderr */
    if (s->use_libv4l2)
        v4l2_log_file = fopen("/dev/null", "w");

    s->fd = device_open(ctx, ctx->url);
    if (s->fd < 0) {
        DEMO_ERR("device_open error\n");
        return s->fd;
    }
    DEMO_INFO("device_open done\n");

    // query capability 
    if (query_cap(ctx) < 0)
        goto fail;

    // enum input dev
    if (enum_input(ctx) < 0)
		goto fail;

    // set input dev
    if (set_input(ctx, 0) < 0)
		goto fail;

	printf("v4l2_s_input exec success\n");

    // get supported fmt list
    v4l2_enum_frameFormat(ctx);

    // cget the configuration closest to the settings
    if (get_commended_frameFormat(ctx) < 0)
        goto fail;

    // set commended format
    res = device_try_init(ctx, &s->width, &s->height, &s->pixelformat);
    if (res < 0)
        goto fail;

    DEMO_INFO("Setting format %s frame size to %dx%d\n", 
        format_fcc_to_str2(s->pixelformat, strFmt, sizeof(strFmt)), s->width, s->height);

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

    g_mjpegFmtCnt = 0;
    INIT_LIST_HEAD(&g_mjpegFmtList);
    g_yuyvFmtCnt = 0;
    INIT_LIST_HEAD(&g_yuyvFmtList);
    g_nv12FmtCnt = 0;
    INIT_LIST_HEAD(&g_nv12FmtList);
    
    return 0;
}

void v4l2_dev_set_fmt(DeviceContex_t *ctx, int v4l2_fmt, int width, int height)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;
    char strFmt[8] = {0};

    assert(ctx);
    assert(s);

    s->pixelformat = v4l2_fmt;
    s->width = width;
    s->height = height;

    printf("desired fmt: %s, width: %d, height: %d\n", format_fcc_to_str2(v4l2_fmt, strFmt, sizeof(strFmt)), width, height);
}

void v4l2_dev_get_fmt(DeviceContex_t *ctx, int *v4l2_fmt, int *width, int *height)
{
    struct video_data *s = (struct video_data *)ctx->priv_data;

    assert(ctx);
    assert(s);

    *v4l2_fmt = s->pixelformat;
    *width = s->width;
    *height = s->height;
}

void v4l2_dev_deinit(DeviceContex_t *ctx)
{
    assert(ctx);
    assert(ctx->inited==true);

    CLEAR_LIST(g_mjpegFmtList);
    g_mjpegFmtCnt = 0;
    CLEAR_LIST(g_yuyvFmtList);
    g_yuyvFmtCnt = 0;
    CLEAR_LIST(g_nv12FmtList);
    g_nv12FmtCnt = 0;

    v4l2_free(ctx->priv_data);
    v4l2_free(ctx);
}

void save_file(void *buf, int length, char type)
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
