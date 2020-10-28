/*
 * ZKCameraView.h
 *
 *  Created on: Mar 10, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKCAMERAVIEW_H_
#define _CONTROL_ZKCAMERAVIEW_H_

#include "ZKBase.h"
#include "camera/ZKCamera.h"

class ZKCameraViewPrivate;

/**
 * @brief 摄像头控件
 */
class ZKCameraView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCameraView)

public:
	ZKCameraView(HWND hParentWnd);
	virtual ~ZKCameraView();

	/**
	 * @brief 开始预览
	 */
	ECameraStatusCode startPreview();

	/**
	 * @brief 停止预览
	 */
	ECameraStatusCode stopPreview();

	/**
	 * @brief 是否预览中
	 */
	bool isPreviewing() const;

	/**
	 * @brief 拍照
	 */
	ECameraStatusCode takePicture();

	void setPosition(const LayoutPosition &position);
	void setRotation(ERotation rotation);
	void setMirror(EMirror mirror);

	/* 设置亮度 */
	bool setBrightness(int brightness);
	/* 设置对比度 */
	bool setContrast(int contrast);
	/* 设置饱和度 */
	bool setSaturation(int saturation);
	/* 设置色调 */
	bool setHue(int hue);
	/* 设置伽马值 */
	bool setGamma(int gamma);

	void setPictureCallback(ZKCamera::IPictureCallback *pCallback);
	void setErrorCodeCallback(ZKCamera::IErrorCodeCallback *pCallback);

protected:
	ZKCameraView(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CAMERAVIEW; }

	virtual void onDraw(HDC hdc);

private:
	void parseCameraViewAttributeFromJson(const Json::Value &json);

private:
	ZKCamera *mCameraPtr;
};

#endif /* _CONTROL_ZKCAMERAVIEW_H_ */
