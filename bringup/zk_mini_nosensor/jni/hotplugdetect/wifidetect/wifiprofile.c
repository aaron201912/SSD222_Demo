/*
 * wifiInfo.c
 *
 *  Created on: 2019年8月15日
 *      Author: koda.xu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#include "wifiprofile.h"
#include "cJSON.h"


#define WIFI_SETTING_CFG	"/appconfigs/wifisetting.json"
#define WPA_CFG				"/appconfigs/wpa_supplicant.conf"
#define WPA_CFG_BACKUP		"/appconfigs/wpa_supplicant.conf_bak"
#define WPA_CFG_PREFIX		"ctrl_interface"
#define ENABLE_DEBUG		0

#if ENABLE_DEBUG
#define DEBUG_ENTRY()		{printf("enter %s\n", __FUNCTION__);}
#define DEBUG_EXIT()		{printf("exit %s\n", __FUNCTION__);}
#define DEBUG_PRINT_JSON()	{printf("%s\n",cJSON_Print(g_pRoot));}
#define DEBUG_LINE()		{printf("%d %s\n", __LINE__, __FUNCTION__);}s
#else
#define DEBUG_ENTRY()
#define DEBUG_EXIT()
#define DEBUG_PRINT_JSON()
#define DEBUG_LINE()
#endif

static bool g_bWifiSupport = true;		// 设备是否支持wifi,由设备决定，不可更改
static bool g_bWifiEnable = true;
static char g_line[256];
static WLAN_HANDLE g_hWlan = -1;
static MI_WLAN_ConnectParam_t g_stConnectParam;
static cJSON *g_pRoot = NULL;
static cJSON *g_pWifi = NULL;
static cJSON *g_pEnable = NULL;
static cJSON *g_pSavedSsid = NULL;
static cJSON *g_pSsidList = NULL;
static cJSON *g_pConnect = NULL;

static void *malloc_fn(size_t sz)
{
	return malloc(sz);
}

static void free_fn(void *ptr)
{
	free(ptr);
}

bool getWifiSupportStatus()
{
	return g_bWifiSupport;
}

void setWifiSupportStatus(bool enable)
{
	g_bWifiSupport = enable;
}

bool getWifiEnableStatus()
{
	return g_bWifiEnable;
}

void setWifiEnableStatus(bool enable)
{
	g_bWifiEnable = enable;
}

WLAN_HANDLE getWlanHandle()
{
	return g_hWlan;
}

void setWlanHandle(WLAN_HANDLE handle)
{
	g_hWlan = handle;
}

MI_WLAN_ConnectParam_t * getConnectParam()
{
	return &g_stConnectParam;
}

void saveConnectParam(MI_WLAN_ConnectParam_t *pConnParam)
{
	memset(&g_stConnectParam, 0, sizeof(MI_WLAN_ConnectParam_t));
	memcpy(&g_stConnectParam, pConnParam, sizeof(MI_WLAN_ConnectParam_t));
}

static char *freadline(FILE *stream)
{
    int count = 0;

    while(!feof(stream) && (count < sizeof(g_line)) && ((g_line[count++] = getc(stream)) != '\n'));
    if (!count)
        return NULL;

    g_line[count - 1] = '\0';

    return g_line;
}

// 文件不存在或文件存在但内容为空时，创建文件并写入默认配置;
// 文件存在且内容不为空时，读取文件内容
int checkProfile()
{
	FILE *pCfgFile = NULL;
	FILE *pBackupFile = NULL;
	char cmd[128];

	DEBUG_ENTRY();

	pCfgFile = fopen(WPA_CFG, "r+");
	if (pCfgFile)
	{
		// judge file prefix
		char *pCurLine = NULL;
		char *pSeek = NULL;
		int nRet = 0;

		pCurLine = freadline(pCfgFile);
		if (pCurLine)
		{
			pSeek = strstr(pCurLine, WPA_CFG_PREFIX);
			if (pSeek)
			{
				printf("%s is not empty\n", WPA_CFG);
				fclose(pCfgFile);
				return 0;
			}
		}

		fclose(pCfgFile);
	}

	// recover cfg file
	pBackupFile = fopen(WPA_CFG_BACKUP, "r+");
	if (!pBackupFile)
		return -1;

	fclose(pBackupFile);

	printf("wpa config file is empty, recover config file\n");
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "cp %s %s", WPA_CFG_BACKUP, WPA_CFG);
	system(cmd);

	if (!access(WIFI_SETTING_CFG, F_OK))
	{
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "rm %s", WIFI_SETTING_CFG);
		system(cmd);
	}

	return 1;
}

int createWifiDefaultConfig()
{
	FILE* fp = NULL;
	cJSON * root;
	cJSON * obj;
	cJSON * connections;
	cJSON * connect;
	cJSON * item;
	char id[8];

	DEBUG_ENTRY();

	fp = fopen(WIFI_SETTING_CFG,"w+");
	if (!fp)
	{
		printf("should open json file first\n");
		return -1;
	}

	printf("open %s success\n", WIFI_SETTING_CFG);
	root = cJSON_CreateObject();
	obj = cJSON_AddObjectToObject(root, "wifi");
	item = cJSON_AddBoolToObject(obj, "isSupport", g_bWifiSupport);
	item = cJSON_AddBoolToObject(obj, "isEnable", g_bWifiEnable);
	item = cJSON_AddStringToObject(obj, "ssid", "");
	item = cJSON_AddArrayToObject(obj, "ssidlist");
	connections = cJSON_AddObjectToObject(obj, "connections");

	fseek(fp, 0, SEEK_SET);
	fwrite(cJSON_Print(root),strlen(cJSON_Print(root)),1,fp);
	cJSON_Delete(root);
	fclose(fp);
	fp = NULL;

	return 0;
}

int initWifiConfig()
{
	FILE* fp = NULL;
	long long len = 0;
	char * pConfData = NULL;
	char * ssid = NULL;
	cJSON * connect;
	cJSON * item;
	cJSON_Hooks hooks;
	int ret = -1;

	DEBUG_ENTRY();

	memset(&g_stConnectParam, 0, sizeof(MI_WLAN_ConnectParam_t));
	g_stConnectParam.eSecurity = E_MI_WLAN_SECURITY_WPA;
	g_stConnectParam.OverTimeMs = 5000;

	// check json config valid
	fp = fopen(WIFI_SETTING_CFG,"r+");
	if (!fp)
	{
		createWifiDefaultConfig();
	}
	else
	{
		int isBlank = 0;
		getc(fp);

		if (feof(fp))
			isBlank = 1;

		fclose(fp);
		fp = NULL;

		if (isBlank)
			createWifiDefaultConfig();
	}

	// read config param
	fp = fopen(WIFI_SETTING_CFG,"r+");
	if (!fp)
	{
		printf("should open json file first\n");
		return -1;
	}

	printf("open %s success\n", WIFI_SETTING_CFG);

	hooks.free_fn = free_fn;
	hooks.malloc_fn = malloc_fn;
	cJSON_InitHooks(&hooks);

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	pConfData = (char *)malloc(len + 1);
	fseek(fp, 0, SEEK_SET);
	fread(pConfData, len, 1, fp);
	fclose(fp);
	fp = NULL;

	// read config
	cJSON_Minify(pConfData);
	g_pRoot = cJSON_Parse(pConfData);
	if (!g_pRoot)
		goto exit;

	g_pWifi = cJSON_GetObjectItem(g_pRoot, "wifi");
	if (!g_pWifi)
		goto exit;

	printf("parse json success\n");
	item = cJSON_GetObjectItem(g_pWifi, "isSupport");
	if (item)
		g_bWifiSupport = cJSON_IsTrue(item);
	printf("isSupport: %d\n", g_bWifiSupport);

	g_pEnable = cJSON_GetObjectItem(g_pWifi, "isEnable");
	if (g_pEnable)
		g_bWifiEnable = cJSON_IsTrue(g_pEnable);
	printf("isSupport: %d\n", g_bWifiEnable);

	g_pSavedSsid = cJSON_GetObjectItem(g_pWifi, "ssid");
	if (!g_pSavedSsid)
	{
		printf("no ssid item\n");
		goto exit;
	}

	ssid = cJSON_GetStringValue(g_pSavedSsid);
	if (!ssid)
	{
		printf("ssid value is null\n");
		goto exit;
	}

	g_pSsidList = cJSON_GetObjectItem(g_pWifi, "ssidlist");
	if (!g_pSsidList)
	{
		printf("no ssidlist item\n");
		goto exit;
	}

	strcpy((char*)g_stConnectParam.au8SSId, ssid);
	g_pConnect = cJSON_GetObjectItem(g_pWifi, "connections");
	if (g_pConnect)
	{
		printf("connections obj exist\n");
		connect = cJSON_GetObjectItem(g_pConnect, ssid);
		if(connect)
		{
			printf("%s obj exist\n", ssid);
			strcpy((char*)g_stConnectParam.au8SSId, ssid);
			item = cJSON_GetObjectItem(connect, "index");
			g_hWlan = (WLAN_HANDLE)atoi(cJSON_GetStringValue(item));
			item = cJSON_GetObjectItem(connect, "passwd");
			strcpy((char*)g_stConnectParam.au8Password, cJSON_GetStringValue(item));
		}

		ret = 0;
	}

	printf("isSupport:%d isEnable:%d id:%d ssid:%s passwd:%s\n", g_bWifiSupport, g_bWifiEnable, g_hWlan, (char*)g_stConnectParam.au8SSId, (char*)g_stConnectParam.au8Password);

exit:
	if (pConfData)
		free(pConfData);
	return ret;
}

int saveWifiConfig()
{
	char id[8];

	DEBUG_ENTRY();
	DEBUG_PRINT_JSON();

	int fd = open(WIFI_SETTING_CFG, O_RDWR | O_DSYNC | O_RSYNC);
	if (fd < 0)
	{
		printf("should open json file first\n");
		return -1;
	}

	ftruncate(fd, strlen(cJSON_Print(g_pRoot)));
	write(fd, cJSON_Print(g_pRoot),strlen(cJSON_Print(g_pRoot)));
	fdatasync(fd);
	close(fd);
	
	DEBUG_EXIT();
	return 0;
}

void checkSsidExist(char *ssid, Wifi_ConnectInfo_t *pConnInfo)
{
	cJSON * connect;
	cJSON * item;

	DEBUG_ENTRY();
	DEBUG_PRINT_JSON();

	memset(pConnInfo, 0, sizeof(Wifi_ConnectInfo_t));
	pConnInfo->index = -1;

	// check ssid
	if (!g_pConnect)
	{
		printf("checkSsidExist:connections obj is null\n");
		return;
	}

	connect = cJSON_GetObjectItem(g_pConnect, ssid);
	if(connect)
	{
		strcpy((char*)g_stConnectParam.au8SSId, ssid);
		item = cJSON_GetObjectItem(connect, "index");
		pConnInfo->index = (WLAN_HANDLE)atoi(cJSON_GetStringValue(item));
		item = cJSON_GetObjectItem(connect, "passwd");
		strcpy(pConnInfo->passwd, cJSON_GetStringValue(item));
	}

	DEBUG_EXIT();
}

int addSsidToApList(char *ssid)
{
	cJSON * item;

	DEBUG_ENTRY();

	if (!g_pSsidList)
	{
		printf("addSsidToApList:ssidlist obj is null\n");
		return -1;
	}

	item = cJSON_CreateString(ssid);
	cJSON_AddItemToArray(g_pSsidList, item);

	return 0;
}

int addSsidInfoToConnList(char *ssid, Wifi_ConnectInfo_t *pConnInfo)
{
	cJSON * conn;
	cJSON * item;
	char index[8];

	DEBUG_ENTRY();

	if (!g_pConnect)
	{
		printf("addSsidInfoToConnList:connections obj is null\n");
		return -1;
	}

	conn = cJSON_AddObjectToObject(g_pConnect, ssid);
	memset(index, 0, sizeof(index));
	sprintf(index, "%d", pConnInfo->index);
	item = cJSON_AddStringToObject(conn, "index", index);
	item = cJSON_AddStringToObject(conn, "passwd", pConnInfo->passwd);

	return 0;
}

int updateCurSsid(char *ssid)
{
	cJSON * item;

	DEBUG_ENTRY();

	if (!g_pWifi)
	{
		printf("updateCurSsid:wifi obj is null\n");
		return -1;
	}

	item = cJSON_CreateString(ssid);
	cJSON_ReplaceItemInObject(g_pWifi, "ssid", item);
	g_pSavedSsid = item;

	return 0;
}

int updateConnPasswd(char *ssid, char *passwd)
{
	cJSON * conn;
	cJSON * item;

	DEBUG_ENTRY();
	DEBUG_PRINT_JSON();

	if (!g_pConnect)
	{
		printf("updateConnPasswd:connections obj is null\n");
		return -1;
	}

	conn = cJSON_GetObjectItem(g_pConnect, ssid);
	if(!conn)
	{
		printf("updateConnPasswd:conn obj is null\n");
		return -1;
	}

	item = cJSON_CreateString(passwd);
	cJSON_ReplaceItemInObject(conn, "passwd", item);
	DEBUG_EXIT();

	return 0;
}

int updateEnableStatus(bool bEnable)
{
	cJSON * item;

	DEBUG_ENTRY();

	if (!g_pWifi)
	{
		printf("updateEnableStatus:wifi obj is null\n");
		return -1;
	}

	item = cJSON_CreateBool((cJSON_bool)bEnable);
	cJSON_ReplaceItemInObject(g_pWifi, "isEnable", item);
	g_pEnable = item;

	return 0;
}

