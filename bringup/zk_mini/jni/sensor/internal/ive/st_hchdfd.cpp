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
#if 0 // FD_ENABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>

#include "mi_common.h"
#include "mi_sys_datatype.h"
#include "mi_sys.h"
#include "st_hchdfd.h"
#include "ssnn.h"
#include "sstar_dynamic_load.h"
#include "st_rgn.h"

#define USE_IVE     	1 //0 CLOSE 1 OPEN
#define USE_HC_HD_FD 	3

static network_config cfg = {0};
static NetworkHandle *ntwk_handle = NULL;

static int sad_x_min = 0, sad_y_min = 0;
static int sad_x_max, sad_y_max;
static pthread_t g_pthread_hchdfd;
static MI_BOOL g_hchdfdExit = FALSE;
static ROI roi = {MD_ROI_MAX, 0, {0}};

 static  MI_IVE_SadCtrl_t sad_ctrl =
 {
        .eMode     = E_MI_IVE_SAD_MODE_MB_8X8,
        .eOutCtrl  = E_MI_IVE_SAD_OUT_CTRL_16BIT_BOTH,
        .u16Thr    = 8160,
        .u8MinVal  = 0,
        .u8MaxVal  = 255
 };

 static MI_IVE_Image_t YUV_image;
 static MI_IVE_SrcImage_t Y_image0, Y_image1;
 static MI_IVE_DstImage_t SadResult, ThdResult;
 static MI_IVE_Image_t RGB_image1, RGB_image1_bak;
 static SAD_METHOD_handle SAD_handle={0};
 MI_IVE_HANDLE ive_handle = 2;
 
 static IveAssembly_t g_stIveAssembly;

 MI_U32 MI_OS_GetTime(void)
 {
     struct timespec t1;
 
     clock_gettime(CLOCK_MONOTONIC, &t1);
 
     return 1000000 * (t1.tv_sec) + (t1.tv_nsec) / 1000;
 }
#if USE_IVE
MI_S32 MI_SYS_AllocateMemory(int size, MI_PHY * phys_addr, MI_U8 **virt_addr)
{
    MI_PHY phys;
    MI_S32 ret;

    char heap_name[] = "mma_heap_name0";

    ret = MI_SYS_MMA_Alloc((MI_U8*)heap_name, size , &phys);
    if (ret != MI_SUCCESS)
    {
        printf("can't allocate from MI SYS (size: %d, ret: %d)\n", size, ret);
        return ret;
    }
    ret = MI_SYS_Mmap(phys, size , (void **)virt_addr , 1);
    if (ret != MI_SUCCESS)
    {
        printf("can't map memory from MI SYS (phys: 0x%llx, size: %d, ret: %d)\n", phys, size, ret);
        MI_SYS_MMA_Free(phys);
        return ret;
    }

   *phys_addr = phys;

    //printf("allocate memory from MI SYS (phys: 0x%lld, virt: %p, size: %d)\n", *phys_addr, *virt_addr, size);

    return MI_SUCCESS;
}
MI_S32 MI_SYS_FreeMemory(MI_PHY phys_addr, MI_U8 *virt_addr, MI_S32 size)
{
    MI_S32 ret;

    //printf("free memory to MI SYS (phys: 0x%p, virt: %p, size: %d)\n", (void *)phys_addr, virt_addr, size);

    ret = MI_SYS_Munmap((void *)virt_addr, size);

    ret |= MI_SYS_MMA_Free(phys_addr);

    if (ret != MI_SUCCESS)
    {
        printf("error occurs when free buffer to MI SYS\n");
        return ret;
    }

    return MI_SUCCESS;
}
#endif
MI_S32 ModuleTest_AllocateImage(MI_IVE_Image_t      *pstImage,
                            MI_IVE_ImageType_e  eImageType,
                            MI_U16              u16Stride,
                            MI_U16              u16Width,
                            MI_U16              u16Height)
{
    MI_S32 ret = MI_SUCCESS;
    int size, i;

    memset(pstImage, 0, sizeof(MI_IVE_Image_t));

    pstImage->eType = eImageType;
    pstImage->u16Width  = u16Width;
    pstImage->u16Height = u16Height;

    switch(eImageType) {
        // 64bit Gray
        case E_MI_IVE_IMAGE_TYPE_S64C1:
        case E_MI_IVE_IMAGE_TYPE_U64C1:
            size = u16Stride * u16Height * sizeof(MI_U64);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;
 
        // 32bit Gray 
        case E_MI_IVE_IMAGE_TYPE_S32C1:
        case E_MI_IVE_IMAGE_TYPE_U32C1:
            size = u16Stride * u16Height * sizeof(MI_U32);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;

        // 16bit Gray
        case E_MI_IVE_IMAGE_TYPE_S16C1:
        case E_MI_IVE_IMAGE_TYPE_U16C1:
            size = u16Stride * u16Height * sizeof(MI_U16);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;

        // 8bit Gray
        case E_MI_IVE_IMAGE_TYPE_S8C1:
        case E_MI_IVE_IMAGE_TYPE_U8C1:
            size = u16Stride * u16Height * sizeof(MI_U8);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;

        // YUV 420 semi plane
        case E_MI_IVE_IMAGE_TYPE_YUV420SP:
            size = u16Stride * u16Height * sizeof(MI_U8);
#if USE_IVE
           MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
           MI_SYS_AllocateMemory(size/2, &pstImage->aphyPhyAddr[1], &pstImage->apu8VirAddr[1]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
            pstImage->aphyPhyAddr[1] = (MI_PHY)(pstImage->apu8VirAddr[1] = (MI_U8*)malloc(size/2));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            pstImage->azu16Stride[1] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL || pstImage->apu8VirAddr[1] == NULL)
            {
                goto ERROR;
            }
            break;

        // YUV 422 semi plane
        case E_MI_IVE_IMAGE_TYPE_YUV422SP:
            size = u16Stride * u16Height * sizeof(MI_U8);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[1], &pstImage->apu8VirAddr[1]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
            pstImage->aphyPhyAddr[1] = (MI_PHY)(pstImage->apu8VirAddr[1] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            pstImage->azu16Stride[1] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL || pstImage->apu8VirAddr[1] == NULL)
            {
                goto ERROR;
            }
            break;

        // RGB packed
        case E_MI_IVE_IMAGE_TYPE_U8C3_PACKAGE:
            size = u16Stride * u16Height * sizeof(MI_U8) * 3;
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;

        // RGB plane
        case E_MI_IVE_IMAGE_TYPE_U8C3_PLANAR:
            size = u16Stride * u16Height * sizeof(MI_U8);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[1], &pstImage->apu8VirAddr[1]);
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[2], &pstImage->apu8VirAddr[2]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
            pstImage->aphyPhyAddr[1] = (MI_PHY)(pstImage->apu8VirAddr[1] = (MI_U8*)malloc(size));
            pstImage->aphyPhyAddr[2] = (MI_PHY)(pstImage->apu8VirAddr[2] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            pstImage->azu16Stride[1] = u16Stride;
            pstImage->azu16Stride[2] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL || pstImage->apu8VirAddr[1] == NULL || pstImage->apu8VirAddr[2] == NULL)
            {
                goto ERROR;
            }
            break;

        case E_MI_IVE_IMAGE_TYPE_S8C2_PACKAGE:
            size = u16Stride * u16Height * sizeof(MI_U8) * 2;
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL)
            {
                goto ERROR;
            }
            break;

        case E_MI_IVE_IMAGE_TYPE_S8C2_PLANAR:
            size = u16Stride * u16Height * sizeof(MI_U8);
#if USE_IVE
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[0], &pstImage->apu8VirAddr[0]);
            MI_SYS_AllocateMemory(size, &pstImage->aphyPhyAddr[1], &pstImage->apu8VirAddr[1]);
#else
            pstImage->aphyPhyAddr[0] = (MI_PHY)(pstImage->apu8VirAddr[0] = (MI_U8*)malloc(size));
            pstImage->aphyPhyAddr[1] = (MI_PHY)(pstImage->apu8VirAddr[1] = (MI_U8*)malloc(size));
#endif
            pstImage->azu16Stride[0] = u16Stride;
            pstImage->azu16Stride[1] = u16Stride;
            if (pstImage->apu8VirAddr[0] == NULL || pstImage->apu8VirAddr[1] == NULL)
            {
                goto ERROR;
            }
            break;

        default:
            printf("Format is not support!!\n");
            return MI_IVE_ERR_ILLEGAL_PARAM;
    }

    return ret;

ERROR:
    for (i=0; i<3; i++)
    {
        if(pstImage->apu8VirAddr[i] != NULL)
        {
            free(pstImage->apu8VirAddr[i]);
        }
    }

    memset(pstImage, 0, sizeof(MI_IVE_Image_t));

    return MI_IVE_ERR_NOMEM;
}

#if USE_IVE
MI_S32 ModuleTest_FreeImage(MI_IVE_Image_t *pstImage)
{
    int32_t size;
    MI_S32 ret = MI_SUCCESS;

    switch(pstImage->eType) {
        case E_MI_IVE_IMAGE_TYPE_S64C1:
        case E_MI_IVE_IMAGE_TYPE_U64C1:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U64);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_S32C1:
        case E_MI_IVE_IMAGE_TYPE_U32C1:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U32);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_S16C1:
        case E_MI_IVE_IMAGE_TYPE_U16C1:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U16);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_S8C1:
        case E_MI_IVE_IMAGE_TYPE_U8C1:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_U8C3_PACKAGE:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8) * 3;
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_S8C2_PACKAGE:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8) * 2;
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_YUV420SP:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[1], pstImage->apu8VirAddr[1], size/2);
            break;

        case E_MI_IVE_IMAGE_TYPE_YUV422SP:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[1], pstImage->apu8VirAddr[1], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_S8C2_PLANAR:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[1], pstImage->apu8VirAddr[1], size);
            break;

        case E_MI_IVE_IMAGE_TYPE_U8C3_PLANAR:
            size = pstImage->azu16Stride[0]*pstImage->u16Height*sizeof(MI_U8);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[0], pstImage->apu8VirAddr[0], size);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[1], pstImage->apu8VirAddr[1], size);
            MI_SYS_FreeMemory(pstImage->aphyPhyAddr[2], pstImage->apu8VirAddr[2], size);
            break;

        default:
            printf("Format is not support!!\n");
            ret = MI_IVE_ERR_ILLEGAL_PARAM;
    }

    memset(pstImage, 0, sizeof(MI_IVE_Image_t));

    return ret;
}
#else 
MI_S32 ModuleTest_FreeImage(MI_IVE_Image_t *pstImage)
{
    MI_S32 ret = MI_SUCCESS;

    switch(pstImage->eType) {
        case E_MI_IVE_IMAGE_TYPE_S64C1:
        case E_MI_IVE_IMAGE_TYPE_U64C1:
        case E_MI_IVE_IMAGE_TYPE_S32C1:
        case E_MI_IVE_IMAGE_TYPE_U32C1:
        case E_MI_IVE_IMAGE_TYPE_S16C1:
        case E_MI_IVE_IMAGE_TYPE_U16C1:
        case E_MI_IVE_IMAGE_TYPE_S8C1:
        case E_MI_IVE_IMAGE_TYPE_U8C1:
        case E_MI_IVE_IMAGE_TYPE_U8C3_PACKAGE:
        case E_MI_IVE_IMAGE_TYPE_S8C2_PACKAGE:
            free(pstImage->apu8VirAddr[0]);
            break;

        case E_MI_IVE_IMAGE_TYPE_YUV420SP:
        case E_MI_IVE_IMAGE_TYPE_YUV422SP:
        case E_MI_IVE_IMAGE_TYPE_S8C2_PLANAR:
            free(pstImage->apu8VirAddr[0]);
            free(pstImage->apu8VirAddr[1]);
            break;

        case E_MI_IVE_IMAGE_TYPE_U8C3_PLANAR:
            free(pstImage->apu8VirAddr[0]);
            free(pstImage->apu8VirAddr[1]);
            free(pstImage->apu8VirAddr[2]);
            break;

        default:
            printf("Format is not support!!\n");
            ret = MI_IVE_ERR_ILLEGAL_PARAM;
    }

    memset(pstImage, 0, sizeof(MI_IVE_Image_t));

    return ret;
}
#endif
MI_S32 Module_SAD()
{
    MI_S32 ret;

    memset(&Y_image0, 0, sizeof(Y_image0));
    memset(&Y_image1, 0, sizeof(Y_image1));
    memset(&SadResult, 0, sizeof(SadResult));
    memset(&ThdResult, 0, sizeof(ThdResult));

    // Init IVE
    ret = g_stIveAssembly.pfnIveCreate(ive_handle);
    if (ret != MI_SUCCESS)
    {
        printf("Could not create IVE handle\n");
        return ret;
    }
   if (ModuleTest_AllocateImage(&RGB_image1, E_MI_IVE_IMAGE_TYPE_U8C3_PACKAGE, HCFD_RAW_W, HCFD_RAW_W, ALIGN_UP(HCFD_RAW_H, 32)))
    {
        printf("[%s %d] Could not allocate MVE image\n",__func__,__LINE__);
        goto RETURN_3;
    }
    memcpy(&RGB_image1_bak, &RGB_image1, sizeof(MI_IVE_Image_t));
    // Allocate output buffer 0
    ret = ModuleTest_AllocateImage(&SadResult, E_MI_IVE_IMAGE_TYPE_U16C1, HCFD_RAW_W/SAD_BLOCK_SIZE, HCFD_RAW_W/SAD_BLOCK_SIZE, ALIGN_DOWN(HCFD_RAW_H, 32)/SAD_BLOCK_SIZE);
    if (ret != MI_SUCCESS)
    {
        printf("Can't allocate output buffer 0\n");
        goto RETURN_2;
    }

    // Allocate output buffer 1
    ret = ModuleTest_AllocateImage(&ThdResult, E_MI_IVE_IMAGE_TYPE_U8C1, HCFD_RAW_W/SAD_BLOCK_SIZE, HCFD_RAW_W/SAD_BLOCK_SIZE, ALIGN_DOWN(HCFD_RAW_H, 32)/SAD_BLOCK_SIZE);
    if (ret != MI_SUCCESS)
    {
        printf("Can't allocate output buffer 1\n");
        goto RETURN_1;
    }
    return ret;
RETURN_1:
    ModuleTest_FreeImage(&SadResult);
RETURN_2:
    ModuleTest_FreeImage(&RGB_image1);
RETURN_3:
	g_stIveAssembly.pfnIveDestroy(ive_handle);

    return -1;
}

static int MD_frame_difference(/*SAD_METHOD_0_Handle*/SAD_METHOD_handle *handle, ROI *roi)
{
    int i, i2, j;// mve_ret;
    MI_S32 ret = 0;
    int x_min, x_max, y_min, y_max, sad_sum;

    ret = g_stIveAssembly.pfnIveSad(handle->ive_handle,handle->Y_image0 , handle->Y_image1, handle->SadResult, handle->ThdResult,handle->sad_ctrl, 0);
    if (ret != MI_SUCCESS)
    {
        printf("MI_IVE_Sad() return ERROR 0x%X\n", ret);
        return ret;
    }
    //printf("MI_IVE_Sad is run ok!\n");
    x_min = 0;
    y_min = 0;
    x_max = handle->ThdResult->u16Width;
    y_max = handle->ThdResult->u16Height;

    // find y min
    sad_sum = 0;
    for (i=0, i2=0; i<handle->ThdResult->u16Height; i++, i2+=handle->ThdResult->u16Width)
    {
        for (j=0; j<handle->ThdResult->u16Width; j++)
        {
            sad_sum += (int)handle->ThdResult->apu8VirAddr[0][i2+j];
        }
        if (sad_sum)
        {
            y_min = i;
            break;
        }
    }
    if(i==handle->ThdResult->u16Height)
    {
        x_min = 0;
        y_min = 0;
        x_max = 0;
        y_max = 0;
        goto RET_1;
    }
    // find y max
    sad_sum = 0;
    for (i=handle->ThdResult->u16Height-1, i2=i*handle->ThdResult->u16Width; i>=0; i--, i2-=handle->ThdResult->u16Width)
    {
        for (j=0; j<handle->ThdResult->u16Width; j++)
        {
            sad_sum += (int)handle->ThdResult->apu8VirAddr[0][i2+j];
        }
        if (sad_sum)
        {
            y_max = (i+1);
            break;
        }
    }

    if(i<0)
    {
        x_min = 0;
        y_min = 0;
        x_max = 0;
        y_max = 0;
        goto RET_1;
    }
    // find x min
    sad_sum = 0;
    for (j=0; j<handle->ThdResult->u16Width; j++)
    {
        for (i=0, i2=0; i<handle->ThdResult->u16Height; i++, i2+=handle->ThdResult->u16Width)
        {
            sad_sum += (int)handle->ThdResult->apu8VirAddr[0][i2+j];
        }
        if (sad_sum)
        {
            x_min = j;
            break;
        }
    }
    if(j == handle->ThdResult->u16Height)
    {
        x_min = 0;
        y_min = 0;
        x_max = 0;
        y_max = 0;
        goto RET_1;
    }
    // find x max
    sad_sum = 0;
    for (j=handle->ThdResult->u16Width-1; j>=0; j--)
    {
        for (i=0, i2=0; i<handle->ThdResult->u16Height; i++, i2+=handle->ThdResult->u16Width)
        {
            sad_sum += (int)handle->ThdResult->apu8VirAddr[0][i2+j];
        }
        if (sad_sum)
        {
            x_max = (j+1);
            break;
        }
    }
    if(j<0)
    {
        x_min = 0;
        y_min = 0;
        x_max = 0;
        y_max = 0;
        goto RET_1;
    }
    
RET_1:
    roi->used_num = 1;
    roi->box[0].x_min = x_min;
    roi->box[0].x_max = x_max;
    roi->box[0].y_min = y_min;
    roi->box[0].y_max = y_max;
    return MI_SUCCESS;
}

static int MD_preprocess()
{
    MI_S32 ret = 0;
    sad_x_min = 0;
    sad_y_min = 0;
    sad_x_max = SadResult.u16Width;
    sad_y_max = SadResult.u16Height;

    SAD_handle.width = HCFD_RAW_W;
    SAD_handle.align_height =  ALIGN_DOWN(HCFD_RAW_H, 32);
    SAD_handle.ive_handle = ive_handle;
    SAD_handle.Y_image0 = &Y_image0;
    SAD_handle.Y_image1 = &Y_image1;
    SAD_handle.SadResult = &SadResult;
    SAD_handle.ThdResult = &ThdResult;
    SAD_handle.sad_ctrl = &sad_ctrl;
    ret = MD_frame_difference(&SAD_handle, &roi);
    if(MI_SUCCESS != ret)
    {
      printf("MD_frame_difference is failed!\n");
      return ret;
    }
    sad_x_min = roi.box[0].x_min;
    sad_x_max = roi.box[0].x_max;
    sad_y_min = roi.box[0].y_min;
    sad_y_max = roi.box[0].y_max;
    if ((sad_y_min == 0) &&(sad_y_max ==0) &&  (sad_x_min == 0) && (sad_x_max == 0))
    {
        //printf("if no motion, do nothing...\n\n");
        //ST_RGN_ClearRect(0);
        return 0;
    }

    // left/right/top/bottom add one block
    if (sad_x_min > 0)
        sad_x_min --;
    if (sad_x_max < ThdResult.u16Width)
        sad_x_max ++;
    if (sad_y_min > 0)
        sad_y_min --;
    if (sad_y_max < ThdResult.u16Height)
        sad_y_max ++;

    //sad level to pixel level
    sad_y_min = sad_y_min*SAD_BLOCK_SIZE;
    sad_y_max = sad_y_max*SAD_BLOCK_SIZE - 1;
    sad_x_min = sad_x_min*SAD_BLOCK_SIZE;
    sad_x_max = sad_x_max*SAD_BLOCK_SIZE - 1;

    /*if ((sad_y_min == 0) && (sad_y_max ==  ALIGN_UP(g_ieHeight, 32) -1) && (sad_x_min == 0) && (sad_x_max == g_ieWidth - 1))
    {
        printf("if full motion, do nothing...\n\n");//if camere move or light change, it is not normal,so dont do hchdfd.
        return 0;
    }
    else
    {
        //printf("motion: %d %d %d %d \n",sad_x_min,sad_y_min,sad_x_max - sad_x_min + 1,sad_y_max - sad_y_min + 1);
        return 1;
    }*/
    //printf("motion: %d %d %d %d \n",sad_x_min,sad_y_min,sad_x_max - sad_x_min + 1,sad_y_max - sad_y_min + 1);
    return 1;

}

void CalcShowRect(ST_RGN_Rect_t *pstSrcRect, ST_RGN_Rect_t *pstDstRect, MI_U16 u16FdWidth, MI_U16 u16FdHeight
                                       , MI_U16 u16ShowWidth, MI_U16 u16ShowHeight)
{
    pstDstRect->u16LeftTopX = ALIGN_DOWN(pstSrcRect->u16LeftTopX *u16ShowWidth, u16FdWidth) / u16FdWidth;
    pstDstRect->u16LeftTopY = ALIGN_DOWN(pstSrcRect->u16LeftTopY * u16ShowHeight, u16FdHeight) / u16FdHeight;
    pstDstRect->u16RightBottomX = ALIGN_UP(pstSrcRect->u16RightBottomX*u16ShowWidth, u16FdWidth) / u16FdWidth;
    pstDstRect->u16RightBottomY = ALIGN_UP(pstSrcRect->u16RightBottomY * u16ShowHeight, u16FdHeight) / u16FdHeight;
}

void* mid_hchdfd_Task(void *argu)
{
    static int nHC_option_mode_count = 0;
    static int nHC_keepYuv_count = 0;
    MI_SYS_ChnPort_t stDivpChnOutputPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hHandle_buffer0;
    MI_SYS_BUF_HANDLE hHandle_buffer1;
    MI_IVE_CscCtrl_t  csc_ctrl;
    MI_S32 ret = -1;
    int i, human_cnt=0x0;
    MI_U32 u32FdDetectCount = 0;
    int sad_height = sad_y_max - sad_y_min + 1, sad_width= sad_x_max - sad_x_min + 1;
    MI_U32 tmpTimeStamp = 0;
    MI_U32 lastTimeStamp = MI_OS_GetTime();
    MI_U32 getInterval = 1000000 / HCFD_FRMRATE;
    MI_SYS_WindowRect_t drawRect[HCHD_DETECT_MAX];
    memset(drawRect,0,sizeof(drawRect));
    stDivpChnOutputPort.eModId   = E_MI_MODULE_ID_DIVP;
    stDivpChnOutputPort.u32ChnId = HCFD_DIVP_CHN;
    stDivpChnOutputPort.u32DevId = 0;
    stDivpChnOutputPort.u32PortId = 0;
    ST_RGN_Rect_t stSrcRGNDrawRect;
    ST_RGN_Rect_t astRGNDrawRect[HCHD_DETECT_MAX];
    /****************************************************************/
    while(ret != MI_SUCCESS)
    {
        memset(&stBufInfo,0,sizeof(MI_SYS_BufInfo_t));
        ret= MI_SYS_ChnOutputPortGetBuf(&stDivpChnOutputPort , &stBufInfo, &hHandle_buffer0);
        usleep(20*1000);
    }
    printf("mid_hchdfd_Task get buf success\n");
    Y_image0.eType = E_MI_IVE_IMAGE_TYPE_U8C1;
    Y_image0.apu8VirAddr[0] = (unsigned char*)stBufInfo.stFrameData.pVirAddr[0];
    Y_image0.aphyPhyAddr[0] = (MI_PHY)stBufInfo.stFrameData.phyAddr[0];
    Y_image0.u16Height = ALIGN_DOWN(HCFD_RAW_H, 32);
    Y_image0.u16Width = HCFD_RAW_W;
    Y_image0.azu16Stride[0] = HCFD_RAW_W;
    nHC_keepYuv_count=1;
/****************************************************************/
    printf("g_hchdfdExit: %d\n",g_hchdfdExit);
    while(!g_hchdfdExit)
    {
        tmpTimeStamp = MI_OS_GetTime();
        if(lastTimeStamp + getInterval <= tmpTimeStamp)
        {
            lastTimeStamp = tmpTimeStamp;
        }
        else
        {
            usleep(lastTimeStamp + getInterval - tmpTimeStamp);
            continue;
        }
        if(nHC_keepYuv_count == 1){
            memset(&stBufInfo,0,sizeof(MI_SYS_BufInfo_t));
            if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stDivpChnOutputPort , &stBufInfo, &hHandle_buffer1))
            {
                usleep(20*1000);
                continue;
            }
            Y_image1.eType = E_MI_IVE_IMAGE_TYPE_U8C1;
            Y_image1.apu8VirAddr[0] = (MI_U8*)stBufInfo.stFrameData.pVirAddr[0];
            Y_image1.aphyPhyAddr[0] = (MI_PHY)stBufInfo.stFrameData.phyAddr[0];
            Y_image1.u16Height =  ALIGN_DOWN(HCFD_RAW_H, 32);
            Y_image1.u16Width = HCFD_RAW_W;
            Y_image1.azu16Stride[0] = HCFD_RAW_W;

            nHC_keepYuv_count = 0;
        }
        else if(nHC_keepYuv_count == 0)
        {
            memset(&stBufInfo,0,sizeof(MI_SYS_BufInfo_t));
            if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stDivpChnOutputPort , &stBufInfo, &hHandle_buffer0))
            {
                usleep(20*1000);
                continue;
            }
            Y_image0.eType = E_MI_IVE_IMAGE_TYPE_U8C1;
            
            Y_image0.apu8VirAddr[0] = (MI_U8*)stBufInfo.stFrameData.pVirAddr[0];
            Y_image0.aphyPhyAddr[0] = (MI_PHY)stBufInfo.stFrameData.phyAddr[0];
            Y_image0.u16Height =  ALIGN_DOWN(HCFD_RAW_H, 32);
            Y_image0.u16Width = HCFD_RAW_W;
            Y_image0.azu16Stride[0] = HCFD_RAW_W;

            nHC_keepYuv_count = 1;
        }
        
        YUV_image.eType = E_MI_IVE_IMAGE_TYPE_YUV420SP;
        YUV_image.u16Height =  ALIGN_DOWN(HCFD_RAW_H, 32);
        YUV_image.u16Width = HCFD_RAW_W;
        YUV_image.azu16Stride[0] = HCFD_RAW_W;
        YUV_image.azu16Stride[1] = HCFD_RAW_W;

        YUV_image.apu8VirAddr[0] = (MI_U8*)stBufInfo.stFrameData.pVirAddr[0];
        YUV_image.apu8VirAddr[1] = (MI_U8*)stBufInfo.stFrameData.pVirAddr[1];
        YUV_image.aphyPhyAddr[0] = (MI_PHY)stBufInfo.stFrameData.phyAddr[0];
        YUV_image.aphyPhyAddr[1] = (MI_PHY)stBufInfo.stFrameData.phyAddr[1];
        if(1 == MD_preprocess())
        {
            int pad_x, pad_y;
            // roi width/height align to 32(some model align to 16)
            /**********************************************************/
            /*sad_y_min = 0;
            sad_x_min = 0;
            sad_x_max = g_ieWidth-1;
            sad_y_max = align_height-1;*/
            /**********************************************************/
            sad_height = sad_y_max - sad_y_min + 1;
            sad_width = sad_x_max - sad_x_min + 1;
            pad_x = sad_width%32?((ALIGN_UP(sad_width,32) - sad_width) / 2):0;
            pad_y = sad_height%32?((ALIGN_UP(sad_height,32) - sad_height) / 2):0;
            if((sad_x_min < pad_x) && (sad_x_max + pad_x > HCFD_RAW_W)){
                sad_x_min = 0;
                sad_x_max = HCFD_RAW_W - 1;
            }
            else if (sad_x_min < pad_x)
            {
                sad_x_max += (pad_x * 2 - sad_x_min);
                sad_x_min = 0;
            } 
            else if (sad_x_max + pad_x >= HCFD_RAW_W)
            {
                sad_x_min -= (pad_x * 2 - (HCFD_RAW_W - 1 - sad_x_max));
                sad_x_max = HCFD_RAW_W - 1;
            } 
            else
            {
                sad_x_max += pad_x;
                sad_x_min -= pad_x;
            }

            if((sad_y_min < pad_y)&&(sad_y_max + pad_y >  ALIGN_DOWN(HCFD_RAW_H, 32)))
            {
                sad_y_min = 0;
                sad_y_max =  ALIGN_DOWN(HCFD_RAW_H, 32) - 1;
            }
            else if (sad_y_min < pad_y)
            {
                sad_y_max += (pad_y * 2 - sad_y_min);
                sad_y_min = 0;
            } 
            else if (sad_y_max + pad_y >=  ALIGN_DOWN(HCFD_RAW_H, 32))
            {
                sad_y_min -= (pad_y * 2 - ( ALIGN_DOWN(HCFD_RAW_H, 32) - 1 - sad_y_max));
                sad_y_max =  ALIGN_DOWN(HCFD_RAW_H, 32) - 1;
            } 
            else
            {
                sad_y_max += pad_y;
                sad_y_min -= pad_y;
            }
            sad_height = sad_y_max - sad_y_min + 1;
            sad_width = sad_x_max - sad_x_min + 1;
                        
            //data rearrange
            int offset_x = sad_x_min;
            int offset_y0 = sad_y_min * YUV_image.u16Width;//YUV_image.u16Stride[0];
            int offset_y1 = sad_y_min * YUV_image.u16Width;//YUV_image.u16Stride[1];
        
            //memcpy(&RGB_image1_bak, &RGB_image1, sizeof(MVE_SRC_IMAGE_S));

            char* tmp1 = (char*)YUV_image.aphyPhyAddr[0];
            char* tmp2 = (char*)YUV_image.aphyPhyAddr[1];
            tmp1 += offset_y0 + offset_x;
            tmp2 += ((offset_y1/2)+ offset_x);
            YUV_image.apu8VirAddr[0] += offset_y0 + offset_x;
            YUV_image.apu8VirAddr[1] += ((offset_y1/2)+ offset_x);
            YUV_image.u16Width = sad_width;
            YUV_image.u16Height = sad_height;
            
            RGB_image1.u16Width = sad_width;
            RGB_image1.u16Height = sad_height;
            RGB_image1.azu16Stride[0] = sad_width;  
            csc_ctrl.eMode = E_MI_IVE_CSC_MODE_PIC_BT601_YUV2RGB;

            ret = g_stIveAssembly.pfnIveCsc(ive_handle,&YUV_image,&RGB_image1,&csc_ctrl,1);
            if(0 != ret)
            {
              printf("[MVE] MI_IVE_CSC error!!! ret[0x%x]\n",ret);
            }
            
            if(nHC_option_mode_count==10)
            {   
                nHC_option_mode_count=0;
            }
            nHC_option_mode_count++;    
            if(USE_HC_HD_FD==3)
            {
                cfg.target_height = ALIGN_UP(sad_height, 64);//FD实际传的图像不要求64对齐，但是targe要求设置对齐。
                cfg.target_width = ALIGN_UP(sad_width, 64);
            }
            else {
                cfg.target_height = ALIGN_UP(sad_height, 32);//HC HD实际传的图像不要求32对齐，但是targe要求设置对齐。
                cfg.target_width = ALIGN_UP(sad_width, 32);
            }
            
            if (Forward_Network(ntwk_handle, RGB_image1.apu8VirAddr[0], sad_height, sad_width, 3))
            {
                printf("[HD] Forward error!!!\n");
            }
            if(USE_HC_HD_FD == 1) //HC
            {
                Get_Prob(ntwk_handle);
                human_cnt = (int)(ntwk_handle->probs[0]*100);
            }
            else {              //HD or FD
                human_cnt = Get_Detection(ntwk_handle, sad_height, sad_width);
            }
            //printf("human_cnt: %d\n",human_cnt);
        }
        else if(USE_HC_HD_FD==2 || USE_HC_HD_FD==3)
        {
            human_cnt = 0;
        }
        if(USE_HC_HD_FD==1) //HC
        {
            if (human_cnt>= HCHD_PROBABILITY*100)
            {
                if((sad_x_min+5) < HCFD_RAW_W)
                    drawRect[0].u16X = sad_x_min+5;
                if((sad_y_min+5 )< HCFD_RAW_H)
                    drawRect[0].u16Y = sad_y_min+5;
                if((sad_width - 10) > 0)
                    drawRect[0].u16Width=sad_width - 10;
                if((sad_height - 10) > 0)
                    drawRect[0].u16Height=sad_height-10;
                printf("human_cnt score=%d: %d %d %d %d \n\n",human_cnt,sad_x_min,sad_x_max,sad_y_min,sad_y_max);
            }
            else {
                drawRect[0].u16Width=0;
                drawRect[0].u16Height=0;
                drawRect[0].u16X=0;
                drawRect[0].u16Y=0;
                printf("human count is 0!\n");
            }
        }
        else {  //HD  or FD
            if (human_cnt > 0)
            {
                //if(USE_HC_HD_FD==2)printf("human_detect num %d !\n\n",human_cnt);
                //else if(USE_HC_HD_FD==3)printf("face_cnt %d !\n\n",human_cnt);
                memset(&drawRect, 0x0, sizeof(drawRect));
                memset(&stSrcRGNDrawRect, 0x0, sizeof(ST_RGN_Rect_t));
                memset(&astRGNDrawRect, 0x0, sizeof(astRGNDrawRect));
                for (i=0; i<human_cnt; i++)
                {
                    /*int rst_xl, rst_xr, rst_yt, rst_yb, rst_w, rst_h;
                    rst_xl = (ntwk_handle->boxes[i].x_min + sad_x_min);
                    rst_yt = (ntwk_handle->boxes[i].y_min + sad_y_min);
                    rst_xr = (ntwk_handle->boxes[i].x_max + sad_x_min);
                    rst_yb = (ntwk_handle->boxes[i].y_max + sad_y_min);
                    rst_w =  rst_xr - rst_xl + 1;
                    rst_h = rst_yb - rst_yt + 1;
                    
                    printf("hm %d, x_min=%d, y_min=%d, x_max=%d, y_max=%d\n", i, rst_xl, rst_yt, rst_xr, rst_yb);
                    */
                    drawRect[i].u16X = (ntwk_handle->boxes[i].x_min + sad_x_min);
                    drawRect[i].u16Y = (ntwk_handle->boxes[i].y_min + sad_y_min);
                    drawRect[i].u16Width = (ntwk_handle->boxes[i].x_max - ntwk_handle->boxes[i].x_min) - 1;
                    drawRect[i].u16Height = (ntwk_handle->boxes[i].y_max - ntwk_handle->boxes[i].y_min) - 1;
                    //printf("number of face %d :%d %d %d %d\n",human_cnt,drawRect[i].u16X,drawRect[i].u16Y,drawRect[i].u16Width,drawRect[i].u16Height);
                    stSrcRGNDrawRect.u16LeftTopX = drawRect[i].u16X;
                    stSrcRGNDrawRect.u16LeftTopY = drawRect[i].u16Y;
                    stSrcRGNDrawRect.u16RightBottomX = drawRect[i].u16X + drawRect[i].u16Width;
                    stSrcRGNDrawRect.u16RightBottomY = drawRect[i].u16Y + drawRect[i].u16Height;
                    
                    CalcShowRect(&stSrcRGNDrawRect, &astRGNDrawRect[i], HCFD_RAW_W, HCFD_RAW_H, 1024, 600);
                    
                    if(u32FdDetectCount < 2)
                    {
                        u32FdDetectCount++;
                    }
                    //printf("human_cnt: %d,u32FdDetectCount: %d\n",human_cnt, u32FdDetectCount);
                }
                ST_RGN_DrawRect(0, astRGNDrawRect, human_cnt);
            }
            else {
                //if(USE_HC_HD_FD==2)printf("no human detected!\n");
                //else if(USE_HC_HD_FD==3)printf("no face detected!\n");
                //drawRect[0].u16Width=0;
                //drawRect[0].u16Height=0;
                //drawRect[0].u16X=0;
                //drawRect[0].u16Y=0; 
                memset(&drawRect, 0x0, sizeof(drawRect));
                
                if(u32FdDetectCount > 0)
                {
                    u32FdDetectCount--;
                    //printf("clean DetectCount: %d\n", u32FdDetectCount);
                    if(!u32FdDetectCount)
                    {
                        ST_RGN_ClearRect(0);
                    }
                }
            }
        }
        if(nHC_keepYuv_count == 0)
        {
            if(MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(hHandle_buffer0))
            {
                printf("%s %d MI_SYS_ChnOutputPortPutBuf 0 is error!\n",__func__,__LINE__);
            }
        }
        else if(nHC_keepYuv_count == 1)
        {
            if(MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(hHandle_buffer1))
            {
                printf("%s %d MI_SYS_ChnOutputPortPutBuf 1 is error!\n",__func__,__LINE__);
            }
        }
    }
/***************************************************************/
    if(nHC_keepYuv_count == 0){
        MI_SYS_ChnOutputPortPutBuf(hHandle_buffer1);
    }
    else if(nHC_keepYuv_count == 1){
        MI_SYS_ChnOutputPortPutBuf(hHandle_buffer0);
    }
/***************************************************************/
    return NULL;
}
int mid_hchdfd_Initial()
{   
	if (SSTAR_IVE_OpenLibrary(&g_stIveAssembly))
	{
		printf("load mi_ive lib failed\n");
		return -1;
	}

    Module_SAD();
    cfg.target_height = ALIGN_UP(HCFD_RAW_H, 32);   //align 32
    cfg.target_width = HCFD_RAW_W;                  //align 32
    cfg.max_detection = HCHD_DETECT_MAX-1;
    cfg.nms_thresh = 0.5;
    cfg.prob_thresh = HCHD_PROBABILITY;
    cfg.num_threads = 1;

    if (Init_Network(&ntwk_handle, &cfg))
    {
        printf("[HD] Could not create Network handle\n");
        return -1;
    }
    printf("mid_hchdfd_Initial\n");
    /*if(USE_HC_FD==1)Change_Model(ntwk_handle, 1);//切换hc。
    else if(USE_HC_FD==2)Change_Model(ntwk_handle, 0);          //切换FD。
    //only libFD_HC_XS_ARM.a
    */
    if (SAD_BLOCK_SIZE == 4)
    {
        sad_ctrl.eMode = E_MI_IVE_SAD_MODE_MB_4X4;
    }
    else if (SAD_BLOCK_SIZE == 8)
    {
        sad_ctrl.eMode = E_MI_IVE_SAD_MODE_MB_8X8;
    }
    else
    {
        sad_ctrl.eMode = E_MI_IVE_SAD_MODE_MB_16X16;
    }
    sad_ctrl.eOutCtrl = E_MI_IVE_SAD_OUT_CTRL_16BIT_BOTH;

    sad_ctrl.u16Thr = SAD_BLOCK_SIZE * SAD_BLOCK_SIZE * MD_PIXEL_DIFF;
    sad_ctrl.u8MinVal = 0;
    sad_ctrl.u8MaxVal = 255;

    Set_Input_Min_Max_Area(ntwk_handle, 128*128, 228*160);
    g_hchdfdExit = FALSE;
    pthread_create(&g_pthread_hchdfd, NULL, mid_hchdfd_Task, NULL);
    pthread_setname_np(g_pthread_hchdfd , "mid_hchdfd_Task");
    return 0;
}
int mid_hchdfd_Uninitial()
{
    MI_S32 ret = 0;
    g_hchdfdExit = TRUE;
    pthread_join(g_pthread_hchdfd, NULL);

    memcpy(&RGB_image1,&RGB_image1_bak,sizeof(MI_IVE_Image_t));

    ret = ModuleTest_FreeImage(&RGB_image1);
    memset(&RGB_image1_bak,0,sizeof(MI_IVE_Image_t));
    printf("line[%d] ret=%d\n",__LINE__,ret);

    ret = ModuleTest_FreeImage(&SadResult);
    printf("line[%d] ret=%d\n",__LINE__,ret);

    ret = ModuleTest_FreeImage(&ThdResult);
    printf("line[%d] ret=%d\n",__LINE__,ret);

    Release_Network(&ntwk_handle);
    ret = g_stIveAssembly.pfnIveDestroy(ive_handle);
    printf("line[%d] ret=%d\n",__LINE__,ret);
    ive_handle = NULL;

    SSTAR_IVE_CloseLibrary(&g_stIveAssembly);

    return 0;
}

#endif
