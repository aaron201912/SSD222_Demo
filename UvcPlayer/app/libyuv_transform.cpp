#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "libyuv.h"

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif
void ST_I422ToYUY2(const uint8_t* pu8src_y,
                   int src_stride_y,
                   const uint8_t* pu8src_u,
                   int src_stride_u,
                   const uint8_t* pu8src_v,
                   int src_stride_v,
                   uint8_t* pu8dst_yuy2,
                   int dst_stride_yuy2,
                   int width,
                   int height)
{
    const uint8_t* pu8stsrc_y = pu8src_y;
    const uint8_t* pu8stsrc_u = pu8src_u;
    const uint8_t* pu8stsrc_v = pu8src_v;
    uint8_t* pu8stdst_yuy2 = pu8dst_yuy2;

    libyuv::I422ToYUY2(pu8stsrc_y,src_stride_y,pu8stsrc_u,src_stride_u,pu8stsrc_v,src_stride_v,pu8stdst_yuy2,dst_stride_yuy2,width,height);
}
#if 1
void ST_I422ToI420(const uint8_t* pu8src_y,
                    int src_stride_y,
                    const uint8_t* pu8src_u,
                    int src_stride_u,
                    const uint8_t* pu8src_v,
                    int src_stride_v,
                    uint8_t* pu8dst_y,
                    int dst_stride_y,
                    uint8_t* pu8dst_u,
                    int dst_stride_u,
                    uint8_t* pu8dst_v,
                    int dst_stride_v,
                    int width,
                    int height)
{
    const uint8_t* pu8stsrc_y = pu8src_y;
    const uint8_t* pu8stsrc_u = pu8src_u;
    const uint8_t* pu8stsrc_v = pu8src_v;
    uint8_t* pu8stdst_y = pu8dst_y;
    uint8_t* pu8stdst_u = pu8dst_u;
    uint8_t* pu8stdst_v = pu8dst_v;

    libyuv::I422ToI420(pu8stsrc_y,src_stride_y,pu8stsrc_u,src_stride_u,pu8stsrc_v,src_stride_v,pu8stdst_y,dst_stride_y,pu8stdst_u,dst_stride_u,pu8stdst_v,dst_stride_v,width,height);
}

void ST_I420ToNV12(const uint8_t* pu8src_y,
                       int src_stride_y,
                       const uint8_t* pu8src_u,
                       int src_stride_u,
                       const uint8_t* pu8src_v,
                       int src_stride_v,
                       uint8_t* pu8dst_y,
                       int dst_stride_y,
                       uint8_t* pu8dst_uv,
                       int dst_stride_uv,
                       int width,
                       int height)
{
    const uint8_t* pu8stsrc_y = pu8src_y;
    const uint8_t* pu8stsrc_u = pu8src_u;
    const uint8_t* pu8stsrc_v = pu8src_v;
    uint8_t* pu8stdst_y = pu8dst_y;
    uint8_t* pu8stdst_uv = pu8dst_uv;

    libyuv::I420ToNV12(pu8stsrc_y,src_stride_y,pu8stsrc_u,src_stride_u,pu8stsrc_v,src_stride_v,pu8stdst_y,dst_stride_y,pu8stdst_uv,dst_stride_uv,width,height);
}
#endif
#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

