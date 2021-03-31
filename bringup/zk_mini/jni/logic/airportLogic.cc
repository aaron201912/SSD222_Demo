#pragma once
#include "uart/ProtocolSender.h"
#include "hotplugdetect.h"
#include "statusbarconfig.h"
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

static AirportInfo_t g_curApInfo;
static int g_isApSupport = 1;
static int g_isApEnable = 0;
static MI_WLAN_Status_t g_curApStatus;
static Mutex g_apStatusLock;

void ShowConnectedDevList(MI_WLAN_Status_t *pstApStatus)
{
	g_apStatusLock.lock();
	memcpy(&g_curApStatus, pstApStatus, sizeof(MI_WLAN_Status_t));
	g_apStatusLock.unlock();
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	if (SSTAR_InitWifiApMode())
	{
		printf("WLAN not support Ap mode\n");
		g_isApSupport = 0;
	}
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }

    if (g_isApSupport)
    {
    	memset(&g_curApInfo, 0, sizeof(AirportInfo_t));
    	strcpy(g_curApInfo.name, SSTAR_GetAirportName());
    	strcpy(g_curApInfo.passwd, SSTAR_GetAirportPasswd());
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	if (g_isApSupport)
		SSTAR_DeinitWifiApMode();

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
static bool onairportActivityTouchEvent(const MotionEvent &ev) {
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
static bool onButtonClick_sys_back(ZKButton *pButton) {
    LOGD(" ButtonClick sys_back !!!\n");
    return false;
}

static bool onButtonClick_Button_airportsw(ZKButton *pButton) {
    LOGD(" ButtonClick Button_airportsw !!!\n");
    if (g_isApSupport)
    {
    	int enableStatus = !g_isApEnable;

    	if (enableStatus)
    	{
    		if (!SSTAR_EnableWifiApMode(&g_curApInfo, ShowConnectedDevList))
    			g_isApEnable = enableStatus;
    	}
    	else
    	{
    		SSTAR_DisableWifiApMode();
    		g_isApEnable = enableStatus;
    	}
    }
    return false;
}

static int getListItemCount_Listview_devInfo(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview_devInfo !\n");
	int itemCnt = 0;

	g_apStatusLock.lock();
	itemCnt = g_curApStatus.stApStatus.u16HostNum;
	g_apStatusLock.unlock();

    return itemCnt;
}

static void obtainListItemData_Listview_devInfo(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ Listview_devInfo  !!!\n");
	ZKListView::ZKListSubItem *pDevNameItem = pListItem->findSubItemByID(ID_AIRPORT_SubItem_devName);
	ZKListView::ZKListSubItem *pIpItem = pListItem->findSubItemByID(ID_AIRPORT_SubItem_ip);
	ZKListView::ZKListSubItem *pMacItem = pListItem->findSubItemByID(ID_AIRPORT_SubItem_mac);
	ZKListView::ZKListSubItem *pConnectTimeItem = pListItem->findSubItemByID(ID_AIRPORT_SubItem_connectTime);
	char connectTime[32] = {0};

	g_apStatusLock.lock();
	if (!g_curApStatus.stApStatus.u16HostNum)
	{
		printf("no connected device\n");
		g_apStatusLock.unlock();
	}

	pDevNameItem->setText((char*)g_curApStatus.stApStatus.astHosts[index].hostname);
	pIpItem->setText((char*)g_curApStatus.stApStatus.astHosts[index].ipaddr);
	pMacItem->setText((char*)g_curApStatus.stApStatus.astHosts[index].macaddr);
	sprintf(connectTime, "%lld ms", g_curApStatus.stApStatus.astHosts[index].connectedtime);
	pConnectTimeItem->setText(connectTime);

	g_apStatusLock.unlock();
}

static void onListItemClick_Listview_devInfo(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview_devInfo  !!!\n");
}

static bool onButtonClick_Button_save(ZKButton *pButton) {
    LOGD(" ButtonClick Button_save !!!\n");
    int updateApInfo = 0;

    if (!g_isApSupport)
    	return false;

    if (strcmp(mTextView_inputNamePtr->getText().c_str(), g_curApInfo.name))
	{
    	memset(g_curApInfo.name, 0, sizeof(g_curApInfo.name));
    	strcpy(g_curApInfo.name, mTextView_inputNamePtr->getText().c_str());
    	updateApInfo = 1;
	}

    if (strcmp(mTextView_inputPasswdPtr->getText().c_str(), g_curApInfo.passwd))
    {
    	memset(g_curApInfo.passwd, 0, sizeof(g_curApInfo.passwd));
		strcpy(g_curApInfo.passwd, mTextView_inputPasswdPtr->getText().c_str());
		updateApInfo = 1;
    }

    if (updateApInfo && g_isApEnable)
    {
    	SSTAR_DisableWifiApMode();
    	if (SSTAR_EnableWifiApMode(&g_curApInfo, ShowConnectedDevList))
    	{
    		g_isApEnable = 0;
    		mButton_airportswPtr->setSelected(g_isApEnable);
    	}
    }

    return false;
}
