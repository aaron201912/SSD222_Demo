/*
 * ZKScrollWindow.h
 *
 *  Created on: Jul 3, 2017
 *      Author: guoxs
 */

#ifndef _WINDOW_ZKSCROLLWINDOW_H_
#define _WINDOW_ZKSCROLLWINDOW_H_

#include "ZKWindow.h"

class ZKScrollWindowPrivate;

class ZKScrollWindow : public ZKWindow {
	ZK_DECLARE_PRIVATE(ZKScrollWindow)

public:
	ZKScrollWindow(HWND hParentWnd);
	virtual ~ZKScrollWindow();

	class IScrollPageChangeListener {
	public:
		virtual ~IScrollPageChangeListener() { }
		virtual void onScrollPageChange(ZKScrollWindow *pScrollWindow, int page) = 0;
	};

	void setScrollPageChangeListener(IScrollPageChangeListener *pListener);

	int getCurrentPage() const;
	int getPageSize() const;

	void turnToPrevPage(bool isAnimatable = false);
	void turnToNextPage(bool isAnimatable = false);
	void turnToPage(int page, bool isAnimatable = false);

protected:
	ZKScrollWindow(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SCROLLWINDOW; }

	virtual BOOL onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

private:
	void parseScrollWindowAttributeFromJson(const Json::Value &json);

	bool needToRoll() const;
};

#endif /* _WINDOW_ZKSCROLLWINDOW_H_ */
