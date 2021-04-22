#include "webproc.h"
#include "setConfig.h"

static bool hasLogin = false;
static bool setwifi = false;


int aspTest(int eid, Webs *wp, int argc, char **argv)
{
	 return websWrite(wp, "Hello Wold jerry");
}

void FormMyTest(Webs* wp, char * path, char* query)
{
    char* strval = NULL;
    strval = websGetVar(wp, T("name"), T(""));

    if (NULL != strval)
    {
        cout << strval << endl;
    }
    
}

void Set_password(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_Password)
        return;
    string account = websGetVar(wp, T("account"), T(""));
    string password = websGetVar(wp, T("password"), T(""));
    set_Password(account, password);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_ethernet(Webs* wp, char* path, char* query)
{
    if (!set_ethernet)
        return;
    string mod = websGetVar(wp, T("mod"), T(""));
    string ip = websGetVar(wp, T("ip"), T(""));
    string dns = websGetVar(wp, T("dns"), T(""));
    string mask = websGetVar(wp, T("mask"), T(""));
	string gateway = websGetVar(wp, T("gateway"), T(""));
    set_ethernet(mod, ip, dns, mask, gateway);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_wlan(Webs* wp, char* path, char* query)
{
    setwifi = true;
    if (!hasLogin || !set_wlan)
        return;
    string mod = websGetVar(wp, T("mod"), T(""));
    string ssid = websGetVar(wp, T("ssid"), T(""));
    string password = websGetVar(wp, T("password"), T(""));
    string encrypt = websGetVar(wp, T("encrypt"), T(""));
    set_wlan(mod, ssid, password, encrypt);
    websRedirect(wp, (char*)"/html/index.asp");
}


char* Get_Wifi_Scaned_Info(Webs* wp, char* path, char* query)
{
   /*   if(!hasLogin || !get_wifi_scaned_info)
    {
        websWrite(wp, "");
    }
    
    wifi_info wifilist[20]={0};
    int j = 0;
    //get_scaned_wifiinfo(wifilist);
    char s[]="{ wifi_list_info: [";
    int count = 0;
    static char wifi_info[WIFI_INFO_LEN_ALL] = {0};

    if(!setwifi)
    {
        j =sprintf(wifi_info, "%s ",s);
        count = get_wifi_scaned_info(wifilist,20);
        for(int i = 0;i<count-1;i++)
        {
            if(j < WIFI_INFO_LEN_ALL -200)
            {
                j+=sprintf(wifi_info+j, " {ssid: \"%s\",Capability: \"%s\",Quality: %d,BSSID: \"%s\"},", wifilist[i].ssid,wifilist[i].Capability,wifilist[i].Quality,wifilist[i].BSSID);
            }
        }
        j += sprintf(wifi_info+j, "{ssid: \"%s\",Capability: \"%s\",Quality: %d,BSSID: \"%s\"}]}", wifilist[count-1].ssid,wifilist[count-1].Capability,wifilist[count-1].Quality,wifilist[count-1].BSSID);
    }*/
    //websWrite(wp, wifi_info);
    websRedirect(wp, (char*)"/html/login.asp");
	static char wifi_info[8] = "todo";
    return wifi_info;
    //websWriteBlock(wp,wifi_info,strlen(wifi_info));
    
}


void Set_wlan_disconnect(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_wlan)
        return;
    set_wifi_disconnect();
    setwifi = false;
    websRedirect(wp, (char*)"/html/index.asp");
}



void Set_serverIp(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_serverIp)
        return;
    string ip = websGetVar(wp, T("ip"), T(""));
    string port = websGetVar(wp, T("port"), T(""));
    int Port = atoi(port.c_str());
    set_serverIp(ip, Port);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_language(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_language)
        return;
    string language = websGetVar(wp, T("language"), T(""));
    set_language(language);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_opendoor(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_opendoor)
        return;
    string opendoor = websGetVar(wp, T("opendoor"), T(""));
    set_opendoor(opendoor);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_deviceName(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_deviceName)
        return;
    string deviceName = websGetVar(wp, T("deviceName"), T(""));
    set_deviceName(deviceName);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_volume(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_volume)
        return;
    string volume = websGetVar(wp, T("volume"), T(""));
    int Volume = atoi(volume.c_str());
    set_volume(Volume);
    websRedirect(wp, (char*)"/html/index.asp");
}


void Login(Webs* wp, char* path, char* query)
{
    string user = websGetVar(wp, T("user"), T(""));
    string pass = websGetVar(wp, T("pass"), T(""));
    if (get_Password == NULL)
    {
        websRedirect(wp, (char*)"/html/passerror.asp");
        return;
    }
    string User, Pass;
    get_Password(User, Pass);
    if (User == user && Pass == pass)//
    {
        hasLogin = true;
        websRedirect(wp, (char*)"/html/index.asp");
    }
    else
    {
        websRedirect(wp, (char*)"/html/passerror.asp");
    }

}

void LoginOut(Webs* wp, char* path, char* query)
{
    hasLogin = false;
    websRedirect(wp, (char*)"/html/login.asp");
}

void Reboot(Webs* wp, char* path, char* query)
{
    if (!hasLogin || reboot)
        reboot();
}

void SetDefaultConfig(Webs* wp, char* path, char* query)
{
    if (!hasLogin || setDefaultConfig)
        setDefaultConfig();
    websRedirect(wp, (char*)"/html/index.asp");
}



void Set_pairwaittime(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_pairwaittime)
        return;
    string waittime = websGetVar(wp, T("waittime"), T(""));
    int int_waittime = atoi(waittime.c_str());
    set_pairwaittime(int_waittime);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_livetype(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_livetype)
        return;
    string liveopen = websGetVar(wp, T("liveopen"), T(""));
    string livetype = websGetVar(wp, T("livetype"), T(""));
    int int_liveopen = atoi(liveopen.c_str());
    int int_livetype = atoi(livetype.c_str());
    set_livetype(int_liveopen, int_livetype);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_defendtime(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_defendtime)
        return;
    string beginTime = websGetVar(wp, T("beginTime"), T(""));
	cout<<beginTime.c_str()<<endl;
	cout<<beginTime.substr(0,5).c_str()<<endl;
    set_defendtime(beginTime);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_doortype(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_doortype)
        return;
    string doortype = websGetVar(wp, T("doortype"), T(""));
	int int_doortype = atoi(doortype.c_str());
    set_doortype(int_doortype);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_upgrade(Webs* wp, char* path, char* query)
{
    if (!hasLogin || !set_upgrade)
        return;
    string upgrade = websGetVar(wp, T("upgrade"), T(""));
    int int_upgrade = atoi(upgrade.c_str());
    set_upgrade(int_upgrade);
    websRedirect(wp, (char*)"/html/index.asp");
}

void Set_antiunpack(Webs* wp, char* path, char* query)
{
	if(!hasLogin || !set_antiunpack)
		return ;
	string anti = websGetVar(wp, T("antiunpack"), T(""));
	int int_anti = atoi(anti.c_str());
	set_antiunpack(int_anti);
	websRedirect(wp, (char*)"/html/index.asp");
}

void Set_protocol_type(Webs* wp, char* path, char* query)
{
	if(!hasLogin || !set_protocol)
    {
        return ;
    }
	string proto = websGetVar(wp, T("protocol"), T(""));
	int int_proto = atoi(proto.c_str());
	//int int_anti = atoi(anti.c_str());
	set_protocol(int_proto);
	websRedirect(wp, (char*)"/html/index.asp");
}




int Get_server_ip(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_serverIp)
        return websWrite(wp, "");
    string ip;
    int port;
    get_serverIp(ip, port);
    return websWrite(wp, (char*)ip.c_str());
}

int Get_server_port(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_serverIp)
        return websWrite(wp, "");
    string ip;
    int port;
    get_serverIp(ip, port);
    char Port[20] = "";
    sprintf(Port, "%d", port);
    return websWrite(wp, Port);
}

int Get_wlan_mod(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_wlan)
        return websWrite(wp, "");
    string mod, ssid, password, encrypt;
    get_wlan(mod, ssid, password, encrypt);
    return websWrite(wp, (char*)mod.c_str());
}
int Get_wlan_ssid(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_wlan)
    {	
        return websWrite(wp, "");
    }
    string mod, ssid, password, encrypt;
    get_wlan(mod, ssid, password, encrypt);

    return websWrite(wp, (char*)ssid.c_str());
}
int Get_wlan_password(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_wlan)
        return websWrite(wp, "");
    string mod, ssid, password, encrypt;
    get_wlan(mod, ssid, password, encrypt);
    return websWrite(wp, (char*)password.c_str());
}
int Get_wlan_encrypt(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_wlan)
        return websWrite(wp, "");
    string mod, ssid, password, encrypt;
    get_wlan(mod, ssid, password, encrypt);
    return websWrite(wp, (char*)encrypt.c_str());
}

int Get_ethernet_mod(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_ethernet)
        return websWrite(wp, "");
    string mod, ip, dns, mask, gateway;
    get_ethernet(mod, ip, dns, mask, gateway);
    return websWrite(wp, (char*)mod.c_str());
}
int Get_ethernet_ip(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_ethernet)
        return websWrite(wp, "");
    string mod, ip, dns, mask ,gateway;
    get_ethernet(mod, ip, dns, mask, gateway);
    return websWrite(wp, (char*)ip.c_str());
}
int Get_ethernet_dns(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_ethernet)
        return websWrite(wp, "");
    string mod, ip, dns, mask, gateway;
    get_ethernet(mod, ip, dns, mask, gateway);
    return websWrite(wp, (char*)dns.c_str());
}
int Get_ethernet_mask(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_ethernet)
        return websWrite(wp, "");
    string mod, ip, dns, mask, gateway;
    get_ethernet(mod, ip, dns, mask, gateway);
    return websWrite(wp, (char*)mask.c_str());
}
int Get_ethernet_gateway(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_ethernet)
        return websWrite(wp, "");
    string mod, ip, dns, mask, gateway;
    get_ethernet(mod, ip, dns, mask, gateway);
    return websWrite(wp, (char*)gateway.c_str());
}

int Get_deviceName(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_deviceName)
        return websWrite(wp, "");
    string name = "";
    get_deviceName(name);
    return websWrite(wp, (char*)name.c_str());
}

int Get_liveswitch(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_liveswitch)
        return websWrite(wp, "");
    int liveswitch;
    get_liveswitch(liveswitch);
    char Liveswitch[20] = "";
    sprintf(Liveswitch, "%d", liveswitch);
    return websWrite(wp, Liveswitch);
}


int Get_livetype(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_livetype)
        return websWrite(wp, "");
    int livetype;
    get_livetype(livetype);
    char Livetype[20] = "";
    sprintf(Livetype, "%d", livetype);
    return websWrite(wp, Livetype);
}

int Get_volume(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_volume)
        return websWrite(wp, "");
    int volume;
    get_volume(volume);
    char Volume[20] = "";
    sprintf(Volume, "%d", volume);
    return websWrite(wp, Volume);
}
int Get_language(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_language)
        return websWrite(wp, "");
    string language = "";
    get_language(language);
    string out = "curVal = ";
    out += "'";
    out += language;
    out += "';";
    return websWrite(wp, (char*)out.c_str());
}

int Get_doortype(int eid, Webs* wp, int argc, char** argv)
{
	if(!hasLogin || !get_doortype)
		return websWrite(wp, "");
	int doortype = 0;
	int optionVal = 0;
	get_doortype(doortype);
	switch(doortype)
	{
		case 0:   optionVal = 0;break;
		case 26:  optionVal = 1;break;
		case 34:  optionVal = 2;break;
		case 260: optionVal = 3;break;
		case 340: optionVal = 4;break;
		default:optionVal = 0;break;
	}
	return websWrite(wp,"%d",optionVal);
}

int Get_log(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_log)
        return websWrite(wp, "");
    string logUrl = "";
    get_log(logUrl);
    return websWrite(wp, (char*)logUrl.c_str());
}
int Get_deviceInfo(int eid, Webs* wp, int argc, char** argv)
{
    if (!hasLogin || !get_deviceInfo)
        return websWrite(wp, "");
    string info = "";
    get_deviceInfo(info);
    return websWrite(wp, (char*)info.c_str());
}
int Get_pairwaittime(int eid, Webs* wp, int argc, char** argv)
{
	if (!hasLogin || !get_pairwaittime)
    	return websWrite(wp, "");
	int waittime = 0;
	get_pairwaittime(waittime);
	return websWrite(wp, "%d",waittime-1);
}
int Get_antiunpack(int eid, Webs* wp, int argc, char** argv)
{
	if(!hasLogin || !get_antiunpack)
		return websWrite(wp, "");
	int anti = 0;
	get_antiunpack(anti);
	return websWrite(wp, "%d",anti);
}

int Get_Wifi_Scaned_Info_for_show(int eid, Webs* wp, int argc, char** argv)
{
	/*
	if(!hasLogin || !get_wifi_scaned_info)
	return websWrite(wp, "");
    wifi_info wifilist[20]={0};
    //get_scaned_wifiinfo(wifilist);
    int count = 0;
    char buff[WIFI_INFO_LEN_ALL] = {0};
    if(!setwifi)
    {
        count = get_wifi_scaned_info(wifilist,20);
        for(int i = 0;i<count;i++)
        {
            sprintf(buff, "%s  ssid %s,Capability %s,Quality %d,BSSID %s <br> ", buff,wifilist[i].ssid,wifilist[i].Capability,wifilist[i].Quality,wifilist[i].BSSID);
        }
        //websWrite(wp, buff);
        //websRedirect(wp, (char*)"/html/index.asp");
    }

    return websWrite(wp, buff);
	*/
	return -1;
}

