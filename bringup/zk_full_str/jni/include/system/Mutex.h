/*
 * Mutex.h
 *
 *  Created on: Aug 30, 2017
 *      Author: guoxs
 */

#ifndef _SYSTEM_MUTEX_H_
#define _SYSTEM_MUTEX_H_

#include <pthread.h>

/**
 * @brief 锁
 */
class Mutex {
public:
	enum {
		E_TYPE_PRIVATE = 0,
		E_TYPE_SHARED = 1
	};

	Mutex();
	Mutex(int type);
	~Mutex();

	/**
	 * @brief 加锁
	 */
	void lock();

	/**
	 * @brief 解锁
	 */
	void unlock();

	void tryLock();

	/**
	 * @brief 自动锁
	 */
	class Autolock {
	public:
		/**
		 * @brief 构造时加锁
		 */
		inline Autolock(Mutex &mutex) : mLock(mutex) {
			mLock.lock();
		}

		/**
		 * @brief 构造时加锁
		 */
		inline Autolock(Mutex *mutex) : mLock(*mutex) {
			mLock.lock();
		}

		/**
		 * @brief 析构时解锁
		 */
		inline ~Autolock() {
			mLock.unlock();
		}

	private:
		Mutex &mLock;
	};

private:
	friend class Condition;

	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

	pthread_mutex_t mMutex;
};

#endif /* _SYSTEM_MUTEX_H_ */
