#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "platform.h"

int sstar_panel_init(void)
{
    MI_DISP_PubAttr_t stPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_PANEL_IntfType_e eIntfType;
    MI_PANEL_ParamConfig_t pstParamCfg;
    
    //init panel
    eIntfType = E_MI_PNL_INTF_TTL;
    STCHECKRESULT(MI_PANEL_Init(eIntfType));

    STCHECKRESULT(MI_PANEL_GetPanelParam(eIntfType, &pstParamCfg));
    
    //config disp
    memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
    stPubAttr.u32BgColor = YUYV_BLACK;
    stPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    MI_DISP_SetPubAttr(0, &stPubAttr);
    STCHECKRESULT(MI_DISP_Enable(0));

    memset(&stLayerAttr, 0, sizeof(MI_DISP_VideoLayerAttr_t));
    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width  = pstParamCfg.u16Width;
    stLayerAttr.stVidLayerDispWin.u16Height = pstParamCfg.u16Height;
    STCHECKRESULT(MI_DISP_BindVideoLayer(0, 0));
    STCHECKRESULT(MI_DISP_SetVideoLayerAttr(0, &stLayerAttr));
    STCHECKRESULT(MI_DISP_EnableVideoLayer(0));

    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    stInputPortAttr.u16SrcWidth         = PANEL_MAX_W;
    stInputPortAttr.u16SrcHeight        = PANEL_MAX_H;
    stInputPortAttr.stDispWin.u16X      = 0;
    stInputPortAttr.stDispWin.u16Y      = 0;
    stInputPortAttr.stDispWin.u16Width  = PANEL_MAX_W;
    stInputPortAttr.stDispWin.u16Height = PANEL_MAX_H;
    STCHECKRESULT(MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr));
    STCHECKRESULT(MI_DISP_EnableInputPort(0, 0));
    STCHECKRESULT(MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN));

    return 0;
}

int sstar_panel_deinit(void)
{
    STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
    STCHECKRESULT(MI_DISP_DisableVideoLayer(0));
    STCHECKRESULT(MI_DISP_UnBindVideoLayer(0, 0));
    STCHECKRESULT(MI_DISP_Disable(0));
    STCHECKRESULT(MI_PANEL_DeInit());

    return 0;
}

void sstar_getpanel_wh(int *width, int *height)
{
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_GetVideoLayerAttr(0, &stLayerAttr);
    *width  = stLayerAttr.stVidLayerDispWin.u16Width;
    *height = stLayerAttr.stVidLayerDispWin.u16Height;

    printf("sstar_getpanel_wh = [%d %d]\n", *width, *height);
}

int sstar_sys_init(void)
{
    MI_SYS_Version_t stVersion;
    MI_U64 u64Pts = 0;

    MI_SYS_Init();

    memset(&stVersion, 0x0, sizeof(MI_SYS_Version_t));

    MI_SYS_GetVersion(&stVersion);

    MI_SYS_GetCurPts(&u64Pts);

    u64Pts = 0xF1237890F1237890;
    MI_SYS_InitPtsBase(u64Pts);

    u64Pts = 0xE1237890E1237890;
    MI_SYS_SyncPts(u64Pts);

    return MI_SUCCESS;
}

int sstar_sys_deinit(void)
{
    MI_SYS_Exit();
    return MI_SUCCESS;
}


