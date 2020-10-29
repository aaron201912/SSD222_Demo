#ifndef  __MI_PSPI_DATATYPE_H
#define  __MI_PSPI_DATATYPE_H

#ifdef   __cplusplus
extern "C" {
#endif

#include "mi_sys_datatype.h"
#include <linux/types.h>
#include "mi_common_datatype.h"

typedef  void   PSPI_HANDLE;

#define   PSPI_SET_PARAM_BUFF_SIZE     8

#define   USE_CMDQ_FOR_PSPI       TRUE

typedef struct{
    MI_U16  tx_size;
    MI_U16  rx_size;
    MI_U8  tx_bitcount;
    MI_U8  rx_bitcount;
    MI_U16  tx_buf[PSPI_SET_PARAM_BUFF_SIZE];
    MI_U16  rx_buf[PSPI_SET_PARAM_BUFF_SIZE];
}MI_PSPI_Param;

typedef struct
{                                
    MI_U32 buff_addr;
    MI_U32 buff_size;
}MI_PSPI_Frame;

typedef enum
{
    MI_PSPI_SENSOR = 0,
    MI_PSPI_PANEL,
    MI_PSPI_NULL_TYPE,
}MI_PSPI_DEV_TYPE_e;

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
#define DATA_QUAL     0x04

//--------rgb_swap
#define RGB_SINGLE   0x01
#define RGB_DUAL     0x02
#define BGR_SINGLE   0x04
#define BGR_DUAL     0x08

typedef enum
{
    MI_PSPI_0 = 0,
    MI_PSPI_1,
    MI_PSPI_NULL,
}MI_PSPI_CHIP_e;

typedef struct
{
    MI_SYS_PixelFormat_e  ePixelFormat;
    MI_U16 u16Width;
    MI_U16 u16Height;
}MI_PSPI_ChnAttr_t;

typedef struct
{
    MI_U32 max_speed_hz;
    MI_U16 delay_cycle;                             /* cs is inactive*/
    MI_U16 wait_cycle;                              /* cs is active  */
    MI_U16  spi_mode;
    MI_U8  data_lane;                               /* cs count      */
    MI_U8  bits_per_word;                         /* The number of bits in an SPI transmission*/
    MI_U8  rgb_swap;                                /* for panel     */
    MI_U8  te_mode;
}MI_PSPI_SpiParam_t;


typedef struct
{
	MI_U8   chip_select;
#define  MI_PSPI_SELECT_0      0
#define  MI_PSPI_SELECT_1      1
#define  MI_PSPI_SELECT_NULL 2
	MI_PSPI_CHIP_e pspi_chip;
	MI_PSPI_DEV_TYPE_e  pspi_type;
}MI_PSPI_DEV;



#define MI_PSPI_SUCCESS (0)
#define MI_PSPI_FAIL    (-1)
#ifdef   __cplusplus
}
#endif

#endif











