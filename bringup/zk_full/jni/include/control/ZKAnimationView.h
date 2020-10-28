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

/**
 * @brief 动画控件
 */
class ZKAnimationView : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKAnimationView)

public:
	ZKAnimationView(HWND hParentWnd);
	virtual ~ZKAnimationView();

	/**
	 * @brief 开始动画
	 */
	void start();

	/**
	 * @brief 暂停动画
	 */
	void pause();

	/**
	 * @brief 动画是否播放中
	 */
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
