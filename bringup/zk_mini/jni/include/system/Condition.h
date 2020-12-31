/*
 * Condition.h
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */

#ifndef _SYSTEM_CONDITION_H_
#define _SYSTEM_CONDITION_H_

#include "Mutex.h"

class Condition {
public:
	enum {
		E_PRIVATE = 0,
		E_SHARED = 1
	};

	typedef enum {
		E_WAKE_UP_TYPE_ONE = 0,
		E_WAKE_UP_TYPE_ALL = 1
	} EWakeUpType;

	Condition();
	Condition(int type);
	virtual ~Condition();

	void wait(Mutex &mutex);
	void waitRelative(Mutex &mutex, long long reltime);

	void signal();
	void broadcast();

	void signal(EWakeUpType type) {
		if (type == E_WAKE_UP_TYPE_ONE) {
			signal();
		} else {
			broadcast();
		}
	}

private:
	pthread_cond_t mCond;
};

#endif /* _SYSTEM_CONDITION_H_ */
