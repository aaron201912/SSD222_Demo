/*
 * ZKEditText_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKEDITTEXT_PRIVATE_H_
#define _CONTROL_ZKEDITTEXT_PRIVATE_H_

#include "ZKTextView_p.h"

class ZKEditText;

class ZKEditTextPrivate : public ZKTextViewPrivate {
	ZK_DECLARE_PUBLIC(ZKEditText)

public:
	virtual ~ZKEditTextPrivate() { }
};

#endif /* _CONTROL_ZKEDITTEXT_PRIVATE_H_ */
