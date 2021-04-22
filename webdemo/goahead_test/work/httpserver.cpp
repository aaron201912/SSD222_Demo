#include <fstream>
#include <iostream>
#include "head.h"
#include "webproc.h"
#include "setConfig.h"

#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>


#define SERVER_FILE "/data/server_ip.txt"
#define DEVICEFILE "/data/device.json"


void init()
{
	websAspDefine("aspTest", aspTest);
	websFormDefine("FormMyTest", (void*)FormMyTest);

	websFormDefine("Set_password", (void*)Set_password);
	websFormDefine("Set_ethernet", (void*)Set_ethernet);
	websFormDefine("Set_wlan", (void*)Set_wlan);
    websFormDefine("Set_wlan_disconnect", (void*)Set_wlan_disconnect);
	websFormDefine("Set_serverIp", (void*)Set_serverIp);
	websFormDefine("Set_language", (void*)Set_language);
	websFormDefine("Set_opendoor", (void*)Set_opendoor);
	websFormDefine("Set_deviceName", (void*)Set_deviceName);
	websFormDefine("Set_volume", (void*)Set_volume);
    websFormDefine("Get_Wifi_Scaned_Info",(char*)Get_Wifi_Scaned_Info);

	websFormDefine("Login", (void*)Login);
	websFormDefine("LoginOut", (void*)LoginOut);
	websFormDefine("Reboot", (void*)Reboot);
	websFormDefine("SetDefaultConfig", (void*)SetDefaultConfig);

	websFormDefine("Set_pairwaittime", (void*)Set_pairwaittime);
	websFormDefine("Set_livetype", (void*)Set_livetype);
	websFormDefine("Set_defendtime", (void*)Set_defendtime);
	websFormDefine("Set_doortype", (void*)Set_doortype);
	websFormDefine("Set_upgrade", (void*)Set_upgrade);
	websFormDefine("Set_antiunpack", (void*)Set_antiunpack);
    websFormDefine("Set_protocol_type", (void*)Set_protocol_type);


	websAspDefine("Get_server_ip", Get_server_ip);
	websAspDefine("Get_server_port", Get_server_port);
	websAspDefine("Get_wlan_mod", Get_wlan_mod);
	websAspDefine("Get_wlan_ssid", Get_wlan_ssid);
	websAspDefine("Get_wlan_password", Get_wlan_password);
	websAspDefine("Get_wlan_encrypt", Get_wlan_encrypt);
	websAspDefine("Get_ethernet_mod", Get_ethernet_mod);
	websAspDefine("Get_ethernet_ip", Get_ethernet_ip);
	websAspDefine("Get_ethernet_dns", Get_ethernet_dns);
	websAspDefine("Get_ethernet_mask", Get_ethernet_mask);
	websAspDefine("Get_ethernet_gateway", Get_ethernet_gateway);
	websAspDefine("Get_deviceName", Get_deviceName);
	websAspDefine("Get_liveswitch", Get_liveswitch);
	websAspDefine("Get_livetype", Get_livetype);
	websAspDefine("Get_volume", Get_volume);
	websAspDefine("Get_language", Get_language);
	websAspDefine("Get_log", Get_log);
	websAspDefine("Get_deviceInfo", Get_deviceInfo);
	websAspDefine("Get_doortype",Get_doortype);
	websAspDefine("Get_pairwaittime",Get_pairwaittime);
	websAspDefine("Get_antiunpack",Get_antiunpack);
    websAspDefine("Get_Wifi_Scaned_Info_for_show",Get_Wifi_Scaned_Info_for_show);
}

void _reboot()
{
	//SSI_ReBoot();
}
void sett_DefaultConfig()
{
	//pbservice
}
void sett_Password(string account, string password)
{
	//TODO
}
bool isValidIP(const char *ipstr)
{
	return 0;
}
void sett_ethernet(string mod, string ip, string dns, string mask,string gateway)
{
	/*char tmp[512];
	if(!strcmp("dynicIP",mod.c_str()))
	{
		cout<<"动态分配IP"<<endl;
		if(my_system("busybox cat /data/ip.sh",(char*)"udhcpc") != 0)
		{
			system("udhcpc -s /etc/init.d/udhcpc.script -i eth0");
		}

		memset(tmp,0,512);
		sprintf(tmp,"busybox echo \"udhcpc -q -i eth0 -s /etc/init.d/udhcpc.script &\" >/data/tmp.ip");
		system(tmp);
		cout<<tmp<<endl;
		system("busybox echo \"/customer/bin/ss_sh_watch_eth.sh eth0 & \" >>/data/tmp.ip");
		system("busybox echo \"sleep 5\" >>/data/tmp.ip");
		system("busybox echo \"/customer/bin/ss_sh_standby_ip.sh &\" >>/data/tmp.ip");
		system("busybox cp /data/tmp.ip /data/ip.sh");
		system("sync");
	}else if(!strcmp("staticIP",mod.c_str()))
	{
	    if(!isValidIP(ip.c_str()) || !isValidIP(mask.c_str()) || !isValidIP(gateway.c_str()))
	    {
	        cout<<"静态设置IP invalid,ip="<<ip<<"mask="<<mask<<"gw="<<gateway<<endl;
	        return;
	    }
	    //ip
		cout<<"静态设置IP"<<endl;
		memset(tmp,0,512);
		sprintf(tmp,"ifconfig eth0 %s netmask %s",ip.c_str(),mask.c_str());
		cout<<tmp<<endl;
		system(tmp);
		//gw
		memset(tmp,0,512);
		sprintf(tmp,"route add default gw %s eth0",gateway.c_str());
		cout<<tmp<<endl;
		system(tmp);
		//ip
		memset(tmp,0,512);
		sprintf(tmp,"busybox echo \"ifconfig eth0 %s netmask %s\" >/data/tmp.ip",ip.c_str(),mask.c_str());
		system(tmp);
		cout<<tmp<<endl;
		//gw
		memset(tmp,0,512);
		sprintf(tmp,"busybox echo \"route add default gw %s eth0\" >>/data/tmp.ip",gateway.c_str());
		system(tmp);
		cout<<tmp<<endl;
		system("busybox cp /data/tmp.ip /data/ip.sh");
		system("sync");
		//dns
		if(!dns.empty() && isValidIP(dns.c_str()))
		{
			memset(tmp,0,512);
			sprintf(tmp,"busybox echo \"nameserver %s\" >/data/tmp.resolv",dns.c_str());
			system(tmp);
			system("busybox cp /data/tmp.resolv /etc/resolv.conf");
			system("sync");
		}
	}
	*/
}

void sett_serverIp(string ip, int port)
{
	//pbservice
	//modify /data/server_ip.txt
	/*ofstream outFile;
	outFile.open(SERVER_FILE);
	char msg[100];
	sprintf(msg,"{\"server_host\":\"tcp://%s:%d\"} ",ip.c_str(),port);
	outFile<<msg;
	outFile.close();
	//reset server thread
	bool isServer = false;
	bool isRestart = true;
#ifdef CUSTOMER_ZH
	isServer = true;
#endif
#ifdef USB_CAM
	isServer = true;
#endif
	PbService::GetInstance()->StartPbService(isRestart,isServer);
*/
}
void sett_language(string language)
{
	//TODO
}
void sett_opendoor(string opendoor)
{
	printf("opendoor\n");
	
}
void sett_deviceName(string deviceName)
{
	//pbservice
}

void sett_livetype(int liveopen, int livetype)
{
	//
}
void sett_volume(int volume)
{
	//
}
void sett_PairWaitTime(int waittime)
{
//
	
}

void sett_defendtime(string time)
{
	//
}
void sett_doortype(int doortype)
{
	//
}
void sett_upgrade(int sw)
{
	//TODO
}
void sett_antiunpack(int sw)
{
	//
}

void sett_protocol(int proto)
{
	//
}


void sett_wifi_disconnect(void)
{
    //
}

void gett_Password(string& account, string& password)
{
	account = "admin";
	password = "admin";
}
extern int my_system(const char *cmd,char* str);
int getDns(string& dns)  
{
    int index = 0;
	/*
    string line;
    system("busybox cat /etc/resolv.conf>/tmp/dns.txt");
    ifstream in("/tmp/dns.txt");
    system("busybox rm /tmp/dns.txt");
    if(!in)
    {
        perror("open error");
        printf("dns file not found!!");
        return -1;
    }
    while(getline(in,line))
	{
		const char *tmp = line.c_str();
		//printf("get dns:%s\n",tmp);
		char dns1[32] = {"0.0.0.0"};
        if(strncmp(tmp, "nameserver", strlen("nameserver")) == 0)
        {
        	sscanf(tmp, "nameserver %s.%s.%s.%s", dns1);
            index++;
            printf("get %d dns:%s\n", index,dns1);
        	if(index > 1)
        	{
        	    dns.assign(";");
        	    dns.assign(dns1);
        	}
        	else
        	{
        	    dns.assign(dns1);
        	}
        }
    }
    */
    return index;  
}
int getNetmakIp(char* mask,char* eth)  
{
   /* int sock_get_ip;
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
         printf("socket create failse...getLocalIp!\n");
         return -1;
    }
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, eth, sizeof(ifr_ip.ifr_name) - 1);
    if( ioctl(sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
    {
    	printf("socket SIOCGIFADDR failse...getLocalIp!\n");
    	close( sock_get_ip );
		return -1;
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    //strcpy(ip,inet_ntoa(sin->sin_addr));
    if( ioctl(sock_get_ip, SIOCGIFNETMASK, &ifr_ip) < 0 )
    {
    	printf("socket SIOCGIFNETMASK failse...getLocalIp!\n");
    	close( sock_get_ip );
		return -1;
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(mask,inet_ntoa(sin->sin_addr));
    //printf("local ip:%s\n netmask:%s\n",ip,mask);
    close( sock_get_ip );
	*/
    return 0;  
}
int getGateWay(string& gateway)  
{
    /*FILE *fp;
    char buf[512];
    char cmd[128];
    char tmpgw[32] = {"0.0.0.0"};
    char *tmp;
    int fount = 0;
    strcpy(cmd, "busybox ip route");
    fp = popen(cmd, "r");
    if(NULL == fp)
    {
        perror("popen error");
        printf("ip route not found!!");
        return -1;
    }
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
            ++ tmp;
        if(strncmp(tmp, "default", strlen("default")) == 0)
        {
			fount = 1;
            break;
        }
    }
    if(0 == fount)
    {
		printf("no fount gateway\n");
		pclose(fp);
		return -2;
    }
    sscanf(buf, "%*s%*s%s", tmpgw);
    printf("default gateway:%s\n", tmpgw);
    pclose(fp);
    gateway.assign(tmpgw);
	*/
    return 0;  
} 

void gett_ethernet(string& mod, string& ip, string& dns, string& mask, string& gateway)
{
    /*if(my_system("busybox cat /data/ip.sh",(char*)"udhcpc") == 0)
    {
        mod.assign("dynicIP");
        printf("gett_ethernet dynicIP\n");
    }
    else
    {
        mod.assign("staticIP");
        printf("gett_ethernet staticIP\n");
    }
	char ip_addr[36]="0.0.0.0";//
	ip.assign(ip_addr);
	getDns(dns);
	getGateWay(gateway);
	char tmpMask[36]="0.0.0.0";
	if(getNetmakIp(tmpMask,(char*)"eth0") == 0)
	{
	    mask.assign(tmpMask);
	}
	else
	{
	    mask.assign("255.255.255.0");
	}
	*/
}

void gett_serverIp(string& ip, int& port)
{
	//移到pbservice
}
void gett_language(string& language)
{
	//TODO
}
void gett_doortype(int& doortype)
{

	cout<<"doortype:"<<"1"<<endl;
}
void gett_deviceName(string& deviceName)
{
	//移到pbservice了
}


void gett_liveswitch(int& liveswitch)
{
	liveswitch = 1;
	printf("\n gett_liveswitch =%d \n",liveswitch);
}


void gett_livetype(int& livetype)
{
	
	livetype = 1;
	printf("\n gett_livetype =%d \n",livetype);
}
void gett_volume(int& volume)
{

	volume =20;
}
void gett_log(string& path)
{
	//TODO
}


void gett_sn(string &sn)
{
	char sn_no[128]={0};
	//if(0==SSI_Get_SN_for_THD(sn_no))
	{
		sn.assign(sn_no);
		cout<<"get sn success"<<endl;
	}
}
void gett_mac(string &mac)
{
	char mac_no[100];

}
void gett_antiunpack(int& sw)
{
	
	sw =1;
}

int gett_wifi_scaned_info(wifi_info * wifiinfo,int listsize)
{
   // wifi_info wifilist[20];
    return 0;
}


int gett_relay()
{

	return 0;
}
int gett_firealarm()
{

	return 0;
}

void gett_deviceInfo(string &Info)
{
	/*char devinfo[300];
	string sn_no;
	string mac_no;
	const char op[] = "打开";
	const char cl[] = "关闭";
	const char high_Le[] = "高电平";
	const char low_Le[] = "低电平";
	gett_sn(sn_no);
	gett_mac(mac_no);
	sprintf(devinfo,"SN:%s <br> MAC:%s <br> 防拆报警:%s <br> 门磁状态:%s <br> 消防联动:%s",
			sn_no.c_str(),mac_no.c_str(),
			1,
			gett_relay()==1?high_Le:low_Le,
			gett_firealarm()==1?high_Le:low_Le);

	Info.assign(devinfo);
	*/
}

void gett_pairwaittime(int& waittime)
{
	
	waittime = 2000/1000;
}
void* HttpServerMain(void *argv)
{
	printf("\n-----httpserver starting---------\n");

	//memset(&pSetting,0,sizeof(stRsyncPerson));
	reboot = _reboot;
	//setDefaultConfig = sett_DefaultConfig;
	//set_serverIp = sett_serverIp;
	//set_deviceName = sett_deviceName;
	set_Password = sett_Password;
	set_ethernet = sett_ethernet;
	//set_wlan = sett_wlan;
	set_wifi_disconnect = sett_wifi_disconnect;
	set_language = sett_language;
	set_opendoor = sett_opendoor;
	set_livetype = sett_livetype;
	set_volume = sett_volume;
	set_pairwaittime = sett_PairWaitTime;
	set_defendtime = sett_defendtime;
	set_doortype = sett_doortype;
	set_upgrade = sett_upgrade;
	set_antiunpack = sett_antiunpack;
    set_protocol = sett_protocol;

	get_Password = gett_Password;
	get_ethernet = gett_ethernet;
	//get_wlan = gett_wlan;
	get_language = gett_language;
	get_doortype = gett_doortype;
	//get_serverIp = gett_serverIp;
	//get_deviceName = gett_deviceName;
	get_liveswitch = gett_liveswitch;
	get_livetype = gett_livetype;
	get_volume = gett_volume;
	get_log = gett_log;
	get_deviceInfo = gett_deviceInfo;
	get_pairwaittime = gett_pairwaittime;
	get_antiunpack = gett_antiunpack;
    get_wifi_scaned_info = gett_wifi_scaned_info;
	setWebServerConfig("www", "html/login.asp");//
	work( (void*) init );//

#if 0
	while(1)
	{
		usleep(100000);
	}
#endif

    return NULL;
}

int main(int argc, char **argv)
{
    HttpServerMain((void*)argv);

    while (1)
        usleep(100000);

    return 0;
}
