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


#ifndef _MI_JPD_DATATYPE_H_
#define _MI_JPD_DATATYPE_H_


#include "mi_common.h"
#include "mi_sys.h"

#define MI_JPD_CHN MI_U32
#define MI_JPD_MAX_CHN_NUM (16)

#define MI_DEF_JPD_ERR(err) MI_DEF_ERR(E_MI_MODULE_ID_JPD, E_MI_ERR_LEVEL_ERROR, err)

#define MI_ERR_JPD_INVALID_DEVID           MI_DEF_JPD_ERR(E_MI_ERR_INVALID_DEVID)
#define MI_ERR_JPD_INVALID_CHNID           MI_DEF_JPD_ERR(E_MI_ERR_INVALID_CHNID)
#define MI_ERR_JPD_ILLEGAL_PARAM           MI_DEF_JPD_ERR(E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_JPD_CHN_EXIST               MI_DEF_JPD_ERR(E_MI_ERR_EXIST)
#define MI_ERR_JPD_CHN_UNEXIST             MI_DEF_JPD_ERR(E_MI_ERR_UNEXIST)
#define MI_ERR_JPD_NULL_PTR                MI_DEF_JPD_ERR(E_MI_ERR_NULL_PTR)
#define MI_ERR_JPD_NOT_CONFIG              MI_DEF_JPD_ERR(E_MI_ERR_NOT_CONFIG)
#define MI_ERR_JPD_NOT_SUPPORT             MI_DEF_JPD_ERR(E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_JPD_NOT_PERM                MI_DEF_JPD_ERR(E_MI_ERR_NOT_PERM)
#define MI_ERR_JPD_NOMEM                   MI_DEF_JPD_ERR(E_MI_ERR_NOMEM)
#define MI_ERR_JPD_NOBUF                   MI_DEF_JPD_ERR(E_MI_ERR_NOBUF)
#define MI_ERR_JPD_BUF_EMPTY               MI_DEF_JPD_ERR(E_MI_ERR_BUF_EMPTY)
#define MI_ERR_JPD_BUF_FULL                MI_DEF_JPD_ERR(E_MI_ERR_BUF_FULL)
#define MI_ERR_JPD_SYS_NOTREADY            MI_DEF_JPD_ERR(E_MI_ERR_SYS_NOTREADY)
#define MI_ERR_JPD_BADADDR                 MI_DEF_JPD_ERR(E_MI_ERR_BADADDR)
#define MI_ERR_JPD_BUSY                    MI_DEF_JPD_ERR(E_MI_ERR_BUSY)
#define MI_ERR_JPD_CHN_NOT_START           MI_DEF_JPD_ERR(E_MI_ERR_CHN_NOT_STARTED)
#define MI_ERR_JPD_CHN_NOT_STOP            MI_DEF_JPD_ERR(E_MI_ERR_CHN_NOT_STOPED)
#define MI_ERR_JPD_NOT_INIT                MI_DEF_JPD_ERR(E_MI_ERR_NOT_INIT)
#define MI_ERR_JPD_INITED                  MI_DEF_JPD_ERR(E_MI_ERR_INITED)
#define MI_ERR_JPD_CHN_NO_CONTENT          MI_DEF_JPD_ERR(E_MI_ERR_CHN_NO_CONTENT)
#define MI_ERR_JPD_FAILED                  MI_DEF_JPD_ERR(E_MI_ERR_FAILED)

typedef enum
{
    E_MI_JPD_ERR_CODE_UNKNOW = 0x0,
    E_MI_JPD_ERR_CODE_ILLEGAL_ACCESS,
    E_MI_JPD_ERR_CODE_DEC_TIMEOUT,
    E_MI_JPD_ERR_CODE_OUT_OF_MEMORY,
    E_MI_JPD_ERR_CODE_MAX
} MI_JPD_ErrCode_e;

typedef struct MI_JPD_InitParam_s
{
    MI_U32 u32Reserved;
} MI_JPD_InitParam_t;

typedef struct MI_JPD_ChnCreatConf_s
{
    MI_U32 u32StreamBufSize;
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_U32 u32MaxPicWidth;
    MI_U32 u32MaxPicHeight;
} MI_JPD_ChnCreatConf_t;

typedef struct MI_JPD_StreamInfo_s
{
    MI_U32 u32CurPicWidth;
    MI_U32 u32CurPicHeight;
    MI_SYS_PixelFormat_e ePixelFormat;
} MI_JPD_StreamInfo_t;

typedef struct MI_JPD_ChnAttr_s
{
    MI_U32 u32StreamBufSize;
    MI_JPD_StreamInfo_t stStreamInfo;
} MI_JPD_ChnAttr_t;

typedef struct MI_JPD_ChnStatus_s
{
    MI_U32 u32LeftStreamBytes;
    MI_U32 u32LeftStreamFrames;
    MI_U32 u32LeftPics;
    MI_U32 u32RecvStreamFrames;
    MI_U32 u32DecodeStreamFrames;
    MI_BOOL bChnStart;
    MI_JPD_ErrCode_e eErrCode;
} MI_JPD_ChnStatus_t;

typedef struct MI_JPD_StreamBuf_s
{
    MI_U8 *pu8HeadVirtAddr;//ring buffer first part user mode VA
    MI_PHY u64HeadPhyAddr;//ring buffer first part PA
    MI_U32 u32HeadLength;//ring buffer first part Length
    MI_U8 *pu8TailVirtAddr;//ring buffer 2nd part user mode VA
    MI_PHY u64TailPhyAddr;//ring buffer 2nd part PA
    MI_U32 u32TailLength;//ring buffer 2nd part length
    MI_U32 u32ContentLength;//current used content length
} MI_JPD_StreamBuf_t;

typedef struct MI_JPD_DirectInputBuf_s
{
    MI_U8 *pu8InputJPGRawFrameDataVirtAddr;
    MI_PHY phyAddr;
    MI_U32 u32Length;
} MI_JPD_DirectInputBuf_t;

typedef struct MI_JPD_DirectOutputBuf_s
{
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_U32 u32Width;
    MI_U32 u32Height;
    MI_U32 u32Stride[2];
    MI_PHY phyAddr[2];
} MI_JPD_DirectOutputBuf_t;

#endif///_MI_JPD_DATATYPE_H_
