/*
 * UartContext.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <termio.h>
#include <sys/ioctl.h>

#include "uart/UartContext.h"
#include "utils/Log.h"


#define PRINT_UART_ON

#ifdef PRINT_UART_ON
#define PRINT_UART(fmt, args...)      ({do{printf(fmt, ##args);}while(0);})
#else
#define PRINT_UART(fmt, args...)
#endif

#define UART_DATA_BUF_LEN			16384	// 16KB

/* SynchFrame SlaveAddr MasterAddr DataLen Data CheckSum */
/*     2Byte     2Byte      2Byte   2Byte  N Byte  1Byte */
// 最小长度: 2 + 2 + 2 + 2 + 1 = 9
#define DATA_PACKAGE_MIN_LEN		9

extern void procParse(const BYTE *pData, UINT len);

/**
 * 获取校验码
 */
BYTE getCheckSum(const BYTE *pData, int len) {
	int sum = 0;
	for (int i = 0; i < len; ++i) {
		sum += pData[i];
	}

	return (BYTE) (~sum + 1);
}

UartContext::UartContext() :
	mIsOpen(false),
	mUartID(0),
	mDataBufPtr(NULL),
	mDataBufLen(0),
	mHook(NULL){

}

UartContext::~UartContext() {
	delete[] mDataBufPtr;
	closeUart();
}

bool UartContext::openUart(const char *pFileName, UINT baudRate) {
	LOGD("openUart pFileName = %s, baudRate = %d\n", pFileName, baudRate);
	mUartID = open(pFileName, O_RDWR|O_NOCTTY);

	if (mUartID <= 0) {
		mIsOpen = false;
	} else {
		struct termios oldtio = { 0 };
		struct termios newtio = { 0 };

		PRINT_UART("openUart step 1\n");
		tcgetattr(mUartID, &oldtio);

		//newtio.c_cflag = baudRate|CS8|CLOCAL|CREAD;
		newtio.c_cflag = CS8|CLOCAL|CREAD;
		newtio.c_iflag = 0;	// IGNPAR | ICRNL
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;	// ICANON
		//newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
		//newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */

		newtio.c_cc[VTIME] = 1;
		newtio.c_cc[VMIN] = 5;
		cfsetospeed(&newtio,B115200);
		cfsetispeed(&newtio,B115200);

		PRINT_UART("openUart step 2\n");
		tcflush(mUartID, TCIOFLUSH);

		PRINT_UART("openUart step 3\n");
		tcsetattr(mUartID, TCSANOW, &newtio);

		PRINT_UART("openUart step 4\n");
		// 设置为非阻塞
		fcntl(mUartID, F_SETFL, O_NONBLOCK);

		PRINT_UART("openUart step 5\n");
		mIsOpen = run("uart");
		PRINT_UART("openUart step 6\n");
		if (!mIsOpen) {
			close(mUartID);
			mUartID = 0;
		}

		LOGD("openUart mIsOpen = %d\n", mIsOpen);
	}

	return mIsOpen;
}

void UartContext::closeUart() {
	LOGD("closeUart mIsOpen: %d...\n", mIsOpen);
	if (mIsOpen) {
		requestExit();

		close(mUartID);
		mUartID = 0;
		mIsOpen = false;
	}
}

bool UartContext::send(const BYTE *pData, UINT len) {
	if (!mIsOpen) {
		return false;
	}

	if (write(mUartID, pData, len) != len) {	// fail
		LOGD("send Fail\n");
		return false;
	}

	// success
//	LOGD("send Success\n");

	return true;
}

UartContext* UartContext::getInstance() {
	static UartContext sUC;
	return &sUC;
}

bool UartContext::readyToRun() {
	if (mDataBufPtr == NULL) {
		mDataBufPtr = new BYTE[UART_DATA_BUF_LEN];
	}

	if (mDataBufPtr == NULL) {
		closeUart();
	}

	return (mDataBufPtr != NULL);
}

bool UartContext::threadLoop() {
	if (mIsOpen) {
		// 可能上一次解析后有残留数据，需要拼接起来
		PRINT_UART("thread step 1\n");
		int readNum = read(mUartID, mDataBufPtr + mDataBufLen, UART_DATA_BUF_LEN - mDataBufLen);

		PRINT_UART("thread step 2, readNum=%d\n", readNum);

		if (readNum > 0) {
			PRINT_UART("thread step 3, mHook is %s\n", mHook? "not null":"null");
		    if (mHook) {
		        mHook(mDataBufPtr + mDataBufLen, readNum);
		    }
		    PRINT_UART("thread step 4\n");

			const BYTE *pData = mDataBufPtr;
			mDataBufLen += readNum;

			int dataLen;	// 数据包长度
			int frameLen;	// 帧长度
			// FF FF cmd data data AA AA
			PRINT_UART("thread step 5\n");

			while (mDataBufLen >= 7) {
				while ((mDataBufLen >= 2) && ((pData[0] != 0xFF) || (pData[1] != 0xFF))) {
					pData++;
					mDataBufLen--;
					continue;
				}

				if (mDataBufLen < 7) {
					break;
				}
				frameLen = 7;

				for (int i = 0; i < frameLen; ++i) {
					LOGD("%x ", pData[i]);
				}

				if ((pData[5] == 0xAA) && (pData[6] == 0xAA)) {
					procParse(pData, frameLen);
				} else {
					LOGE("CheckSum error!!!!!!\n");
				}

				pData += frameLen;
				mDataBufLen -= frameLen;
			}

			PRINT_UART("thread step 6\n");
			// 数据未解析完
			if ((mDataBufLen > 0) && (mDataBufPtr != pData)) {
				LOGD("mDataBufLen: %d\n", mDataBufLen);

				// 将数据移到头部，方便下一包数据拼接
				memcpy(mDataBufPtr, pData, mDataBufLen);
			}
		} else {
			Thread::sleep(50);
		}

		return true;
	}

	return false;
}

void UartContext::addReadHook(void (*hook)(const unsigned char*, int)) {
    mHook = hook;
}

void UartContext::removeReadHook() {
    mHook = NULL;
}
