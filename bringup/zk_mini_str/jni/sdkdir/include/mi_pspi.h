#ifndef    __MI_PSPI_H
#define    __MI_PSPI_H

#ifdef    __cplusplus
extern "C" {
#endif


#include "mi_common_datatype.h"


MI_S32 MI_PSPI_Init(MI_PSPI_DEV * stPspiDev, MI_PSPI_SpiParam_t *  stSpiParam);
MI_S32 MI_PSPI_DeInit(MI_PSPI_DEV * stPspiDev);
MI_S32 MI_PSPI_SetParam(MI_PSPI_DEV * stPspiDev, MI_PSPI_Param *pstParam);
MI_S32 MI_PSPI_Config(MI_PSPI_DEV * stPspiDev, MI_PSPI_SpiParam_t *  stSpiParam);
MI_S32 MI_PSPI_SetChnAttr(MI_PSPI_ChnAttr_t stOutputChnAttr);
MI_S32 MI_PSPI_Disable(MI_PSPI_DEV_TYPE_e  ePspiType);
MI_S32 MI_PSPI_Enable(MI_PSPI_DEV_TYPE_e  ePspiType);


#ifdef     __cplusplus
}
#endif

#endif
