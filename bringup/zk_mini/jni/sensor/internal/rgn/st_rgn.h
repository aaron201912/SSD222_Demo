#ifndef __DRAWRHN_H__
#define _DRAWRGN_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "mi_sys_datatype.h"
#include "mi_rgn_datatype.h"
#include "mi_rgn.h"

#if 0	// FD_ENABLE

typedef struct
{
    MI_U32 u16X;
    MI_U32 u16Y;
}DrawPoint_t;

typedef struct
{
    MI_U16 u16LeftTopX;
    MI_U16 u16LeftTopY;
    MI_U16 u16RightBottomX;
    MI_U16 u16RightBottomY;
}ST_RGN_Rect_t;

typedef struct
{
    MI_RGN_PixelFormat_e ePixelFmt;
    MI_U32 u32Color;
}DrawRgnColor_t;

typedef struct
{
    ST_RGN_Rect_t stRect;
    MI_RGN_PixelFormat_e ePixelFmt;
    MI_RGN_ChnPort_t stRgnChnPort;
    MI_U32 u32Layer;
    char szBitmapFile[64];
    char szAsciiBitmapFile[64];
} ST_RGN_Attr_T;

void DrawPoint(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stPt, DrawRgnColor_t stColor);
void DrawLine(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stStartPt, DrawPoint_t stEndPt, MI_U8 u8BorderWidth, DrawRgnColor_t stColor);
void DrawRect(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stLetfTopPt, DrawPoint_t stRightBottomPt, MI_U8 u8BorderWidth, DrawRgnColor_t stColor);
void FillRect(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stLetfTopPt, DrawPoint_t stRightBottomPt, DrawRgnColor_t stColor);
void DrawCircular(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stCenterPt, MI_U32 u32Radius, MI_U8 u8BorderWidth, DrawRgnColor_t stColor);
void FillCircular(void *pBaseAddr, MI_U32 u32Stride, DrawPoint_t stCenterPt, MI_U32 u32Radius, DrawRgnColor_t stColor);

int ST_RGN_Init(void);
int ST_RGN_Create(MI_RGN_HANDLE hHandle);
int ST_RGN_Destroy(MI_RGN_HANDLE hHandle);
int ST_RGN_Deinit(void);
int ST_RGN_DrawRect(MI_RGN_HANDLE hHandle,ST_RGN_Rect_t *pstDrawRect,MI_S32 s32RectCnt);
int ST_RGN_ClearRect(MI_RGN_HANDLE hHandle);

#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DRAWRGN_H__
