/*
 * NetUtils.h
 *
 *  Created on: Jul 27, 2018
 *      Author: guoxs
 */

#ifndef _INCLUDE_NET_NETUTILS_H_
#define _INCLUDE_NET_NETUTILS_H_

#define INVALID_IP_ADDR			"0.0.0.0"
#define INVALID_MAC_ADDR		"00:00:00:00:00:00"

class NetUtils {
public:
	static bool enableIfc(const char *iname, bool isEnable);

	static bool configure(const char *iname, const char *pIpAddr, const char *pMask,
			const char *pGateway, const char *pDns1, const char *pDns2);
	static bool getConfigureInfo(const char *iname, char *pIpAddr, char *pMask,
				char *pGateway, char *pDns1, char *pDns2);

	static bool dhcpRequestIp(const char *iname);
	static bool dhcpReleaseIp(const char *iname);

	static const char* getIp(const char *iname);
	static const char* getMacAddr(const char *iname);
};

#endif /* _INCLUDE_NET_NETUTILS_H_ */
