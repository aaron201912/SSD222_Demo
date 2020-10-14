/*
 * ZKAnimation.h
 *
 *  Created on: Nov 30, 2017
 *      Author: guoxs
 */

#ifndef _ANIMATION_ZKANIMATION_H_
#define _ANIMATION_ZKANIMATION_H_

#include <mgeff/mgeff.h>

class ZKAnimation {
	friend class ZKAnimationGroup;
public:
	ZKAnimation();
	virtual ~ZKAnimation();

	bool create(enum EffVariantType varianttype);
	bool createEx(int variantsize);

	void setStartValue(const void *value);
	void setEndValue(const void *value);

	void setDuration(int ms);
	void setLoopCount(int count);
	void setDirection(bool isForward);

	void setCurve(enum EffMotionType type);

	void start(bool isAsyncRun = true);
	void pause();
	void resume();
	void stop();
	void wait();

	bool isRunning() const;

public:
	class IAnimationListener {
	public:
		virtual ~IAnimationListener() { }

		virtual void onAnimationUpdate(ZKAnimation *pAnimation, const void *value) { }
		virtual void onAnimationFinished(ZKAnimation *pAnimation) { }
		virtual void onAnimationCurloopChanged(ZKAnimation *pAnimation) { }
		virtual void onAnimationStateChanged(ZKAnimation *pAnimation, enum EffState newEffState, enum EffState oldEffState) { }
		virtual void* onAnimationVarMalloc(ZKAnimation *pAnimation) { return NULL; }
		virtual void onAnimationCalcValue(ZKAnimation *pAnimation, const void *start, const void *end, void *value, float factor) { }
	};

	void setAnimationListener(IAnimationListener *pListener) {
		mAnimationListenerPtr = pListener;
	}

protected:
	void initSetup();

	void setProperty(enum EffAnimProperty property, int value);
	int getProperty(enum EffAnimProperty property) const;

private:
	static void updateCallback(MGEFF_ANIMATION handle, void *target, int id, void *value);
	static void finishedCallback(MGEFF_ANIMATION handle);
	static void curloopChangedCallback(MGEFF_ANIMATION handle);
	static void stateChangedCallback(MGEFF_ANIMATION handle, enum EffState newEffState, enum EffState oldEffState);
	static void* varMallocCallback(MGEFF_ANIMATION handle);
	static void calcValueCallback(MGEFF_ANIMATION handle, void *s, void *e, void *v, float factor);

protected:
	MGEFF_ANIMATION mAnimationHandle;
	IAnimationListener *mAnimationListenerPtr;
};

#endif /* _ANIMATION_ZKANIMATION_H_ */
