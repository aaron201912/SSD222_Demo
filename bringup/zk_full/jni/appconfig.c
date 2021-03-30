/*
 * panelconfig.c
 *
 *  Created on: 2019年10月22日
 *      Author: koda.xu
 */

#include "appconfig.h"

static LanguageCode_e g_eLocalLanguageCode = E_LANGUAGE_CHINESE;

LanguageCode_e SSTAR_GetLocalLanguageCode()
{
	return g_eLocalLanguageCode;
}

void SSTAR_SetLocalLanguageCode(LanguageCode_e languageCode)
{
	g_eLocalLanguageCode = languageCode;
}
