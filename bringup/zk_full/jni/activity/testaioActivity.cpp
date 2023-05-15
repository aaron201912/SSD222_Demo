/***********************************************
/gen auto by zuitools
***********************************************/
#include "testaioActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKTextView* mTextView_curGainPtr;
static ZKTextView* mTextView_maxGainPtr;
static ZKTextView* mTextView_minGainPtr;
static ZKSeekBar* mSeekBar_micGainPtr;
static ZKTextView* mTextView_micGaunPtr;
static ZKRadioGroup* mRadioGroup_mictypePtr;
static ZKCheckBox* mCheckbox_playbgsoundPtr;
static ZKTextView* mTextView_mictypePtr;
static ZKTextView* mTextview_headphonePtr;
static ZKTextView* mTextview_speakerPtr;
static ZKTextView* mTextview_recordPtr;
static ZKTextView* mTextview_sampleratePtr;
static ZKButton* mButton_headphonePtr;
static ZKButton* mButton_playstereoPtr;
static ZKButton* mButton_playrecordPtr;
static ZKListView* mListview_recordfilePtr;
static ZKButton* mButton_recordfilePtr;
static ZKButton* mButton_recordPtr;
static ZKListView* mListview_sampleratePtr;
static ZKButton* mButton_sampleratePtr;
static ZKButton* msys_backPtr;
static testaioActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(testaioActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/testaioLogic.cc"

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
    ID_TESTAIO_Button_headphone, onButtonClick_Button_headphone,
    ID_TESTAIO_Button_playstereo, onButtonClick_Button_playstereo,
    ID_TESTAIO_Button_playrecord, onButtonClick_Button_playrecord,
    ID_TESTAIO_Button_recordfile, onButtonClick_Button_recordfile,
    ID_TESTAIO_Button_record, onButtonClick_Button_record,
    ID_TESTAIO_Button_samplerate, onButtonClick_Button_samplerate,
    ID_TESTAIO_sys_back, onButtonClick_sys_back,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_TESTAIO_SeekBar_micGain, onProgressChanged_SeekBar_micGain,
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
    ID_TESTAIO_Listview_recordfile, getListItemCount_Listview_recordfile, obtainListItemData_Listview_recordfile, onListItemClick_Listview_recordfile,
    ID_TESTAIO_Listview_samplerate, getListItemCount_Listview_samplerate, obtainListItemData_Listview_samplerate, onListItemClick_Listview_samplerate,
};


typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
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


typedef void (*CheckboxCallback)(ZKCheckBox*, bool);
typedef struct {
  int id;
  CheckboxCallback onCheckedChanged;
}S_CheckboxCallback;
/*TAG:CheckboxCallbackTab*/
static S_CheckboxCallback SCheckboxCallbackTab[] = {
    ID_TESTAIO_Checkbox_playbgsound, onCheckedChanged_Checkbox_playbgsound,
};

typedef void (*RadioGroupCallback)(ZKRadioGroup*, int);
typedef struct {
  int id;
  RadioGroupCallback onCheckedChanged;
}S_RadioGroupCallback;
/*TAG:RadioGroupCallbackTab*/
static S_RadioGroupCallback SRadioGroupCallbackTab[] = {
    ID_TESTAIO_RadioGroup_mictype, onCheckedChanged_RadioGroup_mictype,
};

testaioActivity::testaioActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

testaioActivity::~testaioActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    onUI_quit();
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    mTextView_curGainPtr = NULL;
    mTextView_maxGainPtr = NULL;
    mTextView_minGainPtr = NULL;
    mSeekBar_micGainPtr = NULL;
    mTextView_micGaunPtr = NULL;
    mCheckbox_playbgsoundPtr = NULL;
    mRadioGroup_mictypePtr = NULL;
    mTextView_mictypePtr = NULL;
    mButton_headphonePtr = NULL;
    mTextview_headphonePtr = NULL;
    mButton_playstereoPtr = NULL;
    mTextview_speakerPtr = NULL;
    mButton_playrecordPtr = NULL;
    mListview_recordfilePtr = NULL;
    mButton_recordfilePtr = NULL;
    mTextview_recordPtr = NULL;
    mButton_recordPtr = NULL;
    mListview_sampleratePtr = NULL;
    mButton_sampleratePtr = NULL;
    mTextview_sampleratePtr = NULL;
    msys_backPtr = NULL;
}

const char* testaioActivity::getAppName() const{
	return "testaio.ftu";
}

//TAG:onCreate
void testaioActivity::onCreate() {
	Activity::onCreate();
    mTextView_curGainPtr = (ZKTextView*)findControlByID(ID_TESTAIO_TextView_curGain);
    mTextView_maxGainPtr = (ZKTextView*)findControlByID(ID_TESTAIO_TextView_maxGain);
    mTextView_minGainPtr = (ZKTextView*)findControlByID(ID_TESTAIO_TextView_minGain);
    mSeekBar_micGainPtr = (ZKSeekBar*)findControlByID(ID_TESTAIO_SeekBar_micGain);if(mSeekBar_micGainPtr!= NULL){mSeekBar_micGainPtr->setSeekBarChangeListener(this);}
    mTextView_micGaunPtr = (ZKTextView*)findControlByID(ID_TESTAIO_TextView_micGaun);
    mRadioGroup_mictypePtr = (ZKRadioGroup*)findControlByID(ID_TESTAIO_RadioGroup_mictype);if(mRadioGroup_mictypePtr!= NULL){mRadioGroup_mictypePtr->setCheckedChangeListener(this);}
    mCheckbox_playbgsoundPtr = (ZKCheckBox*)findControlByID(ID_TESTAIO_Checkbox_playbgsound);if(mCheckbox_playbgsoundPtr!= NULL){mCheckbox_playbgsoundPtr->setCheckedChangeListener(this);}
    mTextView_mictypePtr = (ZKTextView*)findControlByID(ID_TESTAIO_TextView_mictype);
    mTextview_headphonePtr = (ZKTextView*)findControlByID(ID_TESTAIO_Textview_headphone);
    mTextview_speakerPtr = (ZKTextView*)findControlByID(ID_TESTAIO_Textview_speaker);
    mTextview_recordPtr = (ZKTextView*)findControlByID(ID_TESTAIO_Textview_record);
    mTextview_sampleratePtr = (ZKTextView*)findControlByID(ID_TESTAIO_Textview_samplerate);
    mButton_headphonePtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_headphone);
    mButton_playstereoPtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_playstereo);
    mButton_playrecordPtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_playrecord);
    mListview_recordfilePtr = (ZKListView*)findControlByID(ID_TESTAIO_Listview_recordfile);if(mListview_recordfilePtr!= NULL){mListview_recordfilePtr->setListAdapter(this);mListview_recordfilePtr->setItemClickListener(this);}
    mButton_recordfilePtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_recordfile);
    mButton_recordPtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_record);
    mListview_sampleratePtr = (ZKListView*)findControlByID(ID_TESTAIO_Listview_samplerate);if(mListview_sampleratePtr!= NULL){mListview_sampleratePtr->setListAdapter(this);mListview_sampleratePtr->setItemClickListener(this);}
    mButton_sampleratePtr = (ZKButton*)findControlByID(ID_TESTAIO_Button_samplerate);
    msys_backPtr = (ZKButton*)findControlByID(ID_TESTAIO_sys_back);
	mActivityPtr = this;
	onUI_init();
    registerProtocolDataUpdateListener(onProtocolDataUpdate); 
    rigesterActivityTimer();
}

void testaioActivity::onClick(ZKBase *pBase) {
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

void testaioActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void testaioActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void testaioActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool testaioActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void testaioActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int testaioActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void testaioActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void testaioActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void testaioActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool testaioActivity::onTouchEvent(const MotionEvent &ev) {
    return ontestaioActivityTouchEvent(ev);
}

void testaioActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void testaioActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void testaioActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
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

void testaioActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
    if (callbackTabIndex >= (sizeof(SVideoViewCallbackTab)/sizeof(S_VideoViewCallback))) {
      break;
    }
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

void testaioActivity::startVideoLoopPlayback() {
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

void testaioActivity::stopVideoLoopPlayback() {
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

bool testaioActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
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

int testaioActivity::removeCharFromString(string& nString, char c) {
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

void testaioActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void testaioActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void testaioActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}
void testaioActivity::onCheckedChanged(ZKCheckBox* pCheckBox, bool isChecked) {
    int tablen = sizeof(SCheckboxCallbackTab) / sizeof(S_CheckboxCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SCheckboxCallbackTab[i].id == pCheckBox->getID()) {
        	SCheckboxCallbackTab[i].onCheckedChanged(pCheckBox, isChecked);
            break;
        }
    }
}
void testaioActivity::onCheckedChanged(ZKRadioGroup* pRadioGroup, int checkedID) {
    int tablen = sizeof(SRadioGroupCallbackTab) / sizeof(S_RadioGroupCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SRadioGroupCallbackTab[i].id == pRadioGroup->getID()) {
        	SRadioGroupCallbackTab[i].onCheckedChanged(pRadioGroup, checkedID);
            break;
        }
    }
}