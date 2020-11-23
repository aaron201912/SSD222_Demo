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

/**
 * @brief 线程类
 */
class Thread {
public:
	Thread();
	virtual ~Thread();

	/**
	 * @brief 启动线程
	 * @param name 线程名称；默认为NULL，由系统自动分配
	 */
	bool run(const char *name = 0);

	/**
	 * @brief 请求退出线程
	 * @attention 调用完函数立即返回，并不代表线程也退出了
	 */
	void requestExit();

	/**
	 * @brief 请求并等待线程退出
	 * @attention 线程退出，函数才返回
	 */
	void requestExitAndWait();

	/**
	 * @brief 线程是否运行中
	 */
	bool isRunning() const;

	static void sleep(int msec);

protected:
	/**
	 * @brief 是否有退出线程请求
	 */
	bool exitPending() const;

	/**
	 * @brief 线程开始运行时回调该接口
	 */
	virtual bool readyToRun();

	/**
	 * @brief 线程循环调用该接口
	 * @return true 不退出线程，false 将退出线程
	 */
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
