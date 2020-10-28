/*
 * WifiInfo.h
 *
 *  Created on: 2017年10月2日
 *      Author: Kwolve
 */

#ifndef _NET_WIFIINFO_H_
#define _NET_WIFIINFO_H_

#include <string>

class WifiInfo {
public:
	WifiInfo() {
		reset();
	}

	const std::string& getPasswd() const {
		return mPasswd;
	}

	void setPasswd(const std::string &passwd) {
		this->mPasswd = passwd;
	}

	int getFreq() const {
		return mFreq;
	}

	void setFreq(int freq) {
		mFreq = freq;
	}

	const std::string& getBssid() const {
		return mBSSID;
	}

	void setBssid(const std::string &bssid) {
		mBSSID = bssid;
	}

	const std::string& getEncryption() const {
		return mEncryption;
	}

	void setEncryption(const std::string &encryption) {
		mEncryption = encryption;
	}

	int getRssi() const {
		return mRssi;
	}

	void setRssi(int rssi) {
		mRssi = rssi;
	}

	const std::string& getSsid() const {
		return mSSID;
	}

	void setSsid(const std::string &ssid) {
		mSSID = ssid;
	}

	const std::string& getStatement() const {
		return mStatement;
	}

	void setStatement(const std::string &statement) {
		mStatement = statement;
	}

	int getLevel() const {
		return mLevel;
	}

	void setLevel(int level) {
		mLevel = level;
	}

	void reset() {
		mSSID.clear();
		mBSSID.clear();
		mEncryption.clear();
		mStatement.clear();
		mPasswd.clear();

		mLevel = 0;
		mFreq = 0;
		mRssi = 0;
	}

private:
	std::string mSSID;
	std::string mBSSID;
	std::string mEncryption;	// 加密方式
	std::string mStatement;		// 描述
	std::string mPasswd;

	int mLevel;
	int mFreq;
	int mRssi;	// 信号强度
};

#endif /* _NET_WIFIINFO_H_ */
