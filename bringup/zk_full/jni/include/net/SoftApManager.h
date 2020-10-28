#ifndef _NET_SOFT_AP_MANAGER_H_
#define _NET_SOFT_AP_MANAGER_H_

#include <vector>

#include "system/Thread.h"
#include "utils/MessageQueue.h"

typedef enum {
	E_SOFTAP_DISABLED,
	E_SOFTAP_ENABLING,
	E_SOFTAP_ENABLED,
	E_SOFTAP_DISABLING,
	E_SOFTAP_ENABLE_ERROR
} ESoftApState;

/**
 * @brief 热点管理器
 */
class SoftApManager {
public:
	SoftApManager();
	~SoftApManager();

	bool init();

	/**
	 * @brief 打开、关闭热点
	 * @param isEnable true 打开热点，false 关闭热点
	 */
	void setEnable(bool isEnable);

	/**
	 * @brief 热点是否已打开
	 */
	bool isEnable() const;

	ESoftApState getSoftApState() const;

	/**
	 * @brief 获取IP地址
	 */
	const char* getIp() const;

	/**
	 * @brief 设置热点名称和密码
	 * @param ssid 热点名称
	 * @param pwd 热点密码
	 */
	void setSsidAndPwd(const char *ssid, const char *pwd);

	/**
	 * @brief 获取热点名称
	 */
	const char* getSsid() const;

	/**
	 * @brief 获取热点密码
	 */
	const char* getPwd() const;

	/**
	 * @brief 热点状态监听接口
	 */
	class ISoftApStateListener {
	public:
		virtual ~ISoftApStateListener() { }
		virtual void handleSoftApState(ESoftApState state) = 0;
	};

	/**
	 * @brief 添加热点状态监听接口
	 */
	void addSoftApStateListener(ISoftApStateListener *pListener);

	/**
	 * @brief 移除热点状态监听接口
	 */
	void removeSoftApStateListener(ISoftApStateListener *pListener);

private:
	void updateSoftApState(ESoftApState state);

	typedef enum {
		E_MSGTYPE_ENABLE = 1,
		E_MSGTYPE_DISABLE,
		E_MSGTYPE_SET_SSID_PWD,
		E_MSGTYPE_QUIT_THREAD
	} EMessageType;

	class SoftApThread : public Thread {
	public:
		SoftApThread(SoftApManager &sam) : mSAM(sam) { }

		bool start();
		void stop();

	protected:
		virtual bool readyToRun();
		virtual bool threadLoop();

	private:
		SoftApManager &mSAM;
	};

private:
	MessageQueue mMsgQueue;

	ESoftApState mState;

	mutable Mutex mLock;

	std::vector<ISoftApStateListener *> mSoftApStateListenerList;
	mutable Mutex mListenerLock;

	SoftApThread mSoftApThread;
};

#endif	// _NET_SOFT_AP_MANAGER_H_
