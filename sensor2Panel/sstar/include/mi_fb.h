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

#ifndef _MI_FB_H_
#define _MI_FB_H_

#include "mi_fb_datatype.h"
#include "mi_common.h"
#include "mi_sys.h"

#define FB_MAJOR_VERSION 3
#define FB_SUB_VERSION 0
#define MACRO_TO_STR(macro) #macro
#define FB_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_fb_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_fb_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_fb_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_FB_API_VERSION FB_VERSION_STR(FB_MAJOR_VERSION,FB_SUB_VERSION)

#ifdef __cplusplus
extern "C" {
#endif

MI_S32 MI_FB_Open(MI_U8 u8FbId);
MI_S32 MI_FB_Close(MI_U8 u8FbId);
MI_S32 MI_FB_Mmap(MI_U8 u8FbId, MI_U32 u32Size, void **pVirAddr);
MI_S32 MI_FB_PanDisplay(MI_U8 u8FbId, MI_FB_VarScreenInfo_t *pstVarScreenInfo);
MI_S32 MI_FB_GetVarScreenInfo(MI_U8 u8FbId, MI_FB_VarScreenInfo_t *pstVarScreenInfo);
MI_S32 MI_FB_SetVarScreenInfo(MI_U8 u8FbId, MI_FB_VarScreenInfo_t *pstVarScreenInfo);
MI_S32 MI_FB_GetFixScreenInfo(MI_U8 u8FbId, MI_FB_FixScreenInfo_t *pstFixScreenInfo);
MI_S32 MI_FB_Munmap(MI_U8 u8FbId, void *pVirAddr, MI_U32 u32Size);
MI_S32 MI_FB_GetScreenLocation(MI_U8 u8FbId, MI_FB_Rectangle_t *pstWinRect);
MI_S32 MI_FB_SetScreenLocation(MI_U8 u8FbId, MI_FB_Rectangle_t *pstWinRect);
MI_S32 MI_FB_GetDisplayLayerAttr(MI_U8 u8FbId, MI_FB_DisplayLayerAttr_t *pstDisplayLayerAttr);
MI_S32 MI_FB_SetDisplayLayerAttr(MI_U8 u8FbId, MI_FB_DisplayLayerAttr_t *pstDisplayLayerAttr);
MI_S32 MI_FB_GetCursorAttr(MI_U8 u8FbId, MI_FB_CursorAttr_t *pstCursorAttr);
MI_S32 MI_FB_SetCursorAttr(MI_U8 u8FbId, MI_FB_CursorAttr_t *pstCursorAttr);
MI_S32 MI_FB_GetAlpha(MI_U8 u8FbId, MI_FB_GlobalAlpha_t *pstAlpha);
MI_S32 MI_FB_SetAlpha(MI_U8 u8FbId, MI_FB_GlobalAlpha_t *pstAlpha);
MI_S32 MI_FB_GetColorKey(MI_U8 u8FbId, MI_FB_ColorKey_t *pstClrKey);
MI_S32 MI_FB_SetColorKey(MI_U8 u8FbId, MI_FB_ColorKey_t *pstClrKey);
MI_S32 MI_FB_SetCmap(MI_U8 u8FbId, MI_FB_Cmap_t *pstCmap);
MI_S32 MI_FB_GetCmap(MI_U8 u8FbId, MI_FB_Cmap_t *pstCmap);
MI_S32 MI_FB_GetShow(MI_U8 u8FbId, MI_BOOL *pbShown);
MI_S32 MI_FB_SetShow(MI_U8 u8FbId, MI_BOOL bShown);
MI_S32 MI_FB_Init(void);
MI_S32 MI_FB_DeInit(void);
#ifdef __cplusplus
}
#endif

#endif

