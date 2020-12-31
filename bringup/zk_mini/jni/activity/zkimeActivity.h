/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __ZKIMEACTIVITY_H__
#define __ZKIMEACTIVITY_H__


#include "app/Activity.h"
#include "entry/EasyUIContext.h"

#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"

#include "utils/Log.h"
#include "control/ZKDigitalClock.h"
#include "control/ZKButton.h"
#include "control/ZKCircleBar.h"
#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKPointer.h"
#include "control/ZKQRCode.h"
#include "control/ZKTextView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKEditText.h"
#include "control/ZKVideoView.h"
#include "window/ZKSlideWindow.h"
#include "control/ZKSlideText.h"

/*TAG:Macro宏ID*/
#define ID_ZKIME_SLIDETEXT_HANZI    51000
#define ID_ZKIME_BUTTON_NUMBER_SPACE    20120
#define ID_ZKIME_BUTTON_NUMBER_DIV    20119
#define ID_ZKIME_BUTTON_NUMBER_MULT    20118
#define ID_ZKIME_BUTTON_NUMBER_PERCENT    20117
#define ID_ZKIME_BUTTON_NUMBER_DOT    20116
#define ID_ZKIME_BUTTON_NUMBER_MINUS    20115
#define ID_ZKIME_BUTTON_NUMBER_0    20114
#define ID_ZKIME_BUTTON_NUMBER_PLUS    20113
#define ID_ZKIME_BUTTON_NUMBER_9    20111
#define ID_ZKIME_BUTTON_NUMBER_8    20110
#define ID_ZKIME_BUTTON_NUMBER_7    20109
#define ID_ZKIME_BUTTON_NUMBER_6    20108
#define ID_ZKIME_BUTTON_NUMBER_5    20107
#define ID_ZKIME_BUTTON_NUMBER_4    20106
#define ID_ZKIME_BUTTON_NUMBER_3    20105
#define ID_ZKIME_BUTTON_NUMBER_2    20104
#define ID_ZKIME_BUTTON_NUMBER_1    20102
#define ID_ZKIME_BUTTON_NUMBER_HIDE    20091
#define ID_ZKIME_BUTTON_NUMBER_ENTER    20087
#define ID_ZKIME_BUTTON_NUMBER_DEL    20086
#define ID_ZKIME_WINDOW_NUMBER    100001
#define ID_ZKIME_TEXTVIEW_CONTENT    50000
#define ID_ZKIME_TEXTVIEW_PINYIN    50003
#define ID_ZKIME_BUTTON_LANG    20035
#define ID_ZKIME_BUTTON_RSHIFT    20031
#define ID_ZKIME_BUTTON_RSYMBOL    20055
#define ID_ZKIME_BUTTON_HIDE    20034
#define ID_ZKIME_BUTTON_SPACE    20033
#define ID_ZKIME_BUTTON_LSYMBOL    20032
#define ID_ZKIME_BUTTON_LSHIFT    20030
#define ID_ZKIME_BUTTON_ENTER    20029
#define ID_ZKIME_BUTTON_DEL    20028
#define ID_ZKIME_BUTTON_DOT    20027
#define ID_ZKIME_BUTTON_COMMA    20026
#define ID_ZKIME_BUTTON_Z    20025
#define ID_ZKIME_BUTTON_Y    20024
#define ID_ZKIME_BUTTON_X    20023
#define ID_ZKIME_BUTTON_W    20022
#define ID_ZKIME_BUTTON_V    20021
#define ID_ZKIME_BUTTON_U    20020
#define ID_ZKIME_BUTTON_T    20019
#define ID_ZKIME_BUTTON_S    20018
#define ID_ZKIME_BUTTON_R    20017
#define ID_ZKIME_BUTTON_Q    20016
#define ID_ZKIME_BUTTON_P    20015
#define ID_ZKIME_BUTTON_N    20014
#define ID_ZKIME_BUTTON_M    20013
#define ID_ZKIME_BUTTON_L    20012
#define ID_ZKIME_BUTTON_O    20011
#define ID_ZKIME_BUTTON_K    20010
#define ID_ZKIME_BUTTON_J    20009
#define ID_ZKIME_BUTTON_I    20008
#define ID_ZKIME_BUTTON_H    20007
#define ID_ZKIME_BUTTON_G    20006
#define ID_ZKIME_BUTTON_F    20005
#define ID_ZKIME_BUTTON_E    20004
#define ID_ZKIME_BUTTON_D    20003
#define ID_ZKIME_BUTTON_C    20002
#define ID_ZKIME_BUTTON_B    20001
#define ID_ZKIME_BUTTON_A    20000
#define ID_ZKIME_WINDOW_ALL    100000
/*TAG:Macro宏ID END*/

class zkimeActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    zkimeActivity();
    virtual ~zkimeActivity();

    /**
     * 注册定时器
     */
	void registerUserTimer(int id, int time);
	/**
	 * 取消定时器
	 */
	void unregisterUserTimer(int id);
	/**
	 * 重置定时器
	 */
	void resetUserTimer(int id, int time);

protected:
    /*TAG:PROTECTED_FUNCTION*/
    virtual const char* getAppName() const;
    virtual void onCreate();
    virtual void onClick(ZKBase *pBase);
    virtual void onResume();
    virtual void onPause();
    virtual void onIntent(const Intent *intentPtr);
    virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);

    virtual void onTextChanged(ZKTextView *pTextView, const string &text);

    void rigesterActivityTimer();

    virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg);
    void videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex);
    void startVideoLoopPlayback();
    void stopVideoLoopPlayback();
    bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList);
    int removeCharFromString(string& nString, char c);


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;

};

#endif