/*
 * JsonParser.h
 *
 *  Created on: Aug 19, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_JSON_PARSER_H_
#define _UTILS_JSON_PARSER_H_

#include <fstream>
#include <errno.h>

#include "json/json.h"
#include "utils/Log.h"

using namespace std;

class JsonParser {
public:
	static bool parseJsonFile(const char *pJsonFile, Json::Value &json) {
		LOGD("parseJsonFile pJsonFile: %s\n", pJsonFile);
		fstream ifs;
		ifs.open(pJsonFile, ios_base::in);
		if (!ifs.is_open()) {
			LOGE("parseJsonFile fail errno: %s !!!\n", strerror(errno));
			return false;
		}

		Json::Reader reader;
		bool ret = reader.parse(ifs, json, false);
		ifs.close();

		LOGD("parseJsonFile json parse %s!\n", ret ? "OK" : "Error");

		return ret;
	}
};

#endif /* _UTILS_JSON_PARSER_H_ */
