#pragma once

#include <vector>
#include <string>
#include "manager/LanguageManager.h"
#include "statusbarconfig.h"

typedef struct {
    const char* name;
    const char* description;
}S_STUDENT;
static S_STUDENT sStudents[] = {
    "student1","student_describe1",
    "student2","student_describe2",
    "student3","student_describe3",
    "student1","student_describe1",
    "student4","student_describe4",
    "student5","student_describe5",
    "student6","student_describe6",
    "student7","student_describe7",
    "student8","student_describe8",
    "student9","student_describe9",
    "student9","student_describe9",
    "student4","student_describe4",
    "student9","student_describe9",
    "student5","student_describe5",
    "student9","student_describe9",
    "student6","student_describe6",
    "student9","student_describe9",
};

typedef struct{
	const char* name;
	bool bOn;
}S_SWITCH_DATA;
static S_SWITCH_DATA sSwitches[]={
	"living_room", true,
	"living_room", false,
	"bedroom", true,
	"bedroom", false,
	"kitchen", true,
	"kitchen", false,
	"access_control", true,
	"bathroom", true,
};

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
}

static void onUI_quit() {
	ShowStatusBar(1, 0, 0);
}

static void onProtocolDataUpdate(const SProtocolData &data) {
    // 串口数据回调接口
}

static bool onUI_Timer(int id){
    //Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
    return true;
}


static bool ontesListActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上
    return false;
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    //LOGD(" ButtonClick sys_back !!!\n");
    return false;
}

static int getListItemCount_Listview1(const ZKListView *pListView) {
    //LOGD(" getListItemCount_ Listview1  !!!\n");
	return sizeof(sSwitches)/sizeof(S_SWITCH_DATA);
}

static void obtainListItemData_Listview1(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {

	ZKListView::ZKListSubItem* sub = NULL;
	sub = pListItem->findSubItemByID(ID_TESLIST_SubItemState);
	if (sub) {
      sub->setSelected(sSwitches[index].bOn);
	}
	pListItem->setTextTr(sSwitches[index].name);
}

static void onListItemClick_Listview1(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview1  !!!\n");
    sSwitches[index].bOn = !sSwitches[index].bOn;
	mListview1Ptr->refreshListView();
}

static int getListItemCount_Listview3(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview3 !\n");
    return sizeof(sStudents)/sizeof(S_STUDENT);
}

static void obtainListItemData_Listview3(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    ZKListView::ZKListSubItem* sub = NULL;
    sub = pListItem->findSubItemByID(ID_TESLIST_SubItemAvatar);
    if (sub) {
        char path[32] = {0};
        snprintf(path, sizeof(path), "testList/%d.png", index + 1);
        sub->setBackgroundPic(path);
    }
    sub = pListItem->findSubItemByID(ID_TESLIST_SubItemName);
    if (sub) {
        sub->setTextTr(sStudents[index].name);
    }
    sub = pListItem->findSubItemByID(ID_TESLIST_SubItemDescription);
    if (sub) {
        sub->setText(LANGUAGEMANAGER->getValue(sStudents[index].description));
    }
}

static void onListItemClick_Listview3(ZKListView *pListView, int index, int id) {
}
