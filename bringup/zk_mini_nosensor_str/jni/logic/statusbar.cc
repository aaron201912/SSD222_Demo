#pragma once
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[标识]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[标识]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[标识]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[标识]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[标识]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXX->setText("****") 在控件TextXXX上显示文字****
*mButton1->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBar->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度

*/

#include "hotplugdetect.h"

static int g_connStatus = 0;


void ShowWiredNetworkStatus(unsigned int index, int status, char *pstIfName)
{
	if (status)
		mTextView_wiredNetStatusPtr->setBackgroundPic("hotplugstatus/connected.png");
	else
		mTextView_wiredNetStatusPtr->setBackgroundPic("hotplugstatus/disconnecte.png");
}

// 显示连接状态，已连接显示全信号，未连接显示无信号。在scanCallback中更新wifi信号强度
// dBm:[-100, -50]  quality:[0, 100]
// level_1:dBm > -50; level_2:dBm (-60, -50]; level_3:dBm (-70, -60]; level_4:dBm <= -70
// quality = 2* (dBm + 100); dBm = (quality / 2) -100
void ShowWifiConnStatus(char *pSsid, int status, int quality)
{
//	printf("conn ssid: %s, status: %d, signalSTR: %d\n", pSsid, status, quality);

	g_connStatus = status;

	if (g_connStatus)
	{
		// quality to dBm
		int dBm = 0;

		if (quality <= 0)
			dBm = -100;
		else if (quality >= 100)
			dBm = -50;
		else
			dBm = (quality / 2) - 100;

		if (dBm >= -50)
		{
			mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_4.png");
		}
		else if (dBm > -60)
		{
			mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_3.png");
		}
		else if (dBm > -70)
		{
			mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_2.png");
		}
		else
		{
			mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_1.png");
		}

		//mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_4.png");
	}
	else
		mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_no_signal.png");
}

// 更新wifi信号强度
void ShowWifiSignalSTRStatus(ScanResult_t *pstScanResult, int resCnt)
{
	if (g_connStatus)
	{
		if (resCnt > 0)
		{
			if (pstScanResult[0].signalSTR > -50)
				mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_4.png");
			else if (pstScanResult[0].signalSTR > -60)
				mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_3.png");
			else if (pstScanResult[0].signalSTR > -70)
				mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_2.png");
			else
				mTextView_wifiStatusPtr->setBackgroundPic("hotplugstatus/wifi_signal_1.png");
		}
	}
}


void ShowUsbStatus(UsbParam_t *pstUsbParam)		// action 0, connect; action 1, disconnect
{
	mTextView_usbStatusPtr->setVisible(pstUsbParam->action);
}


/**
 * 注册定时器
 * 在此数组中添加即可
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1->setText("123");
	printf("statusbar init start\n");
	SSTAR_InitHotplugDetect();
//	SSTAR_RegisterWiredNetworkListener(ShowWiredNetworkStatus);
	SSTAR_RegisterWifiStaConnListener(ShowWifiConnStatus);
	SSTAR_RegisterUsbListener(ShowUsbStatus);

	mTextView_usbStatusPtr->setVisible(SSTAR_GetUsbCurrentStatus());

	if (SSTAR_GetWiredNetworkCurrentStatus())
		mTextView_wiredNetStatusPtr->setBackgroundPic("hotplugstatus/connected.png");
	else
		mTextView_wiredNetStatusPtr->setBackgroundPic("hotplugstatus/disconnect.png");

	printf("statusbar init end\n");
}

static void onUI_quit() {
	SSTAR_UnRegisterUsbListener(ShowUsbStatus);
	SSTAR_UnRegisterWifiStaConnListener(ShowWifiConnStatus);
//	SSTAR_UnRegisterWiredNetworkListener(ShowWiredNetworkStatus);
	SSTAR_DeinitHotPlugDetect();
}

static void onProtocolDataUpdate(const SProtocolData &data) {
    //串口数据回调接口
}

static bool onUI_Timer(int id){
	//Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
    return true;
}

static bool onstatusbarActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上
    return false;
}


static bool onButtonClick_sys_back(ZKButton *pButton) {
    //LOGD(" ButtonClick sys_back !!!\n");
    return false;
}

static bool onButtonClick_sys_home(ZKButton *pButton) {
    //LOGD(" ButtonClick sys_home !!!\n");
    return false;
}

void ShowStatusBar(int visible, int enableReturnKey, int enableHomeKey)
{
	msys_backPtr->setVisible(enableReturnKey);
	msys_homePtr->setVisible(enableHomeKey);

	if (visible)
		EASYUICONTEXT->showStatusBar();
	else
		EASYUICONTEXT->hideStatusBar();
}
