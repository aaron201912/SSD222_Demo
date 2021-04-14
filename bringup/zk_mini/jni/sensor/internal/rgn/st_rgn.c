#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <stdint.h>


#include "st_rgn.h"
#include "sstar_dynamic_load.h"

#if 0 // FD_ENABLE

#define ARGB1555_RED    (0x7c00 | 0x8000)
#define ARGB1555_GREEN  (0x03e0 | 0x8000)
#define ARGB1555_BLUE   (0x001f | 0x8000)


static MI_RGN_PaletteTable_t _gstPaletteTable =
{
    {{0, 0, 0, 0}, {255, 255, 0, 0}, {255, 0, 255, 0}, {255, 0, 0, 255},
    {255, 255, 255, 0}, {255, 0, 112, 255}, {255, 0, 255, 255}, {255, 255, 255, 255},
    {255, 128, 0, 0}, {255, 128, 128, 0}, {255, 128, 0, 128}, {255, 0, 128, 0},
    {255, 0, 0, 128}, {255, 0, 128, 128}, {255, 128, 128, 128}, {255, 64, 64, 64}}
};

static MI_RGN_InitParam_t g_stInitParam =
{
	.pstPaletteTable = &_gstPaletteTable
};

static pthread_mutex_t g_rgnOsd_mutex = PTHREAD_MUTEX_INITIALIZER;

static RgnAssembly_t g_stRgnAssembly;

// I4 pt.x % 2 == 0, w % 2 == 0
// I2 pt.x % 4 == 0, w % 4 == 0

void DrawPoint(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stPt, DrawRgnColor_t stColor)
{
    switch(stColor.ePixelFmt)
    {
        case E_MI_RGN_PIXEL_FORMAT_I4:
            {
                MI_U8 *pDrawBase = (MI_U8*)pBaseAddr;
                if (stPt.u16X % 2)
                    *(pDrawBase+(u32Stride*stPt.u16Y)+stPt.u16X/2) |= stColor.u32Color&0x0f;    // copy 1 byte
                else
                    *(pDrawBase+(u32Stride*stPt.u16Y)+stPt.u16X/2) |= (stColor.u32Color&0x0f) << 4;
            }
            break;
        case E_MI_RGN_PIXEL_FORMAT_ARGB1555:
            {
                MI_U16 *pDrawBase = (MI_U16*)pBaseAddr;
                *(pDrawBase+(u32Stride/2*stPt.u16Y)+stPt.u16X) = stColor.u32Color & 0xffff;      // copy 2 byte, app check alignment
            }
            break;
        default:
            printf("format not support\n");
    }
}

void DrawLine(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stStartPt, DrawPoint_t stEndPt, MI_U8 u8BorderWidth, DrawRgnColor_t stColor)
{
    MI_U32 i = 0;
    MI_S32 j = 0;
    DrawPoint_t stPt;
    MI_U32 u32Width = stEndPt.u16X - stStartPt.u16X + 1;
    MI_U32 u32Height = stEndPt.u16Y - stStartPt.u16Y + 1;

    if ( (u8BorderWidth > u32Width/2) || (u8BorderWidth > u32Height/2) )
    {
        printf("invalid border width\n");
        return;
    }

    for (i = 0; i < (stEndPt.u16X - stStartPt.u16X); i++)
    {
        for (j = (1-u8BorderWidth)/2; j <= u8BorderWidth/2; j++)
        {
            stPt.u16X = (stStartPt.u16X+j) + i;
            stPt.u16Y = stStartPt.u16Y + u32Height * (j+i) / u32Width;
            DrawPoint(pBaseAddr, u32Stride, stPt, stColor);
        }
    }
}

void DrawRect(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stLetfTopPt, DrawPoint_t stRightBottomPt, MI_U8 u8BorderWidth, DrawRgnColor_t stColor)
{
    MI_U32 i = 0;
    MI_U32 j = 0;
    MI_U32 u32Width = stRightBottomPt.u16X - stLetfTopPt.u16X + 1;
    MI_U32 u32Height = stRightBottomPt.u16Y - stLetfTopPt.u16Y + 1;

    if ( (u8BorderWidth > u32Width/2) || (u8BorderWidth > u32Height/2) )
    {
    //    printf("invalid border width\n");
        return;
    }

    switch(stColor.ePixelFmt)
    {
        case E_MI_RGN_PIXEL_FORMAT_I4:
            {
                MI_U8 *pDrawBase = (MI_U8*)pBaseAddr;

                if (stLetfTopPt.u16X%2 || u32Width%2)
                {
                    printf("invalid rect position\n");
                    return;
                }

                for (i = 0; i < u32Width/2; i++)
                {
                    for (j = 0; j < u8BorderWidth; j++)
                    {
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+j)+stLetfTopPt.u16X/2+i) = (stColor.u32Color&0x0f) | ((stColor.u32Color&0x0f) << 4);          // copy 1 byte
                        *(pDrawBase+u32Stride*(stRightBottomPt.u16Y-j)+stLetfTopPt.u16X/2+i) = (stColor.u32Color&0x0f) | ((stColor.u32Color&0x0f) << 4);      // copy 1 byte
                    }
                }

                for (i = 0; i < u32Height; i++)
                {
                    for (j = 0; j < u8BorderWidth/2; j++)
                    {
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X/2+j) = (stColor.u32Color&0x0f) | ((stColor.u32Color&0x0f) << 4);          // copy 1 byte
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stRightBottomPt.u16X/2-j) = (stColor.u32Color&0x0f) | ((stColor.u32Color&0x0f) << 4);      // copy 1 byte
                    }

                    if (u8BorderWidth % 2)
                    {
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X/2+u8BorderWidth/2) |= stColor.u32Color&0x0f;
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stRightBottomPt.u16X/2-u8BorderWidth/2) |= (stColor.u32Color&0x0f) << 4;
                    }
                }
            }
            break;
        case E_MI_RGN_PIXEL_FORMAT_ARGB1555:
            {
                MI_U16 *pDrawBase = (MI_U16*)pBaseAddr;

                for (i = 0; i < u32Width; i++)
                {
                    for (j = 0; j < u8BorderWidth; j++)
                    {
                        *(pDrawBase+u32Stride/2*(stLetfTopPt.u16Y+j)+stLetfTopPt.u16X+i) = stColor.u32Color & 0xffff;            // copy 2 byte, app check alignment
                        *(pDrawBase+u32Stride/2*(stRightBottomPt.u16Y-j)+stLetfTopPt.u16X+i) = stColor.u32Color & 0xffff;        // copy 2 byte, app check alignment
                    }
                }

                for (i = 0; i < u32Height; i++)
                {
                    for (j = 0; j < u8BorderWidth; j++)
                    {
                        *(pDrawBase+u32Stride/2*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X+j) = stColor.u32Color & 0xffff;            // copy 2 byte, app check alignment
                        *(pDrawBase+u32Stride/2*(stLetfTopPt.u16Y+i)+stRightBottomPt.u16X-j) = stColor.u32Color & 0xffff;        // copy 2 byte, app check alignment
                    }
                }
            }
            break;
        default:
            printf("format not support\n");
    }
}

void FillRect(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stLetfTopPt, DrawPoint_t stRightBottomPt, DrawRgnColor_t stColor)
{
    MI_U32 i = 0;
    MI_U32 j = 0;
    MI_U32 u32Width = stRightBottomPt.u16X - stLetfTopPt.u16X + 1;
    MI_U32 u32Height = stRightBottomPt.u16Y - stLetfTopPt.u16Y + 1;

    for (i = 0; i < u32Height; i++)
    {
        switch(stColor.ePixelFmt)
        {
            case E_MI_RGN_PIXEL_FORMAT_I4:
                {
                    MI_U8 *pDrawBase = (MI_U8*)pBaseAddr;

                    if (stLetfTopPt.u16X%2 || u32Width%2)
                    {
                        printf("invalid rect position\n");
                        return;
                    }

                    for (j = 0; j < u32Width/2; j++)
                    {
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X/2+j) = (stColor.u32Color&0x0f) | ((stColor.u32Color&0x0f) << 4);
                    }

                    if (u32Width % 2)
                    {
                        *(pDrawBase+u32Stride*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X/2+u32Width/2) = stColor.u32Color & 0x0f;
                    }
                }
                break;
            case E_MI_RGN_PIXEL_FORMAT_ARGB1555:
                {
                    MI_U16 *pDrawBase = (MI_U16*)pBaseAddr;

                    for (j = 0; j < u32Width; j++)
                    {
                        *(pDrawBase+u32Stride/2*(stLetfTopPt.u16Y+i)+stLetfTopPt.u16X+j) = stColor.u32Color & 0xffff;
                    }
                }
                break;
            default:
                printf("format not support\n");
        }
    }
}

void DrawCircular(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stCenterPt, MI_U32 u32Radius, MI_U8 u8BorderWidth, DrawRgnColor_t stColor)
{

}

void FillCircular(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stCenterPt, MI_U32 u32Radius, DrawRgnColor_t stColor)
{

}

MI_S32 ClearRgnRect(MI_RGN_CanvasInfo_t *pstRgnCanvasInfo)
{
    switch(pstRgnCanvasInfo->ePixelFmt)
    {
        case E_MI_RGN_PIXEL_FORMAT_ARGB1555:
            memset((void*)pstRgnCanvasInfo->virtAddr, 0x23, pstRgnCanvasInfo->stSize.u32Height*pstRgnCanvasInfo->u32Stride);
            break;
        default:
            printf("only support argb1555 & I4 now\n");
            return -1;
    }

    return 0;
}

int ST_RGN_Init(void)
{
	if (SSTAR_RGN_OpenLibrary(&g_stRgnAssembly))
	{
		printf("load mi_rgn lib failed\n");
		return -1;
	}

	if (MI_RGN_OK != g_stRgnAssembly.pfnRgnInitDev(&g_stInitParam))
    {
        printf("MI_RGN_Init fail\n");
        return -1;
    }

    return 0;
}

int ST_RGN_Create(MI_RGN_HANDLE hHandle)
{
    MI_RGN_ChnPort_t stRgnChnPort;
    MI_RGN_Attr_t stRgnAttr;
    MI_RGN_ChnPortParam_t stChnAttr;

    // create rgn
    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    memset(&stChnAttr, 0, sizeof(MI_RGN_ChnPortParam_t));

    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = 0;
    stRgnChnPort.s32OutputPortId = 0;
    stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
    stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_ARGB1555;

    stRgnAttr.stOsdInitParam.stSize.u32Width = 1024;
    stRgnAttr.stOsdInitParam.stSize.u32Height = 600;

    stChnAttr.stPoint.u32X = 0;
    stChnAttr.stPoint.u32Y = 0;
    stChnAttr.bShow = TRUE;
    stChnAttr.unPara.stOsdChnPort.u32Layer = 0;
    stChnAttr.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
    stChnAttr.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
    stChnAttr.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;

    if (MI_RGN_OK != g_stRgnAssembly.pfnRgnCreate(hHandle, &stRgnAttr))
    {
        printf("MI_RGN_Create fail\n");
        return -1;
    }

    if (MI_RGN_OK != g_stRgnAssembly.pfnRgnAttachToChn(hHandle, &stRgnChnPort, &stChnAttr))
    {
        printf("MI_RGN_AttachToChn fail\n");
        return -1;
    }

    return 0;
}

int ST_RGN_Destroy(MI_RGN_HANDLE hHandle)
{
    MI_RGN_ChnPort_t stRgnChnPort;

    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = 0;
    stRgnChnPort.s32OutputPortId = 0;

    if (MI_RGN_OK != g_stRgnAssembly.pfnRgnDetachFromChn(hHandle, &stRgnChnPort))
    {
        printf("MI_RGN_DetachFromChn fail\n");
        return -1;
    }

    if (MI_RGN_OK != g_stRgnAssembly.pfnRgnDestroy(hHandle))
    {
        printf("MI_RGN_Destroy fail\n");
        return -1;
    }

    return 0;
}

int ST_RGN_Deinit(void)
{
	if (MI_RGN_OK != g_stRgnAssembly.pfnRgnDeInitDev())
    {
        printf("MI_RGN_DeInit fail\n");
        return -1;
    }

	SSTAR_RGN_CloseLibrary(&g_stRgnAssembly);

    return 0;
}

int ST_RGN_DrawRect(MI_RGN_HANDLE hHandle,ST_RGN_Rect_t *pstDrawRect,MI_S32 s32RectCnt)
{
    MI_S32 s32Ret = 0;
    MI_RGN_CanvasInfo_t stRgnCanvasInfo;
    MI_S32 i = 0;
    
    memset(&stRgnCanvasInfo, 0, sizeof(stRgnCanvasInfo));
    pthread_mutex_lock(&g_rgnOsd_mutex);

    s32Ret = g_stRgnAssembly.pfnRgnGetCanvasInfo(hHandle, &stRgnCanvasInfo);
    if (s32Ret != MI_RGN_OK)
    {
        printf("%s; MI_RGN_GetCanvasInfo error, handle=%d\n", __FUNCTION__, hHandle);
        g_stRgnAssembly.pfnRgnUpdateCanvas(hHandle);
        pthread_mutex_unlock(&g_rgnOsd_mutex);
        return -1;
    }

    ClearRgnRect(&stRgnCanvasInfo);

    DrawRgnColor_t stColor;
    DrawPoint_t stLefTopPt;
    DrawPoint_t stRightBottomPt;
    
    for (i = 0; i < s32RectCnt; i++)
    {
        stColor.ePixelFmt = stRgnCanvasInfo.ePixelFmt;
        stLefTopPt.u16X = pstDrawRect[i].u16LeftTopX;
        stLefTopPt.u16Y = pstDrawRect[i].u16LeftTopY;
        stRightBottomPt.u16X = pstDrawRect[i].u16RightBottomX;
        stRightBottomPt.u16Y = pstDrawRect[i].u16RightBottomY;

        stColor.u32Color = ARGB1555_GREEN;
        DrawRect((void*)stRgnCanvasInfo.virtAddr, stRgnCanvasInfo.u32Stride, stLefTopPt, stRightBottomPt, 3, stColor);
    }

    g_stRgnAssembly.pfnRgnUpdateCanvas(hHandle);
    pthread_mutex_unlock(&g_rgnOsd_mutex);

    return 0;
    
}

int ST_RGN_ClearRect(MI_RGN_HANDLE hHandle)
{
    MI_S32 s32Ret = 0;
    MI_RGN_CanvasInfo_t stRgnCanvasInfo;
    
    memset(&stRgnCanvasInfo, 0, sizeof(stRgnCanvasInfo));
    pthread_mutex_lock(&g_rgnOsd_mutex);
    s32Ret = g_stRgnAssembly.pfnRgnGetCanvasInfo(hHandle, &stRgnCanvasInfo);

    if (s32Ret != MI_RGN_OK)
    {
        printf("%s; MI_RGN_GetCanvasInfo error, handle=%d\n", __FUNCTION__, hHandle);
        g_stRgnAssembly.pfnRgnUpdateCanvas(hHandle);
        pthread_mutex_unlock(&g_rgnOsd_mutex);
        return -1;
    }

    ClearRgnRect(&stRgnCanvasInfo);

    g_stRgnAssembly.pfnRgnUpdateCanvas(hHandle);
    pthread_mutex_unlock(&g_rgnOsd_mutex);

    return 0;
}


#endif

