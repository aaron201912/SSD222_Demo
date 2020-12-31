/*
 * ZKDigitalClock.h
 *
 *  Created on: Sep 19, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKDIGITALCLOCK_H_
#define _CONTROL_ZKDIGITALCLOCK_H_

#include "ZKTextView.h"

class ZKDigitalClockPrivate;

/**
 * @brief 数字时钟控件
 */
class ZKDigitalClock : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKDigitalClock)

public:
	ZKDigitalClock(ZKBase *pParent);
	virtual ~ZKDigitalClock();

protected:
	ZKDigitalClock(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIGITALCLOCK; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual void onTimer(int id);

private:
	void parseDigitalClockAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKDIGITALCLOCK_H_ */
