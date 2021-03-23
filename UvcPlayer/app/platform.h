#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif

#include "mi_common.h"
#include "mi_common_datatype.h"

#include "mi_sys.h"
#include "mi_sys_datatype.h"

#include "mi_disp.h"
#include "mi_disp_datatype.h"

#include "mi_divp.h"
#include "mi_divp_datatype.h"

#include "mi_panel.h"
#include "mi_panel_datatype.h"

#include "mi_vpe.h"
#include "mi_vpe_datatype.h"

#define STCHECKRESULT(result)\
    if (result != MI_SUCCESS)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__);\
        return 1;\
    }\
    else\
    {\
        printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__);\
    }

#define VMAX(a,b)   ((a) > (b) ? (a) : (b))
#define VMIN(a,b)   ((a) < (b) ? (a) : (b))
#define ALIGN_DOWN(val, align)  (((val) / (align)) * (align))
#define ALIGN_UP(val, align)    (((val) + ((align) - 1)) & ~((align) - 1))

#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK  MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE  MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED    MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN  MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE   MAKE_YUYV_VALUE(29,225,107)

#define DBG_COLOR_NONE          "\033[0m"
#define DBG_COLOR_BLACK         "\033[0;30m"
#define DBG_COLOR_BLUE          "\033[0;34m"
#define DBG_COLOR_GREEN         "\033[0;32m"
#define DBG_COLOR_CYAN          "\033[0;36m"
#define DBG_COLOR_RED           "\033[0;31m"
#define DBG_COLOR_YELLOW        "\033[1;33m"
#define DBG_COLOR_WHITE         "\033[1;37m"
    
#define ST_NOP(fmt, args...)
#define ST_DBG(fmt, args...)  ({do{printf(DBG_COLOR_GREEN"[DBG]:%s[%d]: " DBG_COLOR_NONE, __FUNCTION__,__LINE__);printf(fmt, ##args);}while(0);})
#define ST_WARN(fmt, args...) ({do{printf(DBG_COLOR_YELLOW"[WARN]:%s[%d]: " DBG_COLOR_NONE, __FUNCTION__,__LINE__);printf(fmt, ##args);}while(0);})
#define ST_INFO(fmt, args...) ({do{printf("[INFO]:%s[%d]: ", __FUNCTION__,__LINE__);printf(fmt, ##args);}while(0);})
#define ST_ERR(fmt, args...)  ({do{printf(DBG_COLOR_RED"[ERR]:%s[%d]: " DBG_COLOR_NONE, __FUNCTION__,__LINE__);printf(fmt, ##args);}while(0);})

#define DISPLAY_1024_600    1

#include "mi_panel.h"
#include "mi_panel_datatype.h"

#if DISPLAY_1024_600
#define  PANEL_MAX_W        1024
#define  PANEL_MAX_H        600
#else
#define  PANEL_MAX_W        800
#define  PANEL_MAX_H        480
#endif


int  sstar_panel_init(void);
int  sstar_panel_deinit(void);
void sstar_getpanel_wh(int *width, int *height);


int  sstar_sys_init(void);
int  sstar_sys_deinit(void);

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

