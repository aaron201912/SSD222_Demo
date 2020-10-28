/*
 * ZKSeekBar.h
 *
 *  Created on: Jun 26, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKSEEKBAR_H_
#define _CONTROL_ZKSEEKBAR_H_

#include "ZKBase.h"

class ZKSeekBarPrivate;

/**
 * @brief 滑动条控件
 */
class ZKSeekBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKSeekBar)

public:
	ZKSeekBar(HWND hParentWnd);
	virtual ~ZKSeekBar();

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

public:
	class ISeekBarChangeListener {
	public:
		virtual ~ISeekBarChangeListener() { }
		virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) = 0;
		virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) { }
		virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) { }
	};

	void setSeekBarChangeListener(ISeekBarChangeListener *pListener) {
		mSeekBarChangeListenerPtr = pListener;
	}

protected:
	ZKSeekBar(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SEEKBAR; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);

	void _section_(zk) drawProgress(HDC hdc);
	void _section_(zk) drawThumb(HDC hdc);

private:
	void _section_(zk) parseSeekBarAttributeFromJson(const Json::Value &json);
	BOOL isHorizontalOrientation() const { return mOrientation == E_ORIENTATION_HORIZONTAL; }
	int calculateProgress(int x, int y);

protected:
	ISeekBarChangeListener *mSeekBarChangeListenerPtr;

	int mMax;
	int mProgress;
	EOrientation mOrientation;	// 方向: 0 横向, 1 纵向

	PBITMAP mProgressPicPtr;
	PBITMAP mThumbNormalPicPtr;
	PBITMAP mThumbPressedPicPtr;

	SIZE mThumbSize;
};

#endif /* _CONTROL_ZKSEEKBAR_H_ */
