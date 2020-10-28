#ifndef _NET_WIFI_MANAGER_H_
#define _NET_WIFI_MANAGER_H_

#include <vector>
#include <map>

#include "system/Thread.h"
#include "utils/MessageQueue.h"
#include "WifiInfo.h"

enum E_WIFI_ENABLE {
	E_WIFI_ENABLE_ENABLE,
	E_WIFI_ENABLE_DISABLE,
	E_WIFI_ENABLE_ENABLEING,
	E_WIFI_ENABLE_DISABLEING,
	E_WIFI_ENABLE_UNKNOW
};

enum E_WIFI_CONNECT {
	E_WIFI_CONNECT_DISCONNECT,
	E_WIFI_CONNECT_CONNECTING,
	E_WIFI_CONNECT_CONNECTED,
	E_WIFI_CONNECT_DICONNECTING,
	E_WIFI_CONNECT_ERROR,
	E_WIFI_CONNECT_UNKNOW
};

enum E_WIFI_ERROR_CODE {
	E_WIFI_ERROR_CODE_PASSWORD_INCORRECT
};

enum E_SUPPLICATION_STATE {
	/**
	 * This state indicates that client is not associated, but is likely to
	 * start looking for an access point. This state is entered when a
	 * connection is lost.
	 */
	DISCONNECTED,

	/**
	 * Interface is disabled
	 * <p/>
	 * This state is entered if the network interface is disabled.
	 * wpa_supplicant refuses any new operations that would
	 * use the radio until the interface has been enabled.
	 */
	INTERFACE_DISABLED,

	/**
	 * Inactive state (wpa_supplicant disabled).
	 * <p/>
	 * This state is entered if there are no enabled networks in the
	 * configuration. wpa_supplicant is not trying to associate with a new
	 * network and external interaction (e.g., ctrl_iface call to add or
	 * enable a network) is needed to start association.
	 */
	INACTIVE,

	/**
	 * Scanning for a network.
	 * <p/>
	 * This state is entered when wpa_supplicant starts scanning for a
	 * network.
	 */
	SCANNING,

	/**
	 * Trying to authenticate with a BSS/SSID
	 * <p/>
	 * This state is entered when wpa_supplicant has found a suitable BSS
	 * to authenticate with and the driver is configured to try to
	 * authenticate with this BSS.
	 */
	AUTHENTICATING,

	/**
	 * Trying to associate with a BSS/SSID.
	 * <p/>
	 * This state is entered when wpa_supplicant has found a suitable BSS
	 * to associate with and the driver is configured to try to associate
	 * with this BSS in ap_scan=1 mode. When using ap_scan=2 mode, this
	 * state is entered when the driver is configured to try to associate
	 * with a network using the configured SSID and security policy.
	 */
	ASSOCIATING,

	/**
	 * Association completed.
	 * <p/>
	 * This state is entered when the driver reports that association has
	 * been successfully completed with an AP. If IEEE 802.1X is used
	 * (with or without WPA/WPA2), wpa_supplicant remains in this state
	 * until the IEEE 802.1X/EAPOL authentication has been completed.
	 */
	ASSOCIATED,

	/**
	 * WPA 4-Way Key Handshake in progress.
	 * <p/>
	 * This state is entered when WPA/WPA2 4-Way Handshake is started. In
	 * case of WPA-PSK, this happens when receiving the first EAPOL-Key
	 * frame after association. In case of WPA-EAP, this state is entered
	 * when the IEEE 802.1X/EAPOL authentication has been completed.
	 */
	FOUR_WAY_HANDSHAKE,

	/**
	 * WPA Group Key Handshake in progress.
	 * <p/>
	 * This state is entered when 4-Way Key Handshake has been completed
	 * (i.e., when the supplicant sends out message 4/4) and when Group
	 * Key rekeying is started by the AP (i.e., when supplicant receives
	 * message 1/2).
	 */
	GROUP_HANDSHAKE,

	/**
	 * All authentication completed.
	 * <p/>
	 * This state is entered when the full authentication process is
	 * completed. In case of WPA2, this happens when the 4-Way Handshake is
	 * successfully completed. With WPA, this state is entered after the
	 * Group Key Handshake; with IEEE 802.1X (non-WPA) connection is
	 * completed after dynamic keys are received (or if not used, after
	 * the EAP authentication has been completed). With static WEP keys and
	 * plaintext connections, this state is entered when an association
	 * has been completed.
	 * <p/>
	 * This state indicates that the supplicant has completed its
	 * processing for the association phase and that data connection is
	 * fully configured. Note, however, that there may not be any IP
	 * address associated with the connection yet. Typically, a DHCP
	 * request needs to be sent at this point to obtain an address.
	 */
	COMPLETED,

	/**
	 * An Android-added state that is reported when a client issues an
	 * explicit DISCONNECT command. In such a case, the supplicant is
	 * not only dissociated from the current access point (as for the
	 * DISCONNECTED state above), but it also does not attempt to connect
	 * to any access point until a RECONNECT or REASSOCIATE command
	 * is issued by the client.
	 */
	DORMANT,

	/**
	 * No connection to wpa_supplicant.
	 * <p/>
	 * This is an additional pseudo-state to handle the case where
	 * wpa_supplicant is not running and/or we have not been able
	 * to establish a connection to it.
	 */
	UNINITIALIZED,

	/**
	 * A pseudo-state that should normally never be seen.
	 */
	INVALID

/**
 * Returns {@code true} if the supplicant state is valid and {@code false}
 * otherwise.
 * @param state The supplicant state
 * @return {@code true} if the supplicant state is valid and {@code false}
 * otherwise.
 */
};

class WifiCtrl;
class DhcpHandler;

/**
 * @brief Wifi管理器
 */
class WifiManager {
public:
	WifiManager(DhcpHandler *dhcpHdlr);
	~WifiManager();

	bool init();

	/**
	 * @brief 是否支持Wifi功能
	 */
	bool isSupported() const;

	/**
	 * @brief 打开、关闭Wifi
	 * @param enable true 打开wifi，false 关闭wifi
	 */
	void enableWifi(bool enable);

	/**
	 * @brief Wifi是否已打开
	 */
	bool isWifiEnable() const;

	/**
	 * @brief 连接Wifi
	 * @param ssid wifi名称
	 * @param pw wifi密码
	 * @param encryption 加密方式
	 */
	void connect(const std::string &ssid, const std::string &pw, const std::string &encryption);

	/**
	 * @brief 连接Wifi
	 * @param ssid wifi名称
	 * @param pw wifi密码
	 */
	void connect(const std::string &ssid, const std::string &pw);

	/**
	 * @brief 断开连接
	 */
	void disconnect();

	/**
	 * @brief 扫描Wifi
	 */
	void startScan();

	/**
	 * @brief 停止扫描Wifi
	 */
	void stopScan();

	void setScanInterval(int scanInterval);

	void saveConfig();

	/**
	 * @brief 获取MAC地址
	 */
	const char* getMacAddr() const;

	/**
	 * @brief 获取IP地址
	 */
	const char* getIp() const;

	/**
	 * @brief Wifi信息监听接口
	 */
	class IWifiListener {
	public:
		virtual ~IWifiListener() { }
		virtual void handleWifiEnable(E_WIFI_ENABLE event, int args) { }
		virtual void handleWifiConnect(E_WIFI_CONNECT event, int args) { }
		virtual void handleWifiErrorCode(E_WIFI_ERROR_CODE code) { }
		virtual void handleWifiScanResult(std::vector<WifiInfo>* wifiInfos) { }
		virtual void notifySupplicantStateChange(int networkid,
				const char* ssid, const char* bssid,
				E_SUPPLICATION_STATE newState) { }
	};

	/**
	 * @brief 添加Wifi信息监听接口
	 */
	void addWifiListener(IWifiListener *pListener);

	/**
	 * @brief 移除Wifi信息监听接口
	 */
	void removeWifiListener(IWifiListener *pListener);

	std::vector<WifiInfo>* getWifiScanInfos() { return &mWifiScanInfos; }

	/**
	 * @brief 获取扫描到的wifi信息
	 */
	void getWifiScanInfosLock(std::vector<WifiInfo> &wifiInfos);

	E_WIFI_ENABLE getEnableStatus() const { return mEnableStatus; }

	/**
	 * @brief 是否已连接
	 */
	bool isConnected() const;

	/**
	 * @brief 获取连接上wifi信息
	 */
	WifiInfo* getConnectionInfo() const;

private:
	typedef enum {
		E_MSGTYPE_ENABLE = 1,
		E_MSGTYPE_DISABLE,
		E_MSGTYPE_CONNECT,
		E_MSGTYPE_DISCONNECT,
		E_MSGTYPE_START_SCAN,
		E_MSGTYPE_STOP_SCAN,
		E_MSGTYPE_QUIT_THREAD,
	} EMessageType;

	/**
	 * wifi控制线程
	 */
	class ControlThread : public Thread {
	public:
		ControlThread(WifiManager &wm) : mWM(wm) { }

		bool start();
		void stop();

	protected:
		virtual bool readyToRun();
		virtual bool threadLoop();

	private:
		WifiManager &mWM;
	};

	/**
	 * wifi事件监听线程
	 */
	class EventThread : public Thread {
	public:
		EventThread(WifiManager &wm) : mWM(wm) { }

		bool start();
		void stop();

	protected:
		virtual bool threadLoop();

	private:
		WifiManager &mWM;
	};

private:
	void sendMessage(EMessageType msgType);

	void updateEnableStatus(E_WIFI_ENABLE status);
	int doEventStateChange(const std::string &str);
	void updateWifiScanInfos();

	void performEnableWifi();
	void performDisableWifi();
	void performConnectWifi();
	void performDisconnectWifi();

private:
	WifiCtrl *mWifiCtrlPtr;
	DhcpHandler *mDhcpHandlerPtr;

	mutable Mutex mLock;

	MessageQueue mMsgQueue;

	std::vector<IWifiListener *> mWifiListenerList;
	mutable Mutex mListenerLock;

	ControlThread mCtrlThread;
	EventThread mEventThread;

	E_WIFI_ENABLE mEnableStatus;
	E_SUPPLICATION_STATE mWifiState;

	std::vector<WifiInfo> mWifiScanInfos;
	mutable Mutex mWifiScanInfosLock;

	std::map<std::string, std::string> mWifiChangeAps;

	WifiInfo mRequestConnectInfo;
	WifiInfo mConnectionInfo;
};

#endif	// _NET_WIFI_MANAGER_H_
