/*
 * ZKTextView_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKTEXTVIEW_PRIVATE_H_
#define _CONTROL_ZKTEXTVIEW_PRIVATE_H_

#include "ZKBase_p.h"
#include "system/Thread.h"

class ZKTextView;

class ZKTextViewPrivate : public ZKBasePrivate {
	ZK_DECLARE_PUBLIC(ZKTextView)

public:
	virtual ~ZKTextViewPrivate() { }

protected:
	std::string mLangName;
	mutable Mutex mTextLock;
};

#endif /* _CONTROL_ZKTEXTVIEW_PRIVATE_H_ */
