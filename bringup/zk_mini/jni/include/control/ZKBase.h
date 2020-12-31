/*
 * ZKBase.h
 *
 *  Created on: Jun 8, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKBASE_H_
#define _CONTROL_ZKBASE_H_

#include <string>
#include <string.h>

#include "Common.h"
#include "json/json.h"

using namespace std;

class ZKCanvas;
class ZKBase;
class ZKBasePrivate;

class ZKBaseData {
public:
	virtual ~ZKBaseData() = 0;
	ZKBase *q_ptr;
};


/**
 * @brief 控件基类
 */
class ZKBase {
	ZK_DECLARE_PRIVATE(ZKBase)
	friend class ControlFactory;
public:
	ZKBase(ZKBase *pParent);
	virtual ~ZKBase();

	bool create(const Json::Value &json);

	/**
	 * @brief 获取控件ID值
	 */
	int getID() const;

	/**
	 * @brief 设置控件显示位置
	 */
	void setPosition(const LayoutPosition &position);

	/**
	 * @brief 获取控件位置
	 */
	const LayoutPosition& getPosition() const;

	/**
	 * @brief 设置无效状态
	 */
	void setInvalid(bool isInvalid);

	/**
	 * @brief 是否是无效状态
	 */
	bool isInvalid() const;

	/**
	 * @brief 设置显示状态
	 */
	void setVisible(bool isVisible);

	/**
	 * @brief 是否是显示状态
	 */
	bool isVisible() const;

	/**
	 * @brief 设置选中状态
	 */
	void setSelected(bool isSelected);

	/**
	 * @brief 是否是选中状态
	 */
	bool isSelected() const;

	/**
	 * @brief 设置按下状态
	 */
	void setPressed(bool isPressed);

	/**
	 * @brief 是否是按下状态
	 */
	bool isPressed() const;

	/**
	 * @brief 设置触摸状态
	 */
	void setTouchable(bool isTouchable);

	/**
	 * @brief 是否是可触摸状态
	 */
	bool isTouchable() const;

	/**
	 * @brief 设置触摸穿透
	 */
	void setTouchPass(bool isPass);

	/**
	 * @brief 是否是触摸穿透状态
	 */
	bool isTouchPass() const;

	bool isWndValid() const;

	/**
	 * @brief 设置背景图
	 * @param pPicPath 图片路径
	 */
	void setBackgroundPic(const char *pPicPath);
	void setBackgroundBmp(struct _bitmap_t *pBmp);

	/**
	 * @brief 设置背景颜色
	 * @param color 颜色值为0x ARGB
	 */
	void setBackgroundColor(uint32_t color);

	/**
	 * @brief 设置背景状态颜色
	 * @param status 状态
	 *    正常状态： ZK_CONTROL_STATUS_NORMAL
	 *    按下状态： ZK_CONTROL_STATUS_PRESSED
	 *    选中状态： ZK_CONTROL_STATUS_SELECTED
	 *    选中按下状态： ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    无效状态： ZK_CONTROL_STATUS_INVALID
	 * @param color 颜色值为0x ARGB
	 */
	void setBgStatusColor(int status, uint32_t color);

	/**
	 * @brief 设置控件透明度
	 * @param alpha 透明度值，范围：0 ~ 255，0 表示全透， 255 表示不透明
	 */
	void setAlpha(uint8_t alpha);

	/**
	 * @brief 出蜂鸣器声音
	 */
	void beep();

	virtual const char* getClassName() const { return NULL; }

public:
	/**
	 * @brief 点击事件监听接口
	 */
	class IClickListener {
	public:
		virtual ~IClickListener() { }
		virtual void onClick(ZKBase *pBase) = 0;
	};

	/**
	 * @brief 长按事件监听接口
	 */
	class ILongClickListener {
	public:
		virtual ~ILongClickListener() { }
		virtual void onLongClick(ZKBase *pBase) = 0;
	};

	/**
	 * @brief 触摸监听接口
	 */
	class ITouchListener {
	public:
		virtual ~ITouchListener() { }
		virtual void onTouchEvent(ZKBase *pBase, const MotionEvent &ev) = 0;
	};

	void setClickListener(IClickListener *pListener);
	void setLongClickListener(ILongClickListener *pListener);
	void setTouchListener(ITouchListener *pListener);

protected:
	ZKBase(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual bool createWindow();
	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);

	virtual ret_t procCtrlFun(const struct _message_t *pMsg);
	static ret_t ctrlFun(const struct _message_t *pMsg);

	virtual bool onInterceptMessage(const struct _message_t *pMsg);
	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void startTimer(int id, uint32_t time);
	void stopTimer(int id);
	void resetTimer(int id, uint32_t time);
	bool hasTimer(int id);

private:
	void parseBaseAttributeFromJson(const Json::Value &json);

protected:
	ZKBaseData *d_ptr;
};


#include <vector>

class ControlFactory {
public:
	static ControlFactory* getInstance();

	bool registerControl(const char *name, ZKBase* (*create)(ZKBase *pParent));
	ZKBase* create(const char *name, ZKBase *pParent);

private:
	ControlFactory() { }

private:
	typedef struct {
		std::string ctrlName;
		ZKBase* (*create)(ZKBase *pParent);
	} SControlInfo;

	std::vector<SControlInfo> mControlInfos;
};

#define CONTROLFACTORY		ControlFactory::getInstance()
#define REGISTER_CONTROL(name, _ctrl) \
    static struct _ControlFactory_##_ctrl { \
        static ZKBase* create(ZKBase *pParent) { \
            return new _ctrl(pParent); \
        } \
        _ControlFactory_##_ctrl() { \
        	CONTROLFACTORY->registerControl(name, create); \
        } \
    } _autoRegister_Control_##_ctrl

#endif /* _CONTROL_ZKBASE_H_ */
