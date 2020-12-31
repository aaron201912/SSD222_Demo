/***********************************************
/gen auto by zuitools
***********************************************/
#include "zkimeActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKSlideText* mSLIDETEXT_HANZIPtr;
static ZKButton* mBUTTON_NUMBER_SPACEPtr;
static ZKButton* mBUTTON_NUMBER_DIVPtr;
static ZKButton* mBUTTON_NUMBER_MULTPtr;
static ZKButton* mBUTTON_NUMBER_PERCENTPtr;
static ZKButton* mBUTTON_NUMBER_DOTPtr;
static ZKButton* mBUTTON_NUMBER_MINUSPtr;
static ZKButton* mBUTTON_NUMBER_0Ptr;
static ZKButton* mBUTTON_NUMBER_PLUSPtr;
static ZKButton* mBUTTON_NUMBER_9Ptr;
static ZKButton* mBUTTON_NUMBER_8Ptr;
static ZKButton* mBUTTON_NUMBER_7Ptr;
static ZKButton* mBUTTON_NUMBER_6Ptr;
static ZKButton* mBUTTON_NUMBER_5Ptr;
static ZKButton* mBUTTON_NUMBER_4Ptr;
static ZKButton* mBUTTON_NUMBER_3Ptr;
static ZKButton* mBUTTON_NUMBER_2Ptr;
static ZKButton* mBUTTON_NUMBER_1Ptr;
static ZKButton* mBUTTON_NUMBER_HIDEPtr;
static ZKButton* mBUTTON_NUMBER_ENTERPtr;
static ZKButton* mBUTTON_NUMBER_DELPtr;
static ZKWindow* mWINDOW_NUMBERPtr;
static ZKTextView* mTEXTVIEW_CONTENTPtr;
static ZKTextView* mTEXTVIEW_PINYINPtr;
static ZKButton* mBUTTON_LANGPtr;
static ZKButton* mBUTTON_RSHIFTPtr;
static ZKButton* mBUTTON_RSYMBOLPtr;
static ZKButton* mBUTTON_HIDEPtr;
static ZKButton* mBUTTON_SPACEPtr;
static ZKButton* mBUTTON_LSYMBOLPtr;
static ZKButton* mBUTTON_LSHIFTPtr;
static ZKButton* mBUTTON_ENTERPtr;
static ZKButton* mBUTTON_DELPtr;
static ZKButton* mBUTTON_DOTPtr;
static ZKButton* mBUTTON_COMMAPtr;
static ZKButton* mBUTTON_ZPtr;
static ZKButton* mBUTTON_YPtr;
static ZKButton* mBUTTON_XPtr;
static ZKButton* mBUTTON_WPtr;
static ZKButton* mBUTTON_VPtr;
static ZKButton* mBUTTON_UPtr;
static ZKButton* mBUTTON_TPtr;
static ZKButton* mBUTTON_SPtr;
static ZKButton* mBUTTON_RPtr;
static ZKButton* mBUTTON_QPtr;
static ZKButton* mBUTTON_PPtr;
static ZKButton* mBUTTON_NPtr;
static ZKButton* mBUTTON_MPtr;
static ZKButton* mBUTTON_LPtr;
static ZKButton* mBUTTON_OPtr;
static ZKButton* mBUTTON_KPtr;
static ZKButton* mBUTTON_JPtr;
static ZKButton* mBUTTON_IPtr;
static ZKButton* mBUTTON_HPtr;
static ZKButton* mBUTTON_GPtr;
static ZKButton* mBUTTON_FPtr;
static ZKButton* mBUTTON_EPtr;
static ZKButton* mBUTTON_DPtr;
static ZKButton* mBUTTON_CPtr;
static ZKButton* mBUTTON_BPtr;
static ZKButton* mBUTTON_APtr;
static ZKWindow* mWINDOW_ALLPtr;
static zkimeActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(zkimeActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/zkimeLogic.cc"

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
    ID_ZKIME_BUTTON_NUMBER_SPACE, onButtonClick_BUTTON_NUMBER_SPACE,
    ID_ZKIME_BUTTON_NUMBER_DIV, onButtonClick_BUTTON_NUMBER_DIV,
    ID_ZKIME_BUTTON_NUMBER_MULT, onButtonClick_BUTTON_NUMBER_MULT,
    ID_ZKIME_BUTTON_NUMBER_PERCENT, onButtonClick_BUTTON_NUMBER_PERCENT,
    ID_ZKIME_BUTTON_NUMBER_DOT, onButtonClick_BUTTON_NUMBER_DOT,
    ID_ZKIME_BUTTON_NUMBER_MINUS, onButtonClick_BUTTON_NUMBER_MINUS,
    ID_ZKIME_BUTTON_NUMBER_0, onButtonClick_BUTTON_NUMBER_0,
    ID_ZKIME_BUTTON_NUMBER_PLUS, onButtonClick_BUTTON_NUMBER_PLUS,
    ID_ZKIME_BUTTON_NUMBER_9, onButtonClick_BUTTON_NUMBER_9,
    ID_ZKIME_BUTTON_NUMBER_8, onButtonClick_BUTTON_NUMBER_8,
    ID_ZKIME_BUTTON_NUMBER_7, onButtonClick_BUTTON_NUMBER_7,
    ID_ZKIME_BUTTON_NUMBER_6, onButtonClick_BUTTON_NUMBER_6,
    ID_ZKIME_BUTTON_NUMBER_5, onButtonClick_BUTTON_NUMBER_5,
    ID_ZKIME_BUTTON_NUMBER_4, onButtonClick_BUTTON_NUMBER_4,
    ID_ZKIME_BUTTON_NUMBER_3, onButtonClick_BUTTON_NUMBER_3,
    ID_ZKIME_BUTTON_NUMBER_2, onButtonClick_BUTTON_NUMBER_2,
    ID_ZKIME_BUTTON_NUMBER_1, onButtonClick_BUTTON_NUMBER_1,
    ID_ZKIME_BUTTON_NUMBER_HIDE, onButtonClick_BUTTON_NUMBER_HIDE,
    ID_ZKIME_BUTTON_NUMBER_ENTER, onButtonClick_BUTTON_NUMBER_ENTER,
    ID_ZKIME_BUTTON_NUMBER_DEL, onButtonClick_BUTTON_NUMBER_DEL,
    ID_ZKIME_BUTTON_LANG, onButtonClick_BUTTON_LANG,
    ID_ZKIME_BUTTON_RSHIFT, onButtonClick_BUTTON_RSHIFT,
    ID_ZKIME_BUTTON_RSYMBOL, onButtonClick_BUTTON_RSYMBOL,
    ID_ZKIME_BUTTON_HIDE, onButtonClick_BUTTON_HIDE,
    ID_ZKIME_BUTTON_SPACE, onButtonClick_BUTTON_SPACE,
    ID_ZKIME_BUTTON_LSYMBOL, onButtonClick_BUTTON_LSYMBOL,
    ID_ZKIME_BUTTON_LSHIFT, onButtonClick_BUTTON_LSHIFT,
    ID_ZKIME_BUTTON_ENTER, onButtonClick_BUTTON_ENTER,
    ID_ZKIME_BUTTON_DEL, onButtonClick_BUTTON_DEL,
    ID_ZKIME_BUTTON_DOT, onButtonClick_BUTTON_DOT,
    ID_ZKIME_BUTTON_COMMA, onButtonClick_BUTTON_COMMA,
    ID_ZKIME_BUTTON_Z, onButtonClick_BUTTON_Z,
    ID_ZKIME_BUTTON_Y, onButtonClick_BUTTON_Y,
    ID_ZKIME_BUTTON_X, onButtonClick_BUTTON_X,
    ID_ZKIME_BUTTON_W, onButtonClick_BUTTON_W,
    ID_ZKIME_BUTTON_V, onButtonClick_BUTTON_V,
    ID_ZKIME_BUTTON_U, onButtonClick_BUTTON_U,
    ID_ZKIME_BUTTON_T, onButtonClick_BUTTON_T,
    ID_ZKIME_BUTTON_S, onButtonClick_BUTTON_S,
    ID_ZKIME_BUTTON_R, onButtonClick_BUTTON_R,
    ID_ZKIME_BUTTON_Q, onButtonClick_BUTTON_Q,
    ID_ZKIME_BUTTON_P, onButtonClick_BUTTON_P,
    ID_ZKIME_BUTTON_N, onButtonClick_BUTTON_N,
    ID_ZKIME_BUTTON_M, onButtonClick_BUTTON_M,
    ID_ZKIME_BUTTON_L, onButtonClick_BUTTON_L,
    ID_ZKIME_BUTTON_O, onButtonClick_BUTTON_O,
    ID_ZKIME_BUTTON_K, onButtonClick_BUTTON_K,
    ID_ZKIME_BUTTON_J, onButtonClick_BUTTON_J,
    ID_ZKIME_BUTTON_I, onButtonClick_BUTTON_I,
    ID_ZKIME_BUTTON_H, onButtonClick_BUTTON_H,
    ID_ZKIME_BUTTON_G, onButtonClick_BUTTON_G,
    ID_ZKIME_BUTTON_F, onButtonClick_BUTTON_F,
    ID_ZKIME_BUTTON_E, onButtonClick_BUTTON_E,
    ID_ZKIME_BUTTON_D, onButtonClick_BUTTON_D,
    ID_ZKIME_BUTTON_C, onButtonClick_BUTTON_C,
    ID_ZKIME_BUTTON_B, onButtonClick_BUTTON_B,
    ID_ZKIME_BUTTON_A, onButtonClick_BUTTON_A,
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


zkimeActivity::zkimeActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

zkimeActivity::~zkimeActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    onUI_quit();
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
}

const char* zkimeActivity::getAppName() const{
	return "zkime.ftu";
}

//TAG:onCreate
void zkimeActivity::onCreate() {
	Activity::onCreate();
    mSLIDETEXT_HANZIPtr = (ZKSlideText*)findControlByID(ID_ZKIME_SLIDETEXT_HANZI);
    mBUTTON_NUMBER_SPACEPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_SPACE);
    mBUTTON_NUMBER_DIVPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_DIV);
    mBUTTON_NUMBER_MULTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_MULT);
    mBUTTON_NUMBER_PERCENTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_PERCENT);
    mBUTTON_NUMBER_DOTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_DOT);
    mBUTTON_NUMBER_MINUSPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_MINUS);
    mBUTTON_NUMBER_0Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_0);
    mBUTTON_NUMBER_PLUSPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_PLUS);
    mBUTTON_NUMBER_9Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_9);
    mBUTTON_NUMBER_8Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_8);
    mBUTTON_NUMBER_7Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_7);
    mBUTTON_NUMBER_6Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_6);
    mBUTTON_NUMBER_5Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_5);
    mBUTTON_NUMBER_4Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_4);
    mBUTTON_NUMBER_3Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_3);
    mBUTTON_NUMBER_2Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_2);
    mBUTTON_NUMBER_1Ptr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_1);
    mBUTTON_NUMBER_HIDEPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_HIDE);
    mBUTTON_NUMBER_ENTERPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_ENTER);
    mBUTTON_NUMBER_DELPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_NUMBER_DEL);
    mWINDOW_NUMBERPtr = (ZKWindow*)findControlByID(ID_ZKIME_WINDOW_NUMBER);
    mTEXTVIEW_CONTENTPtr = (ZKTextView*)findControlByID(ID_ZKIME_TEXTVIEW_CONTENT);
    mTEXTVIEW_PINYINPtr = (ZKTextView*)findControlByID(ID_ZKIME_TEXTVIEW_PINYIN);
    mBUTTON_LANGPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_LANG);
    mBUTTON_RSHIFTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_RSHIFT);
    mBUTTON_RSYMBOLPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_RSYMBOL);
    mBUTTON_HIDEPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_HIDE);
    mBUTTON_SPACEPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_SPACE);
    mBUTTON_LSYMBOLPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_LSYMBOL);
    mBUTTON_LSHIFTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_LSHIFT);
    mBUTTON_ENTERPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_ENTER);
    mBUTTON_DELPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_DEL);
    mBUTTON_DOTPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_DOT);
    mBUTTON_COMMAPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_COMMA);
    mBUTTON_ZPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_Z);
    mBUTTON_YPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_Y);
    mBUTTON_XPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_X);
    mBUTTON_WPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_W);
    mBUTTON_VPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_V);
    mBUTTON_UPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_U);
    mBUTTON_TPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_T);
    mBUTTON_SPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_S);
    mBUTTON_RPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_R);
    mBUTTON_QPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_Q);
    mBUTTON_PPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_P);
    mBUTTON_NPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_N);
    mBUTTON_MPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_M);
    mBUTTON_LPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_L);
    mBUTTON_OPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_O);
    mBUTTON_KPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_K);
    mBUTTON_JPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_J);
    mBUTTON_IPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_I);
    mBUTTON_HPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_H);
    mBUTTON_GPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_G);
    mBUTTON_FPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_F);
    mBUTTON_EPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_E);
    mBUTTON_DPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_D);
    mBUTTON_CPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_C);
    mBUTTON_BPtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_B);
    mBUTTON_APtr = (ZKButton*)findControlByID(ID_ZKIME_BUTTON_A);
    mWINDOW_ALLPtr = (ZKWindow*)findControlByID(ID_ZKIME_WINDOW_ALL);
	mActivityPtr = this;
	onUI_init();
    registerProtocolDataUpdateListener(onProtocolDataUpdate); 
    rigesterActivityTimer();
}

void zkimeActivity::onClick(ZKBase *pBase) {
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

void zkimeActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void zkimeActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void zkimeActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool zkimeActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void zkimeActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int zkimeActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void zkimeActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void zkimeActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void zkimeActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool zkimeActivity::onTouchEvent(const MotionEvent &ev) {
    return onzkimeActivityTouchEvent(ev);
}

void zkimeActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void zkimeActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void zkimeActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
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

void zkimeActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

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

void zkimeActivity::startVideoLoopPlayback() {
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

void zkimeActivity::stopVideoLoopPlayback() {
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

bool zkimeActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
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

int zkimeActivity::removeCharFromString(string& nString, char c) {
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

void zkimeActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void zkimeActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void zkimeActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}