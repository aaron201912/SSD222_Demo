/*
 * SpiHelper.h
 *
 *  Created on: May 3, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_SPI_HELPER_H_
#define _UTILS_SPI_HELPER_H_

#include <stdint.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

struct spi_dev;

/**
 * @brief SPI操作帮助类
 */
class SpiHelper {
public:
	/**
	 * @brief 构造函数
	 * @param nr spi总线号
	 * @param mode 模式，可选值： SPI_MODE_0、SPI_MODE_1、SPI_MODE_2、SPI_MODE_3
	 * @param speed spi时钟频率
	 * @param bits 一个字有多少位，默认值： 8
	 * @param isLSB 位顺序，true： 表示低位在前，false： 表示高位在前； 默认值： false，高位在前
	 */
	SpiHelper(int nr, uint8_t mode, uint32_t speed, uint8_t bits = 8, bool isLSB = false);
	virtual ~SpiHelper();

	/**
	 * @brief 设置spi模式
	 * @param mode 模式，可选值： SPI_MODE_0、SPI_MODE_1、SPI_MODE_2、SPI_MODE_3
	 * @return true 成功， false 失败
	 */
	bool setMode(uint8_t mode);

	/**
	 * @brief 设置spi时钟频率
	 * @param speed 时钟频率
	 * @return true 成功， false 失败
	 */
	bool setSpeed(uint32_t speed);

	/**
	 * @brief 设置一个字有多少位
	 * @param bits 位数
	 * @return true 成功， false 失败
	 */
	bool setBitsPerWord(uint8_t bits);

	/**
	 * @brief 设置位顺序
	 * @param isLSB true： 表示低位在前，false： 表示高位在前
	 * @return true 成功， false 失败
	 */
	bool setBitSeq(bool isLSB);

	/**
	 * @brief 全双工传输
	 * @param tx 写数据地址
	 * @param rx 读数据地址
	 * @param len 数据长度，注意： 读、写数据长度需一致
	 * @return true 成功， false 失败
	 */
	bool fullduplexTransfer(const uint8_t *tx, uint8_t *rx, uint32_t len);

	/**
	 * @brief 半双工传输
	 * @param tx 写数据地址
	 * @param txLen 写数据长度
	 * @param rx 读数据地址
	 * @param rxLen 读数据长度
	 * @return true 成功， false 失败
	 */
	bool halfduplexTransfer(const uint8_t *tx, uint32_t txLen, uint8_t *rx, uint32_t rxLen);

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
	struct spi_dev *mSpiDevPtr;
};

#endif /* _UTILS_SPI_HELPER_H_ */
