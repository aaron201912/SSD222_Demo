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

static int g_curPageIdx = 0;
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
	EASYUICONTEXT->hideStatusBar();
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
static void onUI_quit() {

}


static void onProtocolDataUpdate(const SProtocolData &data) {
    // 串口数据回调接口
}

static bool onUI_Timer(int id){
    //Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
    return true;
}


static bool onmainActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上

	switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			//printf("down: x=%d, y=%d\n", ev.mX, ev.mY);

			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			//printf("move: x=%d, y=%d\n", ev.mX, ev.mY);

			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			//printf("up: x=%d, y=%d\n", ev.mX, ev.mY);
			break;
		default:
			break;
	}

    return false;
}
const char* IconTab[]={
		"testTextActivity",
		"testSliderActivity",
		"testButtonActivity",
		"inputtextActivity",
		"waveViewActivity",
		"testpointerActivity",
		"windowActivity",
		"tesListActivity",
		"qrcodeActivity",
		"animationActivity",
		"sliderwindowActivity",
		"uartActivity",
		"painterActivity",
};

static void onSlideItemClick_Slidewindow1(ZKSlideWindow *pSlideWindow, int index) {
	EASYUICONTEXT->openActivity(IconTab[index]);
}

static void onSlidePageChange_Slidewindow1(ZKSlideWindow *pSlideWindow, int page) {
	int totalPage = pSlideWindow->getPageSize();
	g_curPageIdx = pSlideWindow->getCurrentPage();
	//printf("Logic: param page is %d, total page is %d, cur page is %d\n", page, totalPage, g_curPageIdx);
	mListview_indicatorPtr->refreshListView();
}

static int getListItemCount_Listview_indicator(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview_indicator !\n");
    return 2;
}

static void obtainListItemData_Listview_indicator(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ Listview_indicator  !!!\n");
	if (index == g_curPageIdx)
		pListItem->setBackgroundPic("slider_/indicator_focus.png");
	else
		pListItem->setBackgroundPic("slider_/indicator.png");
}

static void onListItemClick_Listview_indicator(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview_indicator  !!!\n");
	int curPageIdx =  g_curPageIdx;
	//printf("click idx is %d, curPageIdx is %d\n", index, g_curPageIdx);

	while (curPageIdx < index)
	{
		mSlidewindow1Ptr->turnToNextPage();
		curPageIdx++;
	}

	while (curPageIdx > index)
	{
		mSlidewindow1Ptr->turnToPrevPage();
		curPageIdx--;
	}
}
