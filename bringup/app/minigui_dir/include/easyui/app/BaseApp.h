/*
 * BaseApp.h
 *
 *  Created on: Aug 24, 2017
 *      Author: guoxs
 */

#ifndef _APP_BASEAPP_H_
#define _APP_BASEAPP_H_

#include "window/ZKMainWindow.h"

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

	virtual void onCreate();
	virtual void onClick(ZKBase *pBase);
	virtual bool onTimer(int id);

	ZKBase* findControlByID(int id);

	void registerTimer(int id, int time);
	void unregisterTimer(int id);
	void resetTimer(int id, int time);

protected:
	ZKMainWindow *mMainWndPtr;
};

#endif /* _APP_BASEAPP_H_ */
