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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgplus/mgplus.h>
#include <mgeff/mgeff.h>

#include "Common.h"
#include "json/json.h"

using namespace std;

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
	ZKBase(HWND hParentWnd);
	virtual ~ZKBase();

	BOOL create(const Json::Value &json);

	HWND getParentWnd() const { return mParentWnd; }
	HWND getWnd() const { return mWnd; }

	/**
	 * @brief 获取控件ID值
	 */
	int getID() const { return mID; }

	/**
	 * @brief 设置控件显示位置
	 */
	void setPosition(const LayoutPosition &position);

	/**
	 * @brief 获取控件位置
	 */
	const LayoutPosition& getPosition() const { return mPosition; }

	/**
	 * @brief 设置无效状态
	 */
	void setInvalid(BOOL isInvalid);

	/**
	 * @brief 是否是无效状态
	 */
	BOOL isInvalid() const;

	/**
	 * @brief 设置显示状态
	 */
	void setVisible(BOOL isVisible);

	/**
	 * @brief 是否是显示状态
	 */
	BOOL isVisible() const;

	/**
	 * @brief 设置选中状态
	 */
	void setSelected(BOOL isSelected);

	/**
	 * @brief 是否是选中状态
	 */
	BOOL isSelected() const;

	/**
	 * @brief 设置按下状态
	 */
	void setPressed(BOOL isPressed);

	/**
	 * @brief 是否是按下状态
	 */
	BOOL isPressed() const;

	/**
	 * @brief 设置触摸状态
	 */
	void setTouchable(BOOL isTouchable);

	/**
	 * @brief 是否是可触摸状态
	 */
	BOOL isTouchable() const;

	void setAnimatable(BOOL isAnimatable);
	BOOL isAnimatable() const;

	BOOL isWndValid() const { return mWnd != HWND_INVALID; }

	/**
	 * @brief 设置背景图
	 * @param pPicPath 图片路径
	 */
	void setBackgroundPic(const char *pPicPath);
	void setBackgroundBmp(PBITMAP pBmp);

	/**
	 * @brief 设置背景颜色
	 * @param color -1时，背景设置为透明；其他颜色值为0x RGB，颜色值不支持alpha
	 */
	void setBackgroundColor(int color);

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

	void setClickListener(IClickListener *pListener) { mClickListenerPtr = pListener; }
	void setLongClickListener(ILongClickListener *pListener) { mLongClickListenerPtr = pListener; }

protected:
	ZKBase(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual BOOL createWindow();
	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);

	virtual int procCtrlFun(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	static int ctrlFun(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

	virtual BOOL onInterceptMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void _section_(zk) invalidate(const RECT *pDirty = NULL, BOOL isEraseBG = TRUE);

	void _section_(zk) doDraw();

	void _section_(zk) startTimer(int id, int time);
	void _section_(zk) stopTimer(int id);
	void _section_(zk) resetTimer(int id, int time);
	BOOL _section_(zk) hasTimer(int id);

	void _section_(zk) sendMessage(int message, WPARAM wParam, LPARAM lParam);
	void _section_(zk) sendNotifyMessage(int message, WPARAM wParam, LPARAM lParam);

	void _section_(zk) drawBackground(HDC hdc);

private:
	void _section_(zk) parseBaseAttributeFromJson(const Json::Value &json);

protected:
	typedef enum {
		E_CONTROL_STATUS_NORMAL 	= 0x00,
		E_CONTROL_STATUS_PRESSED 	= 0x01,
		E_CONTROL_STATUS_SELECTED	= 0x02,
		E_CONTROL_STATUS_INVALID	= 0x04,
		E_CONTROL_STATUS_VISIBLE	= 0x08
	} EControlStatus;

	const static int S_CONTROL_STATUS_TAB_LEN = 5;

	typedef enum {
		E_CONTROL_FLAG_NONE 		= 0x00,
		E_CONTROL_FLAG_TOUCHABLE 	= 0x01,
		E_CONTROL_FLAG_SWITCH	 	= 0x02,
		E_CONTROL_FLAG_ANIMATABLE	= 0x04
	} EControlFlag;

	typedef enum {
		E_ORIENTATION_HORIZONTAL,
		E_ORIENTATION_VERTICAL
	} EOrientation;

protected:
	ZKBaseData *d_ptr;

	HWND mParentWnd;
	HWND mWnd;
	int mID;

	DWORD mDwExStyle;

	int mBackgroundColor;
	PBITMAP mBackgroundPicPtr;

	LayoutPosition mPosition;

	int mControlStatus;
	int mControlFlags;

	IClickListener *mClickListenerPtr;
	ILongClickListener *mLongClickListenerPtr;

	POINT mDownPoint;
	POINT mUpPoint;
	BOOL mHasDown;

	BOOL mIsFirstDraw;
};


#include <vector>

class ControlFactory {
public:
    static ControlFactory* getInstance();

    bool addControlInfo(const char *name, ZKBase* (*create)(HWND hParentWnd));

    void registerControl();
    void unregisterControl();

    ZKBase* create(const char *name, HWND hParentWnd);

private:
    ControlFactory() { }

private:
    typedef struct {
        std::string ctrlName;
        ZKBase* (*create)(HWND hParentWnd);
    } SControlInfo;

    std::vector<SControlInfo> mControlInfos;
};

#define CONTROLFACTORY		ControlFactory::getInstance()
#define REGISTER_CONTROL(name, _ctrl) \
    static struct _ControlFactory_##_ctrl { \
        static ZKBase* create(HWND hParentWnd) { \
            return new _ctrl(hParentWnd); \
        } \
        _ControlFactory_##_ctrl() { \
        	CONTROLFACTORY->addControlInfo(name, create); \
        } \
    } _autoRegister_Control_##_ctrl

#endif /* _CONTROL_ZKBASE_H_ */
