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
#ifndef  __MI_PSPI_DATATYPE_H
#define  __MI_PSPI_DATATYPE_H

#ifdef   __cplusplus
extern "C" {
#endif

#include "mi_sys_datatype.h"
#include "mi_common_datatype.h"

typedef  void   PSPI_HANDLE;

#define   PSPI_PARAM_BUFF_SIZE     8


typedef struct{
    MI_U16  u16TxSize;
    MI_U16  u16RxSize;
    MI_U8   u8TxBitCount;
    MI_U8   u8RxBitCount;
    MI_U16  au16TxBuf[PSPI_PARAM_BUFF_SIZE];
    MI_U16  au16RxBuf[PSPI_PARAM_BUFF_SIZE];
} MI_PSPI_Msg_t ;



//----spi_mode
#define SPI_CPHA    0x01            /* clock phase    */
#define SPI_CPOL    0x02            /* clock polarity */
#define SPI_SSCTL   0x04
#define SPI_SSPOL   0x08
#define SPI_SLAVE   0x10
#define SPI_LSB     0x20


//------data_lane
#define DATA_SINGLE   0x01
#define DATA_DUAL     0x02
#define DATA_QUAD     0x04

//--------rgb_swap
#define RGB_SINGLE   0x01
#define RGB_DUAL     0x02
#define BGR_SINGLE   0x04
#define BGR_DUAL     0x08


//---------chip_select
#define  MI_PSPI_SELECT_0      0
#define  MI_PSPI_SELECT_1      1
#define  MI_PSPI_SELECT_NULL 2

typedef  MI_S32    MI_PSPI_DEV;



typedef struct
{
    MI_SYS_PixelFormat_e  ePixelFormat;
    MI_U16 u16Width;
    MI_U16 u16Height;
}MI_PSPI_OutputAttr_t;

typedef struct
{
    MI_U32  u32MaxSpeedHz;
    MI_U16  u16DelayCycle;                             /* cs is inactive*/
    MI_U16  u16WaitCycle;                              /* cs is active  */
    MI_U16  u16PspiMode;
    MI_U8   u8DataLane;                               /* cs count      */
    MI_U8   u8BitsPerWord;                         /* The number of bits in an SPI transmission*/
    MI_U8   u8RgbSwap;                                /* for panel     */
    MI_U8   u8TeMode;
    MI_U8   u8ChipSelect;
}MI_PSPI_Param_t;






#define MI_PSPI_SUCCESS                                      (0)
#define MI_PSPI_FAIL                                         (-1)
#define MI_ERR_PSPI_NULL_PTR                                 (-2)
#define MI_ERR_PSPI_NO_MEM                                   (-3)
#define MI_ERR_PSPI_ILLEGAL_PARAM                            (-4)
#define MI_ERR_PSPI_DEV_NOT_INIT                             (-5)
#define MI_ERR_PSPI_ENABLE_CHN_FAILED                        (-6)
#define MI_ERR_PSPI_ENABLE_PORT_FAILED                       (-7)
#define MI_ERR_PSPI_DISABLE_CHN_FAILED                       (-8)
#define MI_ERR_PSPI_DISABLE_PORT_FAILED                      (-9)
#define MI_ERR_PSPI_DEV_HAVE_INITED                          (-10)
#define MI_ERR_PSPI_FAILED_IN_MHAL                           (-11)

#ifdef   __cplusplus
}
#endif

#endif











