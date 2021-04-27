#pragma once
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
*mTextXXX->setText("****") 在控件TextXXX上显示文字****
*mButton1->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBar->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/
#include "uart/ProtocolSender.h"
#include "statusbarconfig.h"

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
	mCirclebar1Ptr->setProgress(50);
#ifdef DISABLE_HELP_INFO
	mButton1Ptr->setVisible(false);
#endif
}

static void onUI_quit() {
	ShowStatusBar(1, 0, 0);
}


static void onProtocolDataUpdate(const SProtocolData &data) {
    // 串口数据回调接口
    if (strcmp(EASYUICONTEXT->currentAppName(), "testSliderActivity") == 0) {
        mSeekBar1Ptr->setProgress(data.seekbar_temperature);
        mSeekBar2Ptr->setProgress(data.seekbar_light);
    }
}

static bool onUI_Timer(int id){
    //Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
    return true;
}


static bool ontestSliderActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上
    return false;
}
static void onProgressChanged_SeekBar2(ZKSeekBar *pSeekBar, int progress) {
	//printf("seekbar2 pos:%d\n", progress);
	mTextValuePtr->setText(progress);
//	BRIGHTNESSHELPER->setBrightness(progress);
    BYTE data[2] = {0x00, (BYTE)progress};
    sendProtocol(CMD_SEEKBAR_LIGHT, data, sizeof(data));
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    return false;
}

static void onProgressChanged_SeekBar1(ZKSeekBar *pSeekBar, int progress) {
//	mCirclebar1Ptr->setProgress(progress);
	//printf("seekbar1 pos:%d\n", progress);
	mTextview3Ptr->setText(progress);
	BYTE data[2] = {0x00, (BYTE)progress};
	sendProtocol(CMD_SEEKBAR_TEMPERATURE, data, sizeof(data));
	//printf("seekbar1 pos xx:%d\n", progress);
}

static void onProgressChanged_CircleBar1(ZKCircleBar *pCircleBar, int progress) {
	//printf("circlebar1 pos xx:%d\n", progress);
}

static bool onButtonClick_Button1(ZKButton *pButton) {
    EASYUICONTEXT->openActivity("helpSliderActivity");
    return false;
}
