/*
 * ProtocolData.h
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */

#ifndef _PROTOCOL_DATA_H_
#define _PROTOCOL_DATA_H_

#include <string>
#include "CommDef.h"

/* CmdID	DeviceID	DataLen		Data */
/* 2Byte     1Byte      1Byte      N Byte*/

/******************** CmdID ***********************/
#define CMDID_POWER							0x0
#define CMDID_RUN_MODE						0x1
#define CMDID_WIND_SPEED_LEVEL				0x2
#define CMDID_CYCLE_MODEM					0x3
#define CMDID_CHILD_LOCK					0x4
#define CMDID_UVLIGHT						0x5
#define CMDID_ANION							0x6
#define CMDID_FILTER_REPLACE_TIMER			0x7
#define CMDID_TIMMING_TIME_CLOSE			0x8
#define CMDID_TIMMING_TIME_OPEN				0x9
#define CMDID_NOW_TIME						0xA
#define CMDID_BEEP							0xB
#define CMDID_ZIGBEE_PAIR					0xC
#define CMDID_DEFROST						0xD
#define CMDID_HEAT							0xE
#define CMDID_HUMIDITY_SET					0xF
#define CMDID_PM2_5_SET						0x10
#define CMDID_HUMIDIFICATION				0x11
#define CMDID_FILTER_REPLACE_TIMER_RESET	0x12
#define CMDID_FRESH_AIR_SET					0x13
#define CMDID_EXHAUST_SET					0x14
#define CMDID_ERROR							0x15
#define CMDID_ERROR_CLEAR					0x16
#define CMDID_SYS_STATUS					0x17
#define CMDID_UPDATE_MCU					0x18
#define CMDID_SMART_LINK					0x19
#define CMDID_DEVICE_TYPE					0x1A
#define CMDID_DISPLAY_CTRL					0x1B
#define CMDID_DISPLAY_ON_TIME				0x1C

#define CMDID_PM2_5							0x8000
#define CMDID_CO2							0x8001
#define CMDID_TVOC							0x8002
#define CMDID_TEMPERATURE					0x8003
#define CMDID_HUMIDITY						0x8004
#define CMDID_O2							0x8005
#define CMDID_HCHO							0x8006
#define CMDID_ESP							0x8007
#define CMDID_WATER_SHORTAGE				0x8008
#define CMDID_WIFI_STATE					0x8009
#define CMDID_ERROR_CODE					0x800A
#define CMDID_REQUEST_ALL_INFO					0x800C


#define CMD_POINTER_ANGLE           0x01
#define CMD_SEEKBAR_TEMPERATURE     0x02
#define CMD_SEEKBAR_LIGHT           0x03
#define CMD_BUTTON_ON               0x03


/**************************************************/

/******************** 错误码 ***********************/
#define ERROR_CODE_CMDID			1
#define ERROR_CODE_DEVICEID			2
#define ERROR_CODE_DATALEN			3
#define ERROR_CODE_PARAM			4
#define ERROR_CODE_PARSER			5
#define ERROR_CODE_CMDID_NO_SUPPORT	6
/**************************************************/

typedef enum {
	E_RUN_MODE_AUTO 	= 0,
	E_RUN_MODE_MANUAL	= 1,
	E_RUN_MODE_SLEEP	= 2,
	E_RUN_MODE_QUIET	= 3,
	E_RUN_MODE_NEWAIR	= 4
} ERunMode;

typedef struct {
	UINT16 slaveAddr;		// 从机地址
	UINT16 masterAddr;		// 主机地址
	BYTE power;				// 开关机状态
	ERunMode eRunMode;		// 运行模式
	BYTE internalWindSpeedLevel;		// 内部风速等级: 0~11
	BYTE externalWindSpeedLevel;		// 外部风速等级: 0~11
	BYTE internalCycle;		// 内循环
	BYTE externalCycle;		// 外循环
	BYTE childLock;			// 童锁
	BYTE uvSJ;				// 杀菌
	BYTE uvXD;				// 消毒
	BYTE anion;				// 负离子
	BYTE ionCloud;			// 离子云
	UINT16 gTimer;			// G级 滤网使用时间，单位为天，最大值为999，即0x3E7
	UINT16 fTimer;			// F级
	UINT16 hTimer;			// H级
	UINT16 iTimer;			// 离子云
	UINT16 closeTime;		// 定时关机时间，如0x1702表示23:02后关机 0x0000表示不进行定时关机
	UINT16 openTime;		// 定时开机时间，如0x1702表示23:02后开机 0x0000表示不进行定时开机
	BYTE nowTime[6];		// 当前时间
	BYTE beep;				// 蜂鸣器发声
	BYTE zigbeePair;		// Zigbee配对, 00 配对失败，01 配对成功，02正在配对
	BYTE defrost;			// 除霜
	BYTE heat;				// 辅热
	UINT16 humidity;		// 湿度
	UINT16 pm2_5Set;		// PM2.5
	BYTE humidification;	// 加湿功能
	BYTE gFilterReset;		// G级 滤网时间复位
	BYTE fFilterReset;		// F级
	BYTE hFilterReset;		// H级
	BYTE iFilterReset;		// 离子云
	UINT16 freshAir;		// 新风窗口
	UINT16 exhaust;			// 排风窗口
	BYTE sError;			// 缺水报警
	BYTE iError;			// 离子云报警
	BYTE gError;			// G级报警
	BYTE hError;			// H级报警
	BYTE fError;			// F级报警
	BYTE smartLink;
	UINT16 deviceType;
	BYTE displayCtrl;
	UINT16 displayOnTime;
	UINT16 pm2_5;			// PM2.5
	UINT16 co2;
	BYTE tvoc;
	int internalTemperature;
	int externalTemperature;
	int internalHumidity;
	int externalHumidity;
	UINT16 o2;
	UINT16 hcho;
	BYTE esp;
	BYTE waterShortage;
	BYTE wifiState;		// 0 disconnect, 1 connected, 2 smartlinking
	BYTE errorCode;		// 0~255
	std::string password;


	int pointer_angle;  //指针偏转角度 0~240
	int seekbar_temperature;//温度
	int seekbar_light;//亮度

} SProtocolData;

#endif /* _PROTOCOL_DATA_H_ */
