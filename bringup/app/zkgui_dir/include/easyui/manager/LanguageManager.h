/*
 * LanguageManager.h
 *
 *  Created on: Mar 6, 2018
 *      Author: guoxs
 */

#ifndef _MANAGER_LANGUAGE_MANAGER_H_
#define _MANAGER_LANGUAGE_MANAGER_H_

#include <vector>
#include <string>

#include "json/json.h"

typedef struct {
	char code[16];			// 国家码
	char language[64];		// 语言名称
} SLanguageInfo;

class LanguageManager {
public:
	static LanguageManager* getInstance();

	const std::vector<SLanguageInfo>& getLanguageInfos() const {
		return mLanguageInfos;
	}

	bool setCurrentCode(const char *code);

	const std::string& getCurrentCode() const {
		return mCurrentCode;
	}

	const std::string& getCurrentLanguage() const {
		return mCurrentLanguage;
	}

	std::string getValue(const char *name);

private:
	LanguageManager();
	~LanguageManager();

private:
	std::vector<SLanguageInfo> mLanguageInfos;

	std::string mCurrentCode;
	std::string mCurrentLanguage;

	Json::Value mContent;
	Json::Value mInternalContent;
};

#define LANGUAGEMANAGER			LanguageManager::getInstance()

#endif /* _MANAGER_LANGUAGE_MANAGER_H_ */
