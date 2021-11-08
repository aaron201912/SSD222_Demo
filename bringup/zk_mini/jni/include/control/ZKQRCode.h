/*
 * ZKQRCode.h
 *
 *  Created on: Oct 9, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKQRCODE_H_
#define _CONTROL_ZKQRCODE_H_

#include "ZKBase.h"

class ZKQRCodePrivate;

/**
 * @brief 二维码控件
 */
class ZKQRCode : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKQRCode)

public:
	ZKQRCode(ZKBase *pParent);
	virtual ~ZKQRCode();

	/**
	 * @brief 设置前景颜色
	 * @param color 颜色值为0x ARGB
	 */
	void setForegroundColor(uint32_t color);

	/**
	 * @brief 加载二维码数据
	 */
	bool loadQRCode(const char *pStr);

protected:
	ZKQRCode(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_QRCODE; }
	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseQRCodeAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKQRCODE_H_ */
