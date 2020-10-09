#ifndef _NET_ETHERNET_MANAGER_H_
#define _NET_ETHERNET_MANAGER_H_

#include <vector>

#include "system/Thread.h"

typedef enum {
	E_ETH_LINK_DOWN,
	E_ETH_LINK_UP,
	E_ETH_LINK_UNKNOWN
} EEthLinkState;

typedef enum {
	E_ETH_DISCONNECTED,
	E_ETH_CONNECTING,
	E_ETH_CONNECTED,
	E_ETH_DISCONNECTING,
	E_ETH_CONN_UNKNOWN
} EEthConnState;

class DhcpHandler;

/**
 * @brief 以太网管理器
 */
class EthernetManager {
public:
	EthernetManager(DhcpHandler *dhcpHdlr);
	~EthernetManager();

	bool init();
	bool isSupported() const;

	const char* getIp() const;
	EEthConnState getConnState() const;
	bool isConnected() const;

	bool configure(const char *pIpAddr, const char *pMask,
			const char *pGateway, const char *pDns1, const char *pDns2);
	bool getConfigureInfo(char *pIpAddr, char *pMask,
				char *pGateway, char *pDns1, char *pDns2);
	bool getStaticConfigureInfo(char *pIpAddr, char *pMask,
				char *pGateway, char *pDns1, char *pDns2);

	bool setAutoMode(bool isAuto);
	bool isAutoMode() const;

	class IEthernetConnStateListener {
	public:
		virtual ~IEthernetConnStateListener() { }
		virtual void handleEthernetConnState(EEthConnState state) = 0;
	};

	void addEthernetConnStateListener(IEthernetConnStateListener *pListener);
	void removeEthernetConnStateListener(IEthernetConnStateListener *pListener);

private:
	void handleLinkState(EEthLinkState state);
	void updateConnState(EEthConnState state);

	class LinkThread : public Thread {
	public:
		LinkThread(EthernetManager &em) : mEM(em) { }

		bool start();
		void stop();

	protected:
		virtual bool readyToRun();
		virtual bool threadLoop();

	private:
		EthernetManager &mEM;
	};

	class ConnThread : public Thread {
	public:
		ConnThread(EthernetManager &em) : mEM(em) { }

		bool start();
		void stop();

	protected:
		virtual bool threadLoop();

	private:
		EthernetManager &mEM;
	};

private:
	EEthConnState mConnState;

	LinkThread mLinkThread;
	ConnThread mConnThread;
	mutable Mutex mLock;

	DhcpHandler *mDhcpHandlerPtr;

	std::vector<IEthernetConnStateListener *> mConnListenerList;
	mutable Mutex mListenerLock;
};

#endif	// _NET_ETHERNET_MANAGER_H_
