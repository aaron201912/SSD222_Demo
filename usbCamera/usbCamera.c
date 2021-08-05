#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "mi_disp.h"
#include "mi_disp_datatype.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_venc.h"
#include "jdec.h"
#include "v4l2.h"
#include "libyuv.h"
#include "sstar_dynamic_load.h"


#define VMAX(a,b) ((a) > (b) ? (a) : (b))
#define VMIN(a,b) ((a) < (b) ? (a) : (b))
#define ALIGN_DOWN(val, align)  (((val) / (align)) * (align))
#define ALIGN_UP(val, align)    (((val) + ((align) - 1)) & ~((align) - 1))

#define STCHECKRESULT(result)\
    if (result != MI_SUCCESS)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__);\
        return;\
    }\
    else\
    {\
        printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__);\
    }


#define YUV420      3/2
#define YUV422      2
#define YUV_TYPE    YUV422

#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0,128,128)

#define TIMEUOT     				1000

#define TIME_DIFF_PRE_FRAME         33 * 1000

#define DIVP_CAP_MAX_WIDTH          1920
#define DIVP_CAP_MAX_HEIGHT         1080
#define DISP_OUT_MAX_WIDTH          1024
#define DISP_OUT_MAX_HEIGHT         600

#define CAMERA_VIDEO_WIDTH    		640         //1280
#define CAMERA_VIDEO_HEIGHT   		480         //720

#define UEVENT_BUFFER_SIZE 			2048


/*
 * UVC image -> SCl -> rotate -> disp
 *
 * do scale:
 * SCL input -> SCL output (panel_max_width, panel_max_height)
 * disp input -> disp out (panel_max_width, panel_max_height)
 *
 * do scale & rotate:
 * SCL  input  -> SCL output (align_down(panel_max_height), align_down(panel_max_width))
 * rotate input -> rotate output (align_down(panel_max_width), align_down(panel_max_height))
 * disp input -> disp output (panel_max_width, panel_max_height）
*/
#define DIVP_SCL_OUTPUT_WIDTH(rotate)     (rotate ? ALIGN_DOWN(DISP_OUT_MAX_HEIGHT, 32) : DISP_OUT_MAX_WIDTH)
#define DIVP_SCL_OUTPUT_HEIGHT(rotate)     (rotate ? ALIGN_DOWN(DISP_OUT_MAX_WIDTH, 32) : DISP_OUT_MAX_HEIGHT)

#define DISP_INPUT_WIDTH(rotate)    (rotate ? ALIGN_DOWN(DISP_OUT_MAX_WIDTH, 32) : DISP_OUT_MAX_WIDTH)
#define DISP_INPUT_HEIGHT(rotate)   (rotate ? ALIGN_DOWN(DISP_OUT_MAX_HEIGHT, 32) : DISP_OUT_MAX_HEIGHT)
#define DISP_OUTPUT_WIDTH           DISP_OUT_MAX_WIDTH           
#define DISP_OUTPUT_HEIGHT          DISP_OUT_MAX_HEIGHT
#define SNAP_PIC_WIDTH              (DISP_OUT_MAX_WIDTH / 4)            // snap pic size
#define SNAP_PIC_HEIGHT             (DISP_OUT_MAX_HEIGHT / 4)

#define DIVP_SCL_CHN        0
#define DIVP_ROTATE_CHN     1

#define SNAP_PIC_NUM						5
#define SNAP_FRAME_INTERVAL					30
#define SNAP_DIR                            "/customer/"

typedef enum
{
    //E_FMT_H264 = 0,
    //E_FMT_H265,
    E_FMT_MJPEG,
    E_FMT_NV12,
    E_FMT_YUYV,
    E_FMT_BUTT
} V4L2FormatPriority_e;

typedef struct
{
	void *pHandle;
	int (*pfnLibyuvI422ToYUY2)(const uint8_t* src_y,
							  int src_stride_y,
							  const uint8_t* src_u,
							  int src_stride_u,
							  const uint8_t* src_v,
							  int src_stride_v,
							  uint8_t* dst_yuy2,
							  int dst_stride_yuy2,
							  int width,
							  int height);
	int (*pfnLibyuvI420ToNV12)(const uint8_t* src_y,
							  int src_stride_y,
							  const uint8_t* src_u,
							  int src_stride_u,
							  const uint8_t* src_v,
							  int src_stride_v,
							  uint8_t* dst_y,
							  int dst_stride_y,
							  uint8_t* dst_uv,
							  int dst_stride_uv,
							  int width,
							  int height);
} LibyuvAssembly_t;

typedef struct
{
    int fd;
    MI_VENC_CHN chnId;
    MI_VENC_ModType_e eModType;
} VencParam_t;

static DivpAssembly_t g_stDivpAssembly;
static LibyuvAssembly_t g_stLibyuvAssembly;
static int g_isRotate = 0;
static pthread_t g_checkHotplugThread = 0;
static int g_bCheckUvcThreadRun = 0;

// venc get es
static pthread_t g_vencThread = 0;
static int g_bvencThreadRun = 0;
static VencParam_t g_vencParam;

// snap pic
static int g_snapTryCnt = 0;
static int g_videoFrameCnt = 0;
static pthread_t g_stretchBufThread = 0;

int SSTAR_LIBYUV_OpenLibrary(LibyuvAssembly_t *pstLibyuvAssembly)
{
	pstLibyuvAssembly->pHandle = dlopen("libyuv.so", RTLD_NOW);
	if (NULL == pstLibyuvAssembly->pHandle)
	{
		printf(" %s: Can not load libyuv.so! %s\n", __func__, dlerror());
		return -1;
	}

	pstLibyuvAssembly->pfnLibyuvI422ToYUY2 = (int(*)(const uint8_t* src_y,
													  int src_stride_y,
													  const uint8_t* src_u,
													  int src_stride_u,
													  const uint8_t* src_v,
													  int src_stride_v,
													  uint8_t* dst_yuy2,
													  int dst_stride_yuy2,
													  int width,
													  int height))dlsym(pstLibyuvAssembly->pHandle, "I422ToYUY2");	// "libyuv::I422ToYUY2"
	if(NULL == pstLibyuvAssembly->pfnLibyuvI422ToYUY2)
	{
		printf(" %s: dlsym libyuv::I422ToYUY2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstLibyuvAssembly->pfnLibyuvI420ToNV12 = (int(*)(const uint8_t* src_y,
											  int src_stride_y,
											  const uint8_t* src_u,
											  int src_stride_u,
											  const uint8_t* src_v,
											  int src_stride_v,
											  uint8_t* dst_y,
											  int dst_stride_y,
											  uint8_t* dst_uv,
											  int dst_stride_uv,
											  int width,
											  int height))dlsym(pstLibyuvAssembly->pHandle, "I420ToNV12");	// "libyuv::I420ToNV12"
	if(NULL == pstLibyuvAssembly->pfnLibyuvI420ToNV12)
	{
		printf(" %s: dlsym libyuv::I420ToNV12 failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_LIBYUV_CloseLibrary(LibyuvAssembly_t *pstLibyuvAssembly)
{
	if(pstLibyuvAssembly->pHandle)
	{
		dlclose(pstLibyuvAssembly->pHandle);
		pstLibyuvAssembly->pHandle = NULL;
	}
	memset(pstLibyuvAssembly, 0, sizeof(*pstLibyuvAssembly));
}


int _sys_mma_alloc(jdecIMAGE *pImage, int size)
{
    if (0 != MI_SYS_MMA_Alloc((unsigned char*)"#jdecI0", size, &(pImage->phyAddr)))
    {
        printf("MI_SYS_MMA_Alloc Failed\n");
        return -1;
    }
    if (0 != MI_SYS_Mmap(pImage->phyAddr, size, (void **)&(pImage->virtAddr), 1))
    {
        MI_SYS_MMA_Free(pImage->phyAddr);
        printf("MI_SYS_Mmap Failed\n");
        return -1;
    }

    return 0;
}

int _sys_mma_free(jdecIMAGE *pImage, int size)
{
    if (!pImage->virtAddr && !pImage->phyAddr)
        return -1;

    if (0 != MI_SYS_Munmap(pImage->virtAddr, size))
    {
    	printf("MI_SYS_Munmap Failed\n");
        return -1;
    }
    if (0 != MI_SYS_MMA_Free(pImage->phyAddr))
    {
    	printf("MI_SYS_MMA_Free Failed\n");
        return -1;
    }

    return 0;
}


struct timeval time_start, time_end;
int64_t time0;

int jdec_convert_yuv_fotmat(jdecIMAGE *pImage, unsigned char *pBuf[3])
{
    unsigned char* pu8STSrcU = NULL;
    unsigned char* pu8STSrcV = NULL;
    //FILE *jpg_fd = NULL;

    if(pImage->esubsamp == SAMP_422)        //divp 只支持YUV422_YUYV      YV16-->420
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2;

        g_stLibyuvAssembly.pfnLibyuvI422ToYUY2((unsigned char*)pImage->virtAddr, pImage->width,
							pu8STSrcU, pImage->width / 2,
							pu8STSrcV, pImage->width / 2,
							(uint8_t*)pBuf[0], pImage->width * 2,
							pImage->width, pImage->height);

        #if 0
        ST_INFO("YUV Type Is SAMP_422, Image Size = %d\n", pImage->width * pImage->height);

        jpg_fd = fopen("/mnt/mjpeg/dump_422.yuv", "w+");
        fwrite(pBuf[0], 1, pImage->width * pImage->height * 2, jpg_fd);
        fclose(jpg_fd);
        #endif

        return 0;
    }
    else if(pImage->esubsamp == SAMP_420)   //divp 只支持NV12（YUV420sp）
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2 / 2;

        g_stLibyuvAssembly.pfnLibyuvI420ToNV12((unsigned char*)pImage->virtAddr, pImage->width,
							pu8STSrcU, pImage->width / 2,
							pu8STSrcV, pImage->width / 2,
							(uint8_t *)pBuf[0], pImage->width,
							(uint8_t *)pBuf[1], pImage->width,
							pImage->width, pImage->height);

        #if 0
        ST_INFO("YUV Type Is SAMP_420, Image Size = %d\n", pImage->width * pImage->height);

        jpg_fd = fopen("/mnt/mjpeg/dump_420.yuv", "w+");
        fwrite(pBuf[0], 1, pImage->width * pImage->height, jpg_fd);
        fwrite(pBuf[1], 1, pImage->width * pImage->height / 2, jpg_fd);
        fclose(jpg_fd);
        #endif

        return 0;
    }
    else
    {
        return -1;
    }
}

void* stretch_buf_thread(void *args)
{
    MI_PHY phySrcBufAddr = 0;
    MI_PHY phyDstBufAddr = 0;
    MI_DIVP_DirectBuf_t stSrcBuf;
    MI_DIVP_DirectBuf_t stDstBuf;
    MI_SYS_WindowRect_t stSrcCrop;

    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_BufInfo_t stSrcBufInfo;
    MI_SYS_BUF_HANDLE hSrcBufHandle = 0;

    MI_SYS_ChnPort_t stDstChnPort;
    MI_SYS_BufConf_t stDstBufConf;
    MI_SYS_BufInfo_t stDstBufInfo;
    MI_SYS_BUF_HANDLE hDstBufHandle = 0;

    stSrcBuf.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stSrcBuf.u32Width = DISP_INPUT_WIDTH(g_isRotate);
    stSrcBuf.u32Height = DISP_INPUT_HEIGHT(g_isRotate);
    stSrcCrop.u16X = 0;
    stSrcCrop.u16Y = 0;
    stSrcCrop.u16Width = stSrcBuf.u32Width;
    stSrcCrop.u16Height = stSrcBuf.u32Height;

    stDstBuf.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stDstBuf.u32Width = SNAP_PIC_WIDTH;
    stDstBuf.u32Height = SNAP_PIC_HEIGHT;

    // divp_rotate output
    memset(&stSrcChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stSrcChnPort.u32DevId = 0;
    stSrcChnPort.u32PortId = 0;
    if (g_isRotate)
        stSrcChnPort.u32ChnId = DIVP_ROTATE_CHN;
    else
        stSrcChnPort.u32ChnId = DIVP_SCL_CHN;

    // venc input
    memset(&stDstChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    //stDstChnPort.u32DevId = 0;
    MI_VENC_GetChnDevid(0, &stDstChnPort.u32DevId);
    stDstChnPort.u32ChnId = 0;
    stDstChnPort.u32PortId = 0;

    stDstBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
    stDstBufConf.u32Flags = 0x80000000;
    stDstBufConf.u64TargetPts = 0x12340000;
    stDstBufConf.stFrameCfg.u16Width = SNAP_PIC_WIDTH;
    stDstBufConf.stFrameCfg.u16Height = SNAP_PIC_HEIGHT;
    stDstBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
    stDstBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;

    printf("stretch_buf_thread.................................\n");
    // rotate_divp output -> venc input 
    //one buf stretch 
    while (1)
    {
        if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stSrcChnPort, &stSrcBufInfo, &hSrcBufHandle))
        {
            printf("try to get divp output buf...\n");
            usleep(100000);
            continue;
        }

        printf("get divp output buf ok...\n");

        while (1)
        {
            if (MI_SUCCESS != MI_SYS_ChnInputPortGetBuf(&stDstChnPort, &stDstBufConf, &stDstBufInfo, &hDstBufHandle, 100))
            {
                usleep(100000);
                continue;
            }
            else
                break;
        }

        printf("get venc intput buf ok...\n");

        stSrcBuf.u32Stride[0] = stSrcBufInfo.stFrameData.u32Stride[0];
        stSrcBuf.u32Stride[1] = stSrcBufInfo.stFrameData.u32Stride[1];
        stSrcBuf.phyAddr[0] = stSrcBufInfo.stFrameData.phyAddr[0];
        stSrcBuf.phyAddr[1] = stSrcBufInfo.stFrameData.phyAddr[1];

        stDstBuf.u32Stride[0] = stDstBufInfo.stFrameData.u32Stride[0];
        stDstBuf.u32Stride[1] = stDstBufInfo.stFrameData.u32Stride[1];
        stDstBuf.phyAddr[0] = stDstBufInfo.stFrameData.phyAddr[0];
        stDstBuf.phyAddr[1] = stDstBufInfo.stFrameData.phyAddr[1];

        printf("stretch buf by divp...\n");
        MI_DIVP_StretchBuf(&stSrcBuf, &stSrcCrop, &stDstBuf);
        MI_SYS_ChnInputPortPutBuf(hDstBufHandle, &stDstBufInfo, false);
        MI_SYS_ChnOutputPortPutBuf(hSrcBufHandle);
        break;
    }

    return NULL;
}

int send_yuv_to_display(void *pData, int fmt, int width, int height)
{
    jdecIMAGE *pImage = NULL; 
    Packet *pPkt = NULL;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_SYS_ChnPort_t stInputChnPort;

    memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
    memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
    memset(&stInputChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

    stInputChnPort.eModId    = E_MI_MODULE_ID_DIVP;
    stInputChnPort.u32ChnId  = 0;
    stInputChnPort.u32DevId  = 0;
    stInputChnPort.u32PortId = 0;

    stBufConf.u64TargetPts   = 0;
    stBufConf.eBufType       = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.u32Flags       = MI_SYS_MAP_VA;
    stBufConf.stFrameCfg.u16Width       = width;
    stBufConf.stFrameCfg.u16Height      = height;
    stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;

    if (fmt == V4L2_PIX_FMT_MJPEG)
    {
        pImage = (jdecIMAGE *)pData;

        switch(pImage->esubsamp)//divp only support YUYV422&&YUV420SP
        {
            case SAMP_422:  // YUV422_YUYV      YV16-->420
            {
                stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
                stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            }
            break;

            case SAMP_420:  // NV12（YUV420sp）
            {
                stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
                stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            }
            break;

            default:
            {
                printf("Not Support pixel type, Esubsamp [%d]\n", pImage->esubsamp);
                return -1;
            }
            break;
        }
    }
    else
    {
        pPkt = (Packet *)pData;

        if (fmt == V4L2_PIX_FMT_YUYV)
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
        } 
        else if (fmt == V4L2_PIX_FMT_NV12)
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        }
        else
        {
            printf("Not Support pixel type, Esubsamp [%d]\n", pImage->esubsamp);
            return -1;
        }
    }

    int ret = MI_SYS_ChnInputPortGetBuf(&stInputChnPort, &stBufConf, &stBufInfo, &stBufHandle, 100);
	
    if (MI_SUCCESS == ret)
    {
        stBufInfo.stFrameData.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
        stBufInfo.stFrameData.eFieldType    = E_MI_SYS_FIELDTYPE_NONE;
        stBufInfo.stFrameData.eTileMode     = E_MI_SYS_FRAME_TILE_MODE_NONE;
        stBufInfo.bEndOfStream              = 0;

        //ST_INFO("Get Buf Info: Width/Height/Stride = [%d %d %d]\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32Stride[0]);

        //gettimeofday(&time_start, NULL);

        if (fmt == V4L2_PIX_FMT_MJPEG)
        {
            if (0 != jdec_convert_yuv_fotmat(pImage, (unsigned char **)&stBufInfo.stFrameData.pVirAddr[0]))
            {
                printf("jdec_convert_yuv_fotmat error!\n");
            }
        }
        else if (fmt == V4L2_PIX_FMT_YUYV)
        {
            //printf("debug YUYV, pkt size=%d, Y dataLen=%d\n", pPkt->size, width*height);
            //printf("stBufInfo idx_0: pVirAddr=%x, width=%d, height=%d, stride=%d, bufSize=%d\n", stBufInfo.stFrameData.pVirAddr[0], (int)stBufInfo.stFrameData.u16Width,
            //(int)stBufInfo.stFrameData.u16Height, (int)stBufInfo.stFrameData.u32Stride, (int)stBufInfo.stFrameData.u32BufSize);
            memcpy((uint8_t *)stBufInfo.stFrameData.pVirAddr[0], (uint8_t*)pPkt->data, width*height*2);
        }
        else if (fmt == V4L2_PIX_FMT_NV12)
        {
            memcpy((uint8_t *)stBufInfo.stFrameData.pVirAddr[0], (uint8_t *)pPkt->data, width*height);
            memcpy((uint8_t *)stBufInfo.stFrameData.pVirAddr[1], (uint8_t *)pPkt->data+width*height, width*height/2);
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //ST_WARN("yuv fotmat convert time = %lld\n", time0);

        if(MI_SUCCESS != MI_SYS_ChnInputPortPutBuf(stBufHandle, &stBufInfo, 0))
        {
        	printf("MI_SYS_ChnInputPortPutBuf Failed!\n");
            return -1;
        }
    }
    else
    {
    	printf("MI_SYS_ChnInputPortGetBuf Failed!, ret=%d\n", ret);
         return -1;
    }

    // stretch buf to encode
    if (g_videoFrameCnt <= SNAP_FRAME_INTERVAL * SNAP_PIC_NUM)
    {
        if (g_snapTryCnt < SNAP_PIC_NUM)
        {
            if (g_videoFrameCnt == (g_snapTryCnt+1) * SNAP_FRAME_INTERVAL)
            {
                // create thread to do snapping
                if (!g_stretchBufThread)
                {
                    printf("create stretch buf\n");
                    pthread_create(&g_stretchBufThread, NULL, stretch_buf_thread, NULL);
                }
                else
                {
                    printf("wait last frame done...\n");
                    pthread_join(g_stretchBufThread, NULL);
                    printf("create stretch buf\n");
                    pthread_create(&g_stretchBufThread, NULL, stretch_buf_thread, NULL);
                }

                g_snapTryCnt++;
            }
        }

        g_videoFrameCnt++;
    }

    return 0;
}

static pthread_t jdec_tid;
static bool b_exit = false;
static DeviceContex_t *ctx = NULL;
static Packet pkt;


static void *displaying_thread(void * args)
{
    int ret;
    int yuv_size = 0;
    int image0Size = 0;
    int image0Width = 0, image0Height = 0, image0Fmt = 0;
    jdecIMAGE image0 = {0};
    int timeout = 0;
    int decode_cnt = 0;

    // get current camera output format & size, calculate alloc buf size
    v4l2_dev_get_fmt(ctx, &image0Fmt, &image0Width, &image0Height);
    image0Size = image0Width * image0Height * YUV422;

    if (image0Fmt == V4L2_PIX_FMT_MJPEG)
    {
        if (SSTAR_TurboJpeg_OpenLibrary())
        {
            printf("open libmi_ao failed\n");
            return NULL;
        }

        if (0 != _sys_mma_alloc(&image0, image0Size))
        {
            printf("_sys_mma_alloc failed\n");
            goto error;
        }
    }

    while (!b_exit)
    {
        // check dev connect status before read packet?

        ret = v4l2_read_packet(ctx, &pkt);
        if(ret < 0)
        {
            if(++ timeout < TIMEUOT)
            {
                usleep(1 * 1000);
                continue;
            }
            else
            {
            	printf("v4l2_read_packet timeout exit\n");
                goto error;
            }
        }
        timeout = 0;

        if (image0Fmt == V4L2_PIX_FMT_MJPEG)
        {
            // decode mjpeg to yuv
            yuv_size = jdec_decode_yuv_from_buf((char *)pkt.data, pkt.size, &image0, TANSFORM_NONE, SAMP_420);
            if(yuv_size < 0)
            {
                printf("Decode done, yuv_size=%d, image w/h=[%d %d], decode_cnt=%d \n", yuv_size, image0.width, image0.height, decode_cnt);
                goto try_again;
            }
            else
            {
                if (image0.width <= 0 || image0.height <= 0)
                {
                    printf("image w/h=[%d %d] parameter invalid!\n", image0.width, image0.height);
                    v4l2_read_packet_end(ctx, &pkt);
                    goto error;
                }

                // compare the size of camera output with the size of image
                if (image0.width != image0Width || image0.height != image0Height)
                    printf("that's unbelievable!\n");
            }
        }

        if (image0Fmt == V4L2_PIX_FMT_MJPEG)
        {
            // show yuv
            if(0 != send_yuv_to_display(&image0, image0Fmt, image0Width, image0Height))
            {
                printf("send_yuv_to_display error\n");
                v4l2_read_packet_end(ctx, &pkt);
                //goto stop_stretch_buf;
                continue;
            }
        }
        else
        {
            // show yuv
            if(0 != send_yuv_to_display(&pkt, image0Fmt, image0Width, image0Height))
            {
                printf("send_yuv_to_display error\n");
                v4l2_read_packet_end(ctx, &pkt);
                //goto stop_stretch_buf;
                continue;
            }
        }

        //save_file((char *)pkt.data, pkt.size, 2);

        if (!decode_cnt)
            printf("decode the first frame done\n");

        decode_cnt ++;

try_again:
        v4l2_read_packet_end(ctx, &pkt);
    }

error:
    if (g_stretchBufThread)
    {
        pthread_join(g_stretchBufThread, NULL);
        g_stretchBufThread = 0;
    }

    if (image0Fmt == V4L2_PIX_FMT_MJPEG)
    {
        _sys_mma_free(&image0, image0Size);
        SSTAR_TurbpJpeg_CloseLibrary();
    }
    
    return NULL;
}

int sstar_usbcamera_init()
{
    printf("sstar_usbcamera_init\n");

    int checkTimeout = 5000;
    printf("check usb camera dev node\n");

    while (checkTimeout--)
    {
        //if (!access("/dev/video0", F_OK))
        if (!access("/dev/video0", R_OK))
        {
            printf("detect usb camera dev\n");
            break;
        }
        else
        {
            usleep(5000);
        }
    }

    if (!checkTimeout)
    {
        printf("usb camera dev is not exist, app exit\n");
        return -1;
    }

    printf("usb camere app start\n");

    if (SSTAR_LIBYUV_OpenLibrary(&g_stLibyuvAssembly))
    {
    	printf("open libyuv failed\n");
    	return -1;
    }

    v4l2_dev_init(&ctx,  (char*)"/dev/video0");
    v4l2_dev_set_fmt(ctx, V4L2_PIX_FMT_MJPEG, CAMERA_VIDEO_WIDTH, CAMERA_VIDEO_HEIGHT);     // desired fmt & size
    //v4l2_dev_set_fmt(ctx, V4L2_PIX_FMT_YUYV, CAMERA_VIDEO_WIDTH, CAMERA_VIDEO_HEIGHT);     // desired fmt & size

    if (0 != v4l2_read_header(ctx))
    {
        printf("Can't find usb camera\n");
        v4l2_dev_deinit(ctx);
        return -1;
    }

	printf("create displaying thread\n");

    b_exit = false;
    if (0 != pthread_create(&jdec_tid, NULL, displaying_thread, NULL))
    {
        printf("pthread_create failed\n");
        v4l2_read_close(ctx);
		v4l2_dev_deinit(ctx);
        return -1;
    }

    return 0;
}

void sstar_usbcamera_deinit()
{
    printf(" sstar_usbcamera_deinit !!!\n");
    if (!g_stLibyuvAssembly.pHandle)
    	return;

    b_exit = true;
	if (jdec_tid)
	{
		pthread_join(jdec_tid, NULL);
		jdec_tid = 0;
	}

	if (ctx)
	{
		v4l2_read_close(ctx);
		v4l2_dev_deinit(ctx);
		ctx = NULL;
	}

    MI_DISP_ClearInputPortBuffer(0, 0, TRUE);

	SSTAR_LIBYUV_CloseLibrary(&g_stLibyuvAssembly);
}

static int set_non_block(int iSock)
{
    int iFlags;
 
    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(iSock, F_SETFL, iFlags);
    return ret;
}

static int init_uvc_hotplug_sock()
{
    struct sockaddr_nl snl;
    const int buffersize = 2 * 1024 * 1024;
    int retval;
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);

    if (hotplug_sock == -1)
    {
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }

    // set nonblock
    set_non_block(hotplug_sock);

    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
    retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));

    if (retval < 0)
    {
        printf("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }

    return hotplug_sock;
}

static void deinit_uvc_hotplug_sock(int sock)
{
    if (sock != -1)
        close(sock);
}

static void *check_uvc_hotplug_proc(void *pdata)
{
    int hotplug_sock = init_uvc_hotplug_sock();

    printf("Exec check_uvc_hotplug_proc\n");

    while(g_bCheckUvcThreadRun)
    {
        char buf[UEVENT_BUFFER_SIZE*2] = {0};
        int len = recv(hotplug_sock, &buf, sizeof(buf), 0);

        if (strstr(buf, "video"))
        {
            char *pstmsg = buf;
			pstmsg = strstr(buf, "video");
			printf("pstmsg is %s\n", pstmsg);

            if (strstr(buf, "add"))
			{
				printf("uvc add\n");

				sstar_usbcamera_init();
			}

			if (strstr(buf, "remove"))
			{
				printf("usb remove\n");
				
                sstar_usbcamera_deinit();
			}
        }

        usleep(20000);
    }

    printf("close socket\n");
    deinit_uvc_hotplug_sock(hotplug_sock);

    printf("exit thread proc\n");
    return NULL;
}

int uvc_start_check_hotplug()
{
	g_bCheckUvcThreadRun = 1;

	pthread_create(&g_checkHotplugThread, NULL, check_uvc_hotplug_proc, NULL);

	if (!g_checkHotplugThread)
	{
		printf("create check hotplug thread failed\n");
		return -1;
	}

	return 0;
}

void uvc_stop_check_hotplug()
{
	g_bCheckUvcThreadRun = 0;

	if (g_checkHotplugThread)
	{
		printf("check thread is exiting\n");
		pthread_join(g_checkHotplugThread, NULL);
		g_checkHotplugThread = 0;
		printf("check thread exit\n");
	}
}


int display_init(int isRotate)
{
    MI_U16 u16Width, u16Height;
    MI_DISP_PubAttr_t stPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_RotateConfig_t stRotateConfig;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;
    MI_PANEL_IntfType_e eIntfType;
    MI_PANEL_ParamConfig_t pstParamCfg;

    // load libmi_divp
    memset(&g_stDivpAssembly, 0, sizeof(DivpAssembly_t));
    if (SSTAR_DIVP_OpenLibrary(&g_stDivpAssembly))
	{
		printf("open libmi_divp failed\n");
		return -1;
	}

    printf("display_init ..., width=%d, height=%d, rotate=%d\n", DISP_OUT_MAX_WIDTH, DISP_OUT_MAX_HEIGHT, isRotate);

    g_isRotate = isRotate;

    // disp & panel init
    MI_SYS_Init();

    memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
    stPubAttr.u32BgColor = YUYV_BLACK;
    stPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    MI_DISP_SetPubAttr(0, &stPubAttr);
    MI_DISP_Enable(0);

    memset(&stLayerAttr, 0, sizeof(MI_DISP_VideoLayerAttr_t));
    memset(&stRotateConfig, 0, sizeof(MI_DISP_RotateConfig_t));
    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width = stPubAttr.stSyncInfo.u16Hact;
    stLayerAttr.stVidLayerDispWin.u16Height = stPubAttr.stSyncInfo.u16Vact;
    MI_DISP_BindVideoLayer(0, 0);
    MI_DISP_SetVideoLayerAttr(0, &stLayerAttr);
    MI_DISP_EnableVideoLayer(0);

    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));

    stInputPortAttr.u16SrcWidth         = DISP_INPUT_WIDTH(isRotate);
    stInputPortAttr.u16SrcHeight        = DISP_INPUT_HEIGHT(isRotate);
    stInputPortAttr.stDispWin.u16X      = 0;
    stInputPortAttr.stDispWin.u16Y      = 0;
    stInputPortAttr.stDispWin.u16Width  = DISP_OUTPUT_WIDTH;
    stInputPortAttr.stDispWin.u16Height = DISP_OUTPUT_HEIGHT;
    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    eIntfType = E_MI_PNL_INTF_TTL;
    MI_PANEL_Init(eIntfType);
    MI_PANEL_GetPanelParam(eIntfType, &pstParamCfg);

    MI_DIVP_CHN sclChnId = DIVP_SCL_CHN;
    MI_SYS_ChnPort_t stDivpSclPort;
    MI_SYS_ChnPort_t stDispPort;
    MI_DIVP_ChnAttr_t stDivpSclChnAttr;
    MI_DIVP_OutputPortAttr_t stDivpSclOutputPortAttr;

    memset(&stDivpSclChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    memset(&stDivpSclOutputPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
    memset(&stDivpSclPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stDispPort, 0, sizeof(MI_SYS_ChnPort_t));

    stDivpSclPort.eModId    = E_MI_MODULE_ID_DIVP;
    stDivpSclPort.u32DevId  = 0;
    stDivpSclPort.u32ChnId  = sclChnId;
    stDivpSclPort.u32PortId = 0;

    stDispPort.eModId    = E_MI_MODULE_ID_DISP;
    stDispPort.u32DevId  = 0;
    stDispPort.u32ChnId  = 0;
    stDispPort.u32PortId = 0;

    stDivpSclChnAttr.bHorMirror            = FALSE;
    stDivpSclChnAttr.bVerMirror            = FALSE;
    stDivpSclChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
    stDivpSclChnAttr.eRotateType           = E_MI_SYS_ROTATE_NONE;
    stDivpSclChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpSclChnAttr.stCropRect.u16X       = 0;
    stDivpSclChnAttr.stCropRect.u16Y       = 0;
    stDivpSclChnAttr.stCropRect.u16Width   = 0;
    stDivpSclChnAttr.stCropRect.u16Height  = 0;
    stDivpSclChnAttr.u32MaxWidth           = DIVP_CAP_MAX_WIDTH;
    stDivpSclChnAttr.u32MaxHeight          = DIVP_CAP_MAX_HEIGHT;

    stDivpSclOutputPortAttr.eCompMode      = E_MI_SYS_COMPRESS_MODE_NONE;
    stDivpSclOutputPortAttr.ePixelFormat   = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stDivpSclOutputPortAttr.u32Width       = DIVP_SCL_OUTPUT_WIDTH(isRotate);
    stDivpSclOutputPortAttr.u32Height      = DIVP_SCL_OUTPUT_HEIGHT(isRotate);

    printf("scl divp output: width=%d, height=%d, rotate=%d\n", stDivpSclOutputPortAttr.u32Width, stDivpSclOutputPortAttr.u32Height, isRotate);

    g_stDivpAssembly.pfnDivpCreateChn(sclChnId, &stDivpSclChnAttr);
    g_stDivpAssembly.pfnDivpSetChnAttr(sclChnId, &stDivpSclChnAttr);
    g_stDivpAssembly.pfnDivpSetOutputPortAttr(sclChnId, &stDivpSclOutputPortAttr);
    g_stDivpAssembly.pfnDivpStartChn(sclChnId);

    if (isRotate)
    {
        // disp don't support rotate on this paltform. bind divp to implement it.
        MI_DIVP_CHN rotateChnId = DIVP_ROTATE_CHN;
        MI_SYS_ChnPort_t stDivpRotatePort;
        MI_DIVP_ChnAttr_t stDivpRotateChnAttr;
        MI_DIVP_OutputPortAttr_t stDivpRotateOutputPortAttr;

        memset(&stDivpRotateChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
        memset(&stDivpRotateOutputPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
        memset(&stDivpRotatePort, 0, sizeof(MI_SYS_ChnPort_t));

        stDivpRotatePort.eModId    = E_MI_MODULE_ID_DIVP;
        stDivpRotatePort.u32DevId  = 0;
        stDivpRotatePort.u32ChnId  = rotateChnId;
        stDivpRotatePort.u32PortId = 0;

        stDivpRotateChnAttr.bHorMirror            = FALSE;
        stDivpRotateChnAttr.bVerMirror            = FALSE;
        stDivpRotateChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
        stDivpRotateChnAttr.eRotateType           = E_MI_SYS_ROTATE_90;
        stDivpRotateChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
        stDivpRotateChnAttr.stCropRect.u16X       = 0;
        stDivpRotateChnAttr.stCropRect.u16Y       = 0;
        stDivpRotateChnAttr.stCropRect.u16Width   = 0;
        stDivpRotateChnAttr.stCropRect.u16Height  = 0;
        stDivpRotateChnAttr.u32MaxWidth           = DIVP_CAP_MAX_WIDTH;
        stDivpRotateChnAttr.u32MaxHeight          = DIVP_CAP_MAX_HEIGHT;

        stDivpRotateOutputPortAttr.eCompMode      = E_MI_SYS_COMPRESS_MODE_NONE;
        stDivpRotateOutputPortAttr.ePixelFormat   = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stDivpRotateOutputPortAttr.u32Width       = DISP_INPUT_WIDTH(isRotate);
        stDivpRotateOutputPortAttr.u32Height      = DISP_INPUT_HEIGHT(isRotate);

        printf("rotate divp output: width=%d, height=%d\n", stDivpRotateOutputPortAttr.u32Width, stDivpRotateOutputPortAttr.u32Height);

        g_stDivpAssembly.pfnDivpCreateChn(rotateChnId, &stDivpRotateChnAttr);
        g_stDivpAssembly.pfnDivpSetChnAttr(rotateChnId, &stDivpRotateChnAttr);
        g_stDivpAssembly.pfnDivpSetOutputPortAttr(rotateChnId, &stDivpRotateOutputPortAttr);
        g_stDivpAssembly.pfnDivpStartChn(rotateChnId);

        MI_SYS_BindChnPort(&stDivpSclPort, &stDivpRotatePort, 30, 30);
        MI_SYS_SetChnOutputPortDepth(&stDivpSclPort, 1, 3);

        MI_SYS_BindChnPort(&stDivpRotatePort, &stDispPort, 30, 30);
        MI_SYS_SetChnOutputPortDepth(&stDivpRotatePort, 1, 4);
    }
    else
    {
        MI_SYS_BindChnPort(&stDivpSclPort, &stDispPort, 30, 30);
        MI_SYS_SetChnOutputPortDepth(&stDivpSclPort, 1, 3);
    }

    printf("display_init done\n");

    return 0;
}

int display_deinit()
{
    printf("display_deinit ...\n");

    MI_SYS_ChnPort_t stDivpSclPort;
    MI_SYS_ChnPort_t stDispPort;

    if (!g_stDivpAssembly.pHandle)
    	return -1;

    memset(&stDivpSclPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stDispPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDivpSclPort.eModId    = E_MI_MODULE_ID_DIVP;
    stDivpSclPort.u32DevId  = 0;
    stDivpSclPort.u32ChnId  = 0;
    stDivpSclPort.u32PortId = 0;

    stDispPort.eModId    = E_MI_MODULE_ID_DISP;
    stDispPort.u32DevId  = 0;
    stDispPort.u32ChnId  = 0;
    stDispPort.u32PortId = 0;

    if (g_isRotate)
    {
        MI_SYS_ChnPort_t stDivpRotatePort;

        memset(&stDivpRotatePort, 0, sizeof(MI_SYS_ChnPort_t));
        stDivpRotatePort.eModId    = E_MI_MODULE_ID_DIVP;
        stDivpRotatePort.u32DevId  = 0;
        stDivpRotatePort.u32ChnId  = 1;
        stDivpRotatePort.u32PortId = 0;

        MI_SYS_UnBindChnPort(&stDivpSclPort, &stDivpRotatePort);
        MI_SYS_UnBindChnPort(&stDivpRotatePort, &stDispPort);

        g_stDivpAssembly.pfnDivpStopChn(1);
        g_stDivpAssembly.pfnDivpDestroyChn(1);
    }
    else
    {
        MI_SYS_UnBindChnPort(&stDivpSclPort, &stDispPort);
    }
    

    g_stDivpAssembly.pfnDivpStopChn(0);
    g_stDivpAssembly.pfnDivpDestroyChn(0);
    g_stDivpAssembly.pfnDivpDeInitDev();

	// unload libmi_divp
	SSTAR_DIVP_CloseLibrary(&g_stDivpAssembly);

	MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0); 
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);
	MI_DISP_DeInitDev();

    MI_PANEL_DeInit();
    MI_PANEL_DeInitDev();
    MI_SYS_Exit();

    printf("display_deinit done\n");

    return 0;
}

void* venc_output_thread(void *args)
{
    VencParam_t *pVencParam = (VencParam_t*)args;

    MI_SYS_ChnPort_t stVencChnInputPort;
    char szFileName[128];
    int fd = -1;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 len = 0;
    MI_U32 u32DevId = 0;
    fd_set read_fds;
    MI_VENC_Pack_t stPack[2];

    MI_VENC_GetChnDevid(pVencParam->chnId, &u32DevId);

    stVencChnInputPort.eModId = E_MI_MODULE_ID_VENC;
    stVencChnInputPort.u32DevId = u32DevId;
    stVencChnInputPort.u32ChnId = pVencParam->chnId;
    stVencChnInputPort.u32PortId = 0;

    pVencParam->fd = MI_VENC_GetFd(pVencParam->chnId);
    if(pVencParam->fd <= 0)
    {
        printf("Unable to get FD:%d for chn:%2d\n", pVencParam->fd, pVencParam->chnId);
        return NULL;
    }
    else
    {
        printf("Venc Chn%2d FD:%d\n", pVencParam->chnId, pVencParam->fd);
    }

    while(g_bvencThreadRun)
    {
        struct timeval TimeoutVal;
        MI_VENC_ChnStat_t stStat;
        MI_VENC_Stream_t stStream;
        int i;
        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(pVencParam->fd, &read_fds);
        s32Ret = select(pVencParam->fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            printf("select failed\n");
            usleep(10 * 1000);
            continue;
        }
        else if (0 == s32Ret)
        {
            //printf("select timeout\n");
            usleep(10 * 1000);
            continue;
        }
        else
        {
            printf("venc get input buf...\n");
            if (FD_ISSET(pVencParam->fd, &read_fds))
            {
                memset(&stStat, 0, sizeof(MI_VENC_ChnStat_t));
                s32Ret = MI_VENC_Query(pVencParam->chnId, &stStat);
                if (MI_SUCCESS != s32Ret || stStat.u32CurPacks == 0)
                {
                    printf("MI_VENC_Query error, %X\n", s32Ret);
                    usleep(10 * 1000);//sleep 10 ms
                    continue;
                }

                //printf("u32CurPacks:%d, u32LeftStreamFrames:%d\n", stStat.u32CurPacks, stStat.u32LeftStreamFrames);

                memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
                memset(&stPack, 0, sizeof(MI_VENC_Pack_t)*2);

                stStream.u32PackCount = stStat.u32CurPacks;
                stStream.pstPack = stPack;
                s32Ret = MI_VENC_GetStream(pVencParam->chnId, &stStream, 40);
                if (MI_SUCCESS == s32Ret)
                {
                    printf("u32PackCount:%d, u32Seq:%d, offset:%d, len:%d, type:%d, pts:0x%llx\n", stStream.u32PackCount, stStream.u32Seq,
                        stStream.pstPack[0].u32Offset, stStream.pstPack[0].u32Len, stStream.pstPack[0].stDataType.eH264EType, stStream.pstPack[0].u64PTS);

                    if(pVencParam->eModType == E_MI_VENC_MODTYPE_H264E)
                    {
                        printf("eRefType:%d\n", stStream.stH264Info.eRefType);
                    }
                    else if(pVencParam->eModType == E_MI_VENC_MODTYPE_H265E)
                    {
                        printf("eRefType:%d\n", stStream.stH265Info.eRefType);
                    }

                    // save encode pic
                    memset(szFileName, 0, sizeof(szFileName));
                    struct timeval    tv; 
                    struct timezone tz;
                    struct tm *p;
                    gettimeofday(&tv, &tz);
                    p =localtime(&tv.tv_sec);

                    len = snprintf(szFileName, sizeof(szFileName) - 1, "%s%04d%02d%02d_%02d%02d%02d.%03ld_%dx%d.", SNAP_DIR,
                        1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000,
                        SNAP_PIC_WIDTH, SNAP_PIC_HEIGHT);
                    if (pVencParam->eModType == E_MI_VENC_MODTYPE_H264E)
                    {
                        snprintf(szFileName + len, sizeof(szFileName) - 1, "%s", "h264");
                    }
                    else if (pVencParam->eModType == E_MI_VENC_MODTYPE_H265E)
                    {
                        snprintf(szFileName + len, sizeof(szFileName) - 1, "%s", "h265");
                    }
                    else
                    {
                        snprintf(szFileName + len, sizeof(szFileName) - 1, "%s", "jpg");
                    }

                    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (fd < 0)
                    {
                        printf("create %s error\n", szFileName);
                        goto open_file_error;
                    }

                    printf("create %s success\n", szFileName);


                    for (i = 0; i < stStream.u32PackCount; i ++)
                    {
                        write(fd, stStream.pstPack[i].pu8Addr + stStream.pstPack[i].u32Offset,
                            stStream.pstPack[i].u32Len - stStream.pstPack[i].u32Offset);
                    }

                    close(fd);
open_file_error:
                    MI_VENC_ReleaseStream(pVencParam->chnId, &stStream);
                }
                else
                {
                    printf("MI_VENC_GetStream error, %X\n", s32Ret);
                    usleep(10 * 1000);//sleep 10 ms
                }
            }
        }
    }

    s32Ret = MI_VENC_CloseFd(pVencParam->chnId);
    if(s32Ret != 0)
    {
        printf("Chn%02d CloseFd error, Ret:%X\n", pVencParam->chnId, s32Ret);
    }

    return NULL;
}

int venc_snap_init(MI_VENC_CHN chnId, MI_VENC_ModType_e eModType, MI_VENC_RcMode_e eRcMode)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_CHN vencChn = chnId;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_SYS_ChnPort_t stVencChnOutputPort;

    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    memset(&stVencChnOutputPort, 0, sizeof(MI_SYS_ChnPort_t));

    if (E_MI_VENC_MODTYPE_H264E == eModType)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_H264E;
        stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrH264e.bByFrame = true;
        stChnAttr.stVeAttr.stAttrH264e.u32Profile = 1;

        if(eRcMode == E_MI_VENC_RC_MODE_H264FIXQP)
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264FIXQP;
            stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH264FixQp.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH264FixQp.u32IQp = 50;
            stChnAttr.stRcAttr.stAttrH264FixQp.u32PQp = 50;
        }
        else if(eRcMode == E_MI_VENC_RC_MODE_H264CBR)
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = 2000000;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
        }
        else
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264VBR;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate = 2000000;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxQp = 48;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32MinQp = 12;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateDen = 1;
        }
    }
    else if (E_MI_VENC_MODTYPE_H265E == eModType)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_H265E;
        stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrH265e.bByFrame = true;

        if(eRcMode == E_MI_VENC_RC_MODE_H265FIXQP)
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265FIXQP;
            stChnAttr.stRcAttr.stAttrH265FixQp.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH265FixQp.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH265FixQp.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH265FixQp.u32IQp = 50;
            stChnAttr.stRcAttr.stAttrH265FixQp.u32PQp = 50;
        }
        else if(eRcMode == E_MI_VENC_RC_MODE_H265CBR)
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265CBR;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = 2000000;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH265Cbr.u32Gop = 30;
        }
        else
        {
            stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265VBR;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32MaxBitRate = 2000000;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32MaxQp = 48;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32MinQp = 12;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32SrcFrmRateNum = 30;
            stChnAttr.stRcAttr.stAttrH265Vbr.u32SrcFrmRateDen = 1;
        }
    }
    else if (E_MI_VENC_MODTYPE_JPEGE == eModType)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = SNAP_PIC_WIDTH;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = SNAP_PIC_HEIGHT;
        stChnAttr.stVeAttr.stAttrJpeg.bByFrame = true;
        stChnAttr.stRcAttr.stAttrMjpegFixQp.u32Qfactor = 50;
        stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateNum = 30;
        stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateDen = 1;
    }

    s32Ret = MI_VENC_CreateChn(chnId, &stChnAttr);
    if (MI_SUCCESS != s32Ret)
    {
        printf("MI_VENC_CreateChn %d error, %X\n", chnId, s32Ret);
    }
    printf(" MI_VENC_CreateChn, vencChn:%d\n", chnId);

    s32Ret = MI_VENC_SetMaxStreamCnt(chnId, 4);
    if(MI_SUCCESS != s32Ret)
    {
        printf("MI_VENC_SetMaxStreamCnt %d error, %X\n", chnId, s32Ret);
    }

    memset(&g_vencParam, 0, sizeof(VencParam_t));
    g_vencParam.chnId = chnId;
    g_vencParam.eModType = eModType;
    g_vencParam.fd = -1;    //MI_VENC_GetFd(chnId);

    printf("venc chn:%d\n", chnId);

    // get es stream
    printf("create venc get es buf thread\n");
    g_bvencThreadRun = 1;
    pthread_create(&g_vencThread, NULL, venc_output_thread, (void *)&g_vencParam);

    s32Ret = MI_VENC_StartRecvPic(chnId);
    if (MI_SUCCESS != s32Ret)
    {
        printf("MI_VENC_StartRecvPic %d error, %X\n", chnId, s32Ret);
    }

    return 0;
}

int venc_snap_deinit(MI_VENC_CHN chnId)
{
    MI_S32 s32Ret = MI_SUCCESS;

    g_bvencThreadRun = 0;
    pthread_join(g_vencThread, NULL);

    s32Ret = MI_VENC_StopRecvPic(chnId);
    if(s32Ret)
        printf("MI_VENC_StopRecvPic error, %X\n", s32Ret);

    s32Ret = MI_VENC_DestroyChn(chnId);
    if(s32Ret)
        printf("MI_VENC_DestroyChn error, %X\n", s32Ret);

    return 0;
}

/*
 * camera -> divp_scl -> divp_rotate -> disp
 *                          ↓
 *                       divp_stretch -> venc
*/
int sstart_baseflow_init()
{
    display_init(1);
    venc_snap_init(0, E_MI_VENC_MODTYPE_JPEGE, E_MI_VENC_RC_MODE_MJPEGFIXQP);

    return 0;
}

void sstar_baseflow_deinit()
{
    venc_snap_deinit(0);
    display_deinit();
}


int main(int argc, char **argv)
{
    char ch = 0;
  
    // init base mi flow
    sstart_baseflow_init();

    // if camera has already connected, save once polling time
    if (!access("/dev/video0", R_OK))
    {
        if (sstar_usbcamera_init())
        {
            printf("usbcamera init error\n");
            return -1;
        }
    }

    // check polling status
    if (uvc_start_check_hotplug())
    {
        sstar_usbcamera_deinit();
        return -1;
    }

    while (1)
    {
        ch = getchar();

        if (ch == 'q' || ch == 'Q')
            break;

        usleep(20000);
    }

    uvc_stop_check_hotplug();
    sstar_usbcamera_deinit();

    // deinit base flow
    sstar_baseflow_deinit();


    return 0;
}
