/*
 * StoragePreferences.h
 *
 *  Created on: Nov 9, 2017
 *      Author: guoxs
 */

#ifndef _STORAGE_STORAGEPREFERENCES_H_
#define _STORAGE_STORAGEPREFERENCES_H_

#include <string>

class StoragePreferences {
public:
	static bool putString(const std::string &key, const std::string &val);
	static bool putInt(const std::string &key, int val);
	static bool putBool(const std::string &key, bool val);
	static bool putFloat(const std::string &key, float val);
	static bool remove(const std::string &key);
	static bool clear();

	static std::string getString(const std::string &key, const std::string &defVal);
	static int getInt(const std::string &key, int defVal);
	static bool getBool(const std::string &key, bool defVal);
	static float getFloat(const std::string &key, float defVal);
};

#endif /* _STORAGE_STORAGEPREFERENCES_H_ */
