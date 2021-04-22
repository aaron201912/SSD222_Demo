#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

#define HI_WIFI_CUST_H
#define WIFI_INFO_LEN   128
#define WIFI_INFO_LEN_ALL   15520 //最大搜索到30个wifi ssid


typedef struct
{
	char ssid[WIFI_INFO_LEN];
	char Capability[WIFI_INFO_LEN];//wifi加密方式
	unsigned int Quality ;//信号强度
	char BSSID[WIFI_INFO_LEN];//wifi mac地址
} wifi_info;
#ifdef __cplusplus
extern "C"
{
#endif
extern void (*reboot)();//
extern void (*setDefaultConfig)();//


extern void (*set_Password)(string account, string password);//
extern void (*set_ethernet)(string mod, string ip, string dns, string mask, string gateway);
extern void (*set_wlan)(string mod, string ssid, string password, string encrypt);
extern void (*set_wifi_disconnect)(void);

extern void (*set_serverIp)(string ip, int port);
extern void (*set_language)(string language);
extern void (*set_opendoor)(string opendoor);
extern void (*set_deviceName)(string deviceName);
extern void (*set_volume)(int volume);

extern void (*set_livetype)(int liveopen, int livetype);
extern void (*set_pairwaittime)(int waittime);
extern void (*set_defendtime)(string time);
extern void (*set_doortype)(int doortype);
extern void (*set_upgrade)(int sw);
extern void (*set_antiunpack)(int sw);
extern void (*set_protocol)(int proto);




extern void (*get_Password)(string& account, string& password);//
extern void (*get_ethernet)(string& mod, string& ip, string& dns, string& mask, string& gateway);
extern void (*get_wlan)(string& mod, string& ssid, string& password, string& encrypt);
extern void (*get_serverIp)(string& ip, int& port);
extern void (*get_language)(string& language);
extern void (*get_doortype)(int& doortype);
extern void (*get_deviceName)(string& deviceName);
extern void (*get_liveswitch)(int& liveswitch);
extern void (*get_livetype)(int& livetype);
extern void (*get_volume)(int& volume);
extern void (*get_log)(string& path);
extern void (*get_deviceInfo)(string& info);
extern void (*get_pairwaittime)(int& waittime);
extern void (*get_antiunpack)(int& sw);
extern int (*get_wifi_scaned_info)(wifi_info * wifiinfo,int listsize);

#ifdef __cplusplus
}
#endif


