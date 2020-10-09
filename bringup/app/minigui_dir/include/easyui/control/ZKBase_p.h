/*
 * ZKBase_p.h
 *
 *  Created on: Jan 8, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKBASE_PRIVATE_H_
#define _CONTROL_ZKBASE_PRIVATE_H_

#include "ZKBase.h"
#include "utils/BitmapWrapper.h"
#include "display/SunxiDisplay1.h"

inline ZKBaseData::~ZKBaseData() { };

class ZKBasePrivate : public ZKBaseData {
	ZK_DECLARE_PUBLIC(ZKBase)

public:
	ZKBasePrivate();
	virtual ~ZKBasePrivate();

	int getLongClickTimeOut() const { return mLongClickTimeOut; }
	int getLongClickIntervalTime() const { return mLongClickIntervalTime; }

protected:
	void setBackgroundPic(const char *pPicPath);
	void setBackgroundBmp(PBITMAP pBmp);

	void drawBackground(HDC hdc);

protected:
	int mBgStatusColorTab[ZKBase::S_CONTROL_STATUS_TAB_LEN];
	BitmapWrapper mBackgroundBmp;

	int mLongClickTimeOut;
	int mLongClickIntervalTime;

	SunxiDisplay1 *mBgDispPtr;		// 背景使用图层方式

	ZKBase::ITouchListener *mTouchListenerPtr;
};

#endif /* _CONTROL_ZKBASE_PRIVATE_H_ */
