/*
 * VelocityTracker.h
 *
 *  Created on: Jul 19, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_VELOCITY_TRACKER_H_
#define _UTILS_VELOCITY_TRACKER_H_

#include <minigui/common.h>
#include "control/Common.h"

class VelocityTracker {
public:
	VelocityTracker();
	virtual ~VelocityTracker();

	void addMovement(const MotionEvent &ev);
	void reset();

	float getXVelocity() const;
	float getYVelocity() const;

private:
	BOOL canComputeVelocity() const;

private:
	int mCurIndex;
	MotionEvent mEvents[2];
};

#endif /* _UTILS_VELOCITY_TRACKER_H_ */
