/*
 * GpioHelper.h
 *
 *  Created on: May 17, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_GPIO_HELPER_H_
#define _UTILS_GPIO_HELPER_H_

#include <stdint.h>

#define GPIO_PIN_B_02		"B_02"
#define GPIO_PIN_B_03		"B_03"
#define GPIO_PIN_E_20		"E_20"

/**
 * @brief GPIO边沿触发方式
 */
typedef enum {
	E_GPIO_EDGE_TYPE_NONE,		/**< 无中断触发 默认值 */
	E_GPIO_EDGE_TYPE_RISING,	/**< 上升沿触发 */
	E_GPIO_EDGE_TYPE_FALLING,	/**< 下降沿触发 */
	E_GPIO_EDGE_TYPE_BOTH		/**< 上升沿或下降沿触发 */
} EGpioEdgeType;

class IGpioListener {
public:
	virtual ~IGpioListener() { }
	virtual bool onGpioEdge(const char *pPin) = 0;
	virtual void onGpioError(const char *pPin, int error) = 0;
};

/**
 * @brief GPIO操作帮助类
 */
class GpioHelper {
public:
	static void initPinMap(uint32_t enmap, uint32_t dirmap, uint32_t levelmap);

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

	/**
	 * @brief 注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 * @param type 触发方式
	 */
	static void registerGpioListener(const char *pPin, IGpioListener *pListener, EGpioEdgeType type);

	/**
	 * @brief 反注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 */
	static void unregisterGpioListener(const char *pPin, IGpioListener *pListener);
};

#endif /* _UTILS_GPIO_HELPER_H_ */
