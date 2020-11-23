#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <linux/usb/ch9.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <video/video.h>
#include <uvc/uvc.h>

#define DEMO_SUPPORT_DEBUG

#define V4L2_PIX_FMT_H265     v4l2_fourcc('H', '2', '6', '5') /* add claude.rao */
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef enum
{
    DEMO_DBG_NONE = 0,
    DEMO_DBG_ERR,
    DEMO_DBG_WRN,
    DEMO_DBG_INFO,
    DEMO_DBG_DEBUG,
    DEMO_DBG_ALL
} DEMO_DBG_LEVEL_e;

#define ASCII_COLOR_RED                          "\033[1;31m"
#define ASCII_COLOR_WHITE                        "\033[1;37m"
#define ASCII_COLOR_YELLOW                       "\033[1;33m"
#define ASCII_COLOR_BLUE                         "\033[1;36m"
#define ASCII_COLOR_GREEN                        "\033[1;32m"
#define ASCII_COLOR_END                          "\033[0m"

extern DEMO_DBG_LEVEL_e demo_debug_level;
extern bool demo_func_trace;

#ifdef DEMO_SUPPORT_DEBUG
#define DEMO_DEBUG(fmt, args...) ({do{if(demo_debug_level>=DEMO_DBG_DEBUG)\
                {printf(ASCII_COLOR_GREEN"[APP INFO]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define DEMO_INFO(fmt, args...)     ({do{if(demo_debug_level>=DEMO_DBG_INFO)\
                {printf(ASCII_COLOR_GREEN"[APP INFO]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define DEMO_WRN(fmt, args...)      ({do{if(demo_debug_level>=DEMO_DBG_WRN)\
                {printf(ASCII_COLOR_YELLOW"[APP WRN ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define DEMO_ERR(fmt, args...)      ({do{if(demo_debug_level>=DEMO_DBG_ERR)\
                {printf(ASCII_COLOR_RED"[APP ERR ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define DEMO_EXIT_ERR(fmt, args...) ({do\
                {printf(ASCII_COLOR_RED"<<<%s[%d] " fmt ASCII_COLOR_END,__FUNCTION__,__LINE__,##args);}while(0);})
#define DEMO_ENTER()                ({do{if(demo_func_trace)\
                {printf(ASCII_COLOR_BLUE">>>%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})
#define DEMO_EXIT_OK()              ({do{if(demo_func_trace)\
                {printf(ASCII_COLOR_BLUE"<<<%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})
#else
#define DEMO_DEBUG(fmt, args...) NULL
#define DEMO_ERR(fmt, args...)   NULL
#define DEMO_INFO(fmt, args...)  NULL
#define DEMO_WRN(fmt, args...)   NULL
#endif

typedef struct DeviceContex_s {
    bool inited;
    char url[24];
    void *priv_data;
} DeviceContex_t;
#endif //_DATATYPE_H_
