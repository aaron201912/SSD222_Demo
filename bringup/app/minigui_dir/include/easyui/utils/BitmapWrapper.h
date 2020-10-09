/*
 * BitmapWrapper.h
 *
 *  Created on: Mar 22, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_BITMAP_WRAPPER_H_
#define _UTILS_BITMAP_WRAPPER_H_

#include <minigui/common.h>
#include <minigui/gdi.h>
#include "system/Thread.h"

class SunxiDisplay1;

class BitmapWrapper {
public:
	BitmapWrapper();
	virtual ~BitmapWrapper();

	bool loadBitmapFromFile(const char *pPath);
	void fillBoxWithBitmap(HDC hdc, int x, int y, int w, int h);

	void fillToDisplay(SunxiDisplay1 *pDisp);

	void setBitmap(PBITMAP pBmp);

	bool isValid() const { return mBmpPtr != NULL; }

	void reset();

private:
	void updateBlackPixelInfo();
	void fillNinePatch(HDC hdc, int x, int y, int w, int h);
	void resetRes();

private:
	PBITMAP mBmpPtr;
	HDC mMemDC;

	typedef struct {
		int pos;
		int size;
	} SBlackPixelInfo;

	SBlackPixelInfo mTopBlackPixelInfo;
	SBlackPixelInfo mLeftBlackPixelInfo;

	mutable Mutex mLock;
};

#endif /* _UTILS_BITMAP_WRAPPER_H_ */
