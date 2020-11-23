#ifndef _NET_NET_MANAGER_H_
#define _NET_NET_MANAGER_H_

#include <vector>

#include "EthernetManager.h"
#include "WifiManager.h"
#include "SoftApManager.h"

enum ENetChannel {
	E_NET_CHANNEL_ETHERNET = 0,
	E_NET_CHANNEL_WIFI,
	E_NET_CHANNEL_SOFTAP,
	E_NET_CHANNEL_NONE
};


class DhcpHandler;

class NetManager :
		EthernetManager::IEthernetConnStateListener,
		WifiManager::IWifiListener,
		SoftApManager::ISoftApStateListener {
public:
	NetManager();

	static NetManager* getInstance();

	void start();

	EthernetManager* getEthernetManager() const {
		return mEthernetManager;
	}

	WifiManager* getWifiManager() const {
		return mWifiManager;
	}

	SoftApManager* getSoftApManager() const {
		return mSoftApManager;
	}

	const char* getIp() const;
	ENetChannel getConnChannel() const;

	class INetChannelChangedListener {
	public:
		virtual ~INetChannelChangedListener() { }
		virtual void onNetChannelChanged(ENetChannel channel) = 0;
	};

	void addNetChannelChangedListener(INetChannelChangedListener *pListener);
	void removeNetChannelChangedListener(INetChannelChangedListener *pListener);

private:
	~NetManager();

	void updateNetChannel(ENetChannel channel);

	virtual void handleEthernetConnState(EEthConnState state);
	virtual void handleWifiEnable(E_WIFI_ENABLE event, int args);
	virtual void handleSoftApState(ESoftApState state);

private:
	DhcpHandler *mDhcpHandler;
	EthernetManager *mEthernetManager;
	WifiManager *mWifiManager;
	SoftApManager *mSoftApManager;

	bool mIsStarted;

	bool mIsWifiLastEnable;

	ENetChannel mNetChannel;
	mutable Mutex mLock;

	std::vector<INetChannelChangedListener *> mNetChannelChangedListenerList;
	mutable Mutex mListenerLock;
};

#define NETMANAGER			NetManager::getInstance()

#endif	// _NET_NET_MANAGER_H_
