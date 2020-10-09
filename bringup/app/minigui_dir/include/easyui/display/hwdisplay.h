/*
 **
 ** Copyright (C) 2008, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#ifndef _DISPLAY_HWDISPLAY_H
#define _DISPLAY_HWDISPLAY_H

#include "system/Mutex.h"
#include "display/hwdisp_def.h"
#include "display/sunxi_display2.h"


#define CHN_NUM			3
#define LYL_NUM			4
#define HLAY(chn, lyl)	(chn*4+lyl)
#define HD2CHN(hlay)	(hlay/4)
#define HD2LYL(hlay)	(hlay%4)

#define ZORDER_MAX		11
#define ZORDER_MIN		0

#define DISP_DEV		"/dev/disp"
#define FB_ANDROID_DEV	"/dev/graphics/fb%d"

#define MAX_LAYER	2
#define SCREEN_0	0
#define SCREEN_1	1
#define RET_OK		0
#define RET_FAIL	-1
#define CK_COLOR	0x000000
#define ID_RESERVED	3

#define HDL2ID(handle)	((handle) - 101)
#define ID2HDL(ID)		((ID) + 101)

#define VALID_LAYER		101


enum {
	HWD_STATUS_REQUESTED = 1,
	HWD_STATUS_NOTUSED = 2,
	HWD_STATUS_OPENED = 4
};


class HwDisplay {
public:
	static HwDisplay* getInstance();

public:
	int hwd_init(void);
	int hwd_exit(void);

	int hwd_layer_request(const struct view_info &pos);
	int hwd_layer_release(unsigned int hlay);
	int hwd_layer_render(unsigned int hlay, const libhwclayerpara_t &picture);

	int hwd_layer_open(unsigned int hlay);
	int hwd_layer_close(unsigned int hlay);

	int hwd_layer_set_src(unsigned int hlay, const struct src_info &src);
	int hwd_layer_set_rect(unsigned int hlay, const struct view_info &view);

	int hwd_layer_top(unsigned int hlay);
	int hwd_layer_bottom(unsigned int hlay);

	int hwd_layer_ck_on(unsigned int hlay);
	int hwd_layer_ck_off(unsigned int hlay);

	int hwd_layer_ck_value(unsigned int hlay, unsigned int color);

	int hwd_layer_exchange(unsigned int hlay1, unsigned int hlay2, int otherOnTop);
	int hwd_layer_switch(unsigned int hlay, int bOpen);
	int hwd_layer_other_screen(int screen, unsigned int hlay1, unsigned int hlay2);

	int hwd_layer_clear(unsigned int hlay);

protected:
	void hwd_set_rot(int screen, int rot);

	int layer_request(int *pCh, int *pId);
	int layer_config(__DISP_t cmd, disp_layer_config *pinfo);

	int layer_request(int screen);
	int layer_release(int hlay);

	int layer_cmd(unsigned int hlay);
	int layer_cmd(int screen, unsigned int hlay);
	int layer_get_para(disp_layer_config *pinfo);
	int layer_set_para(disp_layer_config *pinfo);

	int layer_set_normal(unsigned int hlay);
	void openHdmi(int screen, int val);

protected:
	struct buf_info {
		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
		unsigned int *vaddr;    // virtual address
		unsigned int *paddr[2]; // physical address
		unsigned int used[2];
	};

	int mDisp_fd;
	int mScaler_hdl;

	unsigned int mLayerStatus[CHN_NUM][LYL_NUM];

private:
	static bool mInitialized;
	static HwDisplay *sHwDisplay;
	static Mutex sLock;

	int mCurLayerCnt;
	int mScreen;

	HwDisplay& operator=(const HwDisplay&);
	HwDisplay(const HwDisplay&);
	HwDisplay();
	~HwDisplay();
};

#define HWDISPLAY		HwDisplay::getInstance()

#endif
