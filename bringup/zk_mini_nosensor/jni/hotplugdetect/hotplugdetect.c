/*
 * hotplugdetect.c
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */


#include "hotplugdetect.h"

void SSTAR_ConnectWifi(MI_WLAN_ConnectParam_t *pstConnParam)
{
	Wifi_Connect(pstConnParam);
}

void SSTAR_DisconnectWifi()
{
	Wifi_Disconnect();
}

int SSTAR_GetWifiEnableStatus()
{
	return Wifi_GetEnableStatus();
}

void SSTAR_SetWifiEnableStatus(int enable)
{
	Wifi_SetEnableStatus(enable);
}

int SSTAR_GetWifiSupportStatus()
{
	return Wifi_GetSupportStatus();
}

int SSTAR_GetWifiCurConnStatus(MI_WLAN_Status_t *status)
{
	return Wifi_GetCurConnStatus(status);
}

int SSTAR_GetWifiLastConnStatus()
{
	return Wifi_GetLastConnStatus();
}

int SSTAR_InitWifiApMode()
{
	return Wifi_InitAp();
}

void SSTAR_DeinitWifiApMode()
{
	Wifi_DeinitAp();
}

int SSTAR_EnableWifiApMode(AirportInfo_t *pApInfo, WifiGetApInfoCallback pfnCallback)
{
	return Wifi_EnableAp(pApInfo, pfnCallback);
}

void SSTAR_DisableWifiApMode()
{
	Wifi_DisableAp();
}

char *SSTAR_GetAirportName()
{
	return Wifi_GetApName();
}

char *SSTAR_GetAirportPasswd()
{
	return Wifi_GetApPasswd();
}

int SSTAR_GetWiredNetworkCurrentStatus()
{
	return 1;
}

int SSTAR_GetUsbCurrentStatus()
{
	return USB_CheckCurrentStatus();
}

void SSTAR_GetUsbPath(char *pUsbPath, int nLen)
{
	USB_GetUsbPath(pUsbPath, nLen);
}

int SSTAR_RegisterWifiStaConnListener(WifiConnCallback pfnCallback)
{
	return Wifi_RegisterConnectCallback(pfnCallback);
}

void SSTAR_UnRegisterWifiStaConnListener(WifiConnCallback pfnCallback)
{
	Wifi_UnRegisterConnectCallback(pfnCallback);
}

int SSTAR_RegisterWifiStaScanListener(WifiScanCallback pfnCallback)
{
	return Wifi_RegisterScanCallback(pfnCallback);
}

void SSTAR_UnRegisterWifiStaScanListener(WifiScanCallback pfnCallback)
{
	Wifi_UnRegisterScanCallback(pfnCallback);
}

int SSTAR_RegisterUsbListener(UsbHotplugCallback pfnCallback)
{
	return USB_RegisterCallback(pfnCallback);
}

void SSTAR_UnRegisterUsbListener(UsbHotplugCallback pfnCallback)
{
	USB_UnRegisterCallback(pfnCallback);
}

int SSTAR_RegisterWiredNetworkListener(NetHotplugCallback pfnCallback)
{
	return WiredNetwork_RegisterCallback(pfnCallback);
}

void SSTAR_UnRegisterWiredNetworkListener(NetHotplugCallback pfnCallback)
{
	WiredNetwork_UnRegisterCallback(pfnCallback);
}

int SSTAR_InitHotplugDetect()
{
	Wifi_StartCheckHotplug();
	USB_StartCheckHotplug();
	//WiredNetwork_StartCheckHotplug();

	return 0;
}

void SSTAR_DeinitHotPlugDetect()
{
	Wifi_StopCheckHotplug();
	USB_StopCheckHotplug();
	//WiredNetwork_StopCheckHotplug();
}
