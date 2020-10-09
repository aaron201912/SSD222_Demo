/*
 * TouchCalibHelper.h
 *
 *  Created on: Jun 8, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_TOUCH_CALIB_HELPER_H_
#define _UTILS_TOUCH_CALIB_HELPER_H_

#include <tslib.h>

class TouchCalibHelper {
public:
	bool isSupportCalibration();
	bool isNeedCalibration();

	bool enterCalibrationMode();
	bool exitCalibrationMode();
	bool getSample(int xfb, int yfb);

	static TouchCalibHelper* getInstance();

private:
	TouchCalibHelper();

	void reset();
	bool clearTSBuf();
	bool performCalibration();
	bool updateCalibFile();

private:
	struct {
		int x[5], xfb[5];
		int y[5], yfb[5];
		int a[7];
	} mCalibration;

	int mIndex;

	struct tsdev *mTSDev;
};

#define TOUCHCALIBHELPER		TouchCalibHelper::getInstance()

#endif /* _UTILS_TOUCH_CALIB_HELPER_H_ */
