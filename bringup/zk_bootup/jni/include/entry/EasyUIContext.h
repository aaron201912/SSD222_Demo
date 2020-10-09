/*
 * EasyUIContext.h
 *
 *  Created on: Aug 24, 2017
 *      Author: guoxs
 */

#ifndef _ENTRY_EASYUI_CONTEXT_H_
#define _ENTRY_EASYUI_CONTEXT_H_

#include <vector>
#include <stddef.h>

#include "control/Common.h"
#include "system/Mutex.h"
#include "ime/IMEContext.h"

class Intent;
class BaseApp;
class IMEBaseApp;

class EasyUIContext {
	friend int main(int argc, const char *argv[]);
public:
	/**
	 * @brief 全局触摸监听接口
	 */
	class ITouchListener {
	public:
		virtual ~ITouchListener() { }
		virtual bool onTouchEvent(const MotionEvent &ev) = 0;
	};

	/**
	 * @brief 注册全局触摸监听接口
	 */
	void registerGlobalTouchListener(ITouchListener *pListener);

	/**
	 * @brief 反注册全局触摸监听接口
	 */
	void unregisterGlobalTouchListener(ITouchListener *pListener);

	bool notifyGlobalTouchListener(const MotionEvent &ev);

	/**
	 * @brief 按键监听接口
	 */
	class IKeyListener {
	public:
		virtual ~IKeyListener() { }
		virtual bool onKeyEvent(const KeyEvent &ke) = 0;
	};

	/**
	 * @brief 注册按键监听接口
	 */
	void registerKeyListener(IKeyListener *pListener);

	/**
	 * @brief 反注册按键监听接口
	 */
	void unregisterKeyListener(IKeyListener *pListener);

	bool notifyKeyListener(const KeyEvent &ke);

public:
	static EasyUIContext* getInstance();

	/**
	 * @brief 打开界面
	 * @param appName 界面名称
	 * @param intentPtr 界面间数据传递对象，默认为NULL
	 * @attention 传入的intentPtr必须是堆里new出来的对象，最后由框架内部完成delete
	 */
	void openActivity(const char *appName, Intent *intentPtr = NULL);

	/**
	 * @brief 关闭界面
	 * @param appName 界面名称
	 */
	void closeActivity(const char *appName);

	/**
	 * @brief 回到上一界面
	 */
	void goBack();

	/**
	 * @brief 回到主界面
	 */
	void goHome();

	/**
	 * @brief 当前界面名称
	 */
	const char* currentAppName() const;

	/**
	 * @brief 显示状态栏
	 */
	void showStatusBar();

	/**
	 * @brief 隐藏状态栏
	 */
	void hideStatusBar();

	/**
	 * @brief 状态栏是否显示
	 */
	bool isStatusBarShow() const;

	/**
	 * @brief 显示导航栏
	 */
	void showNaviBar();

	/**
	 * @brief 隐藏导航栏
	 */
	void hideNaviBar();

	/**
	 * @brief 导航栏是否显示
	 */
	bool isNaviBarShow() const;

	void powerOff();
	void powerOn();
	bool isPowerOff() const;

	/**
	 * @brief 设置屏保超时时间
	 * @param timeOut 单位为秒，-1时表示不进屏保
	 */
	void setScreensaverTimeOut(int timeOut);

	/**
	 * @brief 获取屏保超时时间
	 */
	int getScreensaverTimeOut() const;

	/**
	 * @brief 重置屏保检测时间
	 */
	void resetScreensaverTimeOut();

	void performResetScreensaverTimeOut();

	/**
	 * @brief 设置是否允许启用屏保
	 * @param isEnable true 允许启用屏保，false 禁止启用屏保
	 * @note 应用场景：如升级界面不能进入屏保界面，可以在升级应用里setScreensaverEnable(false)关闭屏保检测，退出setScreensaverEnable(true)恢复屏保功能
	 */
	void setScreensaverEnable(bool isEnable);

	/**
	 * @brief 是否允许启用屏保
	 */
	bool isScreensaverEnable() const;

	/**
	 * @brief 打开屏保
	 */
	void screensaverOn();

	/**
	 * @brief 关闭屏保
	 */
	void screensaverOff();

	/**
	 * @brief 是否已进入屏保
	 */
	bool isScreensaverOn() const;

	void performScreensaverOn();
	void performScreensaverOff();

	void showIME(const IMEContext::SIMETextInfo &info, IMEContext::IIMETextUpdateListener *pListener);
	void hideIME();
	void performHideIME();

	/**
	 * @brief 输入法是否显示
	 */
	bool isIMEShow() const;

	/**
	 * @brief 切换多国语言
	 * @param code 语言编码
	 */
	void updateLocalesCode(const char *code);

private:
	EasyUIContext();
	~EasyUIContext();

	bool initEasyUI();
	void deinitEasyUI();
	void runEasyUI();

	bool initLib();
	void deinitLib();

	void loadStatusBar();
	void loadNaviBar();

	void unloadStatusBar();
	void unloadNaviBar();

	void performEasyUIInit();
	void performEasyUIDeinit();
	void performStartupApp();
	void performLoadStatusBar();
	void performLoadNaviBar();
	void performScreensaver();

private:
	std::vector<ITouchListener *> mGlobalTouchListenerList;
	mutable Mutex mTouchLock;

	BaseApp *mStatusBarPtr;
	BaseApp *mNaviBarPtr;
	BaseApp *mScreensaverPtr;

	IMEBaseApp *mIMEPtr;

	struct {
		void (*onEasyUIInit)(EasyUIContext*);
		void (*onEasyUIDeinit)(EasyUIContext*);
		const char* (*onStartupApp)(EasyUIContext*);
	} mLibFunInfo;

	void *mLibHandle;
	void *mInternalLibHandle;

	bool mIsUpgradeMode;

	int mScreensaverTimeOut;
	bool mIsScreensaverEnable;

	void *mReservePtr;
};

#define EASYUICONTEXT			EasyUIContext::getInstance()

#endif /* _ENTRY_EASYUI_CONTEXT_H_ */
