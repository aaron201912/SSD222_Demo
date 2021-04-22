#include "setConfig.h"


void (*reboot)() = NULL;
void (*setDefaultConfig)() = NULL;



void (*set_Password)(string account, string password) = NULL;
void (*set_ethernet)(string mod, string ip, string dns, string mask, string gateway) = NULL;
void (*set_wlan)(string mod, string ssid, string password, string encrypt) = NULL;
void (*set_wifi_disconnect)(void) = NULL;

void (*set_serverIp)(string ip, int port) = NULL;
void (*set_language)(string language) = NULL;
void (*set_opendoor)(string opendoor) = NULL;
void (*set_deviceName)(string deviceName) = NULL;
void (*set_volume)(int volume) = NULL;
void (*set_livetype)(int liveopen, int livetype) = NULL;
void (*set_pairwaittime)(int waittime) = NULL;
void (*set_defendtime)(string time) = NULL;
void (*set_doortype)(int doortype) = NULL;
void (*set_upgrade)(int sw) = NULL;
void (*set_antiunpack)(int sw) = NULL;
void (*set_protocol)(int proto) = NULL;



void (*get_Password)(string &account, string &password) = NULL;
void (*get_ethernet)(string &mod, string &ip, string &dns, string &mask, string& gateway) = NULL;
void (*get_wlan)(string &mod, string &ssid, string &password, string &encrypt) = NULL;
void (*get_serverIp)(string &ip, int &port) = NULL;
void (*get_language)(string &language) = NULL;
void (*get_doortype)(int& doortype) = NULL;
void (*get_deviceName)(string &deviceName) = NULL;
void (*get_liveswitch)(int &liveswitch) = NULL;
void (*get_livetype)(int &livetype) = NULL;
void (*get_volume)(int &volume) = NULL;
void (*get_log)(string& path) = NULL;
void (*get_deviceInfo)(string &info) = NULL;
void (*get_pairwaittime)(int& waittime) = NULL;
void (*get_antiunpack)(int& sw) = NULL;
int (*get_wifi_scaned_info)(wifi_info * wifiinfo,int listsize) = NULL;



