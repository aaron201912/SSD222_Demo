/*
 * WifiCtrl.h
 *
 *  Created on: Nov 10, 2017
 *      Author: guoxs
 */

#ifndef _NET_WIFICTRL_H_
#define _NET_WIFICTRL_H_

#include <string>

class WifiCtrl {
public:
	WifiCtrl(const char *iface);
	virtual ~WifiCtrl();

	bool setWifiEnable(bool isEnable);

	int connect(const std::string &ssid, const std::string &pw, const std::string &encryption);
	bool disconnect();
	bool reconnect();
	bool reassociate();

	bool connectToSupplicant();
	void closeSupplicantConnection();

	bool wifiStartSupplication(bool p2psupport);
	bool wifiStopSupplication(bool p2psupport);

	int waitForEvent(char *buf, int len);

	bool scan();
	/**
	 * Format of results:
	 * =================
	 * bssid=68:7f:74:d7:1b:6e
	 * freq=2412
	 * level=-43
	 * tsf=1344621975160944
	 * age=2623
	 * flags=[WPA2-PSK-CCMP][WPS][ESS]
	 * ssid=zubyb
	 *
	 * RANGE=ALL gets all scan results
	 * MASK=<N> see wpa_supplicant/src/common/wpa_ctrl.h for details
	 */
	std::string scanResults();
	void setScanInterval(int scanInterval);
	bool setScanMode(bool setActive);
	bool setScanResultHandling(int mode);
	void enableBackgroundScan(bool enable);

	bool ping();

	/* Does a graceful shutdown of supplicant. Is a common stop function for both p2p and sta.
	 *
	 * Note that underneath we use a harsh-sounding "terminate" supplicant command
	 * for a graceful stop and a mild-sounding "stop" interface
	 * to kill the process
	 */
	bool stopSupplicant();

	std::string listNetworks();

	bool setNetworkVariable(int netId, const std::string &name, const std::string &value);
	bool setNetworkVariable2(int netId, const std::string &name, const std::string &value);
	std::string getNetworkVariable(int netId, const std::string &name);

	int addNetwork();
	bool removeNetwork(int netId);
	bool selectNetwork(int netId);
	bool enableNetwork(int netId);
	bool disableNetwork(int netId);

	std::string status();

	std::string getMacAddress();

	bool startDriver();
	bool stopDriver();

	/**
	 * Start filtering out Multicast V4 packets
	 * @return {@code true} if the operation succeeded, {@code false} otherwise
	 *
	 * Multicast filtering rules work as follows:
	 *
	 * The driver can filter multicast (v4 and/or v6) and broadcast packets when in
	 * a power optimized mode (typically when screen goes off).
	 *
	 * In order to prevent the driver from filtering the multicast/broadcast packets, we have to
	 * add a DRIVER RXFILTER-ADD rule followed by DRIVER RXFILTER-START to make the rule effective
	 *
	 * DRIVER RXFILTER-ADD Num
	 *   where Num = 0 - Unicast, 1 - Broadcast, 2 - Mutil4 or 3 - Multi6
	 *
	 * and DRIVER RXFILTER-START
	 * In order to stop the usage of these rules, we do
	 *
	 * DRIVER RXFILTER-STOP
	 * DRIVER RXFILTER-REMOVE Num
	 *   where Num is as described for RXFILTER-ADD
	 *
	 * The  SETSUSPENDOPT driver command overrides the filtering rules
	 */
	bool startFilteringMulticastV4Packets();

	/**
	 * Stop filtering out Multicast V4 packets.
	 * @return {@code true} if the operation succeeded, {@code false} otherwise
	 */
	bool stopFilteringMulticastV4Packets();

	/**
	 * Start filtering out Multicast V6 packets
	 * @return {@code true} if the operation succeeded, {@code false} otherwise
	 */
	bool startFilteringMulticastV6Packets();

	/**
	 * Stop filtering out Multicast V6 packets.
	 * @return {@code true} if the operation succeeded, {@code false} otherwise
	 */
	bool stopFilteringMulticastV6Packets();

	int getBand();
	bool setBand(int band);

	bool saveConfig();

	bool addToBlacklist(const std::string &bssid);

	bool clearBlacklist();
	bool setSuspendOptimizations(bool enabled);

	bool setCountryCode(const std::string &countryCode);

	/** Example output:
	 * RSSI=-65
	 * LINKSPEED=48
	 * NOISE=9999
	 * FREQUENCY=0
	 */
	std::string signalPoll();
	/** Example outout:
	 * TXGOOD=396
	 * TXBAD=1
	 */
	std::string pktcntPoll();

	bool startWpsPbc(const std::string &bssid);
	bool startWpsPbc(const std::string &iface, const std::string &bssid);

	bool startWpsPinKeypad(const std::string &pin);
	bool startWpsPinKeypad(const std::string &iface, const std::string &pin);

	std::string startWpsPinDisplay(const std::string &bssid);
	std::string startWpsPinDisplay(const std::string &iface, const std::string &bssid);

	/* Configures an access point connection */
	bool startWpsRegistrar(const std::string &bssid, const std::string &pin);

	bool cancelWps();
	bool setPersistentReconnect(bool enabled);

	bool setDeviceName(const std::string &name);
	bool setDeviceType(const std::string &type);

	bool setConfigMethods(const std::string &cfg);

	bool setManufacturer(const std::string &value);

	bool setModelName(const std::string &value);
	bool setModelNumber(const std::string &value);
	bool setSerialNumber(const std::string &value);

	void setPowerSave(bool isEnable);

private:
	void copyWifiConf();
	bool isWepPskValid(const char *pw);

	std::string format(const char *format, ...);

	bool doBooleanCommand(const std::string &command);
	int doIntCommand(const std::string &command);
	std::string doStringCommand(const std::string &command);
	bool doCommand(const std::string &command, char *reply, size_t replyLen);

private:
	std::string mInterface;
	bool mSuspendOptEnabled;
};

#endif /* _NET_WIFICTRL_H_ */
