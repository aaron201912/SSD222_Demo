#ifndef _NET_DHCP_HANDLER_H_
#define _NET_DHCP_HANDLER_H_

#include "system/Mutex.h"

class DhcpHandler {
public:
	DhcpHandler();
	~DhcpHandler();

	bool requestIp(const char *iname);
	bool releaseIp(const char *iname);

	const char* getIp(const char *iname) const;

private:
	mutable Mutex mLock;
};

#endif	//_NET_DHCP_HANDLER_H_
