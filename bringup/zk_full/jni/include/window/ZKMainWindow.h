/*
 * ZKMainWindow.h
 *
 *  Created on: Jun 10, 2017
 *      Author: guoxs
 */

#ifndef _WINDOW_ZKMAINWINDOW_H_
#define _WINDOW_ZKMAINWINDOW_H_

#include "ZKWindow.h"
#include "system/Mutex.h"

class ZKMainWindowPrivate;

class ZKMainWindow : public ZKWindow {
	ZK_DECLARE_PRIVATE(ZKMainWindow)

public:
	ZKMainWindow();
	virtual ~ZKMainWindow();

	class ITimerListener {
	public:
		virtual ~ITimerListener() { }
		virtual bool onTimer(int id) = 0;
	};

	void registerTimerListener(int id, uint32_t time, ITimerListener *pListener);
	void unregisterTimerListener(int id, ITimerListener *pListener);
	void resetTimer(int id, uint32_t time);

protected:
	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual bool createWindow();
	virtual ret_t procCtrlFun(const struct _message_t *pMsg);

private:
	const char* getClassName() const { return NULL; }
	void parseMainWindowAttributeFromJson(const Json::Value &json);

	void notifyTimerListener(int id);
};

#endif /* _WINDOW_ZKMAINWINDOW_H_ */
