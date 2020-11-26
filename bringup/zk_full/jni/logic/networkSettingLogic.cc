#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/
#include <algorithm>
#include "statusbarconfig.h"
#include "hotplugdetect.h"

#define TIMER_LOADING	0
#define TIMER_SCAN   	1

// loading timer
static bool isRegistered = false;
static bool isLoading = false;
static bool loadingStatus = false;	// save last loading status
static bool bLogOn = true;
#define WIFI_LOG(fmt, args...) {if(bLogOn) {printf("\033[1;34m");printf("%s[%d]:", __FUNCTION__, __LINE__);printf(fmt, ##args);printf("\033[0m");}}

static int g_connStatus = 0;
static char g_connSsid[256] = {0};
static std::vector<ScanResult_t> g_vecScanResult;
static Mutex g_scanResLock;

unsigned long long getSysTime()
{
	struct timespec ts;
	unsigned long long ms;
	memset(&ts, 0, sizeof(ts));
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ms = (unsigned long long)(ts.tv_sec * 1000) + (unsigned long long)(ts.tv_nsec / 1000000);

	return ms;
}

static void registerPrivTimer(bool *pIsRegistered, int id, int time) {
     //如果没有注册才进行注册定时器
     if (!(*pIsRegistered)) {
         mActivityPtr->registerUserTimer(id, time);
         *pIsRegistered = true;
     }
 }

static void unregisterPrivTimer(bool *pIsRegistered, int id) {
    //如果已经注册了定时器，则取消注册
    if (*pIsRegistered) {
        mActivityPtr->unregisterUserTimer(id);
        *pIsRegistered = false;
    }
}

static void updateAnimation() {
    char path[50];
    static int animationIndex = 0;
    sprintf(path,"animation/loading_%d.png",animationIndex++ % 60);
    mTextview_loadingPtr->setBackgroundPic(path);
}

// 显示连接状态，已连接显示全信号，未连接显示无信号。在scanCallback中更新wifi信号强度
void WifiConnStatusCallback(char *pSsid, int status, int quality)
{
//	printf("conn ssid: %s, status: %d, quality: %d\n", pSsid, status, quality);
	if (!pSsid)
		return;

	g_scanResLock.lock();
	g_connStatus = status;
	memset(g_connSsid, 0, sizeof(g_connSsid));
	memcpy(g_connSsid, pSsid, strlen(pSsid));
	g_scanResLock.unlock();

	if (mListviewNetworkPtr->isVisible())
	{
		mListviewNetworkPtr->refreshListView();
	}
}

// 更新wifi信号强度
void WifiSignalSTRStatusCallback(ScanResult_t *pstScanResult, int resCnt)
{
	g_scanResLock.lock();
	g_vecScanResult.clear();

	for (int i = 0; i < resCnt; i++)
	{
		g_vecScanResult.push_back(pstScanResult[i]);
	}

	g_scanResLock.unlock();

	if (!resCnt)
	{
		registerPrivTimer(&isRegistered, TIMER_LOADING, 50);
		mTextview_loadingPtr->setVisible(true);
		mListviewNetworkPtr->setVisible(false);
	}
	else
	{
		unregisterPrivTimer(&isRegistered, TIMER_LOADING);
		mTextview_loadingPtr->setVisible(false);
		mListviewNetworkPtr->setVisible(true);
		mListviewNetworkPtr->refreshListView();
	}
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	WIFI_LOG("onUI_init\n");
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
	WIFI_LOG("onUI_intent\n");

	SSTAR_RegisterWifiStaConnListener(WifiConnStatusCallback);
	SSTAR_RegisterWifiStaScanListener(WifiSignalSTRStatusCallback);

	WIFI_LOG("register callback done\n");

	if (SSTAR_GetWifiSupportStatus())
	{
		WIFI_LOG("support wifi\n");
		bool bWifiEnable = (bool)SSTAR_GetWifiEnableStatus();
		mTextviewNotSupportPtr->setVisible(false);
		mTextviewWifiPtr->setVisible(true);
		mButtonWifiswPtr->setVisible(true);
		mButtonWifiswPtr->setSelected(bWifiEnable);
		mTextviewWifiListPtr->setVisible(bWifiEnable);
		mTextview_loadingPtr->setVisible(bWifiEnable);

		if (bWifiEnable)
		{
			isLoading = true;
			loadingStatus = true;

			WIFI_LOG("register loading timer\n");
			registerPrivTimer(&isRegistered, TIMER_LOADING, 50);
		}
	}
	else
	{
		WIFI_LOG("not support wifi\n");
		mTextviewNotSupportPtr->setVisible(true);
		mTextviewWifiPtr->setVisible(false);
		mButtonWifiswPtr->setVisible(false);
		mTextviewWifiListPtr->setVisible(false);
		mTextview_loadingPtr->setVisible(false);
	}

	mListviewNetworkPtr->setVisible(false);

	g_scanResLock.lock();
	g_vecScanResult.clear();
	g_scanResLock.unlock();

	WIFI_LOG("Leave onUI_intent\n");
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
	WIFI_LOG("onUI_show\n");
	WIFI_LOG("Leave onUI_show\n");
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	WIFI_LOG("onUI_hide\n");
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	WIFI_LOG("onUI_quit\n");

	SSTAR_UnRegisterWifiStaConnListener(WifiConnStatusCallback);
	SSTAR_UnRegisterWifiStaScanListener(WifiSignalSTRStatusCallback);
	unregisterPrivTimer(&isRegistered, TIMER_LOADING);

	g_scanResLock.lock();
	g_vecScanResult.clear();
	g_scanResLock.unlock();
	ShowStatusBar(1, 0, 0);
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id){
	switch (id) {
		case TIMER_LOADING:
			{
				//WIFI_LOG("loading timer tick, preLoadStatus=%d, loadStatus=%d\n", loadingStatus, isLoading);
				if (isLoading)
				{
					if (loadingStatus != isLoading)
					{
						mListviewNetworkPtr->setVisible(false);
						mTextview_loadingPtr->setVisible(true);
					}

					// load pic
					updateAnimation();
				}

				loadingStatus = isLoading;
			}
			break;
		default:
			break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onnetworkSettingActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			break;
		default:
			break;
	}
	return false;
}

static bool onButtonClick_ButtonWifisw(ZKButton *pButton) {
    //LOGD(" ButtonClick ButtonWifisw !!!\n");
	WIFI_LOG("onButtonClick_ButtonWifisw\n");
	bool bWifiEnable = false;
	pButton->setSelected(!pButton->isSelected());
	bWifiEnable = pButton->isSelected();
	SSTAR_SetWifiEnableStatus(bWifiEnable);
	mTextviewWifiListPtr->setVisible(bWifiEnable);
	mTextview_loadingPtr->setVisible(bWifiEnable);
	mListviewNetworkPtr->setVisible(false);

	if (bWifiEnable)
	{
		WIFI_LOG("register loading timer\n");
		registerPrivTimer(&isRegistered, TIMER_LOADING, 50);
	}
	else
	{
		unregisterPrivTimer(&isRegistered, TIMER_LOADING);
		SSTAR_DisconnectWifi();
		g_scanResLock.lock();
		g_vecScanResult.clear();
		g_scanResLock.unlock();
	}

    return false;
}

static int getListItemCount_ListviewNetwork(const ZKListView *pListView) {
    //LOGD("getListItemCount_ListviewNetwork !\n");
	int size = 0;

	g_scanResLock.lock();
	size = g_vecScanResult.size();
	g_scanResLock.unlock();

	return size;
}

static void obtainListItemData_ListviewNetwork(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ListviewNetwork  !!!\n");
	ZKListView::ZKListSubItem *pLevelItem = pListItem->findSubItemByID(ID_NETWORKSETTING_SubItemSignal);
	ZKListView::ZKListSubItem *pNameItem = pListItem->findSubItemByID(ID_NETWORKSETTING_SubItemNetworkID);
	ZKListView::ZKListSubItem *pEncryItem = pListItem->findSubItemByID(ID_NETWORKSETTING_SubItemEncry);
	ZKListView::ZKListSubItem *pConnectStatusItem = pListItem->findSubItemByID(ID_NETWORKSETTING_SubItemConnected);

	g_scanResLock.lock();
	if (!g_vecScanResult.size())
	{
		printf("no scan result\n");
		g_scanResLock.unlock();
		return;
	}

	const ScanResult_t &scanRes = g_vecScanResult.at(index);
	bool bConnected = (g_connStatus && !strcmp(scanRes.ssid, g_connSsid));
	g_scanResLock.unlock();

	pNameItem->setText(scanRes.ssid);
	pEncryItem->setVisible(scanRes.bEncrypt);

	if (scanRes.signalSTR > -50) {
		pLevelItem->setBackgroundPic("wifi/wifi_signal_4.png");
	} else if (scanRes.signalSTR > -60) {
		pLevelItem->setBackgroundPic("wifi/wifi_signal_3.png");
	} else if (scanRes.signalSTR > -70) {
		pLevelItem->setBackgroundPic("wifi/wifi_signal_2.png");
	} else {
		pLevelItem->setBackgroundPic("wifi/wifi_signal_1.png");
	}

	if (bConnected) {
		pConnectStatusItem->setVisible(true);
	} else {
		pConnectStatusItem->setVisible(false);
	}
}

static void onListItemClick_ListviewNetwork(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListviewNetwork  !!!\n");
	g_scanResLock.lock();
	const ScanResult_t &scanRes = g_vecScanResult.at(index);
	bool bConnected = (g_connStatus && !strcmp(scanRes.ssid, g_connSsid));
	g_scanResLock.unlock();

	if (bConnected)
	{
		EASYUICONTEXT->openActivity("networkSetting2Activity");
	}
	else
	{
		// show select ssid info
		if (scanRes.bEncrypt)
		{
			Intent* intent = new Intent();
			intent->putExtra("ssid", string(scanRes.ssid));
			EASYUICONTEXT->openActivity("networkSetting3Activity", intent);
		}
		else
		{
			MI_WLAN_ConnectParam_t connParam;
			memset(&connParam, 0, sizeof(MI_WLAN_ConnectParam_t));
			connParam.eSecurity = E_MI_WLAN_SECURITY_WPA;
			connParam.OverTimeMs = 5000;
			memcpy(connParam.au8SSId, scanRes.ssid, strlen(scanRes.ssid));

			SSTAR_DisconnectWifi();
			SSTAR_ConnectWifi(&connParam);
		}
	}
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    //LOGD(" ButtonClick sys_back !!!\n");
    return false;
}
