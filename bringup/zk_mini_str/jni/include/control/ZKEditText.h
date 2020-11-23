/*
 * ZKEditText.h
 *
 *  Created on: Oct 27, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKEDITTEXT_H_
#define _CONTROL_ZKEDITTEXT_H_

#include "ZKTextView.h"
#include "ime/IMEContext.h"

class ZKEditTextPrivate;

/**
 * @brief 编辑文本控件
 */
class ZKEditText : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKEditText)

public:
	ZKEditText(HWND hParentWnd);
	virtual ~ZKEditText();

	/**
	 * @brief 设置为密码类型
	 */
	void setPassword(bool isPassword);

	/**
	 * @brief 是否是密码类型
	 */
	bool isPassword() const { return mIMETextInfo.isPassword; }

protected:
	ZKEditText(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_EDITTEXT; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);

	void drawEditText(HDC hdc);

private:
	void parseEditTextAttributeFromJson(const Json::Value &json);

	class IMETextUpdateListener : public IMEContext::IIMETextUpdateListener {
	public:
		IMETextUpdateListener(ZKEditText *pEditText) : mEditTextPtr(pEditText) { }
		virtual void onIMETextUpdate(const std::string &text) {
			mEditTextPtr->setText(text);
		}

	private:
		ZKEditText *mEditTextPtr;
	};

private:
	IMETextUpdateListener mIMETextUpdateListener;
	IMEContext::SIMETextInfo mIMETextInfo;

	string mHintText;
	int mHintTextColor;
};

#endif /* _CONTROL_ZKEDITTEXT_H_ */
