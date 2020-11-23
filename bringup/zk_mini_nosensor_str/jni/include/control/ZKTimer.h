/*
 * ZKTimer.h
 *
 *  Created on: Jul 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKTIMER_H_
#define _CONTROL_ZKTIMER_H_

#include "ZKBase.h"

class ZKTimerPrivate;

/**
 * @brief 定时器控件
 */
class ZKTimer : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKTimer)

public:
	ZKTimer(HWND hParentWnd);
	virtual ~ZKTimer();

	/**
	 * @brief 设置定时器时间间隔
	 * @param time 单位为毫秒
	 */
	void setIntervalTime(UINT time);

	/**
	 * @brief 启动定时器
	 */
	void start();

	/**
	 * @brief 停止定时器
	 */
	void stop();

	/**
	 * @brief 重置定时器
	 */
	void reset();

	/**
	 * @brief 定时器是否运行中
	 */
	bool isRunning() const;

	/**
	 * @brief 定时器监听接口
	 */
	class ITimerListener {
	public:
		virtual ~ITimerListener() { }
		virtual void onTimer(ZKTimer *pTimer) = 0;
	};

	void setTimerListener(ITimerListener *pListener);

protected:
	ZKTimer(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_TIMER; }

	virtual BOOL onInterceptMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	virtual void onTimer(int id);
};

#endif /* _CONTROL_ZKTIMER_H_ */
