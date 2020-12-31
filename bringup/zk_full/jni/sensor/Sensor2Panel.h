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
#include "mi_isp.h"
#include "mi_iqserver.h"
#include "isp_cus3a_if.h"

#include <st_common.h>
#include <st_vif.h>
#include <st_vpe.h>
#include <st_rgn.h>
#include <st_hchdfd.h>

typedef struct
{
    MI_S32 s32UseOnvif;
    MI_S32 s32UseVdf;
    MI_S32 s32UseAudio;
    MI_S32 s32LoadIQ;
    MI_S32 s32UsePanel;
    MI_U8 u8SensorNum;
    MI_U8 u8FaceDetect;
    MI_U8 u8SpiInit;
    MI_SYS_PixelFormat_e enPixelFormat;
    MI_S32 s32HDRtype;
    ST_Sensor_Type_T enSensorType;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
    MI_U8 u8SNRChocieRes;
    MI_SYS_WindowRect_t  stVPEPortCrop;
    MI_SYS_WindowSize_t  stVPEPortSize;
} ST_Config_S;

int ST_Sensor2PanelInit(ST_Config_S* pstConfig);
int ST_Sensor2PanelDeinit(ST_Config_S* pstConfig);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__SENSOR2PANEL__H__
