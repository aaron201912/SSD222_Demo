/*
 * hotplugdetect.h
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */

#ifndef JNI_HOTPLUGDETECT_HOTPLUGDETECT_H_
#define JNI_HOTPLUGDETECT_HOTPLUGDETECT_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "usbhotplug.h"
#include "wifihotplug.h"
#include "wirednetwork.h"

void SSTAR_ConnectWifi(MI_WLAN_ConnectParam_t *pstConnParam);
void SSTAR_DisconnectWifi();
int SSTAR_GetWifiEnableStatus();
void SSTAR_SetWifiEnableStatus(int enable);
int SSTAR_GetWifiSupportStatus();
int SSTAR_GetWifiCurConnStatus(MI_WLAN_Status_t *status);
int SSTAR_GetWifiLastConnStatus();
int SSTAR_InitWifiApMode();
void SSTAR_DeinitWifiApMode();
int SSTAR_EnableWifiApMode(AirportInfo_t *pApInfo, WifiGetApInfoCallback pfnCallback);
void SSTAR_DisableWifiApMode();
char *SSTAR_GetAirportName();
char *SSTAR_GetAirportPasswd();

int SSTAR_GetWiredNetworkCurrentStatus();
int SSTAR_GetUsbCurrentStatus();
void SSTAR_GetUsbPath(char *pUsbPath, int nLen);

int SSTAR_RegisterWifiStaConnListener(WifiConnCallback pfnCallback);
void SSTAR_UnRegisterWifiStaConnListener(WifiConnCallback pfnCallback);
int SSTAR_RegisterWifiStaScanListener(WifiScanCallback pfnCallback);
void SSTAR_UnRegisterWifiStaScanListener(WifiScanCallback pfnCallback);
int SSTAR_RegisterUsbListener(UsbHotplugCallback pfnCallback);
void SSTAR_UnRegisterUsbListener(UsbHotplugCallback pfnCallback);
int SSTAR_RegisterWiredNetworkListener(NetHotplugCallback pfnCallback);
void SSTAR_UnRegisterWiredNetworkListener(NetHotplugCallback pfnCallback);
int SSTAR_InitHotplugDetect();
void SSTAR_DeinitHotPlugDetect();

#ifdef  __cplusplus
}
#endif

#endif /* JNI_HOTPLUGDETECT_HOTPLUGDETECT_H_ */
