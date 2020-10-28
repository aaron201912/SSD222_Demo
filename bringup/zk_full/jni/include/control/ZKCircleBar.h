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
	ZKCircleBar(HWND hParentWnd);
	virtual ~ZKCircleBar();

	/**
	 * @brief 设置最大进度值
	 */
	void setMax(int max);

	/**
	 * @brief 获取最大进度值
	 */
	int getMax() const { return mMax; }

	/**
	 * @brief 设置当前进度值
	 */
	void setProgress(int progress);

	/**
	 * @brief 获取当前进度值
	 */
	int getProgress() const { return mProgress; }

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
	ZKCircleBar(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CIRCLEBAR; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);

	void _section_(zk) drawProgress(HDC hdc);
	void _section_(zk) drawThumb(HDC hdc);
	void _section_(zk) drawProgressText(HDC hdc);

private:
	void _section_(zk) parseCircleBarAttributeFromJson(const Json::Value &json);

	bool isInTouchRange(int x, int y);
	int calculateProgress(int x, int y);

protected:
	typedef enum {
		E_TEXT_TYPE_NONE,				// 不绘制文本
		E_TEXT_TYPE_NUM,				// 数字
		E_TEXT_TYPE_NUM_WITH_UNIT		// 数字 + 单位
	} EProgressTextType;

	int mMax;
	int mProgress;

	EProgressTextType mTextType;
	PLOGFONT mLogFontPtr;
	int mTextSize;
	int mTextColor;
	string mUnitStr;		// 单位, 默认: %

	int mStartAngle;
	bool mIsClockwise;	// 顺时针方向

	PBITMAP mProgressPicPtr;
	HDC mProgressDC;
};

#endif /* _CONTROL_ZKCIRCLEBAR_H_ */
