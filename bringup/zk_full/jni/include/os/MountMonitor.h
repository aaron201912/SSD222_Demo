#ifndef _OS_MOUNT_MONITOR_H
#define _OS_MOUNT_MONITOR_H

#include <vector>
#include "system/Thread.h"

/**
 * @brief 挂载监听
 */
class MountMonitor : Thread {
public:
	/**
	 * @brief 挂载监听接口
	 */
	class IMountListener {
	public:
		virtual ~IMountListener() { };
		virtual void notify(int what, int status, const char *msg) = 0;
	};

	/**
	 * @brief 挂载状态
	 */
	typedef enum {
		E_MOUNT_STATUS_NO_MEDIA = 0,
		E_MOUNT_STATUS_IDLE,
		E_MOUNT_STATUS_PENDING,
		E_MOUNT_STATUS_CHECKING,
		E_MOUNT_STATUS_MOUNTED,		/**< 已挂载上 */
		E_MOUNT_STATUS_UNMOUNTING,
		E_MOUNT_STATUS_FORMATTING,
		E_MOUNT_STATUS_SHARED,
		E_MOUNT_STATUS_SHAREDMNT,
		E_MOUNT_STATUS_REMOVE		/**< 移除挂载 */
	} EMountStatus;

public:
	virtual ~MountMonitor();

	/**
	 * @brief 添加挂载监听
	 */
	void addMountListener(IMountListener *pListener);

	/**
	 * @brief 移除挂载监听
	 */
	void removeMountListener(IMountListener *pListener);

	int mountToPC();
	int unmountFromPC();
	int formatSDcard();
	bool isInsert();

	/**
	 * @brief SD卡是否已挂载
	 */
	bool isMount();

	bool isShare();
	bool isFormated();

	bool isMounted(const char *checkPath);
	const char* getSDcardPath() const;

	static MountMonitor* getInstance();

protected:
	virtual bool threadLoop();

private:
	MountMonitor();

	int init();
	int exit();

	void notifyListeners(int what, int status, const char *msg);

	void processStr(char *src);

private:
	bool mShared;
	bool mSharing;
	bool mInserted;
	bool mMounted;
	bool mFormated;
	bool mFormating;

	int mStatus;
	int mConnectFD;
	char mPath[125];

	std::vector<IMountListener *> mListeners;

	static Mutex sLock;
};

#define MOUNTMONITOR		MountMonitor::getInstance()

#endif

