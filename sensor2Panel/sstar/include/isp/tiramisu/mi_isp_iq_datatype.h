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

/*
 * iq_apiset.h
 *
 *  Created on: 2016/8/16
 *      Author: Elsa-cf.Lin
 */

#ifndef _MI_ISP_IQ_DATATYPE_H_
#define _MI_ISP_IQ_DATATYPE_H_

#include "mi_isp_3a_datatype.h"

//put here temporarily, should add mi_isp_cali_datatype.h?

typedef enum __attribute__ ((aligned (1)))
{
    SS_CALI_ITEM_AWB = 0,
    SS_CALI_ITEM_OBC = 1,
    SS_CALI_ITEM_SDC = 2,
    SS_CALI_ITEM_ALSC = 3,
    SS_CALI_ITEM_LSC = 4,
    SS_CALI_ITEM_MAX,
} MI_ISP_CALI_ITEM_e;

/************************************* AF  - Define the structure declaration - START *************************************/

typedef struct MI_ISP_AF_HW_ROI_MODE_TYPE_s
{
    MI_U8  u8Mode; // 0: Normal, 1: Matrix
    MI_U32 u32VerticalBlockNumber;
} MI_ISP_AF_HW_ROI_MODE_TYPE_t;

typedef struct MI_ISP_AF_HW_WIN_TYPE_s
{
    MI_U16 u16Win[4*16];
} MI_ISP_AF_HW_WIN_TYPE_t;

typedef struct MI_ISP_AF_HW_FILTER_ATTR_TYPE_s
{
    MI_U16 u16IIR1_a0;
    MI_U16 u16IIR1_a1;
    MI_U16 u16IIR1_a2;
    MI_U16 u16IIR1_b1;
    MI_U16 u16IIR1_b2;
    MI_U16 u16IIR1_1st_low_clip;
    MI_U16 u16IIR1_1st_high_clip;
    MI_U16 u16IIR1_2nd_low_clip;
    MI_U16 u16IIR1_2nd_high_clip;
    MI_U16 u16IIR2_a0;
    MI_U16 u16IIR2_a1;
    MI_U16 u16IIR2_a2;
    MI_U16 u16IIR2_b1;
    MI_U16 u16IIR2_b2;
    MI_U16 u16IIR2_1st_low_clip;
    MI_U16 u16IIR2_1st_high_clip;
    MI_U16 u16IIR2_2nd_low_clip;
    MI_U16 u16IIR2_2nd_high_clip;

    MI_U16 u16IIR1_e1_en;
    MI_U16 u16IIR1_e1_a0;
    MI_U16 u16IIR1_e1_a1;
    MI_U16 u16IIR1_e1_a2;
    MI_U16 u16IIR1_e1_b1;
    MI_U16 u16IIR1_e1_b2;
    MI_U16 u16IIR1_e2_en;
    MI_U16 u16IIR1_e2_a0;
    MI_U16 u16IIR1_e2_a1;
    MI_U16 u16IIR1_e2_a2;
    MI_U16 u16IIR1_e2_b1;
    MI_U16 u16IIR1_e2_b2;

    MI_U16 u16IIR2_e1_en;
    MI_U16 u16IIR2_e1_a0;
    MI_U16 u16IIR2_e1_a1;
    MI_U16 u16IIR2_e1_a2;
    MI_U16 u16IIR2_e1_b1;
    MI_U16 u16IIR2_e1_b2;
    MI_U16 u16IIR2_e2_en;
    MI_U16 u16IIR2_e2_a0;
    MI_U16 u16IIR2_e2_a1;
    MI_U16 u16IIR2_e2_a2;
    MI_U16 u16IIR2_e2_b1;
    MI_U16 u16IIR2_e2_b2;
} MI_ISP_AF_HW_FILTER_ATTR_TYPE_t;

typedef struct MI_ISP_AF_HW_FILTERSQ_TYPE_s
{
    MI_U8  bSobelYSatEn;
    MI_U16 u16SobelYThd;
    MI_U8  bIIRSquareAccEn;
    MI_U8  bSobelSquareAccEn;
    MI_U16 u16IIR1Thd;
    MI_U16 u16IIR2Thd;
    MI_U16 u16SobelHThd;
    MI_U16 u16SobelVThd;
    MI_U8  u8AFTblX[12];
    MI_U16 u16AFTblY[13];
} MI_ISP_AF_HW_FILTERSQ_TYPE_t;

/************************************* AF  - Define the structure declaration - END   *************************************/

/************************************* IQ  - Define the structure declaration - START *************************************/

typedef struct
{
    MI_U16 u16NumOfPoints;
    MI_S32 s32Y[16];
    MI_S32 s32X[16];
} MI_INTP_LUT_IQ;

#if 1 //API20 struct

#define MI_ISP_AUTO_NUM             (16)
#define ALSC_TBL_MAX                459
#define ENABLE_CCM_COEFF_SUM_INFO   1

typedef enum __attribute__ ((aligned (4)))
{
    API20_AUTO = 0,     //[00]
    //API2.0
    API20_FPN,          //[01]
    API20_CCM,          //[02]
    API20_SAT,          //[03]
    API20_OBC,          //[04]
    API20_OBC_P1,       //[05]
    API20_FCOLOR,       //[06]
    API20_CR,           //[07]
    API20_NR_DeSpike,   //[08]
    API20_SHP,          //[09]
    API20_NR_3D,        //[10]
    API20_BRI,          //[11]
    API20_LIG,          //[12]
    API20_CST,          //[13]
    API20_GMA,          //[14]
    API20_CTG,          //[15]
    API20_NR_NLM,       //[16]
    API20_DEFOG,        //[17]
    API20_DRC,          //[18]
    API20_DPC,          //[19]
    API20_HSV,          //[20]
    API20_WDR_LOC,      //[21]
    API20_RGBIR,        //[22]
    API20_YUVGMA,       //[23]
    API20_HDR16to12,    //[24]
    API20_COLORTRANS,   //[25]
    API20_HDR,          //[26]
    API20_EFFECT,       //[27]
    API20_PFC,          //[28]
    API20_SIIR,         //[29]
    API20_DM,           //[30]
    API20_NR_3D_P1,     //[31]
    API20_WDRCurveAdv,  //[32]
    API20_MXNR,         //[33]
    API20_R2Y,          //[34]
    API20_LSC,          //[35]
    API20_LSC_CTRL,     //[36]
    API20_ALSC,         //[37]
    API20_ALSC_CTRL,    //[38]
    API20_DPC_CLUSTER,  //[39]
    API20_PFC_EX,       //[40]
    API20_HDR_EX,       //[41]
    API20_SHP_EX,       //[42]
    API20_NR_3D_EX,     //[43]
    API20_GNR,          //[44]
    API20_DUMMY,        //[45]
    API20_DUMMY_EX,     //[46]
    API20_WDRCurveFull, //[47]
    API20_ADAPTIVE_GMA, //[48]
    API20_XNR,          //[49]
    API20_TEMP,         //[50]
    API20_TEMP_INFO,    //[51]
    API20_MAX,        //for para reset
} MI_ISP_API_ID_e;

typedef struct MI_ISP_IQ_COLORTOGRAY_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
} MI_ISP_IQ_COLORTOGRAY_TYPE_t;

typedef struct LEVEL_BASE_PARAM_s
{
    MI_U32 u32Lev;                      //0 ~ 100
} LEVEL_BASE_PARAM_t;

typedef struct LIGHTNESS_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} LIGHTNESS_MANUAL_ATTR_t;

typedef struct LIGHTNESS_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} LIGHTNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_LIGHTNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    LIGHTNESS_AUTO_ATTR_t stAuto;
    LIGHTNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_LIGHTNESS_TYPE_t;

typedef struct CONTRAST_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} CONTRAST_MANUAL_ATTR_t;

typedef struct CONTRAST_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} CONTRAST_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_CONTRAST_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    CONTRAST_AUTO_ATTR_t stAuto;
    CONTRAST_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_CONTRAST_TYPE_t;

typedef struct BRIGHTNESS_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} BRIGHTNESS_MANUAL_ATTR_t;

typedef struct BRIGHTNESS_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} BRIGHTNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_BRIGHTNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    BRIGHTNESS_AUTO_ATTR_t stAuto;
    BRIGHTNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_BRIGHTNESS_TYPE_t;


typedef struct RGBGAMMA_PARAM_s
{
    MI_U16 u16LutR[256];                //0 ~ 4095
    MI_U16 u16LutG[256];                //0 ~ 4095
    MI_U16 u16LutB[256];                //0 ~ 4095
} RGBGAMMA_PARAM_t;

typedef struct RGBGAMMA_MANUAL_ATTR_s
{
    RGBGAMMA_PARAM_t stParaAPI;
} RGBGAMMA_MANUAL_ATTR_t;

typedef struct RGBGAMMA_AUTO_ATTR_s
{
    RGBGAMMA_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBGAMMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBGAMMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBGAMMA_AUTO_ATTR_t stAuto;
    RGBGAMMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBGAMMA_TYPE_t;

typedef struct YUVGAMMA_PAMAR_s
{
    MI_U16 u16LutY[256];                //0 ~ 1023
    MI_U16 u16LutU[128];                //0 ~  511
    MI_U16 u16LutV[128];                //0 ~  511
} YUVGAMMA_PAMAR_t;

typedef struct YUVGAMMA_MANUAL_ATTR_s
{
    YUVGAMMA_PAMAR_t stParaAPI;
} YUVGAMMA_MANUAL_ATTR_t;

typedef struct YUVGAMMA_AUTO_ATTR_s
{
    YUVGAMMA_PAMAR_t stParaAPI[MI_ISP_AUTO_NUM];
} YUVGAMMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_YUVGAMMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    YUVGAMMA_AUTO_ATTR_t stAuto;
    YUVGAMMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_YUVGAMMA_TYPE_t;

typedef struct SATURATION_PARAM_s
{
    MI_U8 u8SatAllStr;                  //0 ~ 127 (32 = 1X)
    MI_U8 u8SatByYSFTAdv[5];            //0 ~ 8
    MI_U8 u8SatByYLUTAdv[6];            //0 ~ 128
    MI_U8 u8SatBySSFTAdv[5];            //0 ~ 8
    MI_U8 u8SatBySLUTAdv[6];            //0 ~ 128
    MI_U8 u8SatCoring;
} SATURATION_PARAM_t;

typedef struct SATURATION_MANUAL_ATTR_s
{
    SATURATION_PARAM_t stParaAPI;
} SATURATION_MANUAL_ATTR_t;

typedef struct SATURATION_AUTO_ATTR_s
{
    SATURATION_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} SATURATION_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_SATURATION_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    SATURATION_AUTO_ATTR_t stAuto;
    SATURATION_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_SATURATION_TYPE_t;

typedef struct DEFOG_PARAM_s
{
    MI_U8         u8Strength;
} DEFOG_PARAM_t;

typedef struct DEFOG_MANUAL_ATTR_s
{
    DEFOG_PARAM_t stParaAPI;
} DEFOG_MANUAL_ATTR_t;

typedef struct DEFOG_AUTO_ATTR_s
{
    DEFOG_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} DEFOG_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_DEFOG_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    DEFOG_AUTO_ATTR_t stAuto;
    DEFOG_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DEFOG_TYPE_t;

typedef struct RGBMATRIX_PARAM_s
{
    MI_U16 u16CCTthr;
#if ENABLE_CCM_COEFF_SUM_INFO
    MI_U16 u16CCM[12];                              //0 ~ 8191(1024 = 1X)
#else
    MI_U16 u16CCM[9];                               //0 ~ 8191(1024 = 1X)
#endif
    MI_U8  u8CCMSat;                                //0 ~ 100 0:Unit matrix, 100:User matrix
} RGBMATRIX_PARAM_t;

typedef struct RGBMATRIX_MANUAL_ATTR_s
{
#if ENABLE_CCM_COEFF_SUM_INFO
    MI_U16 u16CCM[12];                              //0 ~ 8191(1024 = 1X)
#else
    MI_U16 u16CCM[9];                               //0 ~ 8191(1024 = 1X)
#endif
    MI_U8  u8CCMSat;                                //0 ~ 100 0:Unit matrix, 100:User matrix
} RGBMATRIX_MANUAL_ATTR_t;

typedef struct RGBMATRIX_AUTO_ATTR_s
{
    MI_ISP_BOOL_e bISOActEn;            //0 ~ 1
    RGBMATRIX_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBMATRIX_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBMATRIX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBMATRIX_AUTO_ATTR_t stAuto;
    RGBMATRIX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBMATRIX_TYPE_t;

typedef struct MI_ISP_IQ_CCM_INFO_TYPE_s
{
    MI_U16 u16CCM[12];                  //0 ~ 8191(1024 = 1X)
    MI_U16 u16CCT;
} MI_ISP_IQ_CCM_INFO_TYPE_t;

typedef struct FALSECOLOR_PARAM_s
{
    MI_ISP_BOOL_e bDbgEn;           //0~1
    MI_U8         u8ColorSpaceSel;  //0~1
    MI_U8         u8Preserve;       //0~7
    MI_U8         u8Strength;       //0~31
} FALSECOLOR_PARAM_t;

typedef struct FALSECOLOR_MANUAL_ATTR_s
{
    FALSECOLOR_PARAM_t stParaAPI;
} FALSECOLOR_MANUAL_ATTR_t;

typedef struct FALSECOLOR_AUTO_ATTR_s
{
    FALSECOLOR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} FALSECOLOR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_FALSECOLOR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    FALSECOLOR_AUTO_ATTR_t stAuto;
    FALSECOLOR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_FALSECOLOR_TYPE_t;

typedef struct NR3D_PARAM_s
{
    MI_U8  u8MdThd;                  //0~255
    MI_U16 u16MdGain;                //0~1023
    MI_U8  u8TfStr;                  //0~64
    MI_U8  u8TfStrEx;                //0~64
    MI_U8  u8MdThdByY[16];           //0~255
    MI_U8  u8MdGainByY[16];          //0~255
    MI_U8  u8M2SLut[16];             //0~31
    MI_U16 u16TfLut[16];             //0~4095
    MI_U8  u8YSfBlendLut[16];        //0~16
    MI_U8  u8YSfBlendOffset;         //0~16
    MI_U8  u8MotHistDelayByDiff[16]; //0~7
    MI_U16 u16DeDotDiffOffset;       //0~4095
    MI_U8  u8DeDotDiffShift;         //0~7
} NR3D_PARAM_t;

typedef struct NR3D_MANUAL_ATTR_s
{
    NR3D_PARAM_t stParaAPI;
} NR3D_MANUAL_ATTR_t;

typedef struct NR3D_AUTO_ATTR_s
{
    NR3D_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NR3D_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NR3D_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NR3D_AUTO_ATTR_t stAuto;
    NR3D_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NR3D_TYPE_t;

typedef struct NR3D_EX_PARAM_s
{
    MI_ISP_BOOL_e  bDbgEn;               //0~1
    MI_U8          u8WinSizeSel;         //0~2
    MI_U8          u8MotHistMapSel;      //0~1
    MI_ISP_BOOL_e  bAREn;                //0~1
    MI_U8          u8ARLumaTh[2];        //0~255
    MI_U8          u8ARMotTh[2];         //0~255
    MI_ISP_BOOL_e  bPREn;                //0~1
    MI_U16         u16PRDiffTh;          //0~4095
    MI_U8          u8PRRatioConf[4];     //0~16
    MI_U8          u8PRContrastTh[2];    //0~15
    MI_U8          u8PRMotTh[2];         //0~255
} NR3D_EX_PARAM_t;

typedef struct NR3D_EX_MANUAL_ATTR_s
{
    NR3D_EX_PARAM_t stParaAPI;
} NR3D_EX_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_NR3D_EX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    NR3D_EX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NR3D_EX_TYPE_t;

typedef struct MI_ISP_IQ_NR3D_P1_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_NR3D_P1_TYPE_t;

typedef struct NRDESPIKE_PARAM_s
{
    MI_U8  u8BlendRatio;                //0 ~ 15
    MI_U8  u8StrengthCenterNeighbor;    //0 ~ 5
    MI_U8  u8StrengthMeanStd;           //0 ~ 5
    MI_U8  u8StrengthCornerCross;       //0 ~ 5
    MI_U8  u8DiffGainMeanStd;           //0 ~ 31
    MI_U8  u8DiffGainCenterNeighbor;    //0 ~ 255
    MI_U8  u8DiffThdCornerCross;        //0 ~ 255
    MI_U16 u16BlendLut[5];              //0 ~ 2047
    MI_U8  u8StrengthByY[15];           //0 ~ 127
} NRDESPIKE_PARAM_t;

typedef struct NRDESPIKE_MANUAL_ATTR_s
{
    NRDESPIKE_PARAM_t stParaAPI;
} NRDESPIKE_MANUAL_ATTR_t;

typedef struct NRDESPIKE_AUTO_ATTR_s
{
    NRDESPIKE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRDESPIKE_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRDESPIKE_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NRDESPIKE_AUTO_ATTR_t stAuto;
    NRDESPIKE_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRDESPIKE_TYPE_t;

typedef struct NRLUMA_PARAM_s
{
    MI_U8  u8Strength;              //0~63
    MI_U8  u8KernelStr;             //0~7
    MI_U8  u8KernelStrByDiff[32];   //0~31
} NRLUMA_PARAM_t;

typedef struct NRLUMA_MANUAL_ATTR_s
{
    NRLUMA_PARAM_t stParaAPI;
} NRLUMA_MANUAL_ATTR_t;

typedef struct NRLUMA_AUTO_ATTR_s
{
    NRLUMA_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRLUMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRLUMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NRLUMA_AUTO_ATTR_t stAuto;
    NRLUMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRLUMA_TYPE_t;

typedef struct NRLUMA_Adv_PARAM_s
{
    MI_U8  u8DbgMode;           //0~3
    MI_U16 u16EdgeTh[16];       //0~8191
    MI_U8  u8SmoDeltaSft;       //0~9
    MI_U16 u16SmoDeltaTh[16];   //0~1023
    MI_U8  u8SmoDeltaLut[16];   //0~63
    MI_U8  u8SmoByMot[16];      //0~64
    MI_U16 u16Strength[2];      //0~256
    MI_U8  u8StrengthByHue[24]; //0~64
    MI_U8  u8StrengthBySat[2];  //0~127
    MI_U8  u8StrengthByMot[16]; //0~64
} NRLUMA_Adv_PARAM_t;

typedef struct NRLUMA_Adv_MANUAL_ATTR_s
{
    NRLUMA_Adv_PARAM_t stParaAPI;
} NRLUMA_Adv_MANUAL_ATTR_t;

typedef struct NRLUMA_Adv_AUTO_ATTR_s
{
    NRLUMA_Adv_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRLUMA_Adv_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRLuma_Adv_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NRLUMA_Adv_AUTO_ATTR_t stAuto;
    NRLUMA_Adv_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRLuma_Adv_TYPE_t;

typedef struct NRChroma_PARAM_s
{
    MI_U8  u8MatchRatio;             //0 ~ 127
    MI_U16 u16UvTh;                  //0 ~ 1023
    MI_U16 u16StrengthByCEdge[6];    //0 ~ 1023
} NRChroma_PARAM_t;

typedef struct NRChroma_MANUAL_ATTR_s
{
    NRChroma_PARAM_t stParaAPI;
} NRChroma_MANUAL_ATTR_t;

typedef struct NRChroma_AUTO_ATTR_s
{
    NRChroma_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRChroma_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRChroma_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;    //M_AUTO ~ (M_MODMAX-1)
    NRChroma_AUTO_ATTR_t stAuto;
    NRChroma_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRChroma_TYPE_t;

typedef struct NRChroma_Adv_PARAM_s
{
    MI_U8  u8StrengthByY[8];         //0 ~ 255
    MI_U8  u8StrengthByYEdge[8];     //0 ~ 63
    MI_U8  u8StrengthByCEdge[8];     //0 ~ 255
    MI_U8  u8MaxStrength;            //0 ~ 255
    MI_U8  u8StrengthByMot[16];      //0 ~ 63
    MI_U8  u8MotionClip;             //0 ~ 255
    MI_U8  u8MotionColorReduce;      //0 ~ 255
    MI_U8  u8MotionColorRecover;     //0 ~ 255
    MI_U8  u8PreStrength;            //0 ~ 128
} NRChroma_Adv_PARAM_t;

typedef struct NRChroma_Adv_MANUAL_ATTR_s
{
    NRChroma_Adv_PARAM_t stParaAPI;
} NRChroma_Adv_MANUAL_ATTR_t;

typedef struct NRChroma_Adv_AUTO_ATTR_s
{
    NRChroma_Adv_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRChroma_Adv_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRChroma_Adv_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;    //M_AUTO ~ (M_MODMAX-1)
    NRChroma_Adv_AUTO_ATTR_t stAuto;
    NRChroma_Adv_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRChroma_Adv_TYPE_t;

typedef struct NRChroma_Pre_PARAM_s
{
    MI_ISP_BOOL_e  bDbgEn;               //0 ~ 1
    MI_U16         u16Strength;          //0 ~ 256
    MI_U8          u8MotionEnhance[2];   //0 ~ 127
    MI_U16         u16MaskGenTh;         //0 ~ 1023
    MI_U16         u16MeanFilterTh[2];   //0 ~ 1023
    MI_U16         u16MedianFilterTh[2]; //0 ~ 1023
    MI_U16         u16BlendTh;           //0 ~ 1023
    MI_U8          u8BlendGap;           //0 ~ 15
} NRChroma_Pre_PARAM_t;

typedef struct NRChroma_Pre_MANUAL_ATTR_s
{
    NRChroma_Pre_PARAM_t stParaAPI;
} NRChroma_Pre_MANUAL_ATTR_t;

typedef struct NRChroma_Pre_AUTO_ATTR_s
{
    NRChroma_Pre_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRChroma_Pre_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRChroma_Pre_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;        //M_AUTO ~ (M_MODMAX-1)
    NRChroma_Pre_AUTO_ATTR_t stAuto;
    NRChroma_Pre_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRChroma_Pre_TYPE_t;

typedef struct SHARPNESS_PARAM_s
{
    MI_U8  u8SharpnessUD[3];         //0 ~ 127
    MI_U8  u8SharpnessD[3];          //0 ~ 127
    MI_U8  u8PreCorUD[3];            //0 ~ 63
    MI_U8  u8PreCorD[3];             //0 ~ 63
    MI_U16 u16MidRatioUDByMot[16];   //0 ~ 256
    MI_U16 u16HighRatioUDByMot[16];  //0 ~ 256
    MI_U16 u16MidRatioDByMot[16];    //0 ~ 256
    MI_U16 u16HighRatioDByMot[16];   //0 ~ 256
    MI_U16 u16HighRatioUDByState[3]; //0 ~ 256
    MI_U16 u16HighRatioDByState[3];  //0 ~ 256
    MI_U8  u8DetailGainUDByMot[16];  //0 ~ 255
    MI_U16 u16DirRatioByState[9];    //0 ~ 256
    MI_U8  u8StdAdjByMot[16];        //0 ~ 63
    MI_U8  u8StdAdjByY[9];           //0 ~ 255
    MI_U8  u8GainByStd[7];           //0 ~ 255
    MI_U8  u8CorByY[9];              //0 ~ 255
    MI_U8  u8CorByMot[16];           //0 ~ 255
    MI_U8  u8OverShootGain;          //0 ~ 255
    MI_U8  u8UnderShootGain;         //0 ~ 255
    MI_U8  u8SclByY[9];              //0 ~ 255
    MI_U16 u16EdgeKillLut[7];        //0 ~ 1023
    MI_U16 u16OverShootLimit;        //0 ~ 1023
    MI_U16 u16UnderShootLimit;       //0 ~ 1023
    MI_U8  u8GainByMot[16];          //0 ~ 255
} SHARPNESS_PARAM_t;

typedef struct SHARPNESS_MANUAL_ATTR_s
{
    SHARPNESS_PARAM_t stParaAPI;
} SHARPNESS_MANUAL_ATTR_t;

typedef struct SHARPNESS_AUTO_ATTR_s
{
    SHARPNESS_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} SHARPNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_SHARPNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    SHARPNESS_AUTO_ATTR_t stAuto;
    SHARPNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_SHARPNESS_TYPE_t;

typedef struct SHARPNESS_EX_PARAM_s
{
    MI_ISP_BOOL_e  bDbgEn;                //0 ~ 1
    MI_U8          u8SmoNRFilterSel;      //0 ~ 1
    MI_U16         u16SmoDiffThByY[8];    //0 ~ 1023
    MI_U8          u8SmoBlendByStd[7];    //0 ~ 16
    MI_U8          u8SmoBlendByMot[16];   //0 ~ 63
    MI_U8          u8SblLut[6];           //0 ~ 127
    MI_U8          u8EdgeLabelSel;        //0 ~ 1
    MI_U8          u8GainByStdSft[6];     //0 ~ 15
    MI_U8          u8EdgeKillLutSft[6];   //0 ~ 15
    MI_U8          u8BoundaryThByY[9];    //0 ~ 255
    MI_U8          u8BoundaryGainByY[9];  //0 ~ 255
    MI_U8          u8BoundaryLut[7];      //0 ~ 255
    MI_U8          u8StrengthByHue[24];   //0 ~ 255
    MI_U8          u8StrengthBySat[2];    //0 ~ 127
} SHARPNESS_EX_PARAM_t;

typedef struct SHARPNESS_EX_MANUAL_ATTR_s
{
    SHARPNESS_EX_PARAM_t stParaAPI;
} SHARPNESS_EX_MANUAL_ATTR_t;

typedef struct SHARPNESS_EX_AUTO_ATTR_s
{
    SHARPNESS_EX_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} SHARPNESS_EX_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_SHARPNESS_EX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;        //M_AUTO ~ (M_MODMAX-1)
    SHARPNESS_EX_AUTO_ATTR_t stAuto;
    SHARPNESS_EX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_SHARPNESS_EX_TYPE_t;

typedef struct CROSSTALK_PARAM_s
{
    MI_U8  u8Strength;         //0 ~ 31
    MI_U8  u8StrengthByY[15];  //0 ~ 127
    MI_U8  u8Threshold;        //0 ~ 255
    MI_U16 u16Offset;          //0 ~ 4095
} CROSSTALK_PARAM_t;

typedef struct CROSSTALK_MANUAL_ATTR_s
{
    CROSSTALK_PARAM_t stParaAPI;
} CROSSTALK_MANUAL_ATTR_t;

typedef struct CROSSTALK_AUTO_ATTR_s
{
    CROSSTALK_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} CROSSTALK_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_CROSSTALK_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    CROSSTALK_AUTO_ATTR_t stAuto;
    CROSSTALK_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_CROSSTALK_TYPE_t;

typedef struct OBC_PARAM_s
{
    MI_U16 u16ValR;                     //0 ~ 255
    MI_U16 u16ValGr;                    //0 ~ 255
    MI_U16 u16ValGb;                    //0 ~ 255
    MI_U16 u16ValB;                     //0 ~ 255
} OBC_PARAM_t;

typedef struct OBC_MANUAL_ATTR_s
{
    OBC_PARAM_t stParaAPI;
} OBC_MANUAL_ATTR_t;

typedef struct OBC_AUTO_ATTR_s
{
    OBC_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} OBC_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_OBC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    OBC_AUTO_ATTR_t stAuto;
    OBC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_OBC_TYPE_t;

typedef struct WDR_PARAM_s
{
    MI_U8         u8BoxNum;                 //1 ~ 4
    MI_ISP_BOOL_e bAutoDetailEnhance;       //0 ~ 1
    MI_U8         u8ManualDetailEnhance;    //0~255; default:128
    MI_U8         u8GlobalDarkToneEnhance;  //0 ~ 15
    MI_U8         u8WDRStrByY[33];          //0 ~ 255
    MI_U8         u8Strength;               //0 ~ 255
    MI_U8         u8DarkLimit;              //0 ~ 255
    MI_U8         u8BrightLimit;            //0 ~ 255
} WDR_PARAM_t;

typedef struct WDR_MANUAL_ATTR_s
{
    WDR_PARAM_t stParaAPI;
} WDR_MANUAL_ATTR_t;

typedef struct WDR_AUTO_ATTR_s
{
    WDR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} WDR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_WDR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    WDR_AUTO_ATTR_t stAuto;
    WDR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_WDR_TYPE_t;


typedef struct MI_ISP_IQ_WDRCurveFull_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_WDRCurveFull_TYPE_t;

typedef struct WDRCurveAdv_PARAM_s
{
    MI_U16 u16StrengthByOpsCnt[8]; //0~4095
    MI_S32 s32GlobalToneParam[8];  //-65536~65536
    MI_U8  u8GlobalToneSft[31];    //3~15
    MI_U16 u16GlobalToneLut[32];   //0~4095
    MI_U16 u16GammaParam[10];      //0~65535
    MI_U16 u16GammaLut[33];        //0~4095
    MI_U16 u16DeGammaParam[10];    //0~65535
    MI_U16 u16DeGammaLut[33];      //0~65535
    MI_U8  u8DeSatSrcSel;          //0~2
    MI_U8  u8DeSatLut[33];         //0~255
} WDRCurveAdv_PARAM_t;

typedef struct WDRCurveAdv_MANUAL_ATTR_s
{
    WDRCurveAdv_PARAM_t stParaAPI;
} WDRCurveAdv_MANUAL_ATTR_t;

typedef struct WDRCurveAdv_AUTO_ATTR_s
{
    WDRCurveAdv_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} WDRCurveAdv_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_WDRCurveAdv_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    WDRCurveAdv_AUTO_ATTR_t stAuto;
    WDRCurveAdv_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_WDRCurveAdv_TYPE_t;

typedef struct DYNAMIC_DP_PARAM_s
{
    MI_ISP_BOOL_e bHotPixEn;            //0 ~ 1
    MI_U16 u16HotPixCompSlpoe;
    MI_ISP_BOOL_e bDarkPixEn;           //0 ~ 1
    MI_U16 u16DarkPixCompSlpoe;
    MI_U16 u16DPCTH;
    MI_ISP_BOOL_e bBlendEn;
    MI_U16 u16DiffLut[6];
    MI_U16 u16YLut[6];
} DYNAMIC_DP_PARAM_t;

typedef struct DYNAMIC_DP_MANUAL_ATTR_s
{
    DYNAMIC_DP_PARAM_t stParaAPI;
} DYNAMIC_DP_MANUAL_ATTR_t;

typedef struct DYNAMIC_DP_AUTO_ATTR_s
{
    DYNAMIC_DP_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} DYNAMIC_DP_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_DYNAMIC_DP_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    DYNAMIC_DP_AUTO_ATTR_t stAuto;
    DYNAMIC_DP_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DYNAMIC_DP_TYPE_t;

typedef struct DYNAMIC_DP_CLUSTER_ATTR_S
{
    MI_ISP_BOOL_e bEdgeMode;
    MI_U16 u16NeiDeltaTh;
    MI_U8  u8NeiSmooth;
    MI_ISP_BOOL_e bSortEn;
    MI_ISP_BOOL_e bSortRGGBEn[4];
    MI_ISP_BOOL_e bSort1x3ModeEn;
    MI_U16 u16SortLumTblL[6];
    MI_U16 u16SortLumTblD[6];
    MI_U16 u16SortLumTblS[6];
} DYNAMIC_DP_CLUSTER_ATTR_S;

typedef struct
{
    DYNAMIC_DP_CLUSTER_ATTR_S stParaAPI;
} DYNAMIC_DP_CLUSTER_MANUAL_ATTR_S;

typedef struct
{
    DYNAMIC_DP_CLUSTER_ATTR_S stParaAPI[MI_ISP_AUTO_NUM];
} DYNAMIC_DP_CLUSTER_AUTO_ATTR_S;

typedef struct MI_ISP_IQ_DYNAMIC_DP_CLUSTER_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;        //M_AUTO ~ (M_MODMAX-1)
    DYNAMIC_DP_CLUSTER_AUTO_ATTR_S stAuto;
    DYNAMIC_DP_CLUSTER_MANUAL_ATTR_S stManual;
} MI_ISP_IQ_DYNAMIC_DP_CLUSTER_TYPE_t;

typedef struct HSV_PARAM_s
{
    MI_S16 s16HueLut[24];               //-64 ~  64
    MI_U16 u16SatLut[24];               //  0 ~ 255 (64->1X)
    MI_S16 s16HueLut_ByIso[24];         //-64 ~  64
    MI_U16 u16SatLut_ByIso[24];         //  0 ~ 255 (64->1X)
    MI_S16 s16HueLut_ForUnitCCM[24];    //-64 ~  64
    MI_U16 u16SatLut_ForUnitCCM[24];    //  0 ~ 255 (64->1X)
    MI_U8  u8GlobalSat;                 //  0 ~ 255 (64->1x)
} HSV_PARAM_t;

typedef struct HSV_MANUAL_ATTR_s
{
    HSV_PARAM_t stParaAPI;
} HSV_MANUAL_ATTR_t;

typedef struct HSV_AUTO_ATTR_s
{
    HSV_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} HSV_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_HSV_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    HSV_AUTO_ATTR_t stAuto;
    HSV_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_HSV_TYPE_t;

typedef struct RGBIR_PARAM_s
{
    MI_U8 u8IrPosType;                  //0 ~ 7, def : 0
    MI_ISP_BOOL_e bRemovelEn;           //0 ~ 1, def : 1
    MI_U16 u16Ratio_R[6];
    MI_U16 u16Ratio_G[6];
    MI_U16 u16Ratio_B[6];
} RGBIR_PARAM_t;

typedef struct RGBIR_MANUAL_ATTR_s
{
    RGBIR_PARAM_t stParaAPI;
} RGBIR_MANUAL_ATTR_t;

typedef struct RGBIR_AUTO_ATTR_s
{
    RGBIR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBIR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBIR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBIR_AUTO_ATTR_t stAuto;
    RGBIR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBIR_TYPE_t;

typedef struct MI_ISP_IQ_FPN_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_FPN_TYPE_t;

typedef struct PFC_PARAM_s
{
    MI_U8  u8Strength;              //0 ~ 255
    MI_U8  u8UStrength;             //0 ~ 63
    MI_U8  u8VStrength;             //0 ~ 63
    MI_U8  u8StrengthByY[8];        //0 ~ 255
    MI_U8  u8FlatProtect;           //0 ~ 127
    MI_U8  u8SatByY[2];             //0 ~ 25
    MI_U8  u8SatSrcSel;             //0 ~ 1
    MI_U8  u8StrengthByContrast[8]; //0 ~ 127
} PFC_PARAM_t;

typedef struct PFC_MANUAL_ATTR_s
{
    PFC_PARAM_t stParaAPI;
} PFC_MANUAL_ATTR_t;

typedef struct PFC_AUTO_ATTR_s
{
    PFC_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} PFC_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_PFC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    PFC_AUTO_ATTR_t stAuto;
    PFC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_PFC_TYPE_t;

typedef struct PFC_EX_PARAM_s
{
    MI_ISP_BOOL_e  bDbgEn;                //0 ~ 1
    MI_ISP_BOOL_e  bGSkipEn;              //0 ~ 1
    MI_U8          u8SatMaskSel;          //0 ~ 1
    MI_ISP_BOOL_e  bSatAmpEn;             //0 ~ 1
    MI_U8          u8StrengthByHue[12];   //0 ~ 255
} PFC_EX_PARAM_t;

typedef struct PFC_EX_MANUAL_ATTR_s
{
    PFC_EX_PARAM_t stParaAPI;
} PFC_EX_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_PFC_EX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    PFC_EX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_PFC_EX_TYPE_t;

typedef struct DEMOSAIC_PARAM_s
{
    MI_U8         u8DbgMode;            //0~2
    MI_U8         u8IsoTh;              //0~127
    MI_U8         u8GradientTh;         //32~63
    MI_ISP_BOOL_e bSmoEn;               //0~1
    MI_U8         u8SmoRange;           //0~7
    MI_U8         u8SmoStrength;        //0~255
    MI_ISP_BOOL_e bDtlBypassColorCheck; //0~1
    MI_U8         u8DtlColorRatio;      //0~255
    MI_U8         u8DtlColorOft;        //0~63
    MI_U8         u8DtlHScale;          //0~15
    MI_U8         u8DtlMScale;          //0~15
} DEMOSAIC_PARAM_t;

typedef struct DEMOSAIC_MANUAL_ATTR_s
{
    DEMOSAIC_PARAM_t stParaAPI;
} DEMOSAIC_MANUAL_ATTR_t;

typedef struct DEMOSAIC_AUTO_ATTR_s
{
    DEMOSAIC_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} DEMOSAIC_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_DEMOSAIC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;        //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    DEMOSAIC_AUTO_ATTR_t stAuto;
    DEMOSAIC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DEMOSAIC_TYPE_t;

typedef struct COLORTRANS_PARAM_s
{
    MI_U16 u16Y_OFST;                   //0 ~ 2047
    MI_U16 u16U_OFST;                   //0 ~ 2047
    MI_U16 u16V_OFST;                   //0 ~ 2047
    MI_U16 u16Matrix[9];                //0 ~ 1023
} COLORTRANS_PARAM_t;

typedef struct COLORTRANS_MANUAL_ATTR_s
{
    COLORTRANS_PARAM_t stParaAPI;
} COLORTRANS_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_COLORTRANS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    COLORTRANS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_COLORTRANS_TYPE_t;

typedef struct HDR_PARAM_s
{
    MI_ISP_BOOL_e  bNrEn;
    MI_U16         u16YwtTh1;
    MI_U16         u16YwtTh2;
    MI_U16         u16NoiseLevel[16];
    MI_U16         u16MotionTh;
    MI_U16         u16MotionAdjLut[7];
} HDR_PARAM_t;

typedef struct HDR_MANUAL_ATTR_s
{
    HDR_PARAM_t stParaAPI;
} HDR_MANUAL_ATTR_t;

typedef struct HDR_AUTO_ATTR_s
{
    HDR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} HDR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_HDR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    HDR_AUTO_ATTR_t stAuto;
    HDR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_HDR_TYPE_t;

typedef struct HDR_EX_PARAM_s
{
    MI_ISP_BOOL_e  bDbgEn;
    MI_U16         u16SensorExpRatio;
    MI_ISP_BOOL_e  bDynRatioEn;
    MI_U8          u8DynRatioSrcSel;
    MI_U8          u8YSrcSel;
    MI_U8          u8NoiseLevelSrcSel;
    MI_U8          u8MotionMaxVal;
    MI_U16         u16MotionLumaTh;
    MI_U8          u8MotionAdjSft[6];
} HDR_EX_PARAM_t;

typedef struct HDR_EX_MANUAL_ATTR_s
{
    HDR_EX_PARAM_t stParaAPI;
} HDR_EX_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_HDR_EX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    HDR_EX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_HDR_EX_TYPE_t;

typedef struct MI_ISP_IQ_EFFECT_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_EFFECT_TYPE_t;

typedef struct MI_ISP_IQ_SYS_MCNR_MEMORY_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_SYS_MCNR_MEMORY_TYPE_t;

typedef struct MI_ISP_IQ_LSC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_U16 u16CenterX;
    MI_U16 u16CenterY;
    MI_U16 u16RateX;
    MI_U16 u16RateY;
    MI_U8  u8Shift;
    MI_U16 u16RGainTable[32*3];
    MI_U16 u16GGainTable[32*3];
    MI_U16 u16BGainTable[32*3];
} MI_ISP_IQ_LSC_TYPE_t;

typedef struct MI_ISP_IQ_LSC_CTRL_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_U8 u8RRatioByCct[3];
    MI_U8 u8GRatioByCct[3];
    MI_U8 u8BRatioByCct[3];
    MI_INTP_LUT_IQ RatioByBv;
} MI_ISP_IQ_LSC_CTRL_TYPE_t;

typedef struct MI_ISP_IQ_ALSC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_U8          u8GridX;
    MI_U8          u8GridY;
    MI_U16         u16CCTthr[3];
    MI_U8          u8DeltaMode;
    MI_ISP_BOOL_e  bDeltaModeAdvEn;
    MI_U8          u8DeltaLutX[26];
    MI_U8          u8DeltaLutY[26];
    MI_U16 u16TableR[3][ALSC_TBL_MAX];
    MI_U16 u16TableG[3][ALSC_TBL_MAX];
    MI_U16 u16TableB[3][ALSC_TBL_MAX];
} MI_ISP_IQ_ALSC_TYPE_t;

typedef struct MI_ISP_IQ_ALSC_CTRL_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_U8 u8RRatioByCct[3];
    MI_U8 u8GRatioByCct[3];
    MI_U8 u8BRatioByCct[3];
    MI_INTP_LUT_IQ RatioByBv;
} MI_ISP_IQ_ALSC_CTRL_TYPE_t;

typedef struct R2Y_PARAM_s
{
    MI_U16 u16Matrix[9];
    MI_U8  u8AddY16;
} R2Y_PARAM_t;

typedef struct R2Y_MANUAL_ATTR_s
{
    R2Y_PARAM_t stParaAPI;
} R2Y_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_R2Y_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    R2Y_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_R2Y_TYPE_t;

typedef struct DUMMY_ATTR_s
{
    MI_S16 s16Dummy0[256];
    MI_S16 s16Dummy1[256];
    MI_S16 s16Dummy2[256];
    MI_S16 s16Dummy3;
    MI_S16 s16Dummy4;
} DUMMY_ATTR_t;

typedef struct DUMMY_MANUAL_ATTR_s
{
    DUMMY_ATTR_t stParaAPI;
} DUMMY_MANUAL_ATTR_t;

typedef struct DUMMY_AUTO_ATTR_s
{
    DUMMY_ATTR_t stParaAPI[MI_ISP_AUTO_NUM];
} DUMMY_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_DUMMY_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    MI_ISP_BOOL_e bLogEn;
    DUMMY_AUTO_ATTR_t stAuto;
    DUMMY_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DUMMY_TYPE_t;

typedef struct DUMMY_EX_ATTR_s
{
    MI_S16 s16DummyEx0[256];
    MI_S16 s16DummyEx1[256];
    MI_S16 s16DummyEx2[256];
    MI_S16 s16DummyEx3;
    MI_S16 s16DummyEx4;
} DUMMY_EX_ATTR_t;

typedef struct
{
    DUMMY_EX_ATTR_t stParaAPI;
} DUMMY_EX_MANUAL_ATTR_S;

typedef struct MI_ISP_IQ_DUMMY_EX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_BOOL_e bLogEn;
    DUMMY_EX_MANUAL_ATTR_S stManual;
} MI_ISP_IQ_DUMMY_EX_TYPE_t;

#endif

#if 1 // ISP IQ API
typedef struct IQ_VERSION_INFO_PARAM_s
{
    MI_U32 u32Vendor;
    MI_U32 u32Major;
    MI_U32 u32Minor;
} IQ_VERSION_INFO_PARAM_t;

typedef struct MI_ISP_IQ_VERSION_INFO_TYPE_s
{
    IQ_VERSION_INFO_PARAM_t stParaAPI;
} MI_ISP_IQ_VERSION_INFO_TYPE_t;

typedef struct IQ_PARAM_INIT_INFO_PARAM_s
{
    MI_ISP_BOOL_e bFlag;
} IQ_PARAM_INIT_INFO_PARAM_t;

typedef struct MI_ISP_IQ_PARAM_INIT_INFO_TYPE_s
{
    IQ_PARAM_INIT_INFO_PARAM_t stParaAPI;
} MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t;

typedef struct FAST_MODE_PARAM_s
{
    MI_ISP_BOOL_e bFastMode;
} FAST_MODE_PARAM_t;

typedef struct MI_ISP_IQ_FAST_MODE_TYPE_s
{
    FAST_MODE_PARAM_t stParaAPI;
} MI_ISP_IQ_FAST_MODE_TYPE_t;

typedef enum __attribute__ ((aligned (4)))
{
    SS_IQ_PARAM_MODE_NORMAL = 0,
    SS_IQ_PARAM_MODE_NIGHT = 1,      //Night Mode
    SS_IQ_PARAM_MODE_MAX
} MI_ISP_IQ_PARAM_MODE_e;

typedef struct MI_ISP_IQ_GAMMA_LUT_TYPE_s
{
    MI_U16 u16LutR[256];
    MI_U16 u16LutG[256];
    MI_U16 u16LutB[256];
} MI_ISP_IQ_GAMMA_LUT_TYPE_t;

typedef struct MI_ISP_IQ_ADAPTIVE_GAMMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;
    MI_U16 u16BTGamma[256];
    MI_U16 u16DTGamma[256];
} MI_ISP_IQ_ADAPTIVE_GAMMA_TYPE_t;

typedef struct MI_ISP_IQ_OBC4Ch_TYPE_s
{
    MI_U16 u16OBC4Ch[4];
} MI_ISP_IQ_OBC4Ch_TYPE_t;
#endif

typedef struct TEMP_PARAM_s
{
    MI_S16 s16ObcOffset;
    MI_U8  u8DynamicDPRatio;
    MI_U8  u8CrosstalkRatio;
    MI_U8  u8NRDeSpikeRatio;
    MI_U8  u8NR3DRatio[2];
    MI_U8  u8NRLumaRatio;
    MI_U8  u8SharpnessRatio[2];
    MI_U8  u8SaturationRatio;
    MI_U8  u8ColorToneRatio[3];
} TEMP_PARAM_t;

typedef struct TEMP_MANUAL_ATTR_s
{
    TEMP_PARAM_t stParaAPI;
} TEMP_MANUAL_ATTR_t;

typedef struct TEMP_AUTO_ATTR_s
{
    TEMP_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} TEMP_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_TEMP_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_U8 u8TemperatureLut[16];
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    TEMP_AUTO_ATTR_t stAuto;
    TEMP_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_TEMP_TYPE_t;

typedef struct MI_ISP_IQ_TEMP_INFO_TYPE_s
{
    MI_U8 u8Temperature;
} MI_ISP_IQ_TEMP_INFO_TYPE_t;

#if 1 // MAINTOOL API
typedef enum __attribute__ ((aligned (4)))
{
    SS_IQ_INDEX_0 = 0,
    SS_IQ_INDEX_1,
    SS_IQ_INDEX_2,
    SS_IQ_INDEX_3,
    SS_IQ_INDEX_4,
    SS_IQ_INDEX_5,
    SS_IQ_INDEX_6,
    SS_IQ_INDEX_7,
    SS_IQ_INDEX_8,
    SS_IQ_INDEX_9,
    SS_IQ_INDEX_10,
    SS_IQ_INDEX_11,
    SS_IQ_INDEX_12,
    SS_IQ_INDEX_13,
    SS_IQ_INDEX_14,
    SS_IQ_INDEX_15,
    SS_IQ_INDEX_MAX
} MI_ISP_IQ_INDEX_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_BYPASS_OFF = 0,
    SS_BYPASS_ON,
    SS_BYPASS_MAX
} MI_ISP_BYPASS_MODE_e;

typedef struct MI_ISP_API_BYPASS_TYPE_s
{
    MI_ISP_BYPASS_MODE_e bEnable;       //0 ~ 1
    MI_ISP_API_ID_e eAPIIndex;
} MI_ISP_API_BYPASS_TYPE_t;
#endif


/************************************* IQ  - Define the structure declaration - END   *************************************/


#endif /* _MI_ISP_IQ_DATATYPE_H_ */
