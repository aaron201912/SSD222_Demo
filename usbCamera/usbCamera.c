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
#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "mi_disp.h"
#include "mi_disp_datatype.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "jdec.h"
#include "v4l2.h"
#include "libyuv.h"
#include "sstar_dynamic_load.h"


#define VMAX(a,b) ((a) > (b) ? (a) : (b))
#define VMIN(a,b) ((a) < (b) ? (a) : (b))

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

#define ENABLE_DIVP 1

#define YUV420      3/2
#define YUV422      2
#define YUV_TYPE    YUV422

#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0,128,128)

#define TIMEUOT     100

#define TIME_DIFF_PRE_FRAME         33 * 1000

#define DIVP_CAP_MAX_WIDTH          1920
#define DIVP_CAP_MAX_HEIGHT         1080
#define DISP_OUT_MAX_WIDTH          1024
#define DISP_OUT_MAX_HEIGHT         600

#define CAMERA_VIDEO_WIDTH_MJPEG    1280
#define CAMERA_VIDEO_HEIGHT_MJPEG   720

#define SIZE_BUFFER_YUV             CAMERA_VIDEO_WIDTH_MJPEG * CAMERA_VIDEO_HEIGHT_MJPEG * YUV_TYPE

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


static DivpAssembly_t g_stDivpAssembly;
static LibyuvAssembly_t g_stLibyuvAssembly;

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

int display_init(int x, int y, int width, int height)
{
    MI_U16 u16Width, u16Height;
    MI_DISP_PubAttr_t stPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_RotateConfig_t stRotateConfig;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;
    MI_PANEL_IntfType_e eIntfType;
    MI_PANEL_ParamConfig_t pstParamCfg;

    if (width > DISP_OUT_MAX_WIDTH || height > DISP_OUT_MAX_HEIGHT)
    {
        printf("Input W/H = [%u %u] Over Display Size [%u %u]\n", width, height, DISP_OUT_MAX_WIDTH, DISP_OUT_MAX_HEIGHT);
#if (!ENABLE_DIVP)
        return -1;
#endif
    }

#if ENABLE_DIVP
    // load libmi_divp
    memset(&g_stDivpAssembly, 0, sizeof(DivpAssembly_t));
    if (SSTAR_DIVP_OpenLibrary(&g_stDivpAssembly))
	{
		printf("open libmi_divp failed\n");
		return -1;
	}
#endif

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

    stRotateConfig.eRotateMode = E_MI_DISP_ROTATE_NONE;
    MI_DISP_SetVideoLayerRotateMode(0, &stRotateConfig);

    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    u16Width  = VMIN(DISP_OUT_MAX_WIDTH , width);
    u16Height = VMIN(DISP_OUT_MAX_HEIGHT, height);

    stInputPortAttr.u16SrcWidth         = u16Width;
    stInputPortAttr.u16SrcHeight        = u16Height;
    stInputPortAttr.stDispWin.u16X      = x;
    stInputPortAttr.stDispWin.u16Y      = y;
    stInputPortAttr.stDispWin.u16Width  = VMIN((DISP_OUT_MAX_WIDTH - x), u16Width);
    stInputPortAttr.stDispWin.u16Height = VMIN((DISP_OUT_MAX_HEIGHT - y), u16Height);
    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    eIntfType = E_MI_PNL_INTF_TTL;
    MI_PANEL_Init(eIntfType);
    MI_PANEL_GetPanelParam(eIntfType, &pstParamCfg);

#if ENABLE_DIVP
    MI_DIVP_CHN u32ChnId = 0;
    MI_SYS_ChnPort_t stDivpSrcPort;
    MI_SYS_ChnPort_t stDispDstPort;
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stDivpOutputPortAttr;

    memset(&stDivpChnAttr, 0, sizeof(stDivpChnAttr));
    memset(&stDivpOutputPortAttr, 0, sizeof(stDivpOutputPortAttr));

    stDivpChnAttr.bHorMirror            = FALSE;
    stDivpChnAttr.bVerMirror            = FALSE;
    stDivpChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType           = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X       = 0;
    stDivpChnAttr.stCropRect.u16Y       = 0;
    stDivpChnAttr.stCropRect.u16Width   = 0;
    stDivpChnAttr.stCropRect.u16Height  = 0;
    stDivpChnAttr.u32MaxWidth           = DIVP_CAP_MAX_WIDTH;
    stDivpChnAttr.u32MaxHeight          = DIVP_CAP_MAX_HEIGHT;

    stDivpOutputPortAttr.eCompMode      = E_MI_SYS_COMPRESS_MODE_NONE;
    stDivpOutputPortAttr.ePixelFormat   = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stDivpOutputPortAttr.u32Width       = u16Width;
    stDivpOutputPortAttr.u32Height      = u16Height;

    g_stDivpAssembly.pfnDivpCreateChn(u32ChnId, &stDivpChnAttr);
    g_stDivpAssembly.pfnDivpSetChnAttr(u32ChnId, &stDivpChnAttr);
    g_stDivpAssembly.pfnDivpSetOutputPortAttr(u32ChnId, &stDivpOutputPortAttr);
    g_stDivpAssembly.pfnDivpStartChn(u32ChnId);

    memset(&stDivpSrcPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stDispDstPort, 0, sizeof(MI_SYS_ChnPort_t));

    stDivpSrcPort.eModId    = E_MI_MODULE_ID_DIVP;
    stDivpSrcPort.u32DevId  = 0;
    stDivpSrcPort.u32ChnId  = u32ChnId;
    stDivpSrcPort.u32PortId = 0;

    stDispDstPort.eModId    = E_MI_MODULE_ID_DISP;
    stDispDstPort.u32DevId  = 0;
    stDispDstPort.u32ChnId  = 0;
    stDispDstPort.u32PortId = 0;

    MI_SYS_BindChnPort(&stDivpSrcPort, &stDispDstPort, 30, 30);
    MI_SYS_SetChnOutputPortDepth(&stDivpSrcPort, 0, 3);
#endif

    return 0;
}

int display_deinit()
{
#if ENABLE_DIVP
    MI_SYS_ChnPort_t stDivpSrcPort;
    MI_SYS_ChnPort_t stDispDstPort;

    if (!g_stDivpAssembly.pHandle)
    	return -1;

    memset(&stDivpSrcPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stDispDstPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDivpSrcPort.eModId    = E_MI_MODULE_ID_DIVP;
    stDivpSrcPort.u32DevId  = 0;
    stDivpSrcPort.u32ChnId  = 0;
    stDivpSrcPort.u32PortId = 0;

    stDispDstPort.eModId    = E_MI_MODULE_ID_DISP;
    stDispDstPort.u32DevId  = 0;
    stDispDstPort.u32ChnId  = 0;
    stDispDstPort.u32PortId = 0;
    MI_SYS_UnBindChnPort(&stDivpSrcPort, &stDispDstPort);

    g_stDivpAssembly.pfnDivpStopChn(0);
    g_stDivpAssembly.pfnDivpDestroyChn(0);
    g_stDivpAssembly.pfnDivpDeInitDev();

	// unload libmi_divp
	SSTAR_DIVP_CloseLibrary(&g_stDivpAssembly);
#endif

	MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0); 
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);
	MI_DISP_DeInitDev();

    MI_PANEL_DeInit();
    MI_PANEL_DeInitDev();
    MI_SYS_Exit();

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

int send_yuv_to_display(jdecIMAGE *pImage)
{
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_SYS_ChnPort_t stInputChnPort;

    memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
    memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
    memset(&stInputChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

#if ENABLE_DIVP
    stInputChnPort.eModId    = E_MI_MODULE_ID_DIVP;
#else
    stInputChnPort.eModId    = E_MI_MODULE_ID_DISP;
#endif
    stInputChnPort.u32ChnId  = 0;
    stInputChnPort.u32DevId  = 0;
    stInputChnPort.u32PortId = 0;

    stBufConf.u64TargetPts   = 0;
    stBufConf.eBufType       = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.u32Flags       = MI_SYS_MAP_VA;
    stBufConf.stFrameCfg.u16Width       = pImage->width; //CAMERA_VIDEO_WIDTH_MJPEG;
    stBufConf.stFrameCfg.u16Height      = pImage->height;//CAMERA_VIDEO_HEIGHT_MJPEG;
    stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;

    switch(pImage->esubsamp)//divp only support YUYV422&&YUV420SP
    {
        case SAMP_422://divp 只支持YUV422_YUYV      YV16-->420
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
        }
        break;

        case SAMP_420://divp 只支持NV12（YUV420sp）
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        }
        break;

        default:
        {
        	printf("Not Support YUV Yype, Esubsamp [%d]\n", pImage->esubsamp);
            return -1;
        }
        break;
    }

    if(MI_SUCCESS == MI_SYS_ChnInputPortGetBuf(&stInputChnPort, &stBufConf, &stBufInfo, &stBufHandle, 0))
    {
        stBufInfo.stFrameData.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
        stBufInfo.stFrameData.eFieldType    = E_MI_SYS_FIELDTYPE_NONE;
        stBufInfo.stFrameData.eTileMode     = E_MI_SYS_FRAME_TILE_MODE_NONE;
        stBufInfo.bEndOfStream              = 0;

        //ST_INFO("Get Buf Info: Width/Height/Stride = [%d %d %d]\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32Stride[0]);

        //gettimeofday(&time_start, NULL);

        if (0 != jdec_convert_yuv_fotmat(pImage, (unsigned char **)&stBufInfo.stFrameData.pVirAddr[0]))
        {
        	printf("jdec_convert_yuv_fotmat error!\n");
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
    	printf("MI_SYS_ChnInputPortGetBuf Failed!\n");
         return -1;
    }

    return 0;
}

static pthread_t jdec_tid;
static bool b_exit = false;
static DeviceContex_t *ctx = NULL;
static Packet pkt;

static void * jpeg_decoding_thread(void * args)
{
    int ret;
    int timeout = 0;
    int decode_cnt = 0;
    int yuv_size = 0;
    int img_width = 0, img_height = 0;
    jdecIMAGE image0 = {0};

	if (SSTAR_TurboJpeg_OpenLibrary())
	{
		printf("open libmi_ao failed\n");
		return NULL;
	}

    if (0 != _sys_mma_alloc(&image0, SIZE_BUFFER_YUV))
    {
    	printf("_sys_mma_alloc failed\n");
        goto error;
    }

    while (!b_exit)
    {
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

        //save_file((char *)pkt.data, pkt.size, 3);

        //gettimeofday(&time_start, NULL);

        //读取内存数据解码得到yuv图像
        yuv_size = jdec_decode_yuv_from_buf((char *)pkt.data, pkt.size, &image0, TANSFORM_NONE, SAMP_420);
        if(yuv_size < 0)
        {
            printf("Decode done, yuv_size=%d, image w/h=[%d %d], decode_cnt=%d \n", yuv_size, image0.width, image0.height, decode_cnt);
            goto try_again;
        }
        else
        {
            //根据jpg图像大小设置divp/disp
            if (image0.width > 0 && image0.height > 0)
            {
                if (!img_width && !img_height)
                {
                    display_init(0, 0, image0.width, image0.height);
                    img_width  = image0.width;
                    img_height = image0.height;
                }
            }
            else
            {
            	printf("image w/h=[%d %d] parameter invalid!\n", image0.width, image0.height);
                v4l2_read_packet_end(ctx, &pkt);
                goto error;
            }
            //ST_INFO("image w/h=[%d %d], jpeg size=%ld, yuv type = %d\n", image0.width, image0.height, one_jepg_size, image0.esubsamp);
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //ST_WARN("jpeg decoded time = %lld\n", time0);

        //将yuv数据送到disp/divp,或放大或缩小显示
        if(0 != send_yuv_to_display(&image0))
        {
            printf("send_yuv_to_display error\n");
            v4l2_read_packet_end(ctx, &pkt);
            goto error;
        }

        //save_file((char *)pkt.data, pkt.size, 2);

        decode_cnt ++;
        //ST_INFO("had decoded jpeg num = [%d]\n", decode_cnt);

try_again:
        v4l2_read_packet_end(ctx, &pkt);
    }

error:
    _sys_mma_free(&image0, SIZE_BUFFER_YUV);

    if (img_width || img_height)
    {
        display_deinit();
    }

    SSTAR_TurbpJpeg_CloseLibrary();

    return NULL;
}


int sstar_usbcamera_init()
{
    printf("welcome to uvc_player!\n");

    if (SSTAR_LIBYUV_OpenLibrary(&g_stLibyuvAssembly))
    {
    	printf("open libyuv failed\n");
    	return -1;
    }

    v4l2_dev_init(&ctx,  (char*)"/dev/video0");
    v4l2_dev_set_fmt(ctx, V4L2_PIX_FMT_MJPEG, CAMERA_VIDEO_WIDTH_MJPEG, CAMERA_VIDEO_HEIGHT_MJPEG);

    if (0 != v4l2_read_header(ctx))
    {
        printf("Can't find usb camera\n");
        v4l2_dev_deinit(ctx);
        return -1;
    }

    b_exit = false;
    if (0 != pthread_create(&jdec_tid, NULL, jpeg_decoding_thread, NULL))
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
    printf(" onUI_quit !!!\n");
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

	STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
	MI_DISP_DeInitDev();

	SSTAR_LIBYUV_CloseLibrary(&g_stLibyuvAssembly);
}

int main(int argc, char **argv)
{
    char ch = 0;
    printf("usb camere app start\n");

    if (sstar_usbcamera_init())
    {
        printf("usbcamera init error\n");
        return -1;
    }

    while (1)
    {
        ch = getchar();

        if (ch == 'q' || ch == 'Q')
            break;

        usleep(20000);
    }

    sstar_usbcamera_deinit();

    return 0;
}
