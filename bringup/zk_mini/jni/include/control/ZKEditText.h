/*
 * ZKEditText.h
 *
 *  Created on: Oct 27, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKEDITTEXT_H_
#define _CONTROL_ZKEDITTEXT_H_

#include "ZKTextView.h"

class ZKEditTextPrivate;

/**
 * @brief 编辑文本控件
 */
class ZKEditText : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKEditText)

public:
	ZKEditText(ZKBase *pParent);
	virtual ~ZKEditText();

	/**
	 * @brief 设置为密码类型
	 */
	void setPassword(bool isPassword);

	/**
	 * @brief 是否是密码类型
	 */
	bool isPassword() const;

protected:
	ZKEditText(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_EDITTEXT; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseEditTextAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKEDITTEXT_H_ */
