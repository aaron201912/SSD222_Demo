/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _ST_COMMON_H_
#define _ST_COMMON_H_

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mi_sys.h"

#ifndef ExecFunc
#define ExecFunc(_func_, _ret_) \
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != _ret_) \
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)
#endif

#ifndef STCHECKRESULT
#define STCHECKRESULT(_func_)\
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != MI_SUCCESS)\
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__); \
        } \
    } while(0)
#endif


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif

#define MI_U32VALUE(pu8Data, index) (pu8Data[index]<<24)|(pu8Data[index+1]<<16)|(pu8Data[index+2]<<8)|(pu8Data[index+3])

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE              MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED                MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN              MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE               MAKE_YUYV_VALUE(29,225,107)

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

extern ST_DBG_LEVEL_e g_eSTDbgLevel;
extern MI_BOOL g_bSTFuncTrace;

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


MI_S32 ST_Sys_Init(void);
MI_S32 ST_Sys_Exit(void);
MI_S32 ST_Sys_Bind(ST_Sys_BindInfo_T *pstBindInfo);
MI_S32 ST_Sys_UnBind(ST_Sys_BindInfo_T *pstBindInfo);
MI_U64 ST_Sys_GetPts(MI_U32 u32FrameRate);
void ST_Flush(void);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif //_ST_COMMON_H_

