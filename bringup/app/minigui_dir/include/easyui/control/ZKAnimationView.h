/*
 * ZKAnimationView.h
 *
 *  Created on: Apr 26, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKANIMATIONVIEW_H_
#define _CONTROL_ZKANIMATIONVIEW_H_

#include "ZKTextView.h"

class ZKAnimationViewPrivate;

class ZKAnimationView : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKAnimationView)

public:
	ZKAnimationView(HWND hParentWnd);
	virtual ~ZKAnimationView();

	void start();
	void pause();
	bool isPlaying() const;

protected:
	ZKAnimationView(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_ANIMVIEW; }

	virtual void onTimer(int id);

private:
	void parseAnimViewAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKANIMATIONVIEW_H_ */
