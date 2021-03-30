#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include "jdec.h"
#include "v4l2.h"
#include "platform.h"

#define ENABLE_V4L2     1

#define YUV420      3/2
#define YUV422      2
#define YUV_TYPE    YUV422

#define TIMEUOT     100

#define TIME_DIFF_PRE_FRAME         33 * 1000

#define DIVP_CAP_MAX_WIDTH          1920
#define DIVP_CAP_MAX_HEIGHT         1080

#define DISP_OUT_MAX_WIDTH          PANEL_MAX_W
#define DISP_OUT_MAX_HEIGHT         PANEL_MAX_H

#define CAMERA_VIDEO_WIDTH_MJPEG    1280
#define CAMERA_VIDEO_HEIGHT_MJPEG   720

#define SIZE_BUFFER_YUV     CAMERA_VIDEO_WIDTH_MJPEG * CAMERA_VIDEO_HEIGHT_MJPEG * YUV_TYPE

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
                       int height);

void ST_I422ToYUY2(const uint8_t* pu8src_y,
                  int src_stride_y,
                  const uint8_t* pu8src_u,
                  int src_stride_u,
                  const uint8_t* pu8src_v,
                  int src_stride_v,
                  uint8_t* pu8dst_yuy2,
                  int dst_stride_yuy2,
                  int width,
                  int height);

int find_marker_index(char* buf, long buf_size);
int parse_marker(char* buf, long buf_size, int *image_width, int *image_height);

static int  Jpg_Fd;
static bool bIsRotate;
static uint32_t u32DispOutWidth, u32DispOutHeight;

static int _OpenFile(const char *pFilePath)
{
    //printf("FilePath:%s \n",pFilePath);
    int s32Fd = open(pFilePath, O_RDWR);
    if (s32Fd < 0)
    {
        perror("open");

        return -1;
    }

    return s32Fd;
}

static int _CloseFile(int s32Fd)
{
    close(s32Fd);
    return 0;
}

static int _FileWrite(int fd, char *pBuf, int size)
{
    int s32WriteCnt = 0;
    int s32Ret = 0;
    do
    {
        s32Ret = write(fd, pBuf, size - s32WriteCnt);
        if (s32Ret < 0)
        {
            perror("write");
            return -1;
        }
        if (s32Ret == 0)
        {
            break;
        }
        s32WriteCnt += s32Ret;
        pBuf += s32Ret;
    }while(s32WriteCnt < size);

    return s32WriteCnt;
}

static int _FileRead(int fd, char *pBuf, int size)
{
    int s32ReadCnt = 0;
    int s32Ret = 0;
    do
    {
        s32Ret = read(fd, pBuf, size - s32ReadCnt);
        if (s32Ret < 0)
        {
            return -1;
        }
        if (s32Ret == 0)
        {
            break;
        }
        s32ReadCnt += s32Ret;
        pBuf += s32Ret;
    }while(s32ReadCnt < size);

    return s32ReadCnt;
}

int _sys_mma_alloc(jdecIMAGE *pImage, int size)
{
    if (0 != MI_SYS_MMA_Alloc((unsigned char*)"#jdecI0", size, &(pImage->phyAddr)))
    {
        ST_ERR("MI_SYS_MMA_Alloc Failed\n");
        return -1;
    }
    if (0 != MI_SYS_Mmap(pImage->phyAddr, size, (void *)&(pImage->virtAddr), 1))
    {
        MI_SYS_MMA_Free(pImage->phyAddr);
        ST_ERR("MI_SYS_Mmap Failed\n");
        return -1;
    }

    return 0;
}

int _sys_mma_free(jdecIMAGE *pImage, int size)
{
    if (!pImage->virtAddr || pImage->phyAddr)
        return -1;

    if (0 != MI_SYS_Munmap(pImage->virtAddr, size))
    {
        ST_ERR("MI_SYS_Munmap Failed\n");
        return -1;
    }
    if (0 != MI_SYS_MMA_Free(pImage->phyAddr))
    {
        ST_ERR("MI_SYS_MMA_Free Failed\n");
        return -1;
    }

    return 0;
}

char* get_buf_from_file(char* fpath,long* length)
{
    long file_len;
    long actual_read_len;
    char *pbuf = NULL;

#if 0
    long current_pos;
    FILE *tmp_fd;
    tmp_fd = fopen(argv[1],"r");
    current_pos = ftell(tmp_fd);
    fseek(tmp_fd,0,SEEK_END);
    file_len = ftell(tmp_fd);
    fseek(tmp_fd,current_pos,SEEK_SET);
    printf("the file length is %d\n",file_len);
    fclose(tmp_fd);
#endif

    Jpg_Fd = _OpenFile(fpath);
    if(Jpg_Fd == -1)
    {
        ST_ERR("Open file %s failed\n", fpath);
        return NULL;
    }
    file_len = lseek(Jpg_Fd, 0, SEEK_END);
    lseek(Jpg_Fd, 0, SEEK_SET);
    if(pbuf == NULL)
    {
        pbuf =(char*) malloc(file_len);
        if(pbuf == NULL)
        {
            ST_ERR("malloc mjpeg_buf fail \n");
            return NULL;
        }
    }
    actual_read_len = _FileRead(Jpg_Fd, pbuf, file_len);
    *length = actual_read_len;

    ST_INFO("the actually read length is %ld file_len=%ld \n", *length, file_len);

    _CloseFile(Jpg_Fd);

    return pbuf;
}

int display_init(int x, int y, int width, int height)
{
    MI_U16 u16Width, u16Height;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    float ratio = 0.0;

    if (width > u32DispOutWidth || height > u32DispOutHeight)
    {
        ST_WARN("Input W/H = [%u %u] Over Display Size [%u %u]\n", width, height, u32DispOutWidth, u32DispOutHeight);
    }

    u16Width  = ALIGN_DOWN(VMIN(u32DispOutWidth , width ), 32);
    u16Height = ALIGN_DOWN(VMIN(u32DispOutHeight, height), 32);
    ST_WARN("Divp Output W/H = [%u %u]\n", u16Width, u16Height);

    MI_DIVP_CHN u32ChnId = 0, u32SecChnId = 1;
    MI_SYS_ChnPort_t stDivpSrcPort;
    MI_SYS_ChnPort_t stDispDstPort;
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stDivpOutputPortAttr;
    if (bIsRotate)
    {
        //yuv -> divp chn0(scaling down/up) -> divp chn1(rotate) -> disp
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

        MI_DIVP_CreateChn(u32ChnId, &stDivpChnAttr);
        MI_DIVP_SetChnAttr(u32ChnId, &stDivpChnAttr);
        MI_DIVP_SetOutputPortAttr(u32ChnId, &stDivpOutputPortAttr);
        MI_DIVP_StartChn(u32ChnId);

        memset(&stDivpChnAttr, 0, sizeof(stDivpChnAttr));
        memset(&stDivpOutputPortAttr, 0, sizeof(stDivpOutputPortAttr));

        stDivpChnAttr.bHorMirror            = FALSE;
        stDivpChnAttr.bVerMirror            = FALSE;
        stDivpChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
        stDivpChnAttr.eRotateType           = E_MI_SYS_ROTATE_90;//rotate 90/180/270
        stDivpChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
        stDivpChnAttr.stCropRect.u16X       = 0;
        stDivpChnAttr.stCropRect.u16Y       = 0;
        stDivpChnAttr.stCropRect.u16Width   = 0;
        stDivpChnAttr.stCropRect.u16Height  = 0;
        stDivpChnAttr.u32MaxWidth           = DIVP_CAP_MAX_WIDTH;
        stDivpChnAttr.u32MaxHeight          = DIVP_CAP_MAX_HEIGHT;

        stDivpOutputPortAttr.eCompMode      = E_MI_SYS_COMPRESS_MODE_NONE;
        stDivpOutputPortAttr.ePixelFormat   = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stDivpOutputPortAttr.u32Width       = u16Height;
        stDivpOutputPortAttr.u32Height      = u16Width;//attention!!! if set roatet, the width and height is exchange

        MI_DIVP_CreateChn(u32SecChnId, &stDivpChnAttr);
        MI_DIVP_SetChnAttr(u32SecChnId, &stDivpChnAttr);
        MI_DIVP_SetOutputPortAttr(u32SecChnId, &stDivpOutputPortAttr);
        MI_DIVP_StartChn(u32SecChnId);
    }
    else
    {
        //yuv -> divp(scaling down/up) -> disp
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

        MI_DIVP_CreateChn(u32ChnId, &stDivpChnAttr);
        MI_DIVP_SetChnAttr(u32ChnId, &stDivpChnAttr);
        MI_DIVP_SetOutputPortAttr(u32ChnId, &stDivpOutputPortAttr);
        MI_DIVP_StartChn(u32ChnId);
    }

    //Init Disp Module
    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
    stInputPortAttr.stDispWin.u16X          = x;
    stInputPortAttr.stDispWin.u16Y          = y;
    if (bIsRotate)
    {
        stInputPortAttr.u16SrcWidth         = u16Height;
        stInputPortAttr.u16SrcHeight        = u16Width;
        stInputPortAttr.stDispWin.u16Width  = u32DispOutHeight;
        stInputPortAttr.stDispWin.u16Height = u32DispOutWidth;
    }
    else
    {
        stInputPortAttr.u16SrcWidth         = u16Width;
        stInputPortAttr.u16SrcHeight        = u16Height;
        stInputPortAttr.stDispWin.u16Width  = u32DispOutWidth;
        stInputPortAttr.stDispWin.u16Height = u32DispOutHeight;
    }
    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    //Bind Divp0/Divp1/Disp Modules
    MI_SYS_ChnPort_t stChnDivpPort, stChnDivp2Port;
    MI_SYS_ChnPort_t stChnDispPort;

    memset(&stChnDivpPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stChnDivp2Port, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stChnDispPort, 0, sizeof(MI_SYS_ChnPort_t));

    stChnDivpPort.eModId     = E_MI_MODULE_ID_DIVP;
    stChnDivpPort.u32DevId   = 0;
    stChnDivpPort.u32ChnId   = u32ChnId;
    stChnDivpPort.u32PortId  = 0;

    stChnDivp2Port.eModId    = E_MI_MODULE_ID_DIVP;
    stChnDivp2Port.u32DevId  = 0;
    stChnDivp2Port.u32ChnId  = u32SecChnId;
    stChnDivp2Port.u32PortId = 0;

    stChnDispPort.eModId     = E_MI_MODULE_ID_DISP;
    stChnDispPort.u32DevId   = 0;
    stChnDispPort.u32ChnId   = 0;
    stChnDispPort.u32PortId  = 0;

    if (bIsRotate)
    {
        MI_SYS_BindChnPort2(&stChnDivpPort, &stChnDivp2Port, 30, 30, 1, 0);
        MI_SYS_SetChnOutputPortDepth(&stChnDivpPort, 1, 3);
        MI_SYS_BindChnPort(&stChnDivp2Port, &stChnDispPort, 30, 30);
        MI_SYS_SetChnOutputPortDepth(&stChnDivp2Port, 1, 3);
    }
    else
    {
        MI_SYS_BindChnPort(&stChnDivpPort, &stChnDispPort, 30, 30);
        MI_SYS_SetChnOutputPortDepth(&stChnDivpPort, 1, 3);
    }

    return 0;
}

int display_deinit()
{
    //UnBind Vpe/Divp/Disp Modules
    MI_DIVP_CHN u32ChnId = 0, u32SecChnId = 1;
    MI_SYS_ChnPort_t stChnDivpPort;
    MI_SYS_ChnPort_t stChnDivp2Port;
    MI_SYS_ChnPort_t stChnDispPort;

    memset(&stChnDivpPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stChnDivp2Port, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stChnDispPort, 0, sizeof(MI_SYS_ChnPort_t));

    stChnDivpPort.eModId    = E_MI_MODULE_ID_DIVP;
    stChnDivpPort.u32DevId  = 0;
    stChnDivpPort.u32ChnId  = u32ChnId;
    stChnDivpPort.u32PortId = 0;

    stChnDivp2Port.eModId    = E_MI_MODULE_ID_DIVP;
    stChnDivp2Port.u32DevId  = 0;
    stChnDivp2Port.u32ChnId  = u32SecChnId;
    stChnDivp2Port.u32PortId = 0;

    stChnDispPort.eModId    = E_MI_MODULE_ID_DISP;
    stChnDispPort.u32DevId  = 0;
    stChnDispPort.u32ChnId  = 0;
    stChnDispPort.u32PortId = 0;

    if (bIsRotate)
    {
        MI_SYS_UnBindChnPort(&stChnDivp2Port, &stChnDispPort);
        MI_SYS_UnBindChnPort(&stChnDivpPort, &stChnDivp2Port);
        MI_DIVP_StopChn(u32SecChnId);
        MI_DIVP_DestroyChn(u32SecChnId);
        MI_DIVP_StopChn(u32ChnId);
        MI_DIVP_DestroyChn(u32ChnId);
    }
    else
    {
        MI_SYS_UnBindChnPort(&stChnDivpPort, &stChnDispPort);
        MI_DIVP_StopChn(u32ChnId);
        MI_DIVP_DestroyChn(u32ChnId);
    }

return 0;
}

int set_display_size(int x, int y, int width, int height)
{
    int ret;
    static int img_width, img_height;

    if (!width || !height)
    {
        ST_ERR("set size w/h=[%d %d] is invalid\n", width, height);
        return -1;
    }

    if (img_width != width || img_height != height)
    {
        ST_WARN("set_display_size [%d %d]\n", width, height);
        ret = display_init(x, y, width, height);
        img_width  = width;
        img_height = height;
        return ret;
    }

    return 0;
}

void dump_file(char* path,void *pbuf,int size)
{
    int tmp_s32fd;
    tmp_s32fd = open(path, O_RDWR | O_APPEND | O_CREAT);
    if(tmp_s32fd == -1)
    {
        ST_ERR("Open file %s failed\n", path);
        return ;
    }
    _FileWrite(tmp_s32fd, pbuf, size);

    _CloseFile(tmp_s32fd);
}

struct timeval time_start, time_end;
int64_t time0;

int jdec_convert_yuv_fotmat(jdecIMAGE *pImage, unsigned char *pBuf[3])
{
    unsigned char* pu8STSrcU = NULL;
    unsigned char* pu8STSrcV = NULL;
    FILE *jpg_fd = NULL;

    if(pImage->esubsamp == SAMP_422)        //divp 只支持YUV422_YUYV      YV16-->420
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2;
        ST_I422ToYUY2((unsigned char*)pImage->virtAddr, pImage->width,
                       pu8STSrcU, pImage->width / 2,
                       pu8STSrcV, pImage->width / 2,
                       pBuf[0], pImage->width * 2,
                       pImage->width, pImage->height);
        #if 0
        ST_INFO("YUV Type Is SAMP_422, Image Size = %d\n", pImage->width * pImage->height);

        jpg_fd = fopen("/mnt/mjpeg/dump_422.yuv", "w+");
        fwrite(pBuf[0], 1, pImage->width * pImage->height * 2, jpg_fd);
        fclose(jpg_fd);
        #endif

        return 0;
    }
#if 0
    else if(pImage->esubsamp == SAMP_420)   //disp 只支持NV12（YUV420sp）
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2 / 2;
        ST_I420ToNV12((unsigned char*)pImage->virtAddr, pImage->width,
                       pu8STSrcU, pImage->width / 2,
                       pu8STSrcV, pImage->width / 2,
                       pBuf[0], pImage->width,
                       pBuf[1], pImage->width,
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
#endif
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


    stInputChnPort.eModId    = E_MI_MODULE_ID_DIVP;
    stInputChnPort.u32ChnId  = 0;
    stInputChnPort.u32DevId  = 0;
    stInputChnPort.u32PortId = 0;
    MI_SYS_SetChnOutputPortDepth(&stInputChnPort, 1, 3);

    stBufConf.u64TargetPts   = 0;
    stBufConf.eBufType       = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.u32Flags       = MI_SYS_MAP_VA;
    stBufConf.stFrameCfg.u16Width       = pImage->width; //CAMERA_VIDEO_WIDTH_MJPEG;
    stBufConf.stFrameCfg.u16Height      = pImage->height;//CAMERA_VIDEO_HEIGHT_MJPEG;
    stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;

    switch(pImage->esubsamp)//divp only support YUYV422&&YUV420SP
    {
        case SAMP_422://vpe/divp 只支持YUV422_YUYV      YV16-->420
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
        }
        break;
#if 0
        case SAMP_420://disp 只支持NV12(YUV420sp)
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        }
        break;
#endif
        default:
        {
             ST_WARN("Not Support YUV Yype, Esubsamp [%d]\n", pImage->esubsamp);
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

        if (0 != jdec_convert_yuv_fotmat(pImage, &stBufInfo.stFrameData.pVirAddr[0]))
        {
            ST_ERR("jdec_convert_yuv_fotmat error!\n");
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //ST_WARN("yuv fotmat convert time = %lld\n", time0);

        if(MI_SUCCESS != MI_SYS_ChnInputPortPutBuf(stBufHandle, &stBufInfo, 0))
        {
            ST_ERR("MI_SYS_ChnInputPortPutBuf Failed!\n");
            return -1;
        }
    }
    else
    {
         ST_ERR("MI_SYS_ChnInputPortGetBuf Failed!\n");
         return -1;
    }
#if 0
    MI_SYS_BufInfo_t stOutBufInfo;
    MI_SYS_BUF_HANDLE stOutHandle;
    MI_SYS_ChnPort_t stOutChnPort;
    MI_S32 stOutputW, stOutputH, stOutputS;
    struct pollfd pfd = {0};

    pfd.fd = 0;
    pfd.events = POLLIN | POLLERR;

    memset(&stOutBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
    memset(&stOutChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stOutChnPort.eModId    = E_MI_MODULE_ID_VPE;
    stOutChnPort.u32ChnId  = 0;
    stOutChnPort.u32DevId  = 0;
    stOutChnPort.u32PortId = 0;
    MI_SYS_SetChnOutputPortDepth(&stOutChnPort, 3, 3);

    if (MI_SUCCESS != MI_SYS_GetFd(&stOutChnPort, (MI_S32 *)&pfd.fd))
    {
        ST_ERR("get vpe fd failed\n");
        return -1;
    }
    else
    {
        int rval = poll(&pfd, 1, 10);
        if (!rval)
        {
            ST_ERR("get frame time out\n");
        }
        else if (rval < 0)
        {
            ST_ERR("poll return value: %d\n", rval);
        }
        else
        {
            if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stOutChnPort, &stOutBufInfo, &stOutHandle))
            {
                stOutputW = stOutBufInfo.stFrameData.u16Width;
                stOutputH = stOutBufInfo.stFrameData.u16Height;
                stOutputS = stOutBufInfo.stFrameData.u32Stride[0];
                ST_WARN("divp out w/h/s = [%d %d %d]\n", stOutputW, stOutputH, stOutputS);

                FILE *out_fd = fopen("/mnt/uvc/dump_out.yuv", "w+");
                fwrite(stOutBufInfo.stFrameData.pVirAddr[0], 1, stOutputW * stOutputH, out_fd);
                fwrite(stOutBufInfo.stFrameData.pVirAddr[1], 1, stOutputW * stOutputH / 2, out_fd);
                //fwrite(stOutBufInfo.stFrameData.pVirAddr[0], 1, stOutputW * stOutputH * 2, out_fd);
                fclose(out_fd);

                if(MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(stOutHandle))
                {
                    ST_ERR("MI_SYS_ChnOutputPortPutBuf Failed!\n");
                }
            }
        }
        MI_SYS_CloseFd(pfd.fd);
    }
#endif
    return 0;
}

static bool b_exit = false;

static void * jpeg_decoding_thread(void * args)
{
    int ret;
    int decode_cnt = 0;
    int yuv_size = 0;
    long one_jepg_size = 0;
    char *tmp_jpeg_buf = NULL;
    jdecIMAGE image0 = {0};

#if ENABLE_V4L2
    int timeout = 0;
    DeviceContex_t *ctx = (DeviceContex_t *)args;
    Packet pkt;
#else
    char *filename = (char *)args;
    char *mjpeg_buf = NULL;
    long file_len = 0, file_read_len = 0;
    int  image_width = 0, image_height = 0;

    mjpeg_buf = get_buf_from_file(filename, &file_len);
    if(file_len < 0 || mjpeg_buf == NULL)
    {
        ST_ERR("get_buf_from_file failed\n");
        goto error;
    }
    tmp_jpeg_buf  = mjpeg_buf;
    file_read_len = file_len;
#endif

    if (0 != _sys_mma_alloc(&image0, SIZE_BUFFER_YUV))
    {
        ST_ERR("_sys_mma_alloc failed\n");
        goto error;
    }

    while (!b_exit)
    {
#if ENABLE_V4L2
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
                ST_ERR("v4l2_read_packet timeout exit\n");
                b_exit = TRUE;
                goto error;
            }
        }
        timeout = 0;
        tmp_jpeg_buf  = (char *)pkt.data;
        one_jepg_size = pkt.size;
#else
        file_read_len = file_read_len - one_jepg_size;
        if(file_read_len <= 0)
        {
            tmp_jpeg_buf  = mjpeg_buf;
            file_read_len = file_len;
            one_jepg_size = 0;
            //ST_ERR("End of input file, finished!!!\n");
            //goto error;
        }

        tmp_jpeg_buf += one_jepg_size;
        one_jepg_size = parse_marker(tmp_jpeg_buf, file_read_len, &image_width, &image_height);
        if(one_jepg_size < 0)
        {
            ST_ERR("End of input file, finished!!!\n");
            goto error;
        }

        if(image_width * image_height * YUV_TYPE > SIZE_BUFFER_YUV || image_width * image_height <= 0)
        {
            ST_ERR("Invalid size, target size=%d is match SIZE_BUFFER_YUV=%d, image w/h = [%d %d]\n",
            image_width * image_height * YUV_TYPE, SIZE_BUFFER_YUV, image_width, image_height);
            goto error;
        }
#endif
        //save_file(tmp_jpeg_buf, one_jepg_size, 3);

        //gettimeofday(&time_start, NULL);

        //读取内存数据解码得到yuv图像
        yuv_size = jdec_decode_yuv_from_buf(tmp_jpeg_buf, one_jepg_size, &image0, TANSFORM_NONE, SAMP_422);
        if(yuv_size < 0)
        {
            ST_ERR("Decode done, yuv_size=%d, image w/h=[%d %d], decode_cnt=%d \n", yuv_size, image0.width, image0.height, decode_cnt);
#if ENABLE_V4L2
            goto try_again;
#else
            goto error;
#endif
        }
        else
        {
            //根据jpg图像大小设置divp/disp
            ret = set_display_size(0, 0, image0.width, image0.height);
            if (ret < 0)
            {
                ST_ERR("set_display_size error!\n");
                goto error;
            }
            //ST_INFO("image w/h=[%d %d], jpeg size=%ld, yuv type = %d\n", image0.width, image0.height, one_jepg_size, image0.esubsamp);
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //time_start.tv_sec  = time_end.tv_sec;
        //time_start.tv_usec = time_end.tv_usec;
        //ST_WARN("jpeg decoded time = %lld\n", time0);

        //将yuv数据送到disp/divp,或放大或缩小显示
        if(0 != send_yuv_to_display(&image0))
        {
            ST_ERR("send_yuv_to_display error\n");
            goto error;
        }

        decode_cnt ++;

#if ENABLE_V4L2
try_again:
        v4l2_read_packet_end(ctx, &pkt);
#else
        usleep(TIME_DIFF_PRE_FRAME);//播放图片时帧率控制,33ms播放一帧,也就是30帧/s
#endif
        //ST_INFO("had decoded jpeg num = [%d]\n", decode_cnt);
    }

error:
    _sys_mma_free(&image0, SIZE_BUFFER_YUV);

#if ENABLE_V4L2
    if (pkt.buf && pkt.data)
    {
        v4l2_read_packet_end(ctx, &pkt);
    }
#else
    if(mjpeg_buf != NULL)
    {
        free(mjpeg_buf);
        mjpeg_buf = NULL;
    }
#endif

    display_deinit();

    return NULL;
}

int main(int argc, char* argv[])
{
    char c = '\0';
    pthread_t jdec_tid = 0;
    void *args = NULL;

#if (!ENABLE_V4L2)
    if(argc < 5)
    {
        ST_ERR("please input valid parameter,eg: ./UvcPlayer sigmastar.jpg width height rotate\n");
        return 0;
    }
    u32DispOutWidth  = atoi(argv[2]);
    u32DispOutHeight = atoi(argv[3]);
    bIsRotate = atoi(argv[4]);
#else
    if(argc < 4)
    {
        ST_ERR("please input valid parameter,eg: ./UvcPlayer width height rotate\n");
        return 0;
    }
    u32DispOutWidth  = atoi(argv[1]);
    u32DispOutHeight = atoi(argv[2]);
    bIsRotate = atoi(argv[3]);
#endif

    if (u32DispOutWidth < 16 || u32DispOutHeight < 16)
    {
        ST_ERR("invalid width or height [%d %d]\n", u32DispOutWidth, u32DispOutHeight);
        return 0;
    }
    if (bIsRotate)
    {
        u32DispOutWidth  = VMIN(PANEL_MAX_H, u32DispOutWidth);
        u32DispOutHeight = VMIN(PANEL_MAX_W, u32DispOutHeight);
    }
    else
    {
        u32DispOutWidth  = VMIN(PANEL_MAX_W, u32DispOutWidth);
        u32DispOutHeight = VMIN(PANEL_MAX_H, u32DispOutHeight);
    }

    ST_INFO("welcome to mjpeg_player!\n");

    //init sys
    sstar_sys_init();

    //init panel
    sstar_panel_init();

#if ENABLE_V4L2
    //open camera
    DeviceContex_t *ctx = NULL;

    v4l2_dev_init(&ctx,  (char*)"/dev/video0");
    v4l2_dev_set_fmt(ctx, V4L2_PIX_FMT_MJPEG, CAMERA_VIDEO_WIDTH_MJPEG, CAMERA_VIDEO_HEIGHT_MJPEG);

    if (0 != v4l2_read_header(ctx))
    {
        ST_ERR("Can't find usb camera\n");
        goto fail;
    }
    args = (void *)ctx;
#else
    args = (void *)argv[1];
#endif

    if (0 != pthread_create(&jdec_tid, NULL, jpeg_decoding_thread, args))
    {
        ST_ERR("pthread_create failed\n");
        goto fail;
    }

    while (!b_exit)
    {
        c = getchar();
        switch (c)
        {
            case 'q': {
                b_exit = true;
                ST_INFO("receive exit signal!\n");
            }
            break;

            default : break;
        }
    }
fail:

#if ENABLE_V4L2
    //deinit v4l2
    if (ctx)
    {
        v4l2_read_close(ctx);
        v4l2_dev_deinit(ctx);
    }
#endif

    if (jdec_tid)
    {
        pthread_join(jdec_tid, NULL);
    }

    sstar_panel_deinit();

    sstar_sys_deinit();

    ST_INFO("mjpeg_player normal exit!\n");

    return 0;
}

