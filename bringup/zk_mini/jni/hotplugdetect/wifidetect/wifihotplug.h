/*
 * wifihotplug.h
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */

#ifndef JNI_HOTPLUGDETECT_WIFIDETECT_WIFIHOTPLUG_H_
#define JNI_HOTPLUGDETECT_WIFIDETECT_WIFIHOTPLUG_H_


#ifdef  __cplusplus
extern "C"
{
#endif

#include "wifiprofile.h"

typedef struct
{
	WLAN_HANDLE hWlan;
	MI_WLAN_ConnectParam_t stConnParam;
}WifiConnInfo_t;

typedef struct
{
	char ssid[MI_WLAN_MAX_SSID_LEN];
	char mac[MI_WLAN_MAX_MAC_LEN];
	bool bEncrypt;
	int signalSTR;
} ScanResult_t;

typedef struct
{
	char name[MI_WLAN_MAX_SSID_LEN];
	char passwd[MI_WLAN_MAX_PASSWD_LEN];
} AirportInfo_t;

typedef void (*WifiConnCallback)(char *pSsid, int status, int quality);
typedef void (*WifiScanCallback)(ScanResult_t *pstScanResult, int resCnt);
typedef void (*WifiGetApInfoCallback)(MI_WLAN_Status_t *stApStatus);

int Wifi_StartCheckHotplug();
void Wifi_StopCheckHotplug();
int Wifi_RegisterConnectCallback(WifiConnCallback pfnCallback);
void Wifi_UnRegisterConnectCallback(WifiConnCallback pfnCallback);
int Wifi_RegisterScanCallback(WifiScanCallback pfnCallback);
void Wifi_UnRegisterScanCallback(WifiScanCallback pfnCallback);

void Wifi_Connect(MI_WLAN_ConnectParam_t *pstConnParam);
void Wifi_Disconnect();
int Wifi_GetEnableStatus();
void Wifi_SetEnableStatus(int enable);
int Wifi_GetSupportStatus();
int Wifi_GetCurConnStatus(MI_WLAN_Status_t *status);
int Wifi_GetLastConnStatus();

int Wifi_InitAp();
void Wifi_DeinitAp();
int Wifi_EnableAp(AirportInfo_t *pApInfo, WifiGetApInfoCallback pfnCallback);
void Wifi_DisableAp();
char *Wifi_GetApName();
char *Wifi_GetApPasswd();

#ifdef  __cplusplus
}
#endif

#endif /* JNI_HOTPLUGDETECT_WIFIDETECT_WIFIHOTPLUG_H_ */
