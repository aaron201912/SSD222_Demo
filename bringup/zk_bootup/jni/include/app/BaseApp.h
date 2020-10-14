/*
 * BaseApp.h
 *
 *  Created on: Aug 24, 2017
 *      Author: guoxs
 */

#ifndef _APP_BASEAPP_H_
#define _APP_BASEAPP_H_

#include "window/ZKMainWindow.h"

/**
 * @brief 应用基类
 */
class BaseApp :
	public ZKBase::IClickListener,
	public ZKMainWindow::ITimerListener {
public:
	BaseApp();
	virtual ~BaseApp();

	bool create();
	void show();
	void hide();
	bool isShow() const;

	HDC snapshot() const;

	void updateLocales();

protected:
	virtual const char* getAppName() const { return NULL; }

	/**
	 * @brief 创建完成回调函数
	 */
	virtual void onCreate();

	/**
	 * @brief 按钮点击事件回调函数
	 */
	virtual void onClick(ZKBase *pBase);

	/**
	 * @brief 定时器回调函数
	 */
	virtual bool onTimer(int id);

	/**
	 * @brief 通过id值找到对应的控件
	 */
	ZKBase* findControlByID(int id);

	/**
	 * @brief 注册定时器
	 * @param id 定时器id
	 * @param time 定时器时间周期
	 */
	void registerTimer(int id, int time);

	/**
	 * @brief 反注册定时器
	 * @param id 定时器id
	 */
	void unregisterTimer(int id);

	/**
	 * @brief 重置定时器
	 * @param id 定时器id
	 * @param time 定时器时间周期
	 */
	void resetTimer(int id, int time);

protected:
	ZKMainWindow *mMainWndPtr;
};

#endif /* _APP_BASEAPP_H_ */
