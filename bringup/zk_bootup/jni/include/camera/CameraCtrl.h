/*
 * CameraCtrl.h
 *
 *  Created on: Feb 12, 2018
 *      Author: guoxs
 */

#ifndef _CAMERA_CAMERACTRL_H_
#define _CAMERA_CAMERACTRL_H_

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

	bool openCaptureDev();
	void closeCaptureDev();
	bool isCaptureDevOpened() const { return mIsCaptureDevOpened; }

	bool getCaptureDevFormatSize(int &width, int &height);

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

private:
	int mCaptureDev;
	bool mIsCaptureDevOpened;

	SBufferInfo *mBufInfos;
	int mLenOfBufInfos;
	int mBufIndex;
};

#endif /* _CAMERA_CAMERACTRL_H_ */
