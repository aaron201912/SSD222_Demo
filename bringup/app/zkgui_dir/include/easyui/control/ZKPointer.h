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
	ZKPointer(ZKBase *pParent);
	virtual ~ZKPointer();

	/**
	 * @brief 设置旋转角度
	 */
	void setTargetAngle(float angle);

protected:
	ZKPointer(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_POINTER; }

	virtual bool onInterceptMessage(const struct _message_t *pMsg);
	virtual void onDraw(ZKCanvas *pCanvas);
	virtual void onTimer(int id);

private:
	void _section_(zk) parsePointerAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKPOINTER_H_ */
