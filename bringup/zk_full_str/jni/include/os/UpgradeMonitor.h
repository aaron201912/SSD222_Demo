/*
 * UpgradeMonitor.h
 *
 *  Created on: Aug 4, 2017
 *      Author: guoxs
 */

#ifndef _OS_UPGRADE_MONITOR_H_
#define _OS_UPGRADE_MONITOR_H_

#include <string>
#include <vector>
#include "MountMonitor.h"

class UpgradeMonitor : Thread {
public:
	virtual ~UpgradeMonitor();

	void startMonitoring();
	void stopMonitoring();

	bool startUpgrade();
	void stopUpgrade();

	bool isUpgrading() const;
	bool needUpgrade() const;

	bool checkUpgrade();

	static UpgradeMonitor* getInstance();

public:
	typedef unsigned char BYTE;

	typedef enum {
		E_UPGRADE_STATUS_START,
		E_UPGRADE_STATUS_SUCCESS,
		E_UPGRADE_STATUS_ERROR
	} EUpgradeStatus;

	typedef enum {
		E_UPGRADE_TYPE_UBOOT,
		E_UPGRADE_TYPE_BOOT,
		E_UPGRADE_TYPE_SYSTEM,
		E_UPGRADE_TYPE_RES,
		E_UPGRADE_TYPE_DATA,
		E_UPGRADE_TYPE_LOGO,
		E_UPGRADE_TYPE_ENV,
		E_UPGRADE_TYPE_PRIVATE,
		E_UPGRADE_TYPE_FULL
	} ESystemUpgradeType;

	/**
	 * 分区信息
	 */
	typedef struct {
		BYTE partn;			// 分区号
		BYTE reserve[3];	// 保留
		BYTE offset[4];		// 基于head的偏移
		BYTE imageSize[4];	// 分区镜像大小
		BYTE imageHead[16];	// image头数据 swap MD5
	} SPartInfo;

	/**
	 * 扩展信息
	 */
	typedef struct {
		BYTE size[4];		// 额外信息长度
		BYTE perm;			// 权限
		BYTE type;			// 机型
		BYTE flag;			// 标记
		BYTE attr;			// 加密信息相关
		BYTE reserve[512];	// append
		BYTE crc32[4];		// 校验
	} SExtendInfo;

	typedef struct {
		ESystemUpgradeType upgradeType;
		std::string upgradeFilePath;
		std::string partName;
		SPartInfo partInfo;
		bool needUpgrade;
	} SSystemUpgradeInfo;

	/**
	 * TS升级文件信息
	 */
	typedef struct {
		BYTE hsize;			// 等于tsinfo大小
		BYTE type;			// 0=未知型号、1=TS_GT911
		BYTE flags;			// BIT(0)=强制升级位(1表示强制升级使能)、BIT(1)-BIT(7)保留
		BYTE reserve;		// 保留
		BYTE version[4];	// ts固件版本号
		BYTE pix[4];		// ts分辨率，H16=x L16=y，高16表示x，低16表示y
		BYTE date[4];		// ts固件生成日期，0x20171104 = 2017年11月4号
		BYTE dataLen[4];	// 数据长度
		BYTE hmd5[16];		// tsdata的MD5校验，不包含tsinfo，#define MD5_DIGEST_LENGTH 16
		BYTE *pData;		// 数据
		bool needUpgrade;
	} STSUpgradeInfo;

	/**
	 * ts类型
	 */
	enum ts_type {
		TS_GT911 = 1,
	};

	class IUpgradeStatusListener {
	public:
		virtual ~IUpgradeStatusListener() { };
		virtual void notify(int what, int status, const char *msg) = 0;
	};

	void setUpgradeStatusListener(IUpgradeStatusListener *pListener) {
		mUpgradeStatusListenerPtr = pListener;
	}

	std::vector<SSystemUpgradeInfo*>& getUpgradeInfoList() {
		return mSystemUpgradeInfoList;
	}

	const STSUpgradeInfo& getTSUpgradeInfo() const { return mTSUpgradeInfo; }

protected:
	virtual bool threadLoop();

private:
	UpgradeMonitor();

	bool checkUpgradeFile(const char *pPath);
	bool checkSystemUpgradeFile();
	bool checkTSUpgradeFile();
	bool upgradeFile(const SSystemUpgradeInfo &upgradeInfo);
	bool checkMD5(FILE *pf, const SPartInfo &partInfo);
	void resetSystemUpgradeInfo();
	void resetTSUpgradeInfo();

	void initMtdInfoList();
	const char* getMtdDevByName(const char *pName) const;
	bool writeMtdDev(const char *pDev, FILE *pf, const SSystemUpgradeInfo &upgradeInfo);

private:
	class UpgradeMountListener : public MountMonitor::IMountListener {
	public:
		UpgradeMountListener(UpgradeMonitor *pUM) : mUMPtr(pUM) {

		}

		virtual void notify(int what, int status, const char *msg) {
			switch (status) {
			case MountMonitor::E_MOUNT_STATUS_MOUNTED:
				mUMPtr->checkUpgradeFile(msg);
				break;

			case MountMonitor::E_MOUNT_STATUS_REMOVE:
				break;
			}
		}

	private:
		UpgradeMonitor *mUMPtr;
	};

private:
	UpgradeMountListener mUpgradeMountListener;
	IUpgradeStatusListener *mUpgradeStatusListenerPtr;

	bool mHasStartMonitor;

	std::vector<SSystemUpgradeInfo*> mSystemUpgradeInfoList;
	std::string mMountPath;

	bool mIsUpgrading;

	typedef struct {
		std::string dev;
		std::string name;
	} SMtdInfo;
	std::vector<SMtdInfo> mMtdInfoList;

	SExtendInfo mExtendInfo;

	STSUpgradeInfo mTSUpgradeInfo;

	std::string mVersionDateStr;
	const char *mErrorCode;
};

#define UPGRADEMONITOR		UpgradeMonitor::getInstance()

#endif /* _OS_UPGRADE_MONITOR_H_ */
