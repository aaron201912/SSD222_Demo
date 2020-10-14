/*
 * CameraCommDef.h
 *
 *  Created on: Feb 14, 2018
 *      Author: guoxs
 */

#ifndef _CAMERA_CAMERACOMMDEF_H_
#define _CAMERA_CAMERACOMMDEF_H_

#include <sys/time.h>
#include <linux/videodev2.h>

#define VIDEO0_DEV_NODE				"/dev/video0"
#define DEF_FORMAT_WIDTH			640
#define DEF_FORMAT_HEIGHT			480
#define DEF_PIXEL_FORMAT			V4L2_PIX_FMT_YUYV
#define DEF_FRAMERATE				30
#define DEF_BUFFERS_COUNT			3

#endif /* _CAMERA_CAMERACOMMDEF_H_ */
