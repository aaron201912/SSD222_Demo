/*
 * ZKCamera.h
 *
 *  Created on: Feb 14, 2018
 *      Author: guoxs
 */

#ifndef _CAMERA_ZKCAMERA_H_
#define _CAMERA_ZKCAMERA_H_

#include "CameraCommDef.h"
#include "system/Thread.h"
#include "control/Common.h"

typedef enum {
	E_CAMERA_STATUS_CODE_OK = 0,
	E_CAMERA_STATUS_CODE_DEV_NOT_EXIST,
	E_CAMERA_STATUS_CODE_DEV_OPEN_FAILED,
	E_CAMERA_STATUS_CODE_TAKE_PIC_FAILED,
	E_CAMERA_STATUS_CODE_UNKNOWN_ERRORNO
} ECameraStatusCode;

typedef enum {
	E_MIRROR_NORMAL = 0,	// 正常镜像
	E_MIRROR_UPPER_LOWER,	// 上下镜像
	E_MIRROR_LEFT_RIGHT		// 左右镜像
} EMirror;

typedef enum {
	E_ROTATION_0 = 0,
	E_ROTATION_90,
	E_ROTATION_180,
	E_ROTATION_270
} ERotation;

class CameraCtrl;

class ZKCamera {
	typedef unsigned char	BYTE;
public:
	ZKCamera();
	virtual ~ZKCamera();

	ECameraStatusCode startPreview();
	ECameraStatusCode stopPreview();
	bool isPreviewing() const;

	void setFormatSize(int width, int height);

	ECameraStatusCode takePicture();

	void setPreviewPos(const LayoutPosition &pos);
	void setRotation(ERotation rotation);
	void setMirror(EMirror mirror);

	class IPictureCallback {
	public:
		virtual ~IPictureCallback() { }
		virtual void onPictureTakenStarted() { }
		virtual void onPictureTakenEnd() { }
		virtual void onPictureTakenError() { }
		virtual const char* onPictureSavePath() = 0;
	};

	void setPictureCallback(IPictureCallback *pCallback) {
		mPictureCallbackPtr = pCallback;
	}

	class IErrorCodeCallback {
	public:
		virtual ~IErrorCodeCallback() { }
		virtual void onErrorCode(int error) = 0;
	};

	void setErrorCodeCallback(IErrorCodeCallback *pCallback) {
		mErrorCodeCallbackPtr = pCallback;
	}

private:
	void procCameraReq();
	void previewLoop();
	void procWaitEvent();
	void retryPreview();
	void checkErrorNo();

	void notifyPictureTaken();

	void initDisp();
	void deinitDisp();

	bool convertYUYVToRGB24(const BYTE *pYUYV, BYTE *pRGB24, int width, int height);
	bool convertYUYVIToYUYVP(const BYTE *pYUYVI, BYTE *pYUYVP, int width, int height);

	typedef enum {
		E_CAMERA_REQ_START_PREVIEW,
		E_CAMERA_REQ_STOP_PREVIEW,
		E_CAMERA_REQ_TAKE_PICTURE,
		E_CAMERA_REQ_NULL
	} ECameraReq;

	class PreviewThread : public Thread {
	public:
		PreviewThread(ZKCamera &cam) : mCam(cam), mWakeWritePipeFd(-1) { }
		void start();
		void stop();

		void wakeup();
		bool waitEvent();

	protected:
		virtual bool threadLoop();

	private:
		ZKCamera &mCam;
		int mWakeWritePipeFd;
	};

	class TakePictureThread : public Thread {
	public:
		TakePictureThread(ZKCamera &cam) : mCam(cam) { }
		void start();
		void stop();

	protected:
		virtual bool threadLoop();

	private:
		ZKCamera &mCam;
	};

private:
	CameraCtrl *mCameraCtrlPtr;
	bool mIsPreviewing;
	bool mHadPreviewReq;

	int mWidth;
	int mHeight;

	EMirror mMirror;
	ERotation mRotation;

	PreviewThread mPreviewThread;
	TakePictureThread mTakePictureThread;

	mutable Mutex mPreviewLock;
	mutable Mutex mPictureLock;

	ECameraReq mCameraReq;
	bool mHasTakePictureReq;

	IPictureCallback *mPictureCallbackPtr;
	IErrorCodeCallback *mErrorCodeCallbackPtr;

	BYTE *mPictureDataPtr;

	int mDisplayLayer;
	LayoutPosition mPreviewPos;

	BYTE *mPreviewDataYUVPtr;
	int mIndexOfData;

	int mRetryCount;
};

#endif /* _CAMERA_ZKCAMERA_H_ */
