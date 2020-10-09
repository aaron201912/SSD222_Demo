#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "mi_sys.h"
#include "sstardisp.h"

#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"
#if ENABLE_HDMI
#include "mi_hdmi.h"
#endif
#define UI_1024_600 1
#define USE_MIPI    0


#if UI_1024_600
//#include "SAT070CP50_1024x600.h"
#include "SAT070JHH_1024x600.h"
#else
#if USE_MIPI
#include "ST7701S_480x480_MIPI.h"
#else
#include "SAT070AT50_800x480.h"
#endif
#endif

// loacl play res
#if UI_1024_600
//#define LOCAL_VIDEO_W  822
//#define LOCAL_VIDEO_H  464
//#define LOCAL_VIDEO_X  100
//#define LOCAL_VIDEO_Y  60
#define LOCAL_VIDEO_W  1024
#define LOCAL_VIDEO_H  600
#define LOCAL_VIDEO_X  0
#define LOCAL_VIDEO_Y  0
#else
//#define LOCAL_VIDEO_W  640
//#define LOCAL_VIDEO_H  360
//#define LOCAL_VIDEO_X  100
//#define LOCAL_VIDEO_Y  60
#define LOCAL_VIDEO_W  800
#define LOCAL_VIDEO_H  480
#define LOCAL_VIDEO_X  0
#define LOCAL_VIDEO_Y  0
#endif

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif
#if ENABLE_HDMI
static MI_S32 Hdmi_callback_impl(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_EventType_e Event, void *pEventParam, void *pUsrParam)
{
    switch (Event)
    {
        case E_MI_HDMI_EVENT_HOTPLUG:
            printf("E_MI_HDMI_EVENT_HOTPLUG.\n");
            break;
        case E_MI_HDMI_EVENT_NO_PLUG:
            printf("E_MI_HDMI_EVENT_NO_PLUG.\n");
            break;
        default:
            printf("Unsupport event.\n");
            break;
    }

    return MI_SUCCESS;
}
#endif
#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0,128,128)

int sstar_disp_init(MI_DISP_PubAttr_t *pstDispPubAttr)
{
    MI_PANEL_LinkType_e eLinkType;
    MI_DISP_InputPortAttr_t stInputPortAttr;
#if ENABLE_HDMI
    MI_HDMI_InitParam_t stInitParam;
    MI_HDMI_Attr_t stAttr;
#endif
    MI_DISP_VideoLayerAttr_t stLayerAttr;

    memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));

    MI_SYS_Init();

    if (pstDispPubAttr->eIntfType == E_MI_DISP_INTF_LCD)
    {
        pstDispPubAttr->stSyncInfo.u16Vact = stPanelParam.u16Height;
        pstDispPubAttr->stSyncInfo.u16Vbb = stPanelParam.u16VSyncBackPorch;
        pstDispPubAttr->stSyncInfo.u16Vfb = stPanelParam.u16VTotal - (stPanelParam.u16VSyncWidth +
                                                                      stPanelParam.u16Height + stPanelParam.u16VSyncBackPorch);
        pstDispPubAttr->stSyncInfo.u16Hact = stPanelParam.u16Width;
        pstDispPubAttr->stSyncInfo.u16Hbb = stPanelParam.u16HSyncBackPorch;
        pstDispPubAttr->stSyncInfo.u16Hfb = stPanelParam.u16HTotal - (stPanelParam.u16HSyncWidth +
                                                                      stPanelParam.u16Width + stPanelParam.u16HSyncBackPorch);
        pstDispPubAttr->stSyncInfo.u16Bvact = 0;
        pstDispPubAttr->stSyncInfo.u16Bvbb = 0;
        pstDispPubAttr->stSyncInfo.u16Bvfb = 0;
        pstDispPubAttr->stSyncInfo.u16Hpw = stPanelParam.u16HSyncWidth;
        pstDispPubAttr->stSyncInfo.u16Vpw = stPanelParam.u16VSyncWidth;
        pstDispPubAttr->stSyncInfo.u32FrameRate = stPanelParam.u16DCLK * 1000000 / (stPanelParam.u16HTotal * stPanelParam.u16VTotal);
        pstDispPubAttr->eIntfSync = E_MI_DISP_OUTPUT_USER;
        pstDispPubAttr->eIntfType = E_MI_DISP_INTF_LCD;
#if USE_MIPI
		eLinkType = E_MI_PNL_LINK_MIPI_DSI;
#else
        eLinkType = E_MI_PNL_LINK_TTL;
#endif
        stInputPortAttr.u16SrcWidth = LOCAL_VIDEO_W;
        stInputPortAttr.u16SrcHeight = LOCAL_VIDEO_H;
        stInputPortAttr.stDispWin.u16X = LOCAL_VIDEO_X;
        stInputPortAttr.stDispWin.u16Y = LOCAL_VIDEO_Y;
        stInputPortAttr.stDispWin.u16Width = LOCAL_VIDEO_W;
        stInputPortAttr.stDispWin.u16Height = LOCAL_VIDEO_H;

        MI_DISP_SetPubAttr(0, pstDispPubAttr);
        MI_DISP_Enable(0);
        MI_DISP_BindVideoLayer(0, 0);
		memset(&stLayerAttr, 0, sizeof(stLayerAttr));
        stLayerAttr.stVidLayerSize.u16Width  = LOCAL_VIDEO_W;
        stLayerAttr.stVidLayerSize.u16Height = LOCAL_VIDEO_H;
        stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_MST_420;
        stLayerAttr.stVidLayerDispWin.u16X      = 0;
        stLayerAttr.stVidLayerDispWin.u16Y      = 0;
        stLayerAttr.stVidLayerDispWin.u16Width  = LOCAL_VIDEO_W;
        stLayerAttr.stVidLayerDispWin.u16Height = LOCAL_VIDEO_H;
        MI_DISP_SetVideoLayerAttr(0, &stLayerAttr);
        MI_DISP_EnableVideoLayer(0);

        MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
        MI_DISP_EnableInputPort(0, 0);
        MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);	
    }
#if ENABLE_HDMI
    else if (E_MI_DISP_INTF_HDMI == pstDispPubAttr->eIntfType)
    {
        stInitParam.pCallBackArgs = NULL;
        stInitParam.pfnHdmiEventCallback = Hdmi_callback_impl;
        MI_HDMI_Init(&stInitParam);
        MI_HDMI_Open(E_MI_HDMI_ID_0);

        memset(&stAttr, 0, sizeof(MI_HDMI_Attr_t));
        stAttr.stEnInfoFrame.bEnableAudInfoFrame  = FALSE;
        stAttr.stEnInfoFrame.bEnableAviInfoFrame  = FALSE;
        stAttr.stEnInfoFrame.bEnableSpdInfoFrame  = FALSE;
        stAttr.stAudioAttr.bEnableAudio = TRUE;
        stAttr.stAudioAttr.bIsMultiChannel = 0;
        stAttr.stAudioAttr.eBitDepth = E_MI_HDMI_BIT_DEPTH_16;
        stAttr.stAudioAttr.eCodeType = E_MI_HDMI_ACODE_PCM;
        stAttr.stAudioAttr.eSampleRate = E_MI_HDMI_AUDIO_SAMPLERATE_48K;
        stAttr.stVideoAttr.bEnableVideo = TRUE;
        stAttr.stVideoAttr.eColorType = E_MI_HDMI_COLOR_TYPE_RGB444;//default color type
        stAttr.stVideoAttr.eDeepColorMode = E_MI_HDMI_DEEP_COLOR_MAX;
        stAttr.stVideoAttr.eTimingType = E_MI_HDMI_TIMING_1080_60P;
        stAttr.stVideoAttr.eOutputMode = E_MI_HDMI_OUTPUT_MODE_HDMI;
        MI_HDMI_SetAttr(E_MI_HDMI_ID_0, &stAttr);
        MI_HDMI_Start(E_MI_HDMI_ID_0);
        pstDispPubAttr->u32BgColor = YUYV_BLACK;
        pstDispPubAttr->eIntfSync = E_MI_DISP_OUTPUT_1080P60;
        MI_DISP_SetPubAttr(0, pstDispPubAttr);
        MI_DISP_Enable(0);
        MI_DISP_BindVideoLayer(0, 0);
        memset(&stLayerAttr, 0, sizeof(stLayerAttr));
        stLayerAttr.stVidLayerSize.u16Width  = 1920;
        stLayerAttr.stVidLayerSize.u16Height = 1080;
        stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_MST_420;
        stLayerAttr.stVidLayerDispWin.u16X      = 0;
        stLayerAttr.stVidLayerDispWin.u16Y      = 0;
        stLayerAttr.stVidLayerDispWin.u16Width  = 1920;
        stLayerAttr.stVidLayerDispWin.u16Height = 1080;
        MI_DISP_SetVideoLayerAttr(0, &stLayerAttr);
        MI_DISP_EnableVideoLayer(0);
    }
#endif
    if (pstDispPubAttr->eIntfType == E_MI_DISP_INTF_LCD)
    {
        MI_PANEL_Init(eLinkType);
        MI_PANEL_SetPanelParam(&stPanelParam);
        if(eLinkType == E_MI_PNL_LINK_MIPI_DSI)
        {
#if USE_MIPI
            MI_PANEL_SetMipiDsiConfig(&stMipiDsiConfig);
#endif
        }
    }
    return 0;
}
int sstar_disp_Deinit(MI_DISP_PubAttr_t *pstDispPubAttr)
{

    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0);
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);

    switch(pstDispPubAttr->eIntfType) {
#if ENABLE_HDMI
        case E_MI_DISP_INTF_HDMI:
            MI_HDMI_Stop(E_MI_HDMI_ID_0);
            MI_HDMI_Close(E_MI_HDMI_ID_0);
            MI_HDMI_DeInit();
            break;
#endif
        case E_MI_DISP_INTF_VGA:
            break;

        case E_MI_DISP_INTF_LCD:
        default:
            MI_PANEL_DeInit();

    }

    MI_SYS_Exit();
    printf("sstar_disp_Deinit...\n");

    return 0;
}

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

