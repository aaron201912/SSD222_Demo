/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#include "st_vpe.h"

MI_S32 ST_Vpe_CreateChannel(MI_VPE_CHANNEL VpeChannel, ST_VPE_ChannelInfo_T *pstChannelInfo)
{
    MI_VPE_ChannelAttr_t stChannelVpeAttr;
    MI_SYS_WindowRect_t stCropWin;
    MI_VPE_ChannelPara_t stChannelVpeParam;

    memset(&stChannelVpeAttr, 0, sizeof(MI_VPE_ChannelAttr_t));
    memset(&stCropWin, 0, sizeof(MI_SYS_WindowRect_t));
    memset(&stChannelVpeParam, 0x00, sizeof(MI_VPE_ChannelPara_t));

    stChannelVpeParam.eHDRType = pstChannelInfo->eHDRtype;
    stChannelVpeParam.e3DNRLevel = pstChannelInfo->e3DNRLevel;
    MI_VPE_SetChannelParam(VpeChannel, &stChannelVpeParam);

    stChannelVpeAttr.u16MaxW = pstChannelInfo->u16VpeMaxW;
    stChannelVpeAttr.u16MaxH = pstChannelInfo->u16VpeMaxH;
    stChannelVpeAttr.bNrEn= FALSE;
    stChannelVpeAttr.bEdgeEn= FALSE;
    stChannelVpeAttr.bEsEn= FALSE;
    stChannelVpeAttr.bContrastEn= FALSE;
    stChannelVpeAttr.bUvInvert= FALSE;
    stChannelVpeAttr.ePixFmt = pstChannelInfo->eFormat;
    stChannelVpeAttr.eRunningMode = pstChannelInfo->eRunningMode;
    stChannelVpeAttr.bRotation = pstChannelInfo->bRotation;
    stChannelVpeAttr.eHDRType  = pstChannelInfo->eHDRtype;
    stChannelVpeAttr.eSensorBindId = pstChannelInfo->eBindSensorId;
    //DBG_INFO("beal.......mode %d %d  .....\n", E_MI_VPE_RUN_REALTIME_MODE, pstChannelInfo->eRunningMode);
    STCHECKRESULT(MI_VPE_CreateChannel(VpeChannel, &stChannelVpeAttr));

    return MI_SUCCESS;
}
#if 0
MI_VPE_RunningMode_e ST_Vpe_GetRunModeByVIFMode(VIF_WORK_MODE_E enWorkMode)
{
    return E_MI_VPE_RUN_REALTIME_MODE;
}
#endif
MI_S32 ST_Vpe_StartChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_StartChannel (VpeChannel));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StopChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_StopChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_DestroyChannel(MI_VPE_CHANNEL VpeChannel)
{
    STCHECKRESULT(MI_VPE_DestroyChannel(VpeChannel));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StartPort(MI_VPE_PORT VpePort, ST_VPE_PortInfo_T *pstPortInfo)
{
    MI_VPE_PortMode_t stVpeMode;
    MI_SYS_ChnPort_t stChnPort;
    DBG_INFO("ST_Vpe_StartPort ch:%d port %d,w %d h %d\n", pstPortInfo->DepVpeChannel, VpePort,pstPortInfo->u16OutputWidth, pstPortInfo->u16OutputHeight);

    memset(&stVpeMode, 0, sizeof(stVpeMode));
    STCHECKRESULT(MI_VPE_GetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));
    stVpeMode.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    stVpeMode.ePixelFormat = pstPortInfo->ePixelFormat;
    stVpeMode.u16Width = pstPortInfo->u16OutputWidth;
    stVpeMode.u16Height= pstPortInfo->u16OutputHeight;
    STCHECKRESULT(MI_VPE_SetPortMode(pstPortInfo->DepVpeChannel, VpePort, &stVpeMode));

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VPE;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = pstPortInfo->DepVpeChannel;
    stChnPort.u32PortId = VpePort;
    //ExecFunc(MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 5), 0);

    STCHECKRESULT(MI_VPE_EnablePort(pstPortInfo->DepVpeChannel, VpePort));

    return MI_SUCCESS;
}

MI_S32 ST_Vpe_StopPort(MI_VPE_CHANNEL VpeChannel, MI_VPE_PORT VpePort)
{
    STCHECKRESULT(MI_VPE_DisablePort(VpeChannel, VpePort));

    return MI_SUCCESS;
}


