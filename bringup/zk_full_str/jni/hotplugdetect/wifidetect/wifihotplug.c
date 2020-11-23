/*
 * wifihotplug.c
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/bitypes.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include "wifihotplug.h"
#include "list.h"


typedef struct
{
	list_t callbackList;
	WifiConnCallback pfnCallback;
}WifiConnCallbackListData_t;

typedef struct
{
	list_t callbackList;
	WifiScanCallback pfnCallback;
}WifiScanCallbackListData_t;

typedef struct
{
	list_t scanResList;
	ScanResult_t stScanRes;
}WifiScanResultListData_t;

typedef struct
{
	list_t scanResListHead;
	int scanResCnt;
}WifiScanResultListHead_t;

static int g_wifiSupported = 1;			// 设备是否支持wifi且MI_WLAN初始化正常
static int g_wifiEnabled = 0;
static int g_manuaConnect = 0;			// 手动连接AP，在用户进行手动连接后会置为1
static int g_userConnect = 0;			// 用户选择ssid进行连接
static int g_registerConnChanged = 0;	// 注册callback列表新增或删减
static WLAN_HANDLE g_hWlan = -1;
static MI_WLAN_OpenParams_t g_stStaOpenParam = {E_MI_WLAN_NETWORKTYPE_INFRA};
MI_WLAN_OpenParams_t stApOpenParam = {E_MI_WLAN_NETWORKTYPE_AP};
static MI_WLAN_InitParams_t g_stParm = {"/config/wifi/wlan.json"};
static MI_WLAN_ConnectParam_t g_stConnParam;
static MI_WLAN_Status_t  g_stStatus;
static pthread_mutex_t g_connParamMutex;

static MI_WLAN_ConnectParam_t g_stConnectParam_ap = {E_MI_WLAN_SECURITY_WPA2, "ssw101bap", "12345678", 5000};

static pthread_t g_wlanThread = 0;
static int g_wlanThreadRun = 0;

static pthread_t g_getApStatusThread = 0;
static int g_getApStatusThreadRun = 0;

static pthread_t g_connThread = 0;
static int g_connThreadRun = 0;
static list_t g_connCallbackListHead;
static pthread_mutex_t g_connCallbackListMutex;

static pthread_t g_scanThread = 0;
static int g_scanThreadRun = 0;
static list_t g_scanCallbackListHead;
static pthread_mutex_t g_scanCallbackListMutex;

static pthread_t g_apStatusThread = 0;
static int g_apStatusThreadRun = 0;
static list_t g_apStatusCallbackListHead;
static pthread_mutex_t g_apStatusCallbackListMutex;

static int g_wifiStart = 0;

//static list_t g_scanResListHead;
static WifiScanResultListHead_t g_stScanResListHead;
static ScanResult_t *g_pstScanRes = NULL;
static int  g_staInit = 0;

unsigned long GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static void ClearScanResult()
{
	WifiScanResultListData_t *pstScanResData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	list_for_each_safe(pListPos, pListPosN, &g_stScanResListHead.scanResListHead)
	{
		pstScanResData = list_entry(pListPos, WifiScanResultListData_t, scanResList);
		list_del(pListPos);
		free(pstScanResData);
	}

	g_stScanResListHead.scanResCnt = 0;
}


static void SaveScanResult(char *curSsid, MI_WLAN_ScanResult_t *pstScanResult)
{
	for (int i = 0; i < pstScanResult->u8APNumber; i++)
	{
		char *pSsid = (char*)pstScanResult->stAPInfo[i].au8SSId;
		char saveSsid[MI_WLAN_MAX_SSID_LEN] = {0};

		if (pSsid && strcmp(pSsid, "\"\"") && strlen(pSsid))
		{
			WifiScanResultListData_t *pstScanResData = NULL;
			int len = strlen(pSsid);

			pstScanResData = (WifiScanResultListData_t*)malloc(sizeof(WifiScanResultListData_t));
			memset(pstScanResData, 0, sizeof(WifiScanResultListData_t));
			strncpy(saveSsid, pSsid+1, len-2);
			strncpy(pstScanResData->stScanRes.ssid, saveSsid, len-2);
			strcpy(pstScanResData->stScanRes.mac, (char*)pstScanResult->stAPInfo[i].au8Mac);
			pstScanResData->stScanRes.bEncrypt = pstScanResult->stAPInfo[i].bEncryptKey;
			pstScanResData->stScanRes.signalSTR = pstScanResult->stAPInfo[i].stQuality.signalSTR;
			g_stScanResListHead.scanResCnt++;


			//printf("curSSid=%s, saveSsid=%s, i=%d\n", curSsid, saveSsid, i);

			if (!strcmp(curSsid, saveSsid))
			{
				list_add(&pstScanResData->scanResList, &g_stScanResListHead.scanResListHead);
			}
			else
			{
				int hasInsert = 0;
				WifiScanResultListData_t *pos = NULL;

				list_for_each_entry(pos, &g_stScanResListHead.scanResListHead, scanResList)
				{
					if (pstScanResData->stScanRes.signalSTR > pos->stScanRes.signalSTR && strcmp(pos->stScanRes.ssid, curSsid))
					{
						list_add(&pstScanResData->scanResList, pos->scanResList.prev);
						hasInsert = 1;
						break;
					}
				}

				if (!hasInsert)
					list_add_tail(&pstScanResData->scanResList, &g_stScanResListHead.scanResListHead);
			}
		}
	}
}

static int WifiInit()
{
	// check wifi profile
	if (checkProfile() < 0)
	{
		printf("wlan config files error\n");
		return -1;
	}

	// get default param
	if (initWifiConfig())
	{
		printf("init wifi config error\n");
		return -1;
	}

	g_wifiSupported = getWifiSupportStatus();
	g_wifiEnabled = getWifiEnableStatus();
	g_hWlan = getWlanHandle();
	memcpy(&g_stConnParam, getConnectParam(), sizeof(MI_WLAN_ConnectParam_t));

	if (!g_wifiSupported)
	{
		printf("not support wifi\n");
		return -1;
	}

	// init wlan dev
	if (MI_WLAN_Init(&g_stParm) || MI_WLAN_Open(&g_stStaOpenParam))
	{
		//setWifiSupportStatus(false);
		g_wifiSupported = 0;
		printf("open wifi failed\n");
		return -1;
	}

	g_staInit = 1;
	return 0;
}

static void WifiDeinit()
{
	if (g_hWlan != -1)
	{
		MI_WLAN_Disconnect(g_hWlan);
	}

	WifiConnCallbackListData_t *pstConnCallbackData = NULL;
	WifiScanCallbackListData_t *pstScanCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_connCallbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_connCallbackListHead)
	{
		pstConnCallbackData = list_entry(pListPos, WifiConnCallbackListData_t, callbackList);
		list_del(pListPos);
		free(pstConnCallbackData);
	}
	pthread_mutex_unlock(&g_connCallbackListMutex);

	pthread_mutex_lock(&g_scanCallbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_scanCallbackListHead)
	{
		pstScanCallbackData = list_entry(pListPos, WifiScanCallbackListData_t, callbackList);
		list_del(pListPos);
		free(pstScanCallbackData);
	}
	pthread_mutex_unlock(&g_scanCallbackListMutex);

	if (g_staInit)
	{
		MI_WLAN_Close(&g_stStaOpenParam);
		sleep(3);			// test if the condition is needed
		MI_WLAN_DeInit();
		g_staInit = 0;
	}
	saveWifiConfig();

	g_manuaConnect = 0;
	g_userConnect = 0;
}

static void *WifiConnectProc(void *pdata)
{
    printf("Exec WifiConnectProc\n");

    int wifiEnabled = 0;
    int manuaConnect = 0;
    int userConnect = 0;
    int enableStatusChanged = 0;
    int registerListChanged = 0;
    int newConnect = 0;
	
    WLAN_HANDLE hWlan = 0;
    MI_WLAN_ConnectParam_t stConnParam;
    MI_WLAN_Status_t stStatus;
    int lastConnStatus = 0;
    int currentConnStatus = 0;
    int freshConnStatusCnt = 0;
    bool bFirstConnect = true;

    memset(&stConnParam, 0, sizeof(MI_WLAN_ConnectParam_t));
    memset(&stStatus, 0, sizeof(MI_WLAN_Status_t));

	while (g_connThreadRun)
	{
		pthread_mutex_lock(&g_connParamMutex);
		enableStatusChanged = (wifiEnabled != g_wifiEnabled);
		wifiEnabled = g_wifiEnabled;
		manuaConnect = g_manuaConnect;			// 是否主动call Connect, netpage3
		userConnect = g_userConnect;
		hWlan = g_hWlan;			// 手动连接前，根据连接ssid判断是否已经存在在配置中吗，获取handle，赋值g_hWlan
		memcpy(&stConnParam, &g_stConnParam, sizeof(MI_WLAN_ConnectParam_t));
		registerListChanged = g_registerConnChanged;
		g_registerConnChanged = 0;
		pthread_mutex_unlock(&g_connParamMutex);

		if (wifiEnabled && (manuaConnect || hWlan != -1))		// wifi使能且是手动连接AP或自动连接配置中已有的AP时，执行一次Connect操作
		{
			if (enableStatusChanged || userConnect)		// 当wifi由disable变为enable，或是有手动连接时，进行连接
			{
				MI_WLAN_Connect(&hWlan, &stConnParam);
				printf("Connect, conn param: id=%d, ssid=%s, passwd=%s\n", hWlan,
						(char*)stConnParam.au8SSId, (char*)stConnParam.au8Password);

				// 若是新的连接，输入handle为-1，执行连接后handle置为当前最大handle+1，与连接是否成功无关
				pthread_mutex_lock(&g_connParamMutex);
				g_hWlan = hWlan;
				g_userConnect = 0;
				pthread_mutex_unlock(&g_connParamMutex);

				// 仅手动连接才更新wifisetting.json
				if (manuaConnect)
					newConnect = 1;
			}

			MI_WLAN_GetStatus(g_hWlan, &stStatus);

			if(stStatus.stStaStatus.state == WPA_COMPLETED)
			{
				currentConnStatus = 1;

				// 判断当前连上的ssid是否存在于配置文件中
				Wifi_ConnectInfo_t stConn;

				checkSsidExist((char*)stStatus.stStaStatus.ssid, &stConn);

				//printf("checkSsidExist: ssid=%s, index=%d\n", (char*)stStatus.stStaStatus.ssid, stConn.index);

				memset(stConn.passwd, 0, sizeof(stConn.passwd));
				strcpy(stConn.passwd, (char*)stConnParam.au8Password);

				if (stConn.index == -1)	// 新增ssid
				{
					printf("add new ssid: hWlan is %d\n", hWlan);
					stConn.index = hWlan;
					printf("add new ssid: stConn index is %d\n", stConn.index);
					addSsidToApList((char*)stStatus.stStaStatus.ssid);
					addSsidInfoToConnList((char*)stStatus.stStaStatus.ssid, &stConn);
					updateCurSsid((char*)stStatus.stStaStatus.ssid);
					saveWifiConfig();
					setWlanHandle(hWlan);
					saveConnectParam(&stConnParam);
				}
				else	// 连接配置中存在的ssid
				{
					// 连接的ssid不是上一个ssid，或连接的ssid是上一个ssid但密码发生变化，更新ssid的信息
					if (newConnect)
					{
						printf("connParam changed: hWlan is %d\n", hWlan);
						stConn.index = hWlan;
						updateConnPasswd((char*)stStatus.stStaStatus.ssid, (char*)stConnParam.au8Password);
						updateCurSsid((char*)stStatus.stStaStatus.ssid);
						saveWifiConfig();
						setWlanHandle(hWlan);
						saveConnectParam(&stConnParam);
					}
				}

				newConnect = 0;
			}
			else
				currentConnStatus = 0;

			// 状态变化或间隔超3s时刷新连接状态
			if (lastConnStatus != currentConnStatus || registerListChanged || freshConnStatusCnt > 60)
			{
				WifiConnCallbackListData_t *pstWifiCallbackData = NULL;
				list_t *pListPos = NULL;
				freshConnStatusCnt = 0;

				pthread_mutex_lock(&g_connCallbackListMutex);
				list_for_each(pListPos, &g_connCallbackListHead)
				{
					pstWifiCallbackData = list_entry(pListPos, WifiConnCallbackListData_t, callbackList);

					// 如果连上，将界面列表第一项设为连上状态；如果断开，将界面列表第一项连上状态清除
					pstWifiCallbackData->pfnCallback((char*)stStatus.stStaStatus.ssid, currentConnStatus, stStatus.stStaStatus.RSSI);
				}
				pthread_mutex_unlock(&g_connCallbackListMutex);

				lastConnStatus = currentConnStatus;
			}
			else
				freshConnStatusCnt++;
		}
#if 1
		else if (!wifiEnabled && enableStatusChanged)
		{
			WifiConnCallbackListData_t *pstWifiCallbackData = NULL;
			list_t *pListPos = NULL;

			currentConnStatus = 0;
			lastConnStatus = 0;

			pthread_mutex_lock(&g_connCallbackListMutex);
			list_for_each(pListPos, &g_connCallbackListHead)
			{
				pstWifiCallbackData = list_entry(pListPos, WifiConnCallbackListData_t, callbackList);

				// 如果连上，将界面列表第一项设为连上状态；如果断开，将界面列表第一项连上状态清除
				pstWifiCallbackData->pfnCallback(NULL, currentConnStatus, 0);
			}
			pthread_mutex_unlock(&g_connCallbackListMutex);
		}
#endif

		usleep(50000);
	}

	printf("exit thread proc\n");
	return NULL;
}

static void *WifiScanProc(void *pdata)
{
    printf("Exec WifiScanProc\n");
    int wifiEnabled = 0;
    MI_WLAN_ScanResult_t scanResult;
    MI_WLAN_ConnectParam_t stConnParam;
	int registModuleCnt = 0;
	
    while (g_scanThreadRun)
    {
    	pthread_mutex_lock(&g_connParamMutex);
		wifiEnabled = g_wifiEnabled;
		memcpy(&stConnParam, &g_stConnParam, sizeof(MI_WLAN_ConnectParam_t));
		pthread_mutex_unlock(&g_connParamMutex);

	    list_t *pos = NULL;
		registModuleCnt = 0;
		pthread_mutex_lock(&g_scanCallbackListMutex);
		list_for_each(pos, &g_scanCallbackListHead)
		{
			registModuleCnt++;
		}
		pthread_mutex_unlock(&g_scanCallbackListMutex);
		
		if (wifiEnabled && registModuleCnt > 0)
		{
			MI_WLAN_Scan(NULL, &scanResult);

			if (scanResult.u8APNumber > 0)
			{
				// save result and sort by signalSTR desc
				// 如果最后一次连接的ssid存在于扫描结果中，将该ssid置为第一项，剩余项按信号强度降序排列；若最后一次连接的ssid不存在于扫描列表中（此场景不存在），将扫描结果降序排列
				SaveScanResult((char*)stConnParam.au8SSId, &scanResult);

				// construct callback param
				if (g_stScanResListHead.scanResCnt > 0)
				{
					WifiScanResultListData_t *pos = NULL;
					int i = 0;
					g_pstScanRes = (ScanResult_t*)malloc(sizeof(ScanResult_t) * g_stScanResListHead.scanResCnt);
					memset(g_pstScanRes, 0, sizeof(ScanResult_t) * g_stScanResListHead.scanResCnt);

					list_for_each_entry(pos, &g_stScanResListHead.scanResListHead, scanResList)
					{
						memcpy(g_pstScanRes+i, &pos->stScanRes, sizeof(ScanResult_t));
						i++;
					}
				}

				WifiScanCallbackListData_t *pstWifiCallbackData = NULL;
				list_t *pListPos = NULL;

				pthread_mutex_lock(&g_scanCallbackListMutex);
				list_for_each(pListPos, &g_scanCallbackListHead)
				{
					pstWifiCallbackData = list_entry(pListPos, WifiScanCallbackListData_t, callbackList);
					pstWifiCallbackData->pfnCallback(g_pstScanRes, g_stScanResListHead.scanResCnt); // 对搜索结果进行排序，将最近一次使用的ssid放置在第一行，其它的结果按信号强度降序排列
				}
				pthread_mutex_unlock(&g_scanCallbackListMutex);

				// clear scan result list
				ClearScanResult();
				if (g_pstScanRes)
				{
					free(g_pstScanRes);
					g_pstScanRes = NULL;
				}
			}
			else
			{
				usleep(50000);
				continue;
			}
		}

		usleep(3000000);	// 3s扫描一次
    }

	printf("exit thread proc\n");
	return NULL;
}

static void *WlanWorkProc(void *pdata)
{
    printf("Exec WlanWorkProc\n");
    int checkCnt = 40;
    int isWlanInsmode = 0;

    while (g_wlanThreadRun)
    {
    	if (!access("/sys/bus/usb", F_OK) && !access("/config/wifi", F_OK) && !access("/appconfigs", F_OK))
		{
			printf("wifi ko mount ok, currnet tryCnt is %d\n", checkCnt);
			isWlanInsmode = 1;
			break;
		}
		else
		{
			if (!checkCnt)
			{
				printf("wifi check failed\n");
				return NULL;
			}

			checkCnt--;
		}

		usleep(500000);
    }

    if (isWlanInsmode)
    {
    	// create thread to connect default AP, then start check
		// 1. read config from profile (check profile, backup profile, get supported & enabled info)
		// 2. creat thread (wlan init & connect default connParam, check connect status in while,
    	// 	  exec callback if connect status changed

    	if (WifiInit())
		{
			printf("wifi profile is not exsit or broken.\n");
			return NULL;
		}

		g_connThreadRun = 1;
		pthread_create(&g_connThread, NULL, WifiConnectProc, NULL);
		if (!g_connThread)
		{
			printf("create check hotplug thread failed\n");
			return NULL;
		}

		g_scanThreadRun = 1;
		pthread_create(&g_scanThread, NULL, WifiScanProc, NULL);
		if (!g_scanThread)
		{
			printf("create check hotplug thread failed\n");
			return NULL;
		}
    }

	printf("exit waln work thread proc\n");
	return NULL;
}

int Wifi_StartCheckHotplug()
{
	pthread_mutex_init(&g_connParamMutex, NULL);
	pthread_mutex_init(&g_connCallbackListMutex, NULL);
	pthread_mutex_init(&g_scanCallbackListMutex, NULL);
	INIT_LIST_HEAD(&g_connCallbackListHead);
	INIT_LIST_HEAD(&g_scanCallbackListHead);
	INIT_LIST_HEAD(&g_stScanResListHead.scanResListHead);
	g_stScanResListHead.scanResCnt = 0;

	g_wlanThreadRun = 1;
	pthread_create(&g_wlanThread, NULL, WlanWorkProc, NULL);
	if (!g_wlanThread)
	{
		printf("create check hotplug thread failed\n");
		return -1;
	}

	return 0;
}

void Wifi_StopCheckHotplug()
{
	// exit thread
	g_connThreadRun = 0;
	g_scanThreadRun = 0;
	g_wlanThreadRun = 0;

	if (g_connThread)
	{
		printf("wifi connect thread is exiting\n");
		pthread_join(g_connThread, NULL);
		g_connThread = 0;
		printf("wifi connect thread exit\n");
	}

	if (g_scanThread)
	{
		printf("wifi scan thread is exiting\n");
		pthread_join(g_scanThread, NULL);
		g_scanThread = 0;
		printf("wifi scan thread exit\n");
	}

	WifiDeinit();

	if (g_wlanThread)
	{
		printf("check hotplug thread is exiting\n");
		pthread_join(g_wlanThread, NULL);
		g_wlanThread = 0;
		printf("check hotplug thread exit\n");
	}

	pthread_mutex_destroy(&g_connCallbackListMutex);
	pthread_mutex_destroy(&g_scanCallbackListMutex);
	pthread_mutex_destroy(&g_connParamMutex);
}

int Wifi_RegisterConnectCallback(WifiConnCallback pfnCallback)
{
	WifiConnCallbackListData_t *pstConnCallbackData = NULL;

	printf("Enter Wifi_RegisterConnectCallback\n");

	if (!pfnCallback)
		return -1;

	pstConnCallbackData = (WifiConnCallbackListData_t*)malloc(sizeof(WifiConnCallbackListData_t));
	memset(pstConnCallbackData, 0, sizeof(WifiConnCallbackListData_t));
	pstConnCallbackData->pfnCallback = pfnCallback;

	pthread_mutex_lock(&g_connCallbackListMutex);
	WifiConnCallbackListData_t *pos = NULL;

	// debug0
	printf("Wifi_RegisterConnectCallback  debug0...\n");

	list_for_each_entry(pos, &g_connCallbackListHead, callbackList)
	{
		if (pos->pfnCallback == pfnCallback)
		{
			printf("have registered wifi connect callback\n");
			pthread_mutex_unlock(&g_connCallbackListMutex);
			return 0;
		}
	}

	// debug1
	printf("Wifi_RegisterConnectCallback  debug1...\n");

	list_add_tail(&pstConnCallbackData->callbackList, &g_connCallbackListHead);
	pthread_mutex_unlock(&g_connCallbackListMutex);

	// debug2
	printf("Wifi_RegisterConnectCallback  debug2...\n");

	pthread_mutex_lock(&g_connParamMutex);
	g_registerConnChanged = 1;
	pthread_mutex_unlock(&g_connParamMutex);

	printf("Leave Wifi_RegisterConnectCallback\n");

	return 0;
}

void Wifi_UnRegisterConnectCallback(WifiConnCallback pfnCallback)
{
	WifiConnCallbackListData_t *pstConnCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_connParamMutex);
	g_registerConnChanged = 0;
	pthread_mutex_unlock(&g_connParamMutex);

	pthread_mutex_lock(&g_connCallbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_connCallbackListHead)
	{
		pstConnCallbackData = list_entry(pListPos, WifiConnCallbackListData_t, callbackList);

		if (pstConnCallbackData->pfnCallback == pfnCallback)
		{
			list_del(pListPos);
			free(pstConnCallbackData);
		}
	}
	pthread_mutex_unlock(&g_connCallbackListMutex);
}

int Wifi_RegisterScanCallback(WifiScanCallback pfnCallback)
{
	WifiScanCallbackListData_t *pstScanCallbackData = NULL;

	printf("Enter Wifi_RegisterScanCallback\n");

	if (!pfnCallback)
		return -1;

	pstScanCallbackData = (WifiScanCallbackListData_t*)malloc(sizeof(WifiScanCallbackListData_t));
	memset(pstScanCallbackData, 0, sizeof(WifiScanCallbackListData_t));
	pstScanCallbackData->pfnCallback = pfnCallback;

	pthread_mutex_lock(&g_scanCallbackListMutex);
	WifiScanCallbackListData_t *pos = NULL;

	list_for_each_entry(pos, &g_scanCallbackListHead, callbackList)
	{
		if (pos->pfnCallback == pfnCallback)
		{
			printf("have registered wifi scan callback\n");
			pthread_mutex_unlock(&g_scanCallbackListMutex);
			return 0;
		}
	}

	list_add_tail(&pstScanCallbackData->callbackList, &g_scanCallbackListHead);
	pthread_mutex_unlock(&g_scanCallbackListMutex);

	printf("Leave Wifi_RegisterScanCallback\n");

	return 0;
}

void Wifi_UnRegisterScanCallback(WifiScanCallback pfnCallback)
{
	WifiScanCallbackListData_t *pstScanCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_scanCallbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_scanCallbackListHead)
	{
		pstScanCallbackData = list_entry(pListPos, WifiScanCallbackListData_t, callbackList);

		if (pstScanCallbackData->pfnCallback == pfnCallback)
		{
			list_del(pListPos);
			free(pstScanCallbackData);
		}
	}
	pthread_mutex_unlock(&g_scanCallbackListMutex);
}

// connect, if param changed, disconnect first, then connect new ssid
void Wifi_Connect(MI_WLAN_ConnectParam_t *pstConnParam)
{
	Wifi_ConnectInfo_t stConn;

	checkSsidExist((char*)pstConnParam->au8SSId, &stConn);
	memset(stConn.passwd, 0, sizeof(stConn.passwd));
	strcpy(stConn.passwd, (char*)pstConnParam->au8Password);

	printf("wifi connect: handle=%d, ssid=%s\n", stConn.index, (char*)pstConnParam->au8SSId);

	pthread_mutex_lock(&g_connParamMutex);
	g_hWlan = (WLAN_HANDLE)stConn.index;		// 传入hWlan,新ssid为-1,已存在的ssid使用保存的handle
	g_manuaConnect = 1;							// 仅开机时做自动连接,之后手动选择ssid进行连接
	g_userConnect = 1;
	memcpy(&g_stConnParam, pstConnParam, sizeof(MI_WLAN_ConnectParam_t));
	pthread_mutex_unlock(&g_connParamMutex);
}

void Wifi_Disconnect()
{
	pthread_mutex_lock(&g_connParamMutex);
	WLAN_HANDLE hWlan = g_hWlan;
	pthread_mutex_unlock(&g_connParamMutex);

	if (hWlan != -1)
		MI_WLAN_Disconnect(hWlan);
}

int Wifi_GetEnableStatus()
{
	int enable = 0;

	pthread_mutex_lock(&g_connParamMutex);
	enable = g_wifiEnabled;
	pthread_mutex_unlock(&g_connParamMutex);

	return enable;
}

// connect / disconnect
void Wifi_SetEnableStatus(int enable)
{
	pthread_mutex_lock(&g_connParamMutex);
	g_wifiEnabled = enable;
	pthread_mutex_unlock(&g_connParamMutex);

	setWifiEnableStatus(enable);
	saveWifiConfig();
}

int Wifi_GetSupportStatus()
{
	return g_wifiSupported;
}

int Wifi_GetCurConnStatus(MI_WLAN_Status_t *status)
{
	return MI_WLAN_GetStatus(g_hWlan, status);
}

int Wifi_GetLastConnStatus()
{
	int lastConnStatus = 0;

	pthread_mutex_lock(&g_connParamMutex);
	if (g_wifiSupported && g_wifiEnabled && (g_hWlan != -1))
		lastConnStatus = 1;
	pthread_mutex_unlock(&g_connParamMutex);

	return lastConnStatus;
}

static void *WifiGetApStatusProc(void *pdata)
{
    printf("Exec GetApStatusProc\n");
    WifiGetApInfoCallback pfnCallback = (WifiGetApInfoCallback)pdata;
    int wifiEnabled = 0;
    MI_WLAN_Status_t stGetApStatus;

    while (g_getApStatusThreadRun)
    {
    	MI_WLAN_GetStatus(WLAN_HANDLE_AP, &stGetApStatus);
    	pfnCallback(&stGetApStatus);

		usleep(3000000);	// 3s获取一次
    }

	printf("exit thread proc\n");
	return NULL;
}

int Wifi_InitAp()
{
	return MI_WLAN_Open(&stApOpenParam);
}

void Wifi_DeinitAp()
{
	MI_WLAN_Close(&stApOpenParam);
}

int Wifi_EnableAp(AirportInfo_t *pApInfo, WifiGetApInfoCallback pfnCallback)
{
	WLAN_HANDLE wlanHdl_ap = WLAN_HANDLE_AP;
	memset(&g_stConnectParam_ap.au8SSId, 0, sizeof(g_stConnectParam_ap.au8SSId));
	memset(&g_stConnectParam_ap.au8Password, 0, sizeof(g_stConnectParam_ap.au8Password));
	strcpy((char*)g_stConnectParam_ap.au8SSId, pApInfo->name);
	strcpy((char*)g_stConnectParam_ap.au8Password, pApInfo->passwd);

	if (!MI_WLAN_Connect(&wlanHdl_ap, &g_stConnectParam_ap))
		return -1;

	g_getApStatusThreadRun = 1;
	pthread_create(&g_getApStatusThread, NULL, WifiGetApStatusProc, (void*)pfnCallback);
	if (!g_getApStatusThread)
	{
		printf("create get ap status thread failed\n");
		return -1;
	}

	return 0;
}

void Wifi_DisableAp()
{
	g_getApStatusThreadRun = 0;

	if (g_getApStatusThread)
	{
		printf("wifi get ap status thread is exiting\n");
		pthread_join(g_getApStatusThread, NULL);
		g_getApStatusThread = 0;
		printf("wifi get ap status thread exit\n");
	}

	MI_WLAN_Disconnect(WLAN_HANDLE_AP);
}

char *Wifi_GetApName()
{
	return (char*)g_stConnectParam_ap.au8SSId;
}

char *Wifi_GetApPasswd()
{
	return (char*)g_stConnectParam_ap.au8Password;
}
