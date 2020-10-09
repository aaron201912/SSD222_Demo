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

class ZKSeekBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKSeekBar)

public:
	ZKSeekBar(HWND hParentWnd);
	virtual ~ZKSeekBar();

	void setMax(int max);
	int getMax() const { return mMax; }

	void setProgress(int progress);
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
