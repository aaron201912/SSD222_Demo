/*
 * ZKWindow_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKWINDOW_PRIVATE_H_
#define _CONTROL_ZKWINDOW_PRIVATE_H_

#include "control/ZKBase_p.h"

#ifdef ENABLE_ANIM
#include "animation/ZKAnimation.h"
#endif

class ZKWindow;

class ZKWindowPrivate : public ZKBasePrivate {
	ZK_DECLARE_PUBLIC(ZKWindow)

public:
	ZKWindowPrivate();
	virtual ~ZKWindowPrivate();

#ifdef ENABLE_ANIM
	void performMoveAnim(bool isIn);

private:
	class MoveAnimationListener : public ZKAnimation::IAnimationListener {
	public:
		MoveAnimationListener(ZKWindowPrivate &wp) : mWP(wp) { }
		virtual void onAnimationUpdate(ZKAnimation *pAnimation, const void *value);
		virtual void onAnimationFinished(ZKAnimation *pAnimation);

	private:
		ZKWindowPrivate &mWP;
	};

	void moveWindow(int pos);
	void moveFinished();

private:
	typedef enum {
		E_ANIM_DIRECTION_LEFT_TO_RIGHT,
		E_ANIM_DIRECTION_RIGHT_TO_LEFT,
		E_ANIM_DIRECTION_TOP_TO_BOTTOM,
		E_ANIM_DIRECTION_BOTTOM_TO_TOP
	} EAnimDirection;

	EAnimDirection mAnimDirection;

	int mAnimDuration;
	bool mIsMovingIn;
	LayoutPosition mOriginPos;


	ZKAnimation mMoveAnimation;
	MoveAnimationListener mMoveAnimationListener;
#endif
};

#endif /* _CONTROL_ZKWINDOW_PRIVATE_H_ */
