/*
 * ZKCircleBar.h
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKCIRCLEBAR_H_
#define _CONTROL_ZKCIRCLEBAR_H_

#include "ZKBase.h"

class ZKCircleBarPrivate;

/**
 * @brief 圆形进度条控件
 */
class ZKCircleBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCircleBar)

public:
	ZKCircleBar(ZKBase *pParent);
	virtual ~ZKCircleBar();

	/**
	 * @brief 设置最大进度值
	 */
	void setMax(int max);

	/**
	 * @brief 获取最大进度值
	 */
	int getMax() const;

	/**
	 * @brief 设置当前进度值
	 */
	void setProgress(int progress);

	/**
	 * @brief 获取当前进度值
	 */
	int getProgress() const;

	/**
	 * @brief 设置起始角度
	 */
	void setStartAngle(int angle);

public:
	class ICircleBarChangeListener {
	public:
		virtual ~ICircleBarChangeListener() { }
		virtual void onProgressChanged(ZKCircleBar *pCircleBar, int progress) = 0;
		virtual void onStartTrackingTouch(ZKCircleBar *pCircleBar) { }
		virtual void onStopTrackingTouch(ZKCircleBar *pCircleBar) { }
	};

	void setCircleBarChangeListener(ICircleBarChangeListener *pListener);

protected:
	ZKCircleBar(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CIRCLEBAR; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseCircleBarAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKCIRCLEBAR_H_ */
