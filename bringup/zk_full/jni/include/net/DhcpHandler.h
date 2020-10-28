#ifndef _NET_DHCP_HANDLER_H_
#define _NET_DHCP_HANDLER_H_

#include "system/Mutex.h"

class DhcpHandler {
public:
	DhcpHandler();
	~DhcpHandler();
	
	int ethernetRequestIp();
	int ethernetReleaseIp();

	int wifiRequestIp();
	int wifiReleaseIp();
	
	const char* getIp() const;
	
private:
	mutable Mutex mLock;
	char mIpAddr[32];
};

#endif	//_NET_DHCP_HANDLER_H_
