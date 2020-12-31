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
	ZKSeekBar(ZKBase *pParent);
	virtual ~ZKSeekBar();

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

public:
	class ISeekBarChangeListener {
	public:
		virtual ~ISeekBarChangeListener() { }
		virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) = 0;
		virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) { }
		virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) { }
	};

	void setSeekBarChangeListener(ISeekBarChangeListener *pListener);

protected:
	ZKSeekBar(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SEEKBAR; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseSeekBarAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKSEEKBAR_H_ */
