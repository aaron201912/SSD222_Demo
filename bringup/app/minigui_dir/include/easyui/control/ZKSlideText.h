/*
 * ZKSlideText.h
 *
 *  Created on: Dec 28, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKSLIDETEXT_H_
#define _CONTROL_ZKSLIDETEXT_H_

#include "ZKTextView.h"
#include "system/Mutex.h"
#include "utils/VelocityTracker.h"

class ZKSlideTextPrivate;

class ZKSlideText : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKSlideText)

public:
	ZKSlideText(HWND hParentWnd);
	virtual ~ZKSlideText();

	void setTextList(const std::vector<string> &textList);
	void clearTextList();

	const std::string& getText(int index) const;

	class ITextUnitClickListener {
	public:
		virtual ~ITextUnitClickListener() { }
		virtual void onTextUnitClick(ZKSlideText *pSlideText, const std::string &text) = 0;
	};

	void setTextUnitClickListener(ITextUnitClickListener *pListener) {
		mTextUnitClickListenerPtr = pListener;
	}

protected:
	ZKSlideText(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SLIDETEXT; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void drawBackgroundBlt(HDC hdc);
	void drawSlideText(HDC hdc);

private:
	void parseSlideTextAttributeFromJson(const Json::Value &json);
	void reset();
	int hitUnitIndex(int x, int y) const;
	int getEndEdgeOffset() const;

	void stopRoll();

protected:
	int mFirstUnitXOffset;
	int mFirstUnitXOffsetOfDown;	// 记录按下时首项的偏移位置

	VelocityTracker mVelocityTracker;
	float mCurVelocity;

	HDC mBackgroundDC;

	typedef struct {
		std::string text;
		LayoutPosition pos;
	} STextUnit;

	std::vector<STextUnit> mTextUnitList;
	int mHitIndex;

	mutable Mutex mLock;

	bool mIsRolling;
	bool mIsDamping;

	ITextUnitClickListener *mTextUnitClickListenerPtr;
};

#endif /* _CONTROL_ZKSLIDETEXT_H_ */
