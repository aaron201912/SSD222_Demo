/*
 * ZKSlideWindow.h
 *
 *  Created on: Jul 3, 2017
 *      Author: guoxs
 */

#ifndef _WINDOW_ZKSLIDEWINDOW_H_
#define _WINDOW_ZKSLIDEWINDOW_H_

#include "ZKWindow.h"
#include "utils/VelocityTracker.h"

class ZKSlideWindowPrivate;

/**
 * @brief 滑动窗口控件
 */
class ZKSlideWindow : public ZKWindow {
	ZK_DECLARE_PRIVATE(ZKSlideWindow)

public:
	ZKSlideWindow(HWND hParentWnd);
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

	void setSlideItemClickListener(ISlideItemClickListener *pListener) {
		mSlideItemClickListenerPtr = pListener;
	}

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

private:
	typedef struct {
		int curStatus;
		PBITMAP itemStatusPicTab[S_CONTROL_STATUS_TAB_LEN];
		int itemStatusTextColorTab[S_CONTROL_STATUS_TAB_LEN];
		string text;
	} SSlideItemInfo;

	typedef struct {
		SSlideItemInfo *itemList;
		int size;
	} SSlidePageInfo;

	typedef struct {
		SSlidePageInfo *pageList;
		int size;
	} SSlidePageInfosList;

protected:
	ZKSlideWindow(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SLIDEWINDOW; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void drawBackgroundBlt(HDC hdc);
	void drawItemsBlt(HDC hdc);
	void drawItems(HDC hdc);
	void drawItem(HDC hdc, const SSlideItemInfo &item, int xOffset, int yOffset);
	void drawIcon(HDC hdc, const SSlideItemInfo &item, int xOffset, int yOffset);
	void drawText(HDC hdc, const SSlideItemInfo &item, int xOffset, int yOffset);

private:
	BOOL isSliding() const { return mIsSliding; }
	BOOL needToRoll() const { return (mFirstPageXOffset % mPosition.mWidth != 0); }

	int hitItemIndex(int x, int y) const;

	const PBITMAP getItemCurStatusPic(const SSlideItemInfo &item) const;
	int getItemCurTextColor(const SSlideItemInfo &item) const;

	void _section_(zk) parseSlideWindowAttributeFromJson(const Json::Value &json);

private:
	UINT mRows;
	UINT mCols;

	SIZE mItemSize;
	SIZE mIconSize;

	PLOGFONT mLogFontPtr;
	int mFontSize;

	LayoutPadding mPadding;

	int mCurrentPage;
	int mFirstPageXOffset;
	int mFirstPageXOffsetOfDown;

	int mHitIndex;

	BOOL mIsSliding;
	BOOL mIsDamping;

	UINT mRollSpeed;
	int mCurRollSpeed;

	VelocityTracker mVelocityTracker;

	SSlidePageInfosList mSlidePageInfosList;

	ISlideItemClickListener *mSlideItemClickListenerPtr;

	HDC mBackgroundDC;
	HDC mItemsDC;

	bool mIsUsedItemsDC;
};

#endif /* _WINDOW_ZKSLIDEWINDOW_H_ */
