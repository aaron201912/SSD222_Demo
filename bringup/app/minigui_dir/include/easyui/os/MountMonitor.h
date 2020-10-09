#ifndef _OS_MOUNT_MONITOR_H
#define _OS_MOUNT_MONITOR_H

#include <vector>
#include "system/Thread.h"

class MountMonitor : Thread {
public:
	class IMountListener {
	public:
		virtual ~IMountListener() { };
		virtual void notify(int what, int status, const char *msg) = 0;
	};

	typedef enum {
		E_MOUNT_STATUS_NO_MEDIA = 0,
		E_MOUNT_STATUS_IDLE,
		E_MOUNT_STATUS_PENDING,
		E_MOUNT_STATUS_CHECKING,
		E_MOUNT_STATUS_MOUNTED,
		E_MOUNT_STATUS_UNMOUNTING,
		E_MOUNT_STATUS_FORMATTING,
		E_MOUNT_STATUS_SHARED,
		E_MOUNT_STATUS_SHAREDMNT,
		E_MOUNT_STATUS_REMOVE
	} EMountStatus;

public:
	virtual ~MountMonitor();

	void addMountListener(IMountListener *pListener);
	void removeMountListener(IMountListener *pListener);

	int mountToPC();
	int unmountFromPC();
	int formatSDcard();
	bool isInsert();
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
};

#define MOUNTMONITOR		MountMonitor::getInstance()

#endif

