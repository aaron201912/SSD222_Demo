/*
 * ZKCheckBox_p.h
 *
 *  Created on: Mar 7, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKCHECKBOX_PRIVATE_H_
#define _CONTROL_ZKCHECKBOX_PRIVATE_H_

#include "ZKButton_p.h"

class ZKCheckBox;

class ZKCheckBoxPrivate : public ZKButtonPrivate {
	ZK_DECLARE_PUBLIC(ZKCheckBox)

public:
	virtual ~ZKCheckBoxPrivate() { }
};

#endif /* _CONTROL_ZKCHECKBOX_PRIVATE_H_ */
