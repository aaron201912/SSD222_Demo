/*
 * I2CHelper.h
 *
 *  Created on: May 3, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_I2C_HELPER_H_
#define _UTILS_I2C_HELPER_H_

#include <stdint.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

struct i2c_dev;

/**
 * @brief I2C操作帮助类
 */
class I2CHelper {
public:
	/**
	 * @brief 构造函数
	 * @param nr i2c总线号
	 * @param slaveAddr 从机地址
	 * @param timeout 超时， 单位: ms
	 * @param retries 重试次数
	 */
	I2CHelper(int nr, uint32_t slaveAddr, uint32_t timeout, uint32_t retries);
	virtual ~I2CHelper();

	/**
	 * @brief 设置从机地址
	 * @param slaveAddr 从机地址
	 * @return true 成功， false 失败
	 */
	bool setSlaveAddr(uint32_t slaveAddr);

	/**
	 * @brief 设置通讯超时
	 * @param timeout 超时， 单位: ms
	 * @return true 成功， false 失败
	 */
	bool setTimeout(uint32_t timeout);

	/**
	 * @brief 设置通讯失败重试次数
	 * @param retries 重试次数
	 * @return true 成功， false 失败
	 */
	bool setRetries(uint32_t retries);

	/**
	 * @brief 半双工传输，即共用读写，中间无stop信号
	 * @param tx 写数据地址
	 * @param txLen 写数据长度
	 * @param rx 读数据地址
	 * @param rxLen 读数据长度
	 * @return true 成功， false 失败
	 */
	bool transfer(const uint8_t *tx, uint32_t txLen, uint8_t *rx, uint32_t rxLen);

	/**
	 * @brief 单工读
	 * @param rx 读数据地址
	 * @param len 数据长度
	 * @return true 成功， false 失败
	 */
	bool read(uint8_t *rx, uint32_t len);

	/**
	 * @brief 单工写
	 * @param tx 写数据地址
	 * @param len 数据长度
	 * @return true 成功， false 失败
	 */
	bool write(const uint8_t *tx, uint32_t len);

private:
	struct i2c_dev *mI2CDevPtr;
};

#endif /* _UTILS_I2C_HELPER_H_ */
