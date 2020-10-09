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

class ZKTimer : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKTimer)

public:
	ZKTimer(HWND hParentWnd);
	virtual ~ZKTimer();

	void setIntervalTime(UINT time);
	void start();
	void stop();
	void reset();
	bool isRunning() const;

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
