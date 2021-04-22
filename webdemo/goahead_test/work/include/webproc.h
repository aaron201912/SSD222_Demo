#include "head.h"
#include <iostream>
#include <string>
using namespace std;
extern "C" PUBLIC void websRedirect(Webs * wp, char* uri);


#ifdef __cplusplus
extern "C"
{
#endif

int aspTest(int eid, Webs *wp, int argc, char **argv);
void FormMyTest(Webs* wp, char* path, char* query);

void Login(Webs* wp, char* path, char* query);
void LoginOut(Webs* wp, char* path, char* query);
void Reboot(Webs* wp, char* path, char* query);
void SetDefaultConfig(Webs* wp, char* path, char* query);

void Set_password(Webs* wp, char* path, char* query);
void Set_ethernet(Webs* wp, char* path, char* query);
void Set_wlan(Webs* wp, char* path, char* query);
void Set_wlan_disconnect(Webs* wp, char* path, char* query);
void Set_serverIp(Webs* wp, char* path, char* query);
void Set_language(Webs* wp, char* path, char* query);
void Set_opendoor(Webs* wp, char* path, char* query);
void Set_deviceName(Webs* wp, char* path, char* query);
void Set_volume(Webs* wp, char* path, char* query);

void SetDefaultConfig(Webs* wp, char* path, char* query);
void Set_pairwaittime(Webs* wp, char* path, char* query);
void Set_livetype(Webs* wp, char* path, char* query);
void Set_defendtime(Webs* wp, char* path, char* query);
void Set_doortype(Webs* wp, char* path, char* query);
void Set_upgrade(Webs* wp, char* path, char* query);
void Set_antiunpack(Webs* wp, char* path, char* query);
void Set_protocol_type(Webs* wp, char* path, char* query);


int Get_server_ip(int eid, Webs* wp, int argc, char** argv);
int Get_server_port(int eid, Webs* wp, int argc, char** argv);
int Get_wlan_mod(int eid, Webs* wp, int argc, char** argv);
int Get_wlan_ssid(int eid, Webs* wp, int argc, char** argv);
int Get_wlan_password(int eid, Webs* wp, int argc, char** argv);
int Get_wlan_encrypt(int eid, Webs* wp, int argc, char** argv);
int Get_ethernet_mod(int eid, Webs* wp, int argc, char** argv);
int Get_ethernet_ip(int eid, Webs* wp, int argc, char** argv);
int Get_ethernet_dns(int eid, Webs* wp, int argc, char** argv);
int Get_ethernet_mask(int eid, Webs* wp, int argc, char** argv);
int Get_ethernet_gateway(int eid, Webs* wp, int argc, char** argv);
int Get_deviceName(int eid, Webs* wp, int argc, char** argv);
int Get_liveswitch(int eid, Webs* wp, int argc, char** argv);
int Get_livetype(int eid, Webs* wp, int argc, char** argv);
int Get_volume(int eid, Webs* wp, int argc, char** argv);
int Get_language(int eid, Webs* wp, int argc, char** argv);
int Get_doortype(int eid, Webs* wp, int argc, char** argv);
int Get_log(int eid, Webs* wp, int argc, char** argv);
int Get_deviceInfo(int eid, Webs* wp, int argc, char** argv);
int Get_pairwaittime(int eid, Webs* wp, int argc, char** argv);
int Get_antiunpack(int eid, Webs* wp, int argc, char** argv);
int Get_antiunpack(int eid, Webs* wp, int argc, char** argv);
char * Get_Wifi_Scaned_Info(Webs* wp, char* path, char* query);
int Get_Wifi_Scaned_Info_for_show(int eid, Webs* wp, int argc, char** argv);


#ifdef __cplusplus
}
#endif










