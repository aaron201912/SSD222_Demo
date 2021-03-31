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
#include "dualsensor.h"


static SSTar_SensorConfig_t g_stConfig = {
	.s32UseOnvif = 0,//not use
	.s32UseVdf = 0,//not use
	.s32UseAudio = 0,//not use
	.s32LoadIQ = 0,//not use
//	.s32UsePanel = 0,//not use
	.u8SensorNum = 2,
	.u8FaceDetect = 0,
	.enPixelFormat = E_MI_SYS_PIXEL_FRAME_FORMAT_MAX,//not use
	.s32HDRtype = 0,
	.enSensorType = ST_Sensor_Type_GC1054,//not use
	.enRotation = E_MI_SYS_ROTATE_NONE,//not use
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

int SSTAR_DualSensorInit()
{
	return SSTAR_SensorFlow_Init(&g_stConfig);
}

void SSTAR_DualSensorDeinit()
{
	SSTAR_SensorFlow_Deinit(&g_stConfig);
}
