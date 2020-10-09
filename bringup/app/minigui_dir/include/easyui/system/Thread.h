/*
 * Thread.h
 *
 *  Created on: Aug 24, 2017
 *      Author: guoxs
 */

#ifndef _SYSTEM_THREAD_H_
#define _SYSTEM_THREAD_H_

#include "Mutex.h"
#include "Condition.h"

class Thread {
public:
	Thread();
	virtual ~Thread();

	bool run(const char *name = 0);

	void requestExit();
	void requestExitAndWait();

	bool isRunning() const;

	static void sleep(int msec);

protected:
	bool exitPending() const;

	virtual bool readyToRun();
	virtual bool threadLoop() = 0;

private:
	Thread& operator=(const Thread&);
	static void* _threadLoop(void *user);

private:
	typedef struct {
		void *userData;
		char *threadName;
	} SThreadData;

	bool mExitPending;
	bool mIsRunning;

	Condition mThreadExitedCondition;
	mutable Mutex mLock;
};

#endif /* _SYSTEM_THREAD_H_ */
