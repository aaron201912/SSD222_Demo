#ifndef __SENSOR2PANEL__H__
#define __SENSOR2PANEL__H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

//#include "mi_common.h"
//#include "mi_sys_datatype.h"
#include "mi_sys.h"
//#include "mi_panel_datatype.h"
#include "mi_panel.h"
//#include "mi_disp_datatype.h"
#include "mi_disp.h"
//#include "mi_vpe_datatype.h"
#include "mi_vpe.h"
//#include "mi_vif_datatype.h"
#include "mi_vif.h"
//#include "mi_sensor_datatype.h"
#include "mi_sensor.h"
//#include "mi_divp_datatype.h"
#include "mi_divp.h"
//#include <mi_vdisp_datatype.h>
#include <mi_vdisp.h>

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif

typedef enum
{
    ST_DBG_NONE = 0,
    ST_DBG_ERR,
    ST_DBG_WRN,
    ST_DBG_INFO,
    ST_DBG_DEBUG,
    ST_DBG_ALL
}ST_DBG_LEVEL_e;
typedef enum
{
    ST_Sys_Input_VPE = 0,
    ST_Sys_Input_DIVP = 1,

    ST_Sys_Input_BUTT,
} ST_Sys_Input_E;

typedef enum
{
    ST_Sys_Func_RTSP = 0x01,
    ST_Sys_Func_CAPTURE = 0x02,
    ST_Sys_Func_DISP = 0x04,
    ST_Sys_Func_UVC = 0x08,

    ST_Sys_Func_BUTT = 0x0,
} ST_Sys_Func_E;


typedef struct ST_Sys_BindInfo_s
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindParam;
} ST_Sys_BindInfo_T;
typedef struct ST_Sys_Rect_s
{
    MI_U32 u32X;
    MI_U32 u32Y;
    MI_U16 u16PicW;
    MI_U16 u16PicH;
} ST_Rect_T;

typedef struct
{
    MI_U32 u32X;
    MI_U32 u32Y;
} ST_Point_T;
typedef enum
{
    ST_Sensor_Type_IMX291 = 0,
    ST_Sensor_Type_IMX307,
    ST_Sensor_Type_AR0237,
    ST_Sensor_Type_os08a10,
    ST_Sensor_Type_IMX274,
    ST_Sensor_Type_GC1054,
    ST_Sensor_Type_OV2710,
    ST_Sensor_Type_BUTT,
} ST_Sensor_Type_T;

typedef struct
{
    MI_S32 s32UseOnvif;                 // 0: not use, else use
    MI_S32 s32UseVdf;                   // 0: not use, else use
    MI_S32 s32UseAudio;                 // 0: not use, else use
    MI_S32 s32LoadIQ;                   // 0: not load, else load IQ file
    MI_S32 s32UsePanel;                 // 0: not use, else use
    MI_U8 u8SensorNum;
    MI_SYS_PixelFormat_e enPixelFormat;
    MI_S32 s32HDRtype;
    ST_Sensor_Type_T enSensorType;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
    MI_U8 u8SNRChocieRes;
    MI_SYS_WindowRect_t  stVPEPortCrop;
    MI_SYS_WindowSize_t  stVPEPortSize;
} ST_Config_S;
typedef enum
{
    /*for MIPI*/
    SAMPLE_VI_MODE_MIPI_1_1080P_VPE,

    /*for MIPI*/
    SAMPLE_VI_MODE_MIPI_1_1080P_VENC,

    /*for MIPI*/
    SAMPLE_VI_MODE_MIPI_1_1080P_FRAME,

    /*for MIPI*/
    SAMPLE_VI_MODE_MIPI_1_1080P_REALTIME,

    /*for Digital CAM*/
    SAMPLE_VI_MODE_DIGITAL_CAM_1_1080P_REALTIME,

    /*for MIPI HDR*/
    SAMPLE_VI_MODE_MIPI_1_1080P_REALTIME_HDR,
} VIF_WORK_MODE_E;

typedef struct ST_VIF_PortInfo_s
{
    MI_U32 u32RectX;
    MI_U32 u32RectY;
    MI_U32 u32RectWidth;
    MI_U32 u32RectHeight;
    MI_U32 u32DestWidth;
    MI_U32 u32DestHeight;
    MI_U32 u32IsInterlace;
    MI_VIF_FrameRate_e eFrameRate;
    MI_SYS_PixelFormat_e ePixFormat;
} ST_VIF_PortInfo_T;
typedef struct ST_VPE_ChannelInfo_s
{
    MI_U16 u16VpeMaxW;
    MI_U16 u16VpeMaxH;
    MI_U16 u16VpeCropW;
    MI_U16 u16VpeCropH;
    MI_S32 u32X;
    MI_S32 u32Y;
    MI_SYS_PixelFormat_e eFormat;
    MI_VPE_RunningMode_e eRunningMode;
    MI_VPE_HDRType_e eHDRtype;
    MI_VPE_3DNR_Level_e e3DNRLevel;
    MI_BOOL bRotation;
    MI_VPE_SensorChannel_e eBindSensorId;
} ST_VPE_ChannelInfo_T;

typedef struct ST_VPE_PortInfo_s
{
    MI_VPE_CHANNEL DepVpeChannel;
    MI_U16 u16OutputWidth;                         // Width of target image
    MI_U16 u16OutputHeight;                        // Height of target image
    MI_SYS_PixelFormat_e  ePixelFormat;      // Pixel format of target image
    MI_SYS_CompressMode_e eCompressMode;     // Compression mode of the output
} ST_VPE_PortInfo_T;

#define ASCII_COLOR_RED                          "\033[1;31m"
#define ASCII_COLOR_WHITE                        "\033[1;37m"
#define ASCII_COLOR_YELLOW                       "\033[1;33m"
#define ASCII_COLOR_BLUE                         "\033[1;36m"
#define ASCII_COLOR_GREEN                        "\033[1;32m"
#define ASCII_COLOR_END                          "\033[0m"

#define DBG_DEBUG(fmt, args...)     ({do{if(g_eSTDbgLevel>=ST_DBG_DEBUG){printf(ASCII_COLOR_GREEN"[APP INFO]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define DBG_INFO(fmt, args...)      ({do{if(g_eSTDbgLevel>=ST_DBG_INFO){printf(ASCII_COLOR_GREEN"[APP INFO]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define DBG_WRN(fmt, args...)       ({do{if(g_eSTDbgLevel>=ST_DBG_WRN){printf(ASCII_COLOR_YELLOW"[APP WRN ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define DBG_ERR(fmt, args...)       ({do{if(g_eSTDbgLevel>=ST_DBG_ERR){printf(ASCII_COLOR_RED"[APP ERR ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define DBG_EXIT_ERR(fmt, args...)  ({do{printf(ASCII_COLOR_RED"<<<%s[%d] " fmt ASCII_COLOR_END,__FUNCTION__,__LINE__,##args);}while(0);})
#define DBG_ENTER()                 ({do{if(g_bSTFuncTrace){printf(ASCII_COLOR_BLUE">>>%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})
#define DBG_EXIT_OK()               ({do{if(g_bSTFuncTrace){printf(ASCII_COLOR_BLUE"<<<%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})

int SSTAR_DualSensorInit();
void SSTAR_DualSensorDeinit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__SENSOR2PANEL__H__
