#ifndef _UTILS_LOG_H_
#define _UTILS_LOG_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_ANDROID_LOG
#include <android/log.h>

#define LOG_TAG "zkgui"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define LOGE(fmt,...)	fprintf(stderr, fmt, ##__VA_ARGS__)
#define LOGD(fmt,...) 	fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#define ZKSWE_DEBUG_EASYGUI
#ifdef ZKSWE_DEBUG_EASYGUI
#include <sys/time.h>
#define TIME_ELAPSED(codeToTime) do { \
    struct timeval beginTime, endTime; \
    gettimeofday(&beginTime, NULL); \
    { codeToTime; } \
    gettimeofday(&endTime, NULL); \
    long secTime  = endTime.tv_sec - beginTime.tv_sec; \
    long usecTime = endTime.tv_usec - beginTime.tv_usec; \
	if (secTime) { \
		long secTime1 = (secTime*1000000 + endTime.tv_usec - beginTime.tv_usec)/1000000; \
		usecTime = (secTime*1000000 + endTime.tv_usec - beginTime.tv_usec)%1000000; \
		fprintf(stderr, "[%s(%d)]Elapsed Time: SecTime = %lds, UsecTime = %ldus!\n", __FUNCTION__, __LINE__, secTime1, usecTime); \
	} else \
		fprintf(stderr, "[%s(%d)]Elapsed Time: SecTime = %lds, UsecTime = %ldus!\n", __FUNCTION__, __LINE__, secTime, usecTime); \
} while (0)
#else
#define TIME_ELAPSED(codeToTime) do { \
	{ codeToTime; } \
} while (0)
#endif

#endif /* _UTILS_LOG_H_ */
