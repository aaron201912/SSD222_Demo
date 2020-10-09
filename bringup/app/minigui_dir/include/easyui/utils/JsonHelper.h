/*
 * JsonHelper.h
 *
 *  Created on: Aug 19, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_JSON_HELPER_H_
#define _UTILS_JSON_HELPER_H_

#include "json/json.h"

using namespace std;

class JsonHelper {
public:
	static bool readJsonFile(const char *pJsonFile, Json::Value &json);
	static bool readJsonData(const char *pData, int len, Json::Value &json);
	static bool writeJsonFile(const char *pJsonFile, const Json::Value &json);
};

#endif /* _UTILS_JSON_HELPER_H_ */
