#ifndef __SSTAR_API_H__
#define __SSTAR_API_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "mi_common.h"
#include "mi_sys.h"
#include "mi_disp.h"
#include "mi_divp.h"
#include "mi_ao.h"
#include "mi_gfx.h"

//#include "mi_vdec.h"
//#include "mi_vdec_extra.h"

#define     SUCCESS         0
#define     FAIL            1

#define CheckFuncResult(result)\
    if (result != SUCCESS)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__);\
        return -1;\
    }\

#define  PLAYER_IDLE        0
#define  PLAYER_DONE        1
#define  PLAYER_ERROR       2

int sstar_player_open(const char *fp, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
int sstar_player_close(void);
int sstar_player_pause(void);
int sstar_player_resume(void);
int sstar_player_seek(double time);
int sstar_player_seek2time(double time);
int sstar_player_status(void);
int sstar_player_getduration(double *duration);
int sstar_player_gettime(double *time);
int sstar_player_set_mute(bool mute);
int sstar_player_set_volumn(int volumn);
int sstar_player_setopts(const char *key, const char *value, int flags);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif

