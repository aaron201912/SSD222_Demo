/*
 * ZKSlideWindow.h
 *
 *  Created on: Jul 3, 2017
 *      Author: guoxs
 */

#ifndef _WINDOW_ZKSLIDEWINDOW_H_
#define _WINDOW_ZKSLIDEWINDOW_H_

#include "ZKWindow.h"

class ZKSlideWindowPrivate;

/**
 * @brief 滑动窗口控件
 */
class ZKSlideWindow : public ZKWindow {
	ZK_DECLARE_PRIVATE(ZKSlideWindow)

public:
	ZKSlideWindow(ZKBase *pParent);
	virtual ~ZKSlideWindow();

public:
	/**
	 * @brief 滑动项点击监听接口
	 */
	class ISlideItemClickListener {
	public:
		virtual ~ISlideItemClickListener() { }
		virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) = 0;
	};

	void setSlideItemClickListener(ISlideItemClickListener *pListener);

	/**
	 * @brief 翻页监听接口
	 */
	class ISlidePageChangeListener {
	public:
		virtual ~ISlidePageChangeListener() { }
		virtual void onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) = 0;
	};

	void setSlidePageChangeListener(ISlidePageChangeListener *pListener);

	/**
	 * @brief 获取当前页位置
	 */
	int getCurrentPage() const;

	/**
	 * @brief 获取总页数
	 */
	int getPageSize() const;

	/**
	 * @brief 切换到下一页
	 * @param isAnimatable 是否开启翻页动画，默认为false，不开启动画
	 */
	void turnToNextPage(bool isAnimatable = false);

	/**
	 * @brief 切换到上一页
	 * @param isAnimatable 是否开启翻页动画，默认为false，不开启动画
	 */
	void turnToPrevPage(bool isAnimatable = false);

protected:
	ZKSlideWindow(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SLIDEWINDOW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

private:
	void parseSlideWindowAttributeFromJson(const Json::Value &json);
};

#endif /* _WINDOW_ZKSLIDEWINDOW_H_ */
