#ifndef    __MI_PSPI_H
#define    __MI_PSPI_H

#ifdef    __cplusplus
extern "C" {
#endif


#include "mi_common_datatype.h"


MI_S32 MI_PSPI_CreateDevice(MI_PSPI_DEV stPspiDev, MI_PSPI_Param_t *  pstPspiParam);
MI_S32 MI_PSPI_DestroyDevice(MI_PSPI_DEV  stPspiDev);
MI_S32 MI_PSPI_Transfer(MI_PSPI_DEV  stPspiDev, MI_PSPI_Msg_t *pstMsg);
MI_S32 MI_PSPI_SetDevAttr(MI_PSPI_DEV  stPspiDev, MI_PSPI_Param_t *  pstPspiParam);
MI_S32 MI_PSPI_SetOutputAttr(MI_PSPI_OutputAttr_t  *stOutputAttr);
MI_S32 MI_PSPI_Disable(MI_PSPI_DEV  stPspiDev);
MI_S32 MI_PSPI_Enable(MI_PSPI_DEV  stPspiDev);


#ifdef     __cplusplus
}
#endif

#endif
