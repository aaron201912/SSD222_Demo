#ifndef __V4L2_H_
#define __V4L2_H_

#include "datatype.h"

typedef struct BufferRef {
    uint8_t *data;
    int      size;
} BufferRef;

typedef struct Packet {
    BufferRef *buf;
    int size;
    uint8_t *data;
    int64_t pts;
} Packet;

void v4l2_free(void *ptr);
void *v4l2_malloc(size_t size);
int  v4l2_dev_init(DeviceContex_t **ctx, char *path);
int  v4l2_read_header(DeviceContex_t *ctx);
int  v4l2_read_packet(DeviceContex_t *ctx, Packet *pkt);
int  v4l2_read_packet_end(DeviceContex_t *ctx, Packet *pkt);
int  v4l2_read_close(DeviceContex_t *ctx);
void v4l2_dev_set_fmt(DeviceContex_t *ctx, int v4l2_fmt, int width, int height);
void v4l2_dev_deinit(DeviceContex_t *ctx);
void save_file(void *buf, int length,char type);

#endif//__V4L2_H_
