/*
 * ZKButton_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKBUTTON_PRIVATE_H_
#define _CONTROL_ZKBUTTON_PRIVATE_H_

#include "ZKTextView_p.h"
#include "utils/BitmapWrapper.h"

class ZKButton;

class ZKButtonPrivate : public ZKTextViewPrivate {
	ZK_DECLARE_PUBLIC(ZKButton)

public:
	ZKButtonPrivate();
	virtual ~ZKButtonPrivate();

protected:
	void drawButton(HDC hdc);

protected:
	BitmapWrapper mButtonStatusPicTab[ZKBase::S_CONTROL_STATUS_TAB_LEN];
};

#endif /* _CONTROL_ZKBUTTON_PRIVATE_H_ */
