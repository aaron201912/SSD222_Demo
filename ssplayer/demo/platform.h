#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "mi_common.h"
#include "mi_common_datatype.h"

#include "mi_sys.h"
#include "mi_sys_datatype.h"

#include "mi_disp.h"
#include "mi_disp_datatype.h"

#include "config.h"

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


#endif

