/*
 * AdcHelper.h
 *
 *  Created on: May 15, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_ADC_HELPER_H_
#define _UTILS_ADC_HELPER_H_

#include <stdio.h>

/**
 * @brief ADC操作帮助类
 */
class AdcHelper {
public:
	/**
	 * @brief 设置adc使能状态
	 * @param isEnable true 使能， false 禁止
	 * @return true 成功， false 失败
	 */
	static bool setEnable(bool isEnable);

	/**
	 * @brief 读取adc值
	 * @return adc值 成功，-1 失败
	 */
	static int getVal();
};

#endif /* _UTILS_ADC_HELPER_H_ */
