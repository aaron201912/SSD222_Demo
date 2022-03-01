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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"
#include "mi_gfx.h"
#include "panelconfig.h"
#include "statusbarconfig.h"
#include "hotplugdetect.h"

#define TRIGLE_BY_GPIO	0
#define BACKLIGHT_GPIO	62	// 7
#define POWERCTRL_GPIO	63	// 8

static int g_curPageIdx = 0;
/**
 * 注册定时器
 * 在此数组中添加即可
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

typedef enum
{
  IPC_KEYEVENT = 0,
  IPC_COMMAND,
  IPC_LOGCMD,
  IPC_EVENT_MAX,
} IPC_EVENT_TYPE;

typedef enum
{
  IPC_COMMAND_EXIT = 0,
  IPC_COMMAND_SUSPEND,
  IPC_COMMAND_RESUME,
  IPC_COMMAND_RELOAD,
  IPC_COMMAND_BROWN_GETFOCUS,
  IPC_COMMAND_BROWN_LOSEFOCUS,
  IPC_COMMAND_APP_START_DONE,
  IPC_COMMAND_APP_STOP_DONE,
  IPC_COMMAND_SETUP_WATERMARK,
  IPC_COMMAND_APP_START,
  IPC_COMMAND_APP_STOP,
  IPC_COMMAND_UI_EXIT,
  IPC_COMMAND_APP_SUSPEND,
  IPC_COMMAND_APP_SUSPEND_DONE,
  IPC_COMMAND_APP_RESUME,
  IPC_COMMAND_APP_RESUME_DONE,
  IPC_COMMAND_MAX,
} IPC_COMMAND_TYPE;

typedef struct {
  uint32_t EventType;
  uint32_t Data;
  char StrData[256];
} IPCEvent;

#define SSD_IPC "/tmp/ssd_apm_input"
#define UI_IPC	"/tmp/zkgui_msg_input"

class IPCNameFifo {
public:
  IPCNameFifo(const char* file): m_file(file) {
    unlink(m_file.c_str());
    printf("mkfifo: %s\n",m_file.c_str());
    m_valid = !mkfifo(m_file.c_str(), 0777);
  }

  ~IPCNameFifo() {
    unlink(m_file.c_str());
  }

  inline const std::string& Path() { return m_file; }
  inline bool IsValid() { return m_valid; }

private:
  bool m_valid;
  std::string m_file;
};

class IPCInput {
public:
  IPCInput(const std::string& file):m_fd(-1),m_file(file),m_fifo(file.c_str()){
  printf("construct ipcinput\n");}

  virtual ~IPCInput() {
    Term();
  }
  bool Init() {
    if (!m_fifo.IsValid()){
        printf("%s non-existent!!!! \n",m_fifo.Path().c_str());
        return false;
    }
    if (m_fd < 0) {
      m_fd = open(m_file.c_str(), O_RDWR | O_NONBLOCK);
    }
    return m_fd >= 0;
  }

  int Read(IPCEvent& evt) {
    if (m_fd >= 0) {
      return read(m_fd, &evt, sizeof(IPCEvent));
    }
    return 0;
  }

  void Term() {
    if (m_fd >= 0) {
      close(m_fd);
      m_fd = -1;
    }
  }

private:
  int m_fd;
  std::string m_file;
  IPCNameFifo m_fifo;
};

class IPCOutput {
public:
  IPCOutput(const std::string& file):m_fd(-1), m_file(file) {
  }

  virtual ~IPCOutput() {
    Term();
  }

  bool Init() {
    if (m_fd < 0) {
      m_fd = open(m_file.c_str(), O_WRONLY | O_NONBLOCK);
    }
    return m_fd >= 0;
  }

  void Term() {
    if (m_fd >= 0) {
      close(m_fd);
      m_fd = -1;
    }
  }

  void Send(const IPCEvent& evt) {
    if (m_fd >= 0) {
      write(m_fd, &evt, sizeof(IPCEvent));
    }
  }

private:
  int m_fd;
  std::string m_file;
};

void setOutputGpio(int gpio, int value)
{
	char gpioDir[64] = {0};
	char gpioExport[64] = {0};
	char gpioDirection[64] = {0};
	char gpioValue[64] = {0};

	sprintf(gpioDir, "/sys/class/gpio/gpio%d", gpio);
	sprintf(gpioExport, "echo %d > /sys/class/gpio/export", gpio);
	sprintf(gpioDirection, "echo out > /sys/class/gpio/gpio%d/direction", gpio);
	sprintf(gpioValue, "echo %d >/sys/class/gpio/gpio%d/value", value, gpio);

	if (!access(gpioDir, F_OK))
	{
		system(gpioValue);
	}
	else
	{
		system(gpioExport);
		system(gpioDirection);
		system(gpioValue);
	}
}

int checkShellResult(char const* cmd, char const*key)
{
    if (cmd == NULL)
    {
        printf("popen cmd is NULL\n");
        return -1;
    }

    char tmp[1024];
    FILE* fp = popen(cmd, "r");
    if (NULL == fp)
    {
        printf("exec cmd: %s failed, err: %s\n", cmd, strerror(errno));
        return 1;
    }
    while (fgets(tmp, 1024, fp) != NULL)
    {
        if (tmp[strlen(tmp) - 1] == '\n')
        {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }

		if (strstr(tmp, key))
		{
			pclose(fp);
			return 0;
		}
    }

    printf("exec cmd: %s ,not find %s item\n", cmd, key);

    pclose(fp);
    return -1;
}

static pthread_t g_getIpThread = 0;

void *GetIpProc(void* pData)
{
	char *cmd = "cat /proc/net/wireless | grep wlan0";
	char *key = "wlan0";
	int tryCnt = 40;

	printf("try to get ip...\n");

	while (tryCnt--)
	{
		if (!checkShellResult(cmd, key))
		{
			system("udhcpc -i wlan0 -s /etc/init.d/udhcpc.script -a -q -b -t 20 -T 1 &");
			break;
		}

		usleep(500*1000);
	}

	return NULL;
}


static void Exit_UI_Process()
{
	if (g_getIpThread)
	{
		pthread_join(g_getIpThread, NULL);
		g_getIpThread = NULL;
	}

	IPCOutput o(SSD_IPC);
	if(!o.Init())
	{
		printf("main ipc init fail!!!\n");
		o.Term();
	}
	IPCEvent sendevt;
	memset(&sendevt,0,sizeof(IPCEvent));
	sendevt.EventType = IPC_COMMAND;
	sendevt.Data = IPC_COMMAND_UI_EXIT;
	o.Send(sendevt);
	printf("UI process send %d to exit\n", IPC_COMMAND_UI_EXIT);
	SSTAR_DeinitHotPlugDetect();
	MI_DISP_DeInitDev();
	MI_GFX_DeInitDev();
	exit(0);
}

static void Enter_STR_SuspendMode()
{
#if TRIGLE_BY_GPIO
	setOutputGpio(BACKLIGHT_GPIO, 0);
	setOutputGpio(POWERCTRL_GPIO, 0);
#else
	MI_PANEL_InitParam_t stInitParam;
	MI_PANEL_BackLightConfig_t stBackLightCfg;

	memset(&stInitParam, 0, sizeof(MI_PANEL_InitParam_t));
	memset(&stBackLightCfg, 0, sizeof(MI_PANEL_BackLightConfig_t));

	stInitParam.eIntfType = E_MI_PNL_INTF_TTL;
	MI_PANEL_InitDev(&stInitParam);
	MI_PANEL_GetBackLight(stInitParam.eIntfType, &stBackLightCfg);
	printf("Get BL cfg: bEn=%d, u8PwmNum=%d, u32Duty=%d, u32Period=%d\n",
			(int)stBackLightCfg.bEn, (int)stBackLightCfg.u8PwmNum, stBackLightCfg.u32Duty,
			stBackLightCfg.u32Period);
	stBackLightCfg.u32Duty = 0;
	MI_PANEL_SetBackLight(stInitParam.eIntfType, &stBackLightCfg);
	MI_PANEL_DeInitDev();

	MI_DISP_DeInitDev();
#endif

    MI_GFX_DeInitDev();
    printf("gfx disable\n");

    IPCOutput o(SSD_IPC);
	if(!o.Init())
	{
		printf("main ipc init fail!!!\n");
		o.Term();
	}
	IPCEvent sendevt;
	memset(&sendevt,0,sizeof(IPCEvent));
	sendevt.EventType = IPC_COMMAND;
	sendevt.Data = IPC_COMMAND_APP_SUSPEND;
	o.Send(sendevt);
	printf("UI process send %d to suspend\n", IPC_COMMAND_APP_SUSPEND);

	IPCEvent getevt;
	IPCInput uiInput(UI_IPC);
	if(!uiInput.Init())
	{
		printf("ui ipc init fail\n");
		return;
	}

	memset(&getevt,0,sizeof(IPCEvent));
	while (1)
	{
		if(uiInput.Read(getevt) > 0)
		{
			if (getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_SUSPEND_DONE)
			{
				printf("recv app suspend done msg %d\n", IPC_COMMAND_APP_SUSPEND_DONE);
				break;
			}
		}
	}

	if (g_getIpThread)
	{
		pthread_join(g_getIpThread, NULL);
		g_getIpThread = NULL;
	}

	printf("begin to rmmod wifi ko\n");
	system("rmmod ssw102b_wifi_sdio");
	printf("rmmod wifi ko success\n");
}

static void Enter_STR_ResumeMode()
{
#if TRIGLE_BY_GPIO
	setOutputGpio(POWERCTRL_GPIO, 1);
#endif

	IPCOutput o(SSD_IPC);
	if(!o.Init())
	{
		printf("main ipc init fail!!!\n");
		o.Term();
	}

	IPCEvent sendevt;
	memset(&sendevt,0,sizeof(IPCEvent));
	sendevt.EventType = IPC_COMMAND;
	sendevt.Data = IPC_COMMAND_APP_RESUME;
	o.Send(sendevt);
	printf("UI process send %d to resume\n", IPC_COMMAND_APP_RESUME);

	IPCEvent getevt;
	IPCInput uiInput(UI_IPC);
	if(!uiInput.Init())
	{
		printf("ui ipc init fail\n");
		return;
	}

	memset(&getevt,0,sizeof(IPCEvent));
	while (1)
	{
		if(uiInput.Read(getevt) > 0)
		{
			if (getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_RESUME_DONE)
			{
				printf("recv app resume done msg %d\n", IPC_COMMAND_APP_RESUME_DONE);
				break;
			}
		}
	}

	MI_GFX_Open();
	MI_DISP_EnableInputPort(0, 0);

//	usleep(30*1000);
#if TRIGLE_BY_GPIO
	setOutputGpio(BACKLIGHT_GPIO, 1);
#endif

	printf("begin to insmod wifi ko\n");
	system("insmod /config/wifi/ssw102b_wifi_sdio.ko");
	printf("insmod wifi ko success\n");

	// udhcpc
	if (SSTAR_GetWifiLastConnStatus())
	{
		printf("start thread to get ip\n");
		pthread_create(&g_getIpThread, NULL, GetIpProc, NULL);
	}
}

static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1->setText("123");
	MI_GFX_Open();
	ShowStatusBar(1, 0, 0);

	printf("begin to clear disp inputport buf\n");
	MI_DISP_EnableInputPort(0, 0);
	MI_DISP_ClearInputPortBuffer(0, 0, TRUE);
	printf("clear disp inputport buf done\n");
}

static void onUI_quit() {
}

static void onUI_show() {
	ShowStatusBar(1, 0, 0);
	printf("%s: enter show\n", __FILE__);
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

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
	"tesListActivity",
	"qrcodeActivity",
	"animationActivity",
	"waveViewActivity",
//	"testmoduleActivity",
	"voicedetectActivity",
	"networkSettingActivity",
//	"airportActivity",
	"playPcmFileActivity",
//	"facedetectActivity",
	"dualsensorActivity",
//	"usbCameraActivity",
//	"scannerActivity",
	"localsettingActivity"
};

static void onSlideItemClick_Slidewindow1(ZKSlideWindow *pSlideWindow, int index) {
	ShowStatusBar(0, 1 ,1);
	printf("select idx is %d\n", index);
	EASYUICONTEXT->openActivity(IconTab[index]);
}

static void onSlidePageChange_Slidewindow1(ZKSlideWindow *pSlideWindow, int page) {
	//int totalPage = pSlideWindow->getPageSize();
	g_curPageIdx = pSlideWindow->getCurrentPage();
	//printf("Logic: param page is %d, total page is %d, cur page is %d\n", page, totalPage, g_curPageIdx);
	mListview_indicatorPtr->refreshListView();
}

static int getListItemCount_Listview_indicator(const ZKListView *pListView) {
    //LOGD("getListItemCount_Listview_indicator !\n");
	int totalPage = mSlidewindow1Ptr->getPageSize();
    return totalPage;
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
