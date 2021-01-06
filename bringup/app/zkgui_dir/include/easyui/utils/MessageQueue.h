#ifndef _UTILS_MESSAGEQUEUE_H_
#define _UTILS_MESSAGEQUEUE_H_

#include <vector>

#include "system/Mutex.h"
#include "system/Condition.h"

class Message {
public:
	int mWhoseMsg;  // enum WhoseMessage
	int mMsgType;
	int mPara0;
	int mPara1;

public:
	Message();
	~Message();

	void setData(const char *pData, int nDataSize);
	void* getData(int *pDataSize = NULL);
	void reset();
	Message& operator=(const Message &rhs);

private:
	void *mDataPtr;
	int mDataSize;
};

class MessageQueue {
public:
	MessageQueue();
	~MessageQueue();

	void queueMessage(Message *pMsgIn);
	bool dequeueMessage(Message *pMsgOut);
	void flushMessage();
	int waitMessage(unsigned int timeout = 0);	// return value is the message number.
	int getMessageCount();

private:
	Mutex mLock;
	Condition mCondMessageQueueChanged;
	bool mWaitMessageFlag;

	std::vector<Message *> mValidMessageList;
	std::vector<Message *> mIdleMessageList;
};

#endif  /* _UTILS_MESSAGEQUEUE_H_ */
