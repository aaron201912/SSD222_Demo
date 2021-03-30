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

#ifndef _MI_CUS3A_DATATYPE_H_
#define _MI_CUS3A_DATATYPE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define CUS3A_VER_STR "CUS3A_V1.1"
#define CUS3A_VER_MAJOR 1
#define CUS3A_VER_MINOR 1

#include "mi_isp_datatype.h"
#include "mi_cus3a_datatype.h"

#define MV_WIN_NUM                 25
#define CAM20_AUTO_NUM             16
#define CAM20_SDC_CTRL_ITEM_NUMBER 2
#define AWB_LV_CT_TBL_NUM          18
#define AWB_CT_TBL_NUM             10

#define MS_GAMMA_TBL               256
#define MS_ALSC_TBL_W              61
#define MS_ALSC_TBL_H              69
#define MS_ALSC_TBL                4209
#define MS_SDC_TBL                 1024
#define MS_FPN_TBL                 4000 //infinity5
#define MS_YUVGAMA_Y_TBL           256
#define MS_YUVGAMA_VU_TBL          128
#define MS_WDR_LOC_TBL             88

#define MS_CAM_AF_MAX_WIN_NUM 16 //
#define MAX_CUST_3A_CHINFO_NUM (16)

/*------AE/AWB interface--------*/
/*! @brief API error code*/
typedef enum CUS_3A_ERR_CODE
{
    CUS_3A_SUCCESS = 0, /**< operation successful */
    CUS_3A_ERROR = -1, /**< unspecified failure */
}CUS3A_ERR_CODE;

#define _3A_ROW             (128)   /**< number of 3A statistic blocks in a row */
#define _3A_COL             (90)    /**< number of 3A statistic blocks in a column */
#define _3A_HIST_BIN0    (40)    /**< histogram type0 resolution*/
#define _3A_HIST_BINX    (128)   /**< histogram type1 resolution*/
#define _3A_IR_HIST_BIN  (256)   /**< histogram type2 resolution*/

typedef enum
{
    E_ISP_CH_0 = 0,
    E_ISP_CH_1,
    E_ISP_CH_2,
    E_ISP_CH_3,
    E_ISP_CH_4,
    E_ISP_CH_5,
    E_ISP_CH_6,
    E_ISP_CH_7,
    E_ISP_CH_8,
    E_ISP_CH_9,
    E_ISP_CH_10,
    E_ISP_CH_11,
    E_ISP_CH_12,
    E_ISP_CH_13,
    E_ISP_CH_14,
    E_ISP_CH_15,
    E_ISP_CH_MAX
}CUS3A_ISP_CH_e;

typedef enum
{
    E_ALGO_TYPE_AE = 0x0,
    E_ALGO_TYPE_AWB,
    E_ALGO_TYPE_AF,
    E_ALGO_TYPE_MAX
}CUS3A_ALGO_TYPE_e;

typedef enum
{
    E_ALGO_ADAPTOR_NATIVE = 0x0,
    E_ALGO_ADAPTOR_1,
    E_ALGO_ADAPTOR_MAX
}CUS3A_ALGO_ADAPTOR_e;

// AWB statistic , one sample
typedef struct
{
    MI_U8 r;
    MI_U8 g;
    MI_U8 b;
} __attribute__((packed, aligned(1))) ISP_AWB_SAMPLE;

/*! @brief AWB HW statistics data*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 AvgBlkX;
    MI_U32 AvgBlkY;
    MI_U32 CurRGain;
    MI_U32 CurGGain;
    MI_U32 CurBGain;
    ISP_AWB_SAMPLE *avgs;   /*awb statis for linear frame or HDR long frame*/
    /*CUS3A V1.1*/
    MI_U8  HDRMode;             /**< Noramal or HDR mode*/
    ISP_AWB_SAMPLE*  pAwbStatisShort; /**<awb statis for HDR short Shutter AWB statistic data */
    MI_U32 u4BVx16384;      /**< From AE output, Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MI_U32 WeightY;                /**< frame brightness with ROI weight*/
} ISP_AWB_INFO;

/*! @brief AWB algorithm result*/
typedef struct isp_awb_result
{
    MI_U32 Size; /**< struct size*/
    MI_U32 Change; /**< if true, apply this result to hw register*/
    MI_U32 R_gain; /**< AWB gain for R channel*/
    MI_U32 G_gain; /**< AWB gain for G channel*/
    MI_U32 B_gain; /**< AWB gain for B channel*/
    MI_U32 ColorTmp; /**< Return color temperature*/
} ISP_AWB_RESULT;

// AE statistics data
typedef struct
{
    MI_U8 r;
    MI_U8 g;
    MI_U8 b;
    MI_U8 y;
} __attribute__((packed, aligned(1))) ISP_AE_SAMPLE;

typedef struct
{
    MI_U16 u2HistY[_3A_HIST_BINX];
} __attribute__((packed, aligned(1))) ISP_HISTX;

typedef struct
{
    MI_U16 u2IRHist[_3A_IR_HIST_BIN];
} __attribute__((packed, aligned(1))) ISP_IR_HISTX;

/*! @brief ISP report to AE, hardware statistic */
typedef struct
{
    MI_U32 Size;       /**< struct size*/
    ISP_HISTX   *hist1;   /**< HW statistic histogram 1*/
    ISP_HISTX   *hist2;   /**< HW statistic histogram 2*/
    MI_U32 AvgBlkX;  /**< HW statistics average block number*/
    MI_U32 AvgBlkY;  /**< HW statistics average block number*/
    ISP_AE_SAMPLE * avgs; /**< HW statistics average block data*/
    MI_U32 Shutter;                    /**< Current shutter in us*/
    MI_U32 SensorGain;                 /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGain;                    /**< Current ISP gain, 1X = 1024*/
    MI_U32 ShutterHDRShort;           /**< Current shutter in us*/
    MI_U32 SensorGainHDRShort;        /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGainHDRShort;           /**< Current ISP gain, 1X = 1024*/

    /*CUS3A V1.1*/
    MI_U32 PreAvgY;                   /**< Previous frame brightness*/
    MI_U8  HDRCtlMode;                /**< 0 = Separate shutter/sensor gain settings; */
                                   /**< 1 = Separate shutter & Share sensor gain settings */
    MI_U32 FNx10;                     /**< Aperture in FNx10*/
    MI_U32 CurFPS;                    /**Current sensor FPS */
    MI_U32 PreWeightY;             /**< Previous frame brightness with ROI weight*/
    /*CUS3A V1.2*/
    ISP_IR_HISTX *histIR;           /**< HW statistic histogram IR*/
} ISP_AE_INFO;

//AE algorithm result
/*! @brief ISP ae algorithm result*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 Change;         /**< if true, apply this result to hw register*/
    MI_U32 Shutter;         /**< Shutter in us */
    MI_U32 SensorGain;      /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGain;         /**< ISP gain, 1X = 1024 */
    MI_U32 ShutterHdrShort;     /**< Shutter in us */
    MI_U32 SensorGainHdrShort;  /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGainHdrShort;     /**< ISP gain, 1X = 1024 */
    MI_U32 u4BVx16384;      /**< Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MI_U32 AvgY;            /**< frame brightness */
    MI_U32 HdrRatio;   /**< hdr ratio, 1X = 1024 */
    /*CUS3A V1.1*/
    MI_U32 FNx10;                     /**< F number * 10*/
    MI_U32 DebandFPS;       /** Target fps when running auto debanding**/
    MI_U32 WeightY;                /**< frame brightness with ROI weight*/
    /*CUS3A v1.3*/
    MI_U16 GMBlendRatio;               /**< Adaptive Gamma Blending Ratio from AE**/
} ISP_AE_RESULT;

/*! @brief ISP initial status*/
typedef struct _isp_ae_init_param
{
    MI_U32 Size; /**< struct size*/
    char sensor_id[32]; /**< sensor module id*/
    MI_U32 shutter; /**< shutter Shutter in us*/
    MI_U32 shutter_step; /**< shutter Shutter step ns*/
    MI_U32 shutter_min; /**< shutter Shutter min us*/
    MI_U32 shutter_max; /**< shutter Shutter max us*/
    MI_U32 sensor_gain; /**< sensor_gain Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_max; /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    MI_U32 isp_gain; /**< isp_gain Isp digital gain , 1X = 1024 */
    MI_U32 isp_gain_max; /**< isp_gain Maximum Isp digital gain , 1X = 1024 */
    MI_U32 FNx10; /**< F number * 10*/
    MI_U32 fps; /**< initial frame per second*/
    MI_U32 shutterHDRShort_step;           /**< shutter Shutter step ns*/
    MI_U32 shutterHDRShort_min;            /**< shutter Shutter min us*/
    MI_U32 shutterHDRShort_max;            /**< shutter Shutter max us*/
    MI_U32 sensor_gainHDRShort_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gainHDRShort_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    MI_U32 AvgBlkX;  /**< HW statistics average block number*/
    MI_U32 AvgBlkY;  /**< HW statistics average block number*/
} ISP_AE_INIT_PARAM;

typedef enum
{
    ISP_AE_FPS_SET, /**< ISP notify AE sensor FPS has changed*/
} ISP_AE_CTRL_CMD;

typedef struct
{
    MI_U32 start_x; /*range : 0~1023*/
    MI_U32 start_y; /*range : 0~1023*/
    MI_U32 end_x;   /*range : 0~1023*/
    MI_U32 end_y;   /*range : 0~1023*/
} ISP_AF_RECT;

/*! @brief ISP initial status*/
typedef struct _isp_af_init_param
{
    MI_U32 Size; /**< struct size*/
    ISP_AF_RECT af_stats_win[16];
    /*CUS3A v1.3*/
    MI_U32 CurPos; //motor current position
    MI_U32 MinPos; //motor minimum position
    MI_U32 MaxPos; //motor maximum position
    MI_U32 MinStep;//motor minimum step
    MI_U32 MaxStep;//motor maximum step
} ISP_AF_INIT_PARAM;

typedef enum
{
    ISP_AF_CMD_MAX,
} ISP_AF_CTRL_CMD;

typedef struct
{
    MI_U8 high_iir[5*16];
    MI_U8 low_iir[5*16];
    MI_U8 luma[4*16];
    MI_U8 sobel_v[5*16];
    MI_U8 sobel_h[5*16];
    MI_U8 ysat[3*16];
} ISP_AF_INFO_STATS_PARAM_t;
//Infinity5. Sync with ISP_AF_STATS

typedef struct
{
    ISP_AF_INFO_STATS_PARAM_t stParaAPI[MS_CAM_AF_MAX_WIN_NUM];
} ISP_AF_INFO_STATS;

/*! @brief ISP report to AF, hardware statistic */
typedef struct
{
    MI_U32 Size; /**< struct size*/
    ISP_AF_INFO_STATS *pStats; /**< AF statistic*/
    /*CUS3A v1.3*/
    MI_U32 CurPos; //motor current position
    MI_U32 MinPos; //motor minimum position
    MI_U32 MaxPos; //motor maximum position
} ISP_AF_INFO;

typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 Change; /**< if true, apply this result to hw*/
    MI_U32 NextPos; /**< Next position*/
    /*CUS3A v1.3*/
    MI_U32                   ChangeParam;      /** if true, apply the following 4 results to hw register **/
    CusAFRoiMode_t        ROIMode;          /** roi mode configuration**/
    CusAFWin_t            Window[16];       /** AF statistics window position **/
    CusAFFilter_Geo_t     Filter;           /** AF filter paramater**/
    CusAFFilterSq_t       FilterSq;         /** AF filter square parameter**/
} ISP_AF_RESULT;

typedef enum
{
    E_ALGO_STATUS_UNINIT,
    E_ALGO_STATUS_RUNNING
}CUS3A_ALGO_STATUS_e;

typedef struct
{
    CUS3A_ALGO_STATUS_e Ae;
    CUS3A_ALGO_STATUS_e Awb;
    CUS3A_ALGO_STATUS_e Af;
} CUS3A_ALGO_STATUS_t;

/**@brief ISP AE interface*/
typedef struct isp_ae_interface
{
    void *pdata; /**< Private data for AE algorithm.*/

    /** @brief AE algorithm init callback
     @param[in] pdata AE algorithm private data
     @param[in] init_state ISP initial status.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AE algorithm initialize.
     */
    int (*init)(void* pdata, ISP_AE_INIT_PARAM *init_state);

    /** @brief AE algorithm close
     @param[in] pdata AE algorithm private data
     @remark ISP call this function when AE close.
     */
    void (*release)(void* pdata);

    /** @brief AE algorithm run
     @param[in] pdata AE algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AE algorithm return calculated result.
     @remark ISP call this function when AE close.
     */
    void (*run)(void* pdata, const ISP_AE_INFO *info, ISP_AE_RESULT *result);

    /** @brief AE algorithm control
     @param[in] pdata AE algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void* pdata, ISP_AE_CTRL_CMD cmd, void* param);
} ISP_AE_INTERFACE;

typedef enum
{
    ISP_AWB_MODE_SET,
} ISP_AWB_CTRL_CMD;
/**@brief ISP AWB interface*/

typedef struct isp_awb_interface
{
    void *pdata; /**< Private data for AE algorithm.*/

    /** @brief AWB algorithm init callback
     @param[in] pdata Algorithm private data
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AE algorithm initialize.
     */
    int (*init)(void *pdata);

    /** @brief AWB algorithm close
     @param[in] pdata Algorithm private data
     @remark ISP call this function when AE close.
     */
    void (*release)(void *pdata);

    /** @brief AWB algorithm run
     @param[in] pdata Algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AWB algorithm return calculated result.
     @remark ISP call this function when AE close.
     */
    void (*run)(void *pdata, const ISP_AWB_INFO *awb_info, ISP_AWB_RESULT *result);

    /** @brief AWB algorithm control
     @param[in] pdata Algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void *pdata, ISP_AWB_CTRL_CMD cmd, void* param);
} ISP_AWB_INTERFACE;

/**@brief ISP AF interface*/
typedef struct isp_af_interface
{
    void *pdata; /**< Private data for AF algorithm.*/

    /** @brief AF algorithm init callback
     @param[in] pdata Algorithm private data
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function when AF algorithm initialize.
     */
    int (*init)(void *pdata, ISP_AF_INIT_PARAM *param);

    /** @brief AF algorithm close
     @param[in] pdata Algorithm private data
     @remark ISP call this function when AF close.
     */
    void (*release)(void *pdata);

    /** @brief AF algorithm run
     @param[in] pdata Algorithm private data
     @param[in] info ISP HW statistics
     @param[out] result AF algorithm return calculated result.
     @remark ISP call this function when AF close.
     */
    void (*run)(void *pdata, const ISP_AF_INFO *af_info, ISP_AF_RESULT *result);

    /** @brief AF algorithm control
     @param[in] pdata Algorithm private data
     @param[in] cmd Control ID
     @param[in out] param Control parameter.
     @retval CUS_3A_SUCCESS or CUS_3A_ERROR if error occurs.
     @remark ISP call this function to change parameter
     */
    int (*ctrl)(void *pdata, ISP_AF_CTRL_CMD cmd, void* param);
} ISP_AF_INTERFACE;

#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_INCLUDE_CUS3A_MI_CUS3A_DATATYPE_H_ */
