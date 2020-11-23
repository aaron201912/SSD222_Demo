/***********************************************
/gen auto by zuitools
***********************************************/
#include "mainActivity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
/*TAG:GlobalVariable全局变量*/
static ZKListView* mListview_indicatorPtr;
static ZKSlideWindow* mSlidewindow1Ptr;
static mainActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(mainActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/mainLogic.cc"

/***********/
typedef struct {
    int id;
    const char *pApp;
} SAppInfo;

/**
 *点击跳转window
 */
static SAppInfo sAppInfoTab[] = {
//  { ID_MAIN_TEXT, "TextViewActivity" },
};

/***************/
typedef bool (*ButtonCallback)(ZKButton *pButton);
/**
 * button onclick表
 */
typedef struct {
    int id;
    ButtonCallback callback;
}S_ButtonCallback;

/*TAG:ButtonCallbackTab按键映射表*/
static S_ButtonCallback sButtonCallbackTab[] = {
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
};


typedef int (*ListViewGetItemCountCallback)(const ZKListView *pListView);
typedef void (*ListViewobtainListItemDataCallback)(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index);
typedef void (*ListViewonItemClickCallback)(ZKListView *pListView, int index, int id);
typedef struct {
    int id;
    ListViewGetItemCountCallback getListItemCountCallback;
    ListViewobtainListItemDataCallback obtainListItemDataCallback;
    ListViewonItemClickCallback onItemClickCallback;
}S_ListViewFunctionsCallback;
/*TAG:ListViewFunctionsCallback*/
static S_ListViewFunctionsCallback SListViewFunctionsCallbackTab[] = {
    ID_MAIN_Listview_indicator, getListItemCount_Listview_indicator, obtainListItemData_Listview_indicator, onListItemClick_Listview_indicator,
};


typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
    ID_MAIN_Slidewindow1, onSlideItemClick_Slidewindow1,
};

typedef void (*SlideWindowPageChangeCallback)(ZKSlideWindow *pSlideWindow, int page);
typedef struct {
    int id;
    SlideWindowPageChangeCallback onSlidePageChangeCallback;
}S_SlideWindowPageChangeCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowPageChangeCallback SSlideWindowPageChangeCallbackTab[] = {
    ID_MAIN_Slidewindow1, onSlidePageChange_Slidewindow1,
};


typedef void (*EditTextInputCallback)(const std::string &text);
typedef struct {
    int id;
    EditTextInputCallback onEditTextChangedCallback;
}S_EditTextInputCallback;
/*TAG:EditTextInputCallback*/
static S_EditTextInputCallback SEditTextInputCallbackTab[] = {
};

typedef void (*VideoViewCallback)(ZKVideoView *pVideoView, int msg);
typedef struct {
    int id; //VideoView ID
    bool loop; // 是否是轮播类型
    int defaultvolume;//轮播类型时,默认视频音量
    VideoViewCallback onVideoViewCallback;
}S_VideoViewCallback;
/*TAG:VideoViewCallback*/
static S_VideoViewCallback SVideoViewCallbackTab[] = {
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


mainActivity::mainActivity() {
	//todo add init code here
	mVideoLoopIndex = 0;
	mVideoLoopErrorCount = 0;

	// animation support
	mNeedSwitchEffect = true;
}

mainActivity::~mainActivity() {
	//todo add init file here
    // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    onUI_quit();
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
}

const char* mainActivity::getAppName() const{
	return "main.ftu";
}

//TAG:onCreate
void mainActivity::onCreate() {
	Activity::onCreate();
    mSlidewindow1Ptr = (ZKSlideWindow*)findControlByID(ID_MAIN_Slidewindow1);if(mSlidewindow1Ptr!= NULL){mSlidewindow1Ptr->setSlideItemClickListener(this);}
    mListview_indicatorPtr = (ZKListView*)findControlByID(ID_MAIN_Listview_indicator);if(mListview_indicatorPtr!= NULL){mListview_indicatorPtr->setListAdapter(this);mListview_indicatorPtr->setItemClickListener(this);}
    mSlidewindow1Ptr = (ZKSlideWindow*)findControlByID(ID_MAIN_Slidewindow1);if(mSlidewindow1Ptr!= NULL){mSlidewindow1Ptr->setSlideItemClickListener(this);mSlidewindow1Ptr->setSlidePageChangeListener(this);}
	mActivityPtr = this;
	onUI_init();
    registerProtocolDataUpdateListener(onProtocolDataUpdate); 
    rigesterActivityTimer();
}

void mainActivity::onClick(ZKBase *pBase) {
	//TODO: add widget onClik code 
    int buttonTablen = sizeof(sButtonCallbackTab) / sizeof(S_ButtonCallback);
    for (int i = 0; i < buttonTablen; ++i) {
        if (sButtonCallbackTab[i].id == pBase->getID()) {
            if (sButtonCallbackTab[i].callback((ZKButton*)pBase)) {
            	return;
            }
            break;
        }
    }


    int len = sizeof(sAppInfoTab) / sizeof(sAppInfoTab[0]);
    for (int i = 0; i < len; ++i) {
        if (sAppInfoTab[i].id == pBase->getID()) {
            EASYUICONTEXT->openActivity(sAppInfoTab[i].pApp);
            return;
        }
    }

	Activity::onClick(pBase);
}

void mainActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
//	onUI_show();
}

void mainActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
//	onUI_hide();
}

void mainActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
//	onUI_intent(intentPtr);
}

bool mainActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void mainActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int mainActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void mainActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void mainActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);

    printf("click index:%d, id:%d\n", index, id);

    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void mainActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);

    for (int i = 0; i < tablen; ++i)
    {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID())
        {
        	if (index < (sizeof(IconTab) / sizeof(const char*)))
        	{
        		SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
        		break;
        	}
        	else
        	{
				//str suspend in
        		printf("suspend in\n");
            	if (!access("/sys/class/gpio/gpio7", F_OK))
        		{
            		system("echo 0 >/sys/class/gpio/gpio7/value");
        		}
            	else
            	{
            		system("echo 7 >/sys/class/gpio/export");
            	    system("echo out >/sys/class/gpio/gpio7/direction");
            	    system("echo 0 >/sys/class/gpio/gpio7/value");
            	}
            	if (!access("/sys/class/gpio/gpio8", F_OK))
        		{
            		system("echo 0 >/sys/class/gpio/gpio8/value");
        		}
            	else
            	{
        		system("echo 8 >/sys/class/gpio/export");
        		system("echo out >/sys/class/gpio/gpio8/direction");
        		system("echo 0 >/sys/class/gpio/gpio8/value");
            	}
            	system("rmmod ssw102b_wifi_sdio");
        		Enter_STR_SuspendMode();

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

        		//mi deinit
        		system("echo mem > /sys/power/state");
        		//usleep(2*1000*1000);
        		printf("resume back\n");

        		memset(&sendevt,0,sizeof(IPCEvent));
				sendevt.EventType = IPC_COMMAND;
				sendevt.Data = IPC_COMMAND_APP_RESUME;
				o.Send(sendevt);
				printf("UI process send %d to resume\n", IPC_COMMAND_APP_RESUME);

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

        		system("echo 1 >/sys/class/gpio/gpio8/value");
        		Enter_STR_ResumeMode();
        		usleep(30*1000);
        		system("insmod /config/wifi/ssw102b_wifi_sdio.ko");

        		system("echo 1 >/sys/class/gpio/gpio7/value");

        		break;
        	}
        }
    }
}

void mainActivity::onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) {
	int tablen = sizeof(SSlideWindowPageChangeCallbackTab) / sizeof(S_SlideWindowPageChangeCallback);
	for (int i = 0; i < tablen; ++i) {
		if (SSlideWindowPageChangeCallbackTab[i].id == pSlideWindow->getID()) {
			SSlideWindowPageChangeCallbackTab[i].onSlidePageChangeCallback(pSlideWindow, page);
			break;
		}
	}
}

bool mainActivity::onTouchEvent(const MotionEvent &ev) {
    return onmainActivityTouchEvent(ev);
}

void mainActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void mainActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void mainActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SVideoViewCallbackTab[i].id == pVideoView->getID()) {
        	if (SVideoViewCallbackTab[i].loop) {
                //循环播放
        		videoLoopPlayback(pVideoView, msg, i);
        	} else if (SVideoViewCallbackTab[i].onVideoViewCallback != NULL){
        	    SVideoViewCallbackTab[i].onVideoViewCallback(pVideoView, msg);
        	}
            break;
        }
    }
}

void mainActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, int callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
		pVideoView->setVolume(SVideoViewCallbackTab[callbackTabIndex].defaultvolume / 10.0);
		mVideoLoopErrorCount = 0;
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		/**错误处理 */
		++mVideoLoopErrorCount;
		if (mVideoLoopErrorCount > 100) {
			LOGD("video loop error counts > 100, quit loop playback !");
            break;
		} //不用break, 继续尝试播放下一个
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED\n");
        std::vector<std::string> videolist;
        std::string fileName(getAppName());
        if (fileName.size() < 4) {
             LOGD("getAppName size < 4, ignore!");
             break;
        }
        fileName = fileName.substr(0, fileName.length() - 4) + "_video_list.txt";
        fileName = "/mnt/extsd/" + fileName;
        if (!parseVideoFileList(fileName.c_str(), videolist)) {
            LOGD("parseVideoFileList failed !");
		    break;
        }
		if (pVideoView && !videolist.empty()) {
			mVideoLoopIndex = (mVideoLoopIndex + 1) % videolist.size();
			pVideoView->play(videolist[mVideoLoopIndex].c_str());
		}
		break;
	}
}

void mainActivity::startVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		//循环播放
    		videoLoopPlayback(videoView, ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED, i);
    		return;
    	}
    }
}

void mainActivity::stopVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		if (videoView->isPlaying()) {
    		    videoView->stop();
    		}
    		return;
    	}
    }
}

bool mainActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
	mediaFileList.clear();
	if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
		return false;
	}

	ifstream is(pFileListPath, ios_base::in);
	if (!is.is_open()) {
		LOGD("cann't open file %s \n", pFileListPath);
		return false;
	}
	char tmp[1024] = {0};
	while (is.getline(tmp, sizeof(tmp))) {
		string str = tmp;
		removeCharFromString(str, '\"');
		removeCharFromString(str, '\r');
		removeCharFromString(str, '\n');
		if (str.size() > 1) {
     		mediaFileList.push_back(str.c_str());
		}
	}
	LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files\n", __FUNCTION__,
			__LINE__, pFileListPath, mediaFileList.size());
	for (size_t i = 0; i < mediaFileList.size(); i++) {
		LOGD("file[%d]:[%s]\n", i, mediaFileList[i].c_str());
	}
	is.close();

	return true;
}

int mainActivity::removeCharFromString(string& nString, char c) {
    string::size_type   pos;
    while(1) {
        pos = nString.find(c);
        if(pos != string::npos) {
            nString.erase(pos, 1);
        } else {
            break;
        }
    }
    return (int)nString.size();
}

void mainActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void mainActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void mainActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}