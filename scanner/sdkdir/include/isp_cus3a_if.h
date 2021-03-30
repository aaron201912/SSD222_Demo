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

#ifndef _ISP_CUS3A_IF_H_
#define _ISP_CUS3A_IF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mi_cus3a_datatype.h"

unsigned int CUS3A_GetVersion(char* pVerStr);
int CUS3A_Init(void);
void CUS3A_Release(void);
int CUS3A_RegInterface(MI_U32 nCh,ISP_AE_INTERFACE *pAE,ISP_AWB_INTERFACE *pAWB,ISP_AF_INTERFACE *pAF); /*This function is deprecated, use CUS3A_RegInterfaceEX instead*/
int CUS3A_AERegInterface(MI_U32 nCh,ISP_AE_INTERFACE *pAE); /*This function is deprecated, use CUS3A_RegInterfaceEX instead*/
int CUS3A_AWBRegInterface(MI_U32 nCh,ISP_AWB_INTERFACE *pAWB); /*This function is deprecated, use CUS3A_RegInterfaceEX instead*/
int CUS3A_AFRegInterface(MI_U32 nCh,ISP_AF_INTERFACE *pAF); /*This function is deprecated, use CUS3A_RegInterfaceEX instead*/
int CUS3A_RegInterfaceEX(MI_U32 nCh ,CUS3A_ALGO_ADAPTOR_e eAdaptor, CUS3A_ALGO_TYPE_e eType, void* pAlgo);
int CUS3A_SetAlgoAdaptor(CUS3A_ISP_CH_e nCh, CUS3A_ALGO_ADAPTOR_e eAdaptor, CUS3A_ALGO_TYPE_e eType);
int CUS3A_GetAlgoAdaptor(CUS3A_ISP_CH_e nCh, CUS3A_ALGO_TYPE_e eType);
int CUS3A_CreateChannel(CUS3A_ISP_CH_e eIspCh);
int CUS3A_DestroyChannel(CUS3A_ISP_CH_e eIspCh);

void* pAllocDmaBuffer(const char* pName, MI_U32 nReqSize, MI_U32 *pPhysAddr, MI_U32 *pMiuAddr, MI_U8 bCache); /*Do not use, This function is for SStar internal use only*/
int FreeDmaBuffer(const char* pName, MI_U32 u32MiuAddr, void *pVirtAddr, MI_U32 u32FreeSize); /*Do not use, This function is for SStar internal use only*/
int Cus3AOpenIspFrameSync(int *fd0, int *fd1);
int Cus3ACloseIspFrameSync(int fd0, int fd1);
unsigned int Cus3AWaitIspFrameSync(int fd0, int fd1, int timeout);
int CUS3A_AeAvgDownSample(const ISP_AE_SAMPLE *pInBuf, ISP_AE_SAMPLE *pOutBuf, unsigned int nInBlkX, unsigned int nInBlkY, unsigned int nOutBlkX, unsigned int nOutBlkY);
int CUS3A_GetAlgoStatus(CUS3A_ISP_CH_e eCh, CUS3A_ALGO_STATUS_t *pStatus);
#ifdef __cplusplus
}
#endif

#endif
