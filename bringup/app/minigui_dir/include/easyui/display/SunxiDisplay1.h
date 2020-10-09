/*
 * SunxiDisplay1.h
 *
 *  Created on: Nov 13, 2018
 *      Author: guoxs
 */

#ifndef _DISPLAY_SUNXI_DISPLAY1_H_
#define _DISPLAY_SUNXI_DISPLAY1_H_

#ifdef ENABLE_SUNXI_DISPLAY1
extern "C" {
#include "videoOutPort.h"
}
#endif

class SunxiDisplay1 {
public:
	SunxiDisplay1(bool isYuvFormat = true);
	virtual ~SunxiDisplay1();

	bool isInit() const;
	bool initDisp(int srcWidth, int srcHeight);
	bool deinitDisp();
	bool setPreviewPos(int left, int top, int width, int height);
	bool setCropPos(int left, int top, int width, int height);
	bool writeData(void *pData, int w, int h);

private:
	SunxiDisplay1& operator=(const SunxiDisplay1&);
	SunxiDisplay1(const SunxiDisplay1&);

#ifdef ENABLE_SUNXI_DISPLAY1
	dispOutPort *mDispOutPortPtr;
	bool mIsYuvFormat;
	bool mIsEnable;
#endif
};

#endif /* _DISPLAY_SUNXI_DISPLAY1_H_ */
