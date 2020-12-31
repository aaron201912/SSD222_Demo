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
static bool onzkimeActivityTouchEvent(const MotionEvent &ev) {
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
static bool onButtonClick_BUTTON_A(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_A !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_B(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_B !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_C(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_C !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_D(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_D !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_E(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_E !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_F(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_F !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_G(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_G !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_H(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_H !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_I(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_I !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_J(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_J !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_K(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_K !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_O(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_O !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_L(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_L !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_M(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_M !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_N(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_N !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_P(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_P !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_Q(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_Q !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_R(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_R !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_S(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_S !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_T(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_T !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_U(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_U !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_V(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_V !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_W(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_W !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_X(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_X !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_Y(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_Y !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_Z(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_Z !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_COMMA(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_COMMA !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_DOT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_DOT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_DEL(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_DEL !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_ENTER(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_ENTER !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_LSHIFT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_LSHIFT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_LSYMBOL(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_LSYMBOL !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_SPACE(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_SPACE !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_HIDE(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_HIDE !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_RSYMBOL(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_RSYMBOL !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_RSHIFT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_RSHIFT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_LANG(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_LANG !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_DEL(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_DEL !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_ENTER(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_ENTER !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_HIDE(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_HIDE !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_1(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_1 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_2(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_2 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_3(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_3 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_4(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_4 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_5(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_5 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_6(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_6 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_7(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_7 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_8(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_8 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_9(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_9 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_PLUS(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_PLUS !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_0(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_0 !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_MINUS(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_MINUS !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_DOT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_DOT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_PERCENT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_PERCENT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_MULT(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_MULT !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_DIV(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_DIV !!!\n");
    return false;
}

static bool onButtonClick_BUTTON_NUMBER_SPACE(ZKButton *pButton) {
    LOGD(" ButtonClick BUTTON_NUMBER_SPACE !!!\n");
    return false;
}
