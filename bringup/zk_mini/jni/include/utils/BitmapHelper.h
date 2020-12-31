/*
 * BitmapHelper.h
 *
 *  Created on: Jun 11, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_BITMAP_HELPER_H_
#define _UTILS_BITMAP_HELPER_H_

#include <stdint.h>

typedef struct _bitmap_t {
	uint8_t type;
	uint8_t bits;
	uint8_t bytes;
	uint8_t alpha;
	uint32_t ck;
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	uint8_t *data;
	uint8_t *am;
	uint32_t ap;
	uint64_t phyAddr;
} bitmap_t;

class BitmapHelper {
public:
	static bool loadBitmapFromFile(struct _bitmap_t *&pBmp, const char *pPath);
	static void unloadBitmap(struct _bitmap_t *&pBmp);
};

#endif /* _UTILS_BITMAP_HELPER_H_ */
