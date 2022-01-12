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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "audio.h"

typedef struct
{
	int idx;
	MI_AUDIO_SampleRate_e eSampleRate;
	char name[32];
} AiSampleRateInfo_t;

typedef struct
{
	int idx;
	char name[32];
} AiRecordInfo_t;


static int g_curSampleRateIdx = 0;
static int g_curRecordIdx = 0;

static AiSampleRateInfo_t g_stSampleRateInfo[] = {
	{0, E_MI_AUDIO_SAMPLE_RATE_8000, "8K sample rate"},
	{1, E_MI_AUDIO_SAMPLE_RATE_16000, "16K sample rate"},
	{2, E_MI_AUDIO_SAMPLE_RATE_32000, "32K sample rate"},
	{3, E_MI_AUDIO_SAMPLE_RATE_48000, "48K sample rate"},
};

static AiRecordInfo_t g_stRecordInfo[] = {
	{0, "AMIC_IN_0"},
	{1, "AMIC_IN_1"},
	{2, "AMIC_IN_2"},
	{3, "DMIC_IN_0"},
};

static bool g_bRecord = false;
static bool g_bPlayRecord = false;
static bool g_bTestSpeaker = false;
static bool g_bTestHeadPhone = false;

static bool g_bTestAec = false;
static bool g_bUseDmic = false;
static int g_curDmicGain = 4;
static int g_curAmicGain = 15;
static int g_maxDmicGain = 4;
static int g_maxAmicGain = 21;
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
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
    // 1. check /customer/record direcoty, if not exist, create it
    if (access(AUDIO_IN_RECORD_DIR, R_OK))
    {
    	if (!mkdir(AUDIO_IN_RECORD_DIR, 0777))
    	{
    		printf("create %s success\n", AUDIO_IN_RECORD_DIR);
    	}
    }

    g_bRecord = false;
    g_bPlayRecord = false;
    g_bTestSpeaker = false;
    g_bTestHeadPhone = false;

    mButton_sampleratePtr->setText(g_stSampleRateInfo[g_curSampleRateIdx].name);
    mButton_recordfilePtr->setText(g_stRecordInfo[g_curRecordIdx].name);
    mCheckbox_playbgsoundPtr->setChecked(g_bTestAec);
    if (g_bUseDmic)
    	mRadioGroup_mictypePtr->setCheckedID(ID_TESTAIO_RadioButton_Dmic);
    else
    	mRadioGroup_mictypePtr->setCheckedID(ID_TESTAIO_RadioButton_Amic);

    char maxGainStr[8] = {0};
    char curGainStr[8] = {0};
    if (g_bUseDmic)
	{
		mSeekBar_micGainPtr->setMax(g_maxDmicGain);
		mSeekBar_micGainPtr->setProgress(g_curDmicGain);
		sprintf(maxGainStr, "%d", g_maxDmicGain);
		sprintf(curGainStr, "%d", g_curDmicGain);
		mTextView_maxGainPtr->setText(maxGainStr);
		mTextView_curGainPtr->setText(curGainStr);
	}
	else
	{
		mSeekBar_micGainPtr->setMax(g_maxAmicGain);
		mSeekBar_micGainPtr->setProgress(g_curAmicGain);
		sprintf(maxGainStr, "%d", g_maxAmicGain);
		sprintf(curGainStr, "%d", g_curAmicGain);
		mTextView_maxGainPtr->setText(maxGainStr);
		mTextView_curGainPtr->setText(curGainStr);
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
	if (g_bRecord)
	{
		g_bRecord = false;
		if (g_bUseDmic)
			SSTAR_AI_StopRecord(DMIC_DEV_ID, g_bTestAec);
		else
			SSTAR_AI_StopRecord(AMIC_DEV_ID, g_bTestAec);
	}

	if (g_bTestAec)
		SSTAR_AO_StopTestStereo();

	if (g_bPlayRecord)
	{
		g_bPlayRecord = false;
		SSTRR_AO_StopPlayRecord();
	}

	if (g_bTestSpeaker)
	{
		g_bTestSpeaker = false;
		SSTAR_AO_StopTestStereo();
	}

	if (g_bTestHeadPhone)
	{
		g_bTestHeadPhone = false;
		SSTAR_AO_StopTestHeadPhone();
	}
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
static bool ontestaioActivityTouchEvent(const MotionEvent &ev) {
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
    //LOGD(" ButtonClick sys_back !!!\n");
    return false;
}
static bool onButtonClick_Button_samplerate(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_samplerate !!!\n");
	bool bVisible = mListview_sampleratePtr->isVisible();
	mListview_sampleratePtr->setVisible(!bVisible);
	mListview_sampleratePtr->setSelection(g_curSampleRateIdx);
    return false;
}

static int getListItemCount_Listview_samplerate(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview_samplerate !\n");
    return sizeof(g_stSampleRateInfo) / sizeof(AiSampleRateInfo_t);
}

static void obtainListItemData_Listview_samplerate(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ Listview_samplerate  !!!\n");
	pListItem->setText(g_stSampleRateInfo[index].name);

	if (index == g_curSampleRateIdx)
	{
		pListItem->setSelected(true);
		SSTAR_AI_SetSampleRate(g_stSampleRateInfo[index].eSampleRate);
	}
	else
		pListItem->setSelected(false);
}

static void onListItemClick_Listview_samplerate(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview_samplerate  !!!\n");
	g_curSampleRateIdx = index;
	SSTAR_AI_SetSampleRate(g_stSampleRateInfo[g_curSampleRateIdx].eSampleRate);
	printf("select samplerate %d\n", (int)g_stSampleRateInfo[g_curSampleRateIdx].eSampleRate);
	pListView->setVisible(false);
	mButton_sampleratePtr->setText(g_stSampleRateInfo[index].name);

	// AEC only support 8K/16K samplerate
	if (g_curSampleRateIdx > 1)
	{
		mCheckbox_playbgsoundPtr->setChecked(false);
		mCheckbox_playbgsoundPtr->setTouchable(false);
		g_bTestAec = false;
	}
	else
	{
		mCheckbox_playbgsoundPtr->setTouchable(true);
	}
}

static bool onButtonClick_Button_record(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_record !!!\n");
	bool bSelected = pButton->isSelected();

	if (g_bPlayRecord || g_bTestSpeaker || g_bTestHeadPhone)
		return false;

	if (!bSelected)
	{
		if (g_bTestAec)
			SSTAR_AO_StartTestStereo();

		g_bRecord = true;
		if (g_bUseDmic)
			SSTAR_AI_StartRecord(DMIC_DEV_ID, g_curDmicGain, g_bTestAec);
		else
			SSTAR_AI_StartRecord(AMIC_DEV_ID, g_curAmicGain, g_bTestAec);
		pButton->setText("停止录音");

		mRadioGroup_mictypePtr->setTouchable(false);
		mCheckbox_playbgsoundPtr->setTouchable(false);
		mButton_sampleratePtr->setTouchable(false);
		mSeekBar_micGainPtr->setTouchable(false);

		if (mListview_sampleratePtr->isVisible())
			mListview_sampleratePtr->setVisible(false);
	}
	else
	{
		g_bRecord = false;
		if (g_bUseDmic)
			SSTAR_AI_StopRecord(DMIC_DEV_ID, g_bTestAec);
		else
			SSTAR_AI_StopRecord(AMIC_DEV_ID, g_bTestAec);
		pButton->setText("开始录音");

		if (g_bTestAec)
			SSTAR_AO_StopTestStereo();

		mRadioGroup_mictypePtr->setTouchable(true);
		mButton_sampleratePtr->setTouchable(true);
		mSeekBar_micGainPtr->setTouchable(true);

		if (g_curSampleRateIdx > 1)
			mCheckbox_playbgsoundPtr->setTouchable(false);
		else
			mCheckbox_playbgsoundPtr->setTouchable(true);
	}

	pButton->setSelected(!bSelected);
    return false;
}

static bool onButtonClick_Button_recordfile(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_recordfile !!!\n");
	bool bVisible = mListview_recordfilePtr->isVisible();
	mListview_recordfilePtr->setVisible(!bVisible);
	mListview_recordfilePtr->setSelection(g_curRecordIdx);
    return false;
}

static int getListItemCount_Listview_recordfile(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview_recordfile !\n");
    return sizeof(g_stRecordInfo) / sizeof(AiRecordInfo_t);
}

static void obtainListItemData_Listview_recordfile(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ Listview_recordfile  !!!\n");
	pListItem->setText(g_stRecordInfo[index].name);

	if (index == g_curRecordIdx)
	{
		pListItem->setSelected(true);
	}
	else
		pListItem->setSelected(false);
}

static void onListItemClick_Listview_recordfile(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview_recordfile  !!!\n");
	g_curRecordIdx = index;
	pListView->setVisible(false);
	mButton_recordfilePtr->setText(g_stRecordInfo[index].name);
}

static bool onButtonClick_Button_playrecord(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_playrecord !!!\n");
	bool bSelected = pButton->isSelected();

	if (g_bRecord || g_bTestSpeaker || g_bTestHeadPhone)
		return false;

	if (!bSelected)
	{
		g_bPlayRecord = true;
		SSTRR_AO_StartPlayRecord(g_curRecordIdx);
		pButton->setText("停止播放");
		mButton_recordfilePtr->setTouchable(false);

		if (mListview_recordfilePtr->isVisible())
			mListview_recordfilePtr->setVisible(false);
	}
	else
	{
		g_bPlayRecord = false;
		SSTRR_AO_StopPlayRecord();
		pButton->setText("开始播放");
		mButton_recordfilePtr->setTouchable(true);
	}

	pButton->setSelected(!bSelected);
    return false;
}

static bool onButtonClick_Button_playstereo(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_playstereo !!!\n");
	bool bSelected = pButton->isSelected();

	if (g_bRecord || g_bPlayRecord || g_bTestHeadPhone)
		return false;

	if (!bSelected)
	{
		g_bTestSpeaker = true;
		SSTAR_AO_StartTestStereo();
		pButton->setText("停止测试");
	}
	else
	{
		g_bTestSpeaker = false;
		SSTAR_AO_StopTestStereo();
		pButton->setText("开始测试");
	}

	pButton->setSelected(!bSelected);
    return false;
}
static bool onButtonClick_Button_headphone(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_headphone !!!\n");
	bool bSelected = pButton->isSelected();

	if (g_bRecord || g_bPlayRecord || g_bTestSpeaker)
		return false;

	if (!bSelected)
	{
		g_bTestHeadPhone = true;
		SSTAR_AO_StartTestHeadPhone();
		pButton->setText("停止测试");
	}
	else
	{
		g_bTestHeadPhone = false;
		SSTAR_AO_StopTestHeadPhone();
		pButton->setText("开始测试");
	}

	pButton->setSelected(!bSelected);
    return false;
}

static void onCheckedChanged_Checkbox_playbgsound(ZKCheckBox* pCheckBox, bool isChecked) {
    //LOGD(" Checkbox Checkbox_playbgsound checked %d", isChecked);
		g_bTestAec = isChecked;
}
static void onCheckedChanged_RadioGroup_mictype(ZKRadioGroup* pRadioGroup, int checkedID) {
    //LOGD(" RadioGroup RadioGroup_mictype checked %d", checkedID);
	char maxGainStr[8] = {0};
	char curGainStr[8] = {0};

    switch (checkedID)
    {
    case ID_TESTAIO_RadioButton_Amic:
    	g_bUseDmic = false;
    	mSeekBar_micGainPtr->setMax(g_maxAmicGain);
		mSeekBar_micGainPtr->setProgress(g_curAmicGain);
		sprintf(maxGainStr, "%d", g_maxAmicGain);
		sprintf(curGainStr, "%d", g_curAmicGain);
		mTextView_maxGainPtr->setText(maxGainStr);
		mTextView_curGainPtr->setText(curGainStr);
    	break;
    case ID_TESTAIO_RadioButton_Dmic:
    	g_bUseDmic = true;
    	mSeekBar_micGainPtr->setMax(g_maxDmicGain);
		mSeekBar_micGainPtr->setProgress(g_curDmicGain);
		sprintf(maxGainStr, "%d", g_maxDmicGain);
		sprintf(curGainStr, "%d", g_curDmicGain);
		mTextView_maxGainPtr->setText(maxGainStr);
		mTextView_curGainPtr->setText(curGainStr);
    	break;
    default:
    	g_bUseDmic = false;
    	mSeekBar_micGainPtr->setMax(g_maxAmicGain);
		mSeekBar_micGainPtr->setProgress(g_curAmicGain);
		sprintf(maxGainStr, "%d", g_maxAmicGain);
		sprintf(curGainStr, "%d", g_curAmicGain);
		mTextView_maxGainPtr->setText(maxGainStr);
		mTextView_curGainPtr->setText(curGainStr);
    	break;
    }
}
static void onProgressChanged_SeekBar_micGain(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged SeekBar_micGain %d !!!\n", progress);
	char curGainStr[8] = {0};
	if (g_bUseDmic)
	{
		g_curDmicGain = progress;
	}
	else
	{
		g_curAmicGain = progress;
	}

	sprintf(curGainStr, "%d", progress);
	mTextView_curGainPtr->setText(curGainStr);
}
