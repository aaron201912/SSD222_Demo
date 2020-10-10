/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/


#ifndef _MI_JPD_H_
#define _MI_JPD_H_


#include "mi_jpd_datatype.h"


#define JPD_MAJOR_VERSION 1
#define JPD_SUB_VERSION 0
#define MACRO_TO_STR(macro) #macro
#define JPD_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_jpd_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_jpd_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_jpd_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_JPD_API_VERSION JPD_VERSION_STR(JPD_MAJOR_VERSION,JPD_SUB_VERSION)


#ifdef __cplusplus
extern "C" {
#endif


MI_S32 MI_JPD_InitDev(MI_JPD_InitParam_t *pstInitParam);//for str support
MI_S32 MI_JPD_DeinitDev(void);//for str support

MI_S32 MI_JPD_CreateChn(MI_JPD_CHN JpdChn, MI_JPD_ChnCreatConf_t *pstChnCreatConf);
MI_S32 MI_JPD_DestroyChn(MI_JPD_CHN JpdChn);
MI_S32 MI_JPD_GetChnAttr(MI_JPD_CHN JpdChn, MI_JPD_ChnAttr_t *pstChnAttr);
MI_S32 MI_JPD_StartChn(MI_JPD_CHN JpdChn);
MI_S32 MI_JPD_StopChn(MI_JPD_CHN JpdChn);
MI_S32 MI_JPD_GetChnStatus(MI_JPD_CHN JpdChn, MI_JPD_ChnStatus_t *pstChnStatus);
MI_S32 MI_JPD_ResetChn(MI_JPD_CHN JpdChn);
MI_S32 MI_JPD_GetStreamBuf(MI_JPD_CHN JpdChn, MI_U32 u32RequiredLength, MI_JPD_StreamBuf_t *pstRetStreamBuf, MI_S32 s32MilliSecToWait);
MI_S32 MI_JPD_PutStreamBuf(MI_JPD_CHN JpdChn, MI_JPD_StreamBuf_t *pstRetStreamBuf);
MI_S32 MI_JPD_DropStreamBuf(MI_JPD_CHN JpdChn, MI_JPD_StreamBuf_t *pstRetStreamBuf);
MI_S32 MI_JPD_QueryStreamInfo(MI_JPD_CHN JpdChn, MI_JPD_DirectInputBuf_t *pstInputBuf, MI_JPD_StreamInfo_t *pstStreamInfo);
MI_S32 MI_JPD_DirectBufDecode(MI_JPD_CHN JpdChn, MI_JPD_DirectInputBuf_t *pstInputBuf, MI_JPD_DirectOutputBuf_t *pstOutputBuf);


#ifdef __cplusplus
}
#endif


#endif///_MI_JPD_H_
