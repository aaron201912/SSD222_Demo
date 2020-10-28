/*
 * ZKVideoView.h
 *
 *  Created on: Nov 13, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKVIDEOVIEW_H_
#define _CONTROL_ZKVIDEOVIEW_H_

#include "ZKBase.h"
#include "media/ZKMediaPlayer.h"

class ZKVideoViewPrivate;

/**
 * @brief 视频控件
 */
class ZKVideoView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKVideoView)

public:
	ZKVideoView(HWND hParentWnd);
	virtual ~ZKVideoView();

	/**
	 * @brief 播放指定路径视频文件
	 * @param pFilePath 视频文件路径
	 * @param msec 指定从哪个位置开始播放，单位为毫秒，默认从头开始播放
	 */
	void play(const char *pFilePath, int msec = 0);

	/**
	 * @brief 暂停播放
	 */
	void pause();

	/**
	 * @brief 恢复播放
	 */
	void resume();

	/**
	 * @brief 定位到msec位置播放
	 * @param msec 单位为毫秒
	 */
	void seekTo(int msec);

	/**
	 * @brief 停止播放
	 */
	void stop();

	/**
	 * @brief 是否播放中
	 */
	bool isPlaying();

	/**
	 * @brief 设置音量
	 * @param volume 范围：0.0 ～ 1.0
	 */
	void setVolume(float volume);

	void setPosition(const LayoutPosition &position);

	/* clockwise rotation: val=0 no rotation, val=1 90 degree; val=2 180 degree, val=3 270 degree */
	void setRotation(int val);

	/**
	 * @brief 获取视频总时长，单位为毫秒
	 */
	int getDuration();

	/**
	 * @brief 获取当前播放位置，单位为毫秒
	 */
	int getCurrentPosition();

public:
	typedef enum {
		E_MSGTYPE_VIDEO_PLAY_STARTED,
		E_MSGTYPE_VIDEO_PLAY_COMPLETED,
		E_MSGTYPE_VIDEO_PLAY_ERROR
	} EMessageType;

	/**
	 * @brief 视频播放状态监听接口
	 */
	class IVideoPlayerMessageListener {
	public:
		virtual ~IVideoPlayerMessageListener() { }
		virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) = 0;
	};

	void setVideoPlayerMessageListener(IVideoPlayerMessageListener *pListener) {
		mVideoPlayerMessageListenerPtr = pListener;
	}

protected:
	ZKVideoView(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_VIDEOVIEW; }

	virtual void onDraw(HDC hdc);

private:
	void parseVideoViewAttributeFromJson(const Json::Value &json);

	class PlayerMessageListener : public ZKMediaPlayer::IPlayerMessageListener {
	public:
		PlayerMessageListener(ZKVideoView *pVideoView) : mVideoViewPtr(pVideoView) { }
		virtual void onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg, void *pMsgData);

	private:
		ZKVideoView * const mVideoViewPtr;
	};

private:
	ZKMediaPlayer *mMediaPlayerPtr;
	IVideoPlayerMessageListener *mVideoPlayerMessageListenerPtr;

	PlayerMessageListener mPlayerMessageListener;
};

#endif /* _CONTROL_ZKVIDEOVIEW_H_ */
