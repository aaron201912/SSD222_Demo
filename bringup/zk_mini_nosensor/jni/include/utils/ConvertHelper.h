/*
 * ConvertHelper.h
 *
 *  Created on: Jun 14, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_CONVERT_HELPER_H_
#define _UTILS_CONVERT_HELPER_H_

#include <minigui/common.h>
#include <minigui/gdi.h>

class ConvertHelper {
public:
	static gal_pixel color2Pixel(int color) {
		Uint8 r = color >> 16;
		Uint8 g = color >> 8;
		Uint8 b = color;

		return RGB2Pixel(HDC_SCREEN, r, g, b);
	}
};

#endif /* _UTILS_CONVERT_HELPER_H_ */
