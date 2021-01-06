/*
 * StoragePreferences.h
 *
 *  Created on: Nov 9, 2017
 *      Author: guoxs
 */

#ifndef _STORAGE_STORAGEPREFERENCES_H_
#define _STORAGE_STORAGEPREFERENCES_H_

#include <string>

/**
 * @brief 数据存储
 */
class StoragePreferences {
public:
	/**
	 * @brief 存储string值
	 */
	static bool putString(const std::string &key, const std::string &val);

	/**
	 * @brief 存储int值
	 */
	static bool putInt(const std::string &key, int val);

	/**
	 * @brief 存储bool值
	 */
	static bool putBool(const std::string &key, bool val);

	/**
	 * @brief 存储float值
	 */
	static bool putFloat(const std::string &key, float val);

	/**
	 * @brief 删除key对应的数据
	 */
	static bool remove(const std::string &key);

	/**
	 * @brief 清空存储数据
	 */
	static bool clear();

	/**
	 * @brief 获取key对应的string值，获取不到则返回defVal默认值
	 */
	static std::string getString(const std::string &key, const std::string &defVal);

	/**
	 * @brief 获取key对应的int值，获取不到则返回defVal默认值
	 */
	static int getInt(const std::string &key, int defVal);

	/**
	 * @brief 获取key对应的bool值，获取不到则返回defVal默认值
	 */
	static bool getBool(const std::string &key, bool defVal);

	/**
	 * @brief 获取key对应的float值，获取不到则返回defVal默认值
	 */
	static float getFloat(const std::string &key, float defVal);
};

#endif /* _STORAGE_STORAGEPREFERENCES_H_ */
