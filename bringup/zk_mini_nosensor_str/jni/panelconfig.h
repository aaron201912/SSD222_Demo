/*
 * panelconfig.h
 *
 *  Created on: 2019年10月22日
 *      Author: koda.xu
 */

#ifndef JNI_PANELCONFIG_H_
#define JNI_PANELCONFIG_H_
#include "appconfig.h"
#include "mi_panel_datatype.h"

extern MI_PANEL_ParamConfig_t stPanelParam;
extern MI_PANEL_MipiDsiConfig_t stMipiDsiConfig;

#if USE_PANEL_1024_600
#define PANEL_WIDTH			1024
#define PANEL_HEIGHT		600
#else
#define PANEL_WIDTH			800
#define PANEL_HEIGHT		480
#endif

#endif /* JNI_PANELCONFIG_H_ */
