/*
 * ZKPointer.h
 *
 *  Created on: Jul 17, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKPOINTER_H_
#define _CONTROL_ZKPOINTER_H_

#include "ZKBase.h"

class ZKPointerPrivate;

/**
 * @brief 指针控件
 */
class ZKPointer : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKPointer)

public:
	ZKPointer(HWND hParentWnd);
	virtual ~ZKPointer();

	/**
	 * @brief 设置旋转角度
	 */
	void setTargetAngle(float angle);

protected:
	ZKPointer(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_POINTER; }

	virtual BOOL onInterceptMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	virtual void onDraw(HDC hdc);
	virtual void onTimer(int id);

	void _section_(zk) drawPointer(HDC hdc);

private:
	void _section_(zk) parsePointerAttributeFromJson(const Json::Value &json);

	void getTransformRect(RECT &rt) const;
	void transform(const MPMatrix &matrix, float &x, float &y) const;

private:
	PBITMAP mPointerPicPtr;
	SIZE mPointerSize;

	float mLastAngle;
	float mTargetAngle;

	UINT mRotateSpeed;

	POINT mFixedPoint;
	POINT mRotationPoint;

	float mStartAngle;
	bool mIsClockwise;

	HGRAPHICS mGraphics;
};

#endif /* _CONTROL_ZKPOINTER_H_ */
