/*
 * BitmapHelper.h
 *
 *  Created on: Jun 11, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_BITMAP_HELPER_H_
#define _UTILS_BITMAP_HELPER_H_

#include <minigui/common.h>
#include <minigui/gdi.h>
#include <vector>
#include <string>
#include "system/Mutex.h"

class BitmapHelper {
public:
	static BOOL loadBitmapFromFile(BITMAP *&pBmp, const char *pPath);
	static BOOL loadBitmapFromMem(BITMAP *&pBmp, const void *pData, int len, const char *pExt);
	static void unloadBitmap(BITMAP *&pBmp);

private:
	typedef struct {
		std::string path;
		int refCount;
		BITMAP *pBmp;
	} SBitmapInfo;

	static std::vector<SBitmapInfo> sBitmapInfoList;
	static Mutex sLock;
};

#endif /* _UTILS_BITMAP_HELPER_H_ */
