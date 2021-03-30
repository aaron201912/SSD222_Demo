/*
 * scanner.h
 *
 *  Created on: 2021年3月12日
 *      Author: koda.xu
 */

#ifndef JNI_SCANNER_SCANNER_H_
#define JNI_SCANNER_SCANNER_H_


typedef void (*GetScanResultCallback)(char *pStr);

int SSTAR_Scanner_Init(int x, int y, int dispWidth, int dispHeight, GetScanResultCallback pfnGetScanResultCallback);
void SSTAR_Scanner_Deinit();

#endif /* JNI_SCANNER_SCANNER_H_ */
