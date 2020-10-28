/*
 * TouchEventTest.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: guoxs
 */

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#include "TouchEventTest.h"
#include "utils/ScreenHelper.h"
#include "utils/Log.h"


static int getRandNum(int range) {
	 srand(time(0));
	 return rand()%range;
}


static long getCurTime() {
	time_t timep;
	time(&timep);

	return timep;
}

TouchEventTest::TouchEventTest() :
	mTestDuration(-1),
	mStartTime(0) {

}

TouchEventTest* TouchEventTest::getInstance() {
	static TouchEventTest sTET;
	return &sTET;
}

void TouchEventTest::setDuration(int duration) {
	mTestDuration = duration;
}

void TouchEventTest::startTest() {
	LOGD("TouchEventTest startTest...\n");
	mStartTime = getCurTime();
	run("touchtest");
}

void TouchEventTest::stopTest() {
	LOGD("TouchEventTest stopTest...\n");
	requestExit();
}

bool TouchEventTest::threadLoop() {
	static int unitWidth = ScreenHelper::getScreenWidth() / 3;
	static int unitHeight = ScreenHelper::getScreenHeight() / 3;

	char cmd[100];
	sprintf(cmd, "echo %x%04x > /sys/bus/i2c/devices/1-005d/virtkey",
			getRandNum(3) * unitHeight + getRandNum(unitHeight),
			getRandNum(3) * unitWidth + getRandNum(unitWidth));
	system(cmd);

	sleep(10);

	return (mTestDuration == -1) || (getCurTime() - mStartTime > mTestDuration);
}
