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
	ZKQRCode(HWND hParentWnd);
	virtual ~ZKQRCode();

	/**
	 * @brief 加载二维码数据
	 */
	bool loadQRCode(const char *pStr);

protected:
	ZKQRCode(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_QRCODE; }
	virtual void onDraw(HDC hdc);

	void drawQRCode(HDC hdc);

private:
	void parseQRCodeAttributeFromJson(const Json::Value &json);
	void deleteQRCodeDC();

private:
	HDC mQRCodeDC;
};

#endif /* _CONTROL_ZKQRCODE_H_ */
