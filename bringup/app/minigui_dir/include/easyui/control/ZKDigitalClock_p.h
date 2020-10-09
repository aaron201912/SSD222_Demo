/*
 * ZKDigitalClock_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKDIGITALCLOCK_PRIVATE_H_
#define _CONTROL_ZKDIGITALCLOCK_PRIVATE_H_

#include "ZKTextView_p.h"

class ZKDigitalClock;

class ZKDigitalClockPrivate : public ZKTextViewPrivate {
	ZK_DECLARE_PUBLIC(ZKDigitalClock)

public:
	virtual ~ZKDigitalClockPrivate() { }
};

#endif /* _CONTROL_ZKDIGITALCLOCK_PRIVATE_H_ */
