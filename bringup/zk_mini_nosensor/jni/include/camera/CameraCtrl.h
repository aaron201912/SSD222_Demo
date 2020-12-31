/*
 * CameraCtrl.h
 *
 *  Created on: Feb 12, 2018
 *      Author: guoxs
 */

#ifndef _CAMERA_CAMERACTRL_H_
#define _CAMERA_CAMERACTRL_H_

#include <string>
#include "CameraCommDef.h"

class CameraCtrl {
public:
	CameraCtrl();
	virtual ~CameraCtrl();

	bool prepareCapture(int w, int h, int format, int framerate);

	typedef struct {
		void *start;
		size_t length;
	} SBufferInfo;

	const SBufferInfo* readCaptureBuffer();
	bool recycleCaptureBuffer();

	bool isCaptureDevExist();

	void setCvbsSignal(bool isCvbs);
	bool isCvbsSignal() const { return mIsCvbsSignal; }

	bool openCaptureDev();
	void closeCaptureDev();
	bool isCaptureDevOpened() const { return mIsCaptureDevOpened; }

	bool getCaptureDevFormatSize(int &width, int &height);

	bool performV4L2Ctrl(int id, int val);

private:
	bool queryCaptureDevCapabilities();
	bool setCaptureDevCropcap();
	bool setCaptureDevFomat(int w, int h, int format);
	void getCaptureDevSupportFormat();
	void getCaptureDevCurFormat();
	bool setCaptureDevFramerate(int framerate);

	bool mmapCaptureDevBuffers();
	bool munmapCaptureDevBuffers();
	bool queueCaptureDevBuffers();
	bool dequeueCaptureDevBuffers();

	bool enableCaptureDev(bool isEnable);

	bool updateControls();

private:
	std::string mDevPath;
	bool mIsCvbsSignal;

	int mCaptureDev;
	bool mIsCaptureDevOpened;

	SBufferInfo *mBufInfos;
	int mLenOfBufInfos;
	int mBufIndex;

	struct {
		bool isEnable;
		int value;
	} mV4L2CtrlFlagTab[V4L2_CID_LASTP1 - V4L2_CID_BASE];
};

#endif /* _CAMERA_CAMERACTRL_H_ */
