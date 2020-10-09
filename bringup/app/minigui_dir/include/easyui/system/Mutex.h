/*
 * Mutex.h
 *
 *  Created on: Aug 30, 2017
 *      Author: guoxs
 */

#ifndef _SYSTEM_MUTEX_H_
#define _SYSTEM_MUTEX_H_

#include <pthread.h>

class Mutex {
public:
	enum {
		E_TYPE_PRIVATE = 0,
		E_TYPE_SHARED = 1
	};

	Mutex();
	Mutex(int type);
	~Mutex();

	void lock();
	void unlock();

	void tryLock();

	class Autolock {
	public:
		inline Autolock(Mutex &mutex) : mLock(mutex) {
			mLock.lock();
		}

		inline Autolock(Mutex *mutex) : mLock(*mutex) {
			mLock.lock();
		}

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
