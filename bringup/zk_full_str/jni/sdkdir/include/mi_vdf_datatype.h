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
#ifndef _MI_VDF_DATATYPE_H_
#define _MI_VDF_DATATYPE_H_

#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"
#include "mi_md.h"
#include "mi_od.h"
#include "mi_vg.h"

#define VDF_VERSION_MAXLEN            64
#define MI_VDF_CHANNEL_MAX            8
#define MI_VDF_MAX_WIDTH              1920
#define MI_VDF_MAX_HEIGHT             1080
#define MI_VDF_MIN_WIDTH              320
#define MI_VDF_MIN_HEIGHT             180


#define MI_VDF_MD_RST_BUF_NUM_MAX     8
#define MI_VDF_OD_RST_BUF_NUM_MAX     16
#define MI_RET_SUCESS                 0

/* VDF Module ErrorCode */
#define MI_ERR_VDF_INVALID_DEVID MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_VDF_INVALID_CHNID MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID)
#define MI_ERR_VDF_ILLEGAL_PARAM MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_VDF_EXIST MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_EXIST)
#define MI_ERR_VDF_UNEXIST MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_UNEXIST)
#define MI_ERR_VDF_NULL_PTR MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_ERR_VDF_NOT_CONFIG MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG)
#define MI_ERR_VDF_NOT_SUPPORT MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_VDF_NOT_PERM MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM)
#define MI_ERR_VDF_NOMEM MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM)
#define MI_ERR_VDF_NOBUF MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOBUF)
#define MI_ERR_VDF_BUF_EMPTY MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY)
#define MI_ERR_VDF_BUF_FULL MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL)
#define MI_ERR_VDF_NOTREADY MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY)
#define MI_ERR_VDF_BADADDR MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BADADDR)
#define MI_ERR_VDF_BUSY MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_ERR_VDF_CHN_NOT_STARTED MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STARTED)
#define MI_ERR_VDF_CHN_NOT_STOPED MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NOT_STOPED)
#define MI_ERR_VDF_NOT_INIT MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_INIT)
#define MI_ERR_VDF_INITED MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INITED)
#define MI_ERR_VDF_NOT_ENABLE MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_ENABLE)
#define MI_ERR_VDF_NOT_DISABLE MI_DEF_ERR(E_MI_MODULE_ID_VDF,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_DISABLE)
#define MI_ERR_VDF_TIMEOUT MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_TIMEOUT)
#define MI_ERR_VDF_DEV_NOT_STARTED MI_DEF_ERR(E_MI_MODULE_ID_VDF,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_DEV_NOT_STARTED)
#define MI_ERR_VDF_DEV_NOT_STOPED MI_DEF_ERR(E_MI_MODULE_ID_VDF,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_DEV_NOT_STOPED)
#define MI_ERR_VDF_CHN_NO_CONTENT MI_DEF_ERR(E_MI_MODULE_ID_VDF,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_CHN_NO_CONTENT)
#define MI_ERR_VDF_NOVASAPCE MI_DEF_ERR(E_MI_MODULE_ID_VDF,E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOVASPACE)
#define MI_ERR_VDF_NOITEM MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOITEM)
#define MI_ERR_VDF_FAILED MI_DEF_ERR(E_MI_MODULE_ID_VDF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_FAILED)

typedef void*                         MD_HANDLE;
typedef void*                         OD_HANDLE;
typedef MI_S32                        MI_VDF_CHANNEL;
typedef MI_S32                        MI_VDF_PORT;
typedef MI_S32                        VDF_RESULT_HANDLE;

typedef struct MI_VDF_Obj_s
{
    MI_U16 u16LtX;      //Horizontal position of window left-top point
    MI_U16 u16LtY;      //Vertical position of window left-top point
    MI_U16 u16RbX;      //Horizontal position of window right-bottom point
    MI_U16 u16RbY;      //Vertical position of window right-bottom point
    MI_U16 u16ImgW;     //the Width of the input image
    MI_U16 u16ImgH;     //the Hight of the input image
} MI_VDF_Obj_t;

typedef enum
{
    E_MI_VDF_COLOR_Y = 1,
    E_MI_VDF_COLOR_MAX
} MI_VDF_Color_e;

typedef struct MI_MD_ResultSize_s
{
    MI_U32 u32RstStatusLen;
    MI_U32 u32RstSadLen;
    MI_U32 u32RstObjLen;
} MI_MD_ResultSize_t;

typedef struct MI_VDF_MdAttr_s
{
    MI_U8  u8Enable;
    MI_U8  u8MdBufCnt;
    MI_U8  u8VDFIntvl;
    MI_U16 u16RstBufSize;
    MI_MD_ResultSize_t stSubResultSize;
    MDCCL_ctrl_t ccl_ctrl;
    // MDOBJ_DATA_t ccobj;
    MI_MD_static_param_t  stMdStaticParamsIn;
    MI_MD_param_t stMdDynamicParamsIn;
} MI_VDF_MdAttr_t;

typedef enum
{
    E_MI_VDF_ODWINDOW_1X1 = 0,
    E_MI_VDF_ODWINDOW_2X2,
    E_MI_VDF_ODWINDOW_3X3,
    E_MI_VDF_ODWINDOW_MAX
} MI_VDF_ODWindow_e;

typedef struct MI_VDF_OdAttr_s
{
    MI_U8 u8Enable;
    MI_U8 u8OdBufCnt;
    MI_U8 u8VDFIntvl;
    MI_U16 u16RstBufSize;
    MI_OD_static_param_t stOdStaticParamsIn;
    MI_OD_param_t stOdDynamicParamsIn;
} MI_VDF_OdAttr_t;

typedef struct MI_VDF_VgAttr_s
{
    MI_U8 u8Enable;
    MI_U8 u8VgBufCnt;
    MI_U8 u8VDFIntvl;
    MI_U16 u16RstBufSize;

    MI_U16 width;
    MI_U16 height;
    MI_U16 stride;

    float object_size_thd;
    uint8_t indoor;
    uint8_t function_state;
    uint16_t line_number;
    MI_VgLine_t line[4];
    MI_VgRegion_t vg_region;

    MI_VgSet_t stVgParamsIn;
} MI_VDF_VgAttr_t;

typedef enum
{
    E_MI_VDF_WORK_MODE_MD = 0,
    E_MI_VDF_WORK_MODE_OD,
    E_MI_VDF_WORK_MODE_VG,
    E_MI_VDF_WORK_MODE_MAX
} MI_VDF_WorkMode_e;

typedef struct MI_VDF_ChnAttr_s
{
    MI_VDF_WorkMode_e enWorkMode;
    union
    {
        MI_VDF_MdAttr_t stMdAttr;
        MI_VDF_OdAttr_t stOdAttr;
        MI_VDF_VgAttr_t stVgAttr;
    };
} MI_VDF_ChnAttr_t;

//======================= VDF Detect result =========================
//——MD -->MI_VDF_MD_RESULT_S
//   |——Vi-Chn0  ->MD_RESULT_HANDLE_LIST
//   |      |——Handle1 -->MD_RESULT_S
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-255
//   |      |
//   |      |——Handle2
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-255
//   |      |
//   |      |——Handle...
//   |
//   |——Vi-Chn1
//   |      |——Handle1 -->MD_RESULT_S
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-255
//   |      |
//   |      |——Handle2
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-255
//   |      |
//   |      |——Handle...
//   |
//   |——Vi-Chnx
//   |

//——OD -->MI_VDF_OD_RESULT_S
//   |——Vi-Chn0
//   |      |——Handle1 -->OD_RESULT_S
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-8
//   |      |
//   |      |——Handle2
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-8
//   |      |
//   |      |——Handle...
//   |
//   |——Vi-Chn1
//   |      |——Handle1 -->OD_RESULT_S
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-8
//   |      |
//   |      |——Handle2
//   |      |        |——sub-window-0
//   |      |        |——sub-window-1
//   |      |        |——...
//   |      |        |__sub-window-8
//   |      |
//   |      |——Handle...
//   |
//   |——Vi-Chnx
//   |

typedef struct MDRST_STATUS_s
{
    MI_U8  *paddr;      // 0-区块未检测运动，255-区块检测到运动
} MDRST_STATUS_t;

typedef struct MI_MD_Result_s
{
    MI_U64 u64Pts;                //The PTS of Image
    MI_U8  u8Enable;              //=1表明该结果值有效
    MI_MD_ResultSize_t stSubResultSize;
    MDRST_STATUS_t* pstMdResultStatus; //The MD result of Status
    MDSAD_DATA_t* pstMdResultSad; //The MD result of SAD
    MDOBJ_DATA_t* pstMdResultObj; //The MD result of Obj
} MI_MD_Result_t;

typedef struct MI_OD_Result_s
{
    MI_U8  u8Enable;
    MI_U8  u8WideDiv;           //The number of divisions of window in horizontal direction
    MI_U8  u8HightDiv;          //The number of divisions of window in vertical direction
    MI_U8  u8DataLen;           //OD detect result readable size
    MI_U64 u64Pts;              //The PTS of Image
    MI_S8  u8RgnAlarm[3][3];    //The OD result of the sub-window
} MI_OD_Result_t;

typedef MI_VgResult_t MI_VG_Result_t;

typedef struct MI_VDF_Result_s
{
    MI_VDF_WorkMode_e enWorkMode;
    VDF_RESULT_HANDLE handle;
    union
    {
        MI_MD_Result_t stMdResult;
        MI_OD_Result_t stOdResult;
        MI_VG_Result_t stVgResult;
    };
} MI_VDF_Result_t;

typedef struct MI_VDF_ChnStat_s
{
    MI_BOOL bStart;
    MI_U8   u32LeftPic;
    MI_U8   u32LeftRst;
} MI_VDF_ChnStat_t;

typedef struct MI_VDF_DebugInfo_s
{
    MI_VDF_WorkMode_e enWorkMode;
    union
    {
        MI_VgDebug_t stVgDebugInfo;
    };
} MI_VDF_DebugInfo_t;

#endif///_MI_VDF_DATATYPE_H_
