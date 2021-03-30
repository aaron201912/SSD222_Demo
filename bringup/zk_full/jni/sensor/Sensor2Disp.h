#ifndef __SENSOR2PANEL__H__
#define __SENSOR2PANEL__H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "mi_disp.h"
#include "sstar_dynamic_load.h"
#include "isp_cus3a_if.h"

#include <st_rgn.h>
#include <st_hchdfd.h>

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
} SSTAR_SensorType_t;

typedef struct
{
    MI_S32 s32UseOnvif;
    MI_S32 s32UseVdf;
    MI_S32 s32UseAudio;
    MI_S32 s32LoadIQ;
//    MI_S32 s32UsePanel;
    MI_U8 u8SensorNum;
    MI_U8 u8FaceDetect;
    MI_U8 u8SpiInit;
    MI_SYS_PixelFormat_e enPixelFormat;
    MI_S32 s32HDRtype;
    SSTAR_SensorType_t enSensorType;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
    MI_U8 u8SNRChocieRes;
    MI_SYS_WindowRect_t  stVPEPortCrop;
    MI_SYS_WindowSize_t  stVPEPortSize;
} SSTar_SensorConfig_t;

int SSTAR_SensorFlow_Init(SSTar_SensorConfig_t* pstConfig);
int SSTAR_SensorFlow_Deinit(SSTar_SensorConfig_t* pstConfig);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__SENSOR2PANEL__H__
