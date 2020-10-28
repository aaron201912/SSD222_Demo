/*
 * TouchEventTest.h
 *
 *  Created on: Jan 21, 2018
 *      Author: guoxs
 */

#ifndef _TEST_TOUCH_EVENT_TEST_H_
#define _TEST_TOUCH_EVENT_TEST_H_

#include "system/Thread.h"

class TouchEventTest : public Thread {
public:
	static TouchEventTest* getInstance();

	void setDuration(int duration = -1);	// 设置触摸测试时长，单位：s，默认-1表示永久触摸测试
	void startTest();
	void stopTest();

private:
	TouchEventTest();

	virtual bool threadLoop();

private:
	int mTestDuration;
	long mStartTime;
};

#endif /* _TEST_TOUCH_EVENT_TEST_H_ */
