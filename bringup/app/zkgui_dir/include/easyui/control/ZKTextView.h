/*
 * ZKTextView.h
 *
 *  Created on: Jun 10, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKTEXTVIEW_H_
#define _CONTROL_ZKTEXTVIEW_H_

#include "ZKBase.h"

class ZKTextViewPrivate;

/**
 * @brief 文本控件
 */
class ZKTextView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKTextView)

public:
	ZKTextView(ZKBase *pParent);
	virtual ~ZKTextView();

	/**
	 * @brief 设置string文本
	 */
	void setText(const std::string &text);

	/**
	 * @brief 设置字符串文本
	 */
	void setText(const char *text);

	/**
	 * @brief 设置字符文本
	 */
	void setText(char text);

	/**
	 * @brief 设置数字文本
	 */
	void setText(int text);

	/**
	 * @brief 获取文本内容
	 */
	const std::string& getText() const;

	/**
	 * @brief 支持多国语言设置接口
	 */
	void setTextTr(const char *name);

	void reloadTextTr();

	/**
	 * @brief 设置文本颜色
	 * @param color 颜色值为0x ARGB
	 */
	void setTextColor(int color);

	/**
	 * @brief 设置文本状态颜色
	 * @param status 状态
	 *    正常状态： ZK_CONTROL_STATUS_NORMAL
	 *    按下状态： ZK_CONTROL_STATUS_PRESSED
	 *    选中状态： ZK_CONTROL_STATUS_SELECTED
	 *    选中按下状态： ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    无效状态： ZK_CONTROL_STATUS_INVALID
	 * @param color 颜色值为0x ARGB
	 */
	void setTextStatusColor(int status, uint32_t color);

	/**
	 * @brief 获取文本内容宽高
	 * @param text 文本内容
	 * @param w 返回文本宽度
	 * @param h 返回文本高度
	 */
	void getTextExtent(const char *text, int &w, int &h);

public:
	/**
	 * @brief 文本内容改变监听接口
	 */
	class ITextChangeListener {
	public:
		virtual ~ITextChangeListener() { }
		virtual void onTextChanged(ZKTextView *pTextView, const std::string &text) = 0;
	};

	void setTextChangeListener(ITextChangeListener *pListener);

protected:
	ZKTextView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_TEXTVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual void onTimer(int id);

private:
	void _section_(zk) parseTextViewAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKTEXTVIEW_H_ */
