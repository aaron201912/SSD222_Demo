/*
 * GpioHelper.h
 *
 *  Created on: May 17, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_GPIO_HELPER_H_
#define _UTILS_GPIO_HELPER_H_

#define GPIO_PIN_B_02		"B_02"
#define GPIO_PIN_B_03		"B_03"
#define GPIO_PIN_E_20		"E_20"

/**
 * @brief GPIO操作帮助类
 */
class GpioHelper {
public:
	/**
	 * @brief 读io口状态
	 * @return -1 失败， 1 / 0(高 / 低电平) 成功
	 */
	static int input(const char *pPin);

	/**
	 * @brief 写io口状态
	 * @param val 1 高电平，0 低电平
	 * @return -1 失败， 0 成功
	 */
	static int output(const char *pPin, int val);
};

#endif /* _UTILS_GPIO_HELPER_H_ */
