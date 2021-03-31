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
#ifndef __MID_HCFD_H__
#define __MID_HCFD_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "mi_ive.h"


#define HCFD_DIVP_CHN		1
#define HCFD_FRMRATE		5
#define HCFD_RAW_W		    352 //384
#define HCFD_RAW_H		    288
#define USE_HC_FD 			1 //1:HC 2:HD

#define SAD_BLOCK_SIZE          8
#define MD_ROI_MAX              50
#define MD_PIXEL_DIFF           15
#define HCHD_DETECT_MAX         3//6
#define HCHD_PROBABILITY        0.7
#define ALIGN_UP(x, align)      (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_BACK(x, a)        (((x) / (a)) * (a))

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif

#if 0 	// FD_ENABLE

typedef struct ive_methon{
	int width;
	int align_height;
	MI_IVE_HANDLE ive_handle;
	MI_IVE_SrcImage_t *Y_image0, *Y_image1;
	MI_IVE_Image_t *SadResult, *ThdResult;
	MI_IVE_SadCtrl_t *sad_ctrl;
}SAD_METHOD_handle;
typedef struct
{
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} BBOX;
typedef struct
{
	int max_num;
	int used_num;
	BBOX box[MD_ROI_MAX];
} ROI;
int mid_hchdfd_Initial();
int mid_hchdfd_Uninitial();


#endif

#ifdef __cplusplus
}
#endif // __cplusplus


#endif 

