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


#include "base_types.h"
#include "statusbarconfig.h"
extern "C"{
#include "voicedetect.h"

int SSTAR_StartPlayAudioFile(const char *WavAudioFile, int aoVolume);
int SSTAR_StopPlayAudioFile(void);
void SSTAR_setVolume(int vol);
}
static TrainedWord_t *g_pstTriggerCmdList = NULL;
static TrainedWord_t *g_pCommonCmdList = NULL;
static int g_nTriggerCmdCnt = 0;
static int g_nCommonCmdCnt = 0;
static int *g_pTriggerCmdSelectList = NULL;
static int *g_pCommonCmdSelectList = NULL;

static void* onVoiceAnalyzeCallback(CommandType_e eType, int commandID) {
	LOGD("Get cmd: type:%d id: %d\n", (int)eType, commandID);

	if (eType == E_TRIGGER_CMD)
	{
		if (commandID >= 0 && commandID < g_nTriggerCmdCnt)
		{
			for (int i=0; i < g_nTriggerCmdCnt; i++)
				memset(g_pTriggerCmdSelectList, 0, sizeof(int)*g_nTriggerCmdCnt);

			for (int i=0; i < g_nCommonCmdCnt; i++)
				memset(g_pCommonCmdSelectList, 0, sizeof(int)*g_nCommonCmdCnt);

			g_pTriggerCmdSelectList[commandID] = true;
			//mListView_triggerPtr->refreshListView();
			mButtonswPtr->setSelected(1);
			mTextView_statusPtr->setTextTr("voicedetect_active");
			mListView_commonPtr->refreshListView();
		}
		else
		{
			for (int i=0; i < g_nTriggerCmdCnt; i++)
				memset(g_pTriggerCmdSelectList, 0, sizeof(int)*g_nTriggerCmdCnt);

			for (int i=0; i < g_nCommonCmdCnt; i++)
				memset(g_pCommonCmdSelectList, 0, sizeof(int)*g_nCommonCmdCnt);

			mButtonswPtr->setSelected(0);
			mTextView_statusPtr->setTextTr("voicedetect_inactive");
			mListView_commonPtr->refreshListView();
		}
	}
	else if (eType == E_COMMON_CMD)
	{
		if (commandID >= 0 && commandID < g_nCommonCmdCnt)
		{
			for (int i=0; i < g_nTriggerCmdCnt; i++)
				memset(g_pTriggerCmdSelectList, 0, sizeof(int)*g_nTriggerCmdCnt);

			for (int i=0; i < g_nCommonCmdCnt; i++)
				memset(g_pCommonCmdSelectList, 0, sizeof(int)*g_nCommonCmdCnt);

			g_pCommonCmdSelectList[commandID] = true;
			//mListView_triggerPtr->refreshListView();
			mListView_commonPtr->refreshListView();
		}
	}


	return NULL;
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
	// set date
	system("date 092912122020.12");

	int nRet = SSTAR_VoiceDetectInit(&g_pstTriggerCmdList, &g_nTriggerCmdCnt, &g_pCommonCmdList, &g_nCommonCmdCnt);
	if (nRet)
	{
		printf("Voice detect init failed\n");
		return;
	}

	if (g_nTriggerCmdCnt && g_nCommonCmdCnt)
	{
		g_pTriggerCmdSelectList = (int*)malloc(sizeof(int) * g_nTriggerCmdCnt);
		g_pCommonCmdSelectList = (int*)malloc(sizeof(int) * g_nCommonCmdCnt);
		
		if (!g_pTriggerCmdSelectList || !g_pCommonCmdSelectList)
		{
			printf("no memory left\n");
			return;
		}
		
		memset(g_pTriggerCmdSelectList, 0, sizeof(int) * g_nTriggerCmdCnt);
		memset(g_pCommonCmdSelectList, 0, sizeof(int) * g_nCommonCmdCnt);
		SSTAR_VoiceDetectStart(onVoiceAnalyzeCallback);
	}
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
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
	printf("onUi quit\n");
	SSTAR_VoiceDetectStop();
	SSTAR_VoiceDetectDeinit();
	SAFE_FREE(g_pstTriggerCmdList);
	SAFE_FREE(g_pTriggerCmdSelectList);
	SAFE_FREE(g_pCommonCmdList);
	SAFE_FREE(g_pCommonCmdSelectList);
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
static bool onvoicedetectActivityTouchEvent(const MotionEvent &ev) {
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
static int getListItemCount_ListView_common(const ZKListView *pListView) {
    //LOGD("getListItemCount_ListView_common !\n");
    return g_nCommonCmdCnt;
}

static void obtainListItemData_ListView_common(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ListView_common  !!!\n");
	if (index < g_nCommonCmdCnt)
	{
		pListItem->setText((char*)g_pCommonCmdList[index].cmd);
		pListItem->setSelected(g_pCommonCmdSelectList[index]);
	}
	else
	{
		pListItem->setText("");
		pListItem->setSelected(false);
	}

	pListItem->setSelected(g_pCommonCmdSelectList[index]);
}

static void onListItemClick_ListView_common(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListView_common  !!!\n");
}

static bool onButtonClick_Buttonsw(ZKButton *pButton) {
    LOGD(" ButtonClick Buttonsw !!!\n");
    return false;
}

static int getListItemCount_ListView_trigger(const ZKListView *pListView) {
    //LOGD("getListItemCount_ListView_trigger !\n");
    return g_nTriggerCmdCnt;
}

static void obtainListItemData_ListView_trigger(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ListView_trigger  !!!\n");
	if (index < g_nTriggerCmdCnt)
	{
		pListItem->setText((char*)g_pstTriggerCmdList[index].cmd);
		pListItem->setSelected(g_pTriggerCmdSelectList[index]);
	}
	else
	{
		pListItem->setText("");
		pListItem->setSelected(false);
	}

	pListItem->setSelected(g_pTriggerCmdSelectList[index]);
}

static void onListItemClick_ListView_trigger(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListView_trigger  !!!\n");
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    LOGD(" ButtonClick sys_back !!!\n");
    return false;
}
