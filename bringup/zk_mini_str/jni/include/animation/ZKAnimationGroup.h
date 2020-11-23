/*
 * ZKAnimationGroup.h
 *
 *  Created on: Dec 1, 2017
 *      Author: guoxs
 */

#ifndef _ANIMATION_ZKANIMATIONGROUP_H_
#define _ANIMATION_ZKANIMATIONGROUP_H_

#include <vector>
#include "ZKAnimation.h"

class ZKAnimationGroup : public ZKAnimation {
public:
	ZKAnimationGroup();
	virtual ~ZKAnimationGroup();

	bool createGroup(bool isParallel = true);

	void addAnimation(ZKAnimation *pAnimation);

private:
	std::vector<ZKAnimation *> mAnimationList;
};

#endif /* _ANIMATION_ZKANIMATIONGROUP_H_ */
