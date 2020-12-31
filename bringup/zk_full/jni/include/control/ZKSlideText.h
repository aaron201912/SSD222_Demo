/*
 * ZKSlideText.h
 *
 *  Created on: Dec 28, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKSLIDETEXT_H_
#define _CONTROL_ZKSLIDETEXT_H_

#include "ZKTextView.h"

class ZKSlideTextPrivate;

class ZKSlideText : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKSlideText)

public:
	ZKSlideText(ZKBase *pParent);
	virtual ~ZKSlideText();

	void setTextList(const std::vector<string> &textList);
	void clearTextList();

	const std::string& getText(int index) const;

	class ITextUnitClickListener {
	public:
		virtual ~ITextUnitClickListener() { }
		virtual void onTextUnitClick(ZKSlideText *pSlideText, const std::string &text) = 0;
	};

	void setTextUnitClickListener(ITextUnitClickListener *pListener);

protected:
	ZKSlideText(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual const char* getClassName() const { return ZK_SLIDETEXT; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);
};

#endif /* _CONTROL_ZKSLIDETEXT_H_ */
