/*
 * appconfig.h
 *
 *  Created on: 2019年10月22日
 *      Author: koda.xu
 */

#ifndef JNI_APPCONFIG_H_
#define JNI_APPCONFIG_H_

#define USE_PANEL_1024_600		1
#define USE_AMIC				0
#define ENABLE_BF				0
#define ENABLE_ROTATE			0

#define DSPOTTER_DATA_PATH			"/customer/res/DSpotter/data"

#define CREADER_SAVE_FILE			0
#define CREADER_LIB_PATH			"/customer/lib"
#define CREADER_DATA_PATH			"/customer/res/CReader/data"

typedef enum
{
	E_LANGUAGE_CHINESE = 0,
	E_LANGUAGE_ENGLISH,
	E_LANGUAGE_BUTT
} LanguageCode_e;

LanguageCode_e SSTAR_GetLocalLanguageCode();
void SSTAR_SetLocalLanguageCode(LanguageCode_e languageCode);

#endif /* JNI_APPCONFIG_H_ */
