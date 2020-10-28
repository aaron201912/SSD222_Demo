/*
 * EtcFileHelper.h
 *
 *  Created on: Jun 16, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_ETCFILE_HELPER_H_
#define _UTILS_ETCFILE_HELPER_H_

#include <minigui/common.h>

class EtcFileHelper {
public:
	EtcFileHelper();
	virtual ~EtcFileHelper();

	BOOL hasLoaded();
	BOOL loadEtcFile(const char *pEtcFile);
	void unloadEtcFile();

	BOOL getValueFromEtc(const char *pSection, const char *pKey, char *pVal, int len);
	BOOL getIntValueFromEtc(const char *pSection, const char *pKey, int *pVal);

	BOOL setValueToEtc(const char *pSection, const char *pKey, char *pVal);

	BOOL removeSectionInEtc(const char* pSection);
	BOOL saveEtcToFile(const char *pFileName);

private:
	GHANDLE mEtcFileHandle;
};

#endif /* _UTILS_ETCFILE_HELPER_H_ */
