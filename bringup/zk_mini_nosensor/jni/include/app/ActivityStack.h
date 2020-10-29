/*
 * ActivityStack.h
 *
 *  Created on: Aug 21, 2017
 *      Author: guoxs
 */

#ifndef _APP_ACTIVITY_STACK_H_
#define _APP_ACTIVITY_STACK_H_

#include "Activity.h"
#include "animation/ZKAnimation.h"

class ActivityStack {
public:
	typedef std::pair<std::string, Activity*> ActivityEntry;
	typedef std::vector<ActivityEntry> ActivityCollection;

	static ActivityStack* getInstance();

	Activity* top() const;
	Activity* getHomeActivity() const;

	bool isHomeActivity(const char *appName) const;

	const char* currentAppName() const;

	bool push(const char *appName, Intent *intentPtr = NULL);
	bool pop();

	bool goBack();
	bool goHome();

	bool removeActivity(const char *appName);

	int depth() const { return mActivityCollection.size(); }

	void switchTo(const char *appName, Intent *intentPtr = NULL);
	void clearStack();

	void dumpStack() const;

private:
	ActivityStack();
	virtual ~ActivityStack();

	Activity* innerPush(const char *appName, Intent *intentPtr);
	bool innerPop();

	int popTo(Activity *which, Intent *intentPtr = NULL);
	Activity* underTop() const;

	bool popToHome(Intent *intentPtr = NULL);

	Activity* searchActivityByName(const char *appName) const;

	void doSwitchEffect(const Activity *prev, const Activity *next, bool switchTo = true);

	void doZoomAnimation(const Activity *prev, const Activity *next, bool switchTo = true);

private:
	class ZoomAnimationListener : public ZKAnimation::IAnimationListener {
	public:
		ZoomAnimationListener();
		virtual void onAnimationUpdate(ZKAnimation *pAnimation, const void *value);

		void reset();

	public:
		LayoutPosition mPrevPos;
		LayoutPosition mDstPos;
		HDC mSrcDC;
		HDC mDstDC;
		HWND mWnd;
	};

private:
	ActivityCollection mActivityCollection;

	ZoomAnimationListener mZoomAnimationListener;
};

#define ACTIVITYSTACK			ActivityStack::getInstance()

#endif /* _APP_ACTIVITY_STACK_H_ */
