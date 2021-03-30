#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Sensor2Disp.h"
#include "facedetect.h"

static SSTar_SensorConfig_t g_stConfig =
{
    .s32UseOnvif = 0,
    .s32UseVdf = 0,
    .s32UseAudio = 0,
    .s32LoadIQ = 0,
//    .s32UsePanel = 0,
    .u8SensorNum = 1,
    .u8FaceDetect = 1,
    .enPixelFormat = E_MI_SYS_PIXEL_FRAME_FORMAT_MAX,
    .s32HDRtype = 0,
    .enSensorType = ST_Sensor_Type_GC1054,
    .enRotation = E_MI_SYS_ROTATE_NONE,
    .en3dNrLevel = E_MI_VPE_3DNR_LEVEL2,
    .u8SNRChocieRes = 0,
    .stVPEPortCrop =
    {
        .u16X = 0,
        .u16Y = 0,
        .u16Width = 0,
        .u16Height = 0,
    },
    .stVPEPortSize = //not use
    {
        .u16Width = 0,
        .u16Height = 0,
    },
};


int SSTAR_FaceDetectInit()
{
	return SSTAR_SensorFlow_Init(&g_stConfig);
}

void SSTAR_FaceDetectDeinit()
{
	SSTAR_SensorFlow_Deinit(&g_stConfig);
}
