/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __TESTAIOACTIVITY_H__
#define __TESTAIOACTIVITY_H__


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
#include "control/ZKCheckbox.h"
#include "control/ZKRadioGroup.h"
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_TESTAIO_TextView_curGain    50009
#define ID_TESTAIO_TextView_maxGain    50008
#define ID_TESTAIO_TextView_minGain    50007
#define ID_TESTAIO_SeekBar_micGain    91001
#define ID_TESTAIO_TextView_micGaun    50006
#define ID_TESTAIO_RadioButton_Dmic    22002
#define ID_TESTAIO_RadioButton_Amic    22001
#define ID_TESTAIO_RadioGroup_mictype    94001
#define ID_TESTAIO_Checkbox_playbgsound    21001
#define ID_TESTAIO_TextView_mictype    50005
#define ID_TESTAIO_Textview_headphone    50004
#define ID_TESTAIO_Textview_speaker    50003
#define ID_TESTAIO_Textview_record    50002
#define ID_TESTAIO_Textview_samplerate    50001
#define ID_TESTAIO_Button_headphone    20006
#define ID_TESTAIO_Button_playstereo    20005
#define ID_TESTAIO_Button_playrecord    20004
#define ID_TESTAIO_Listview_recordfile    80002
#define ID_TESTAIO_Button_recordfile    20003
#define ID_TESTAIO_Button_record    20002
#define ID_TESTAIO_Listview_samplerate    80001
#define ID_TESTAIO_Button_samplerate    20001
#define ID_TESTAIO_sys_back   100
/*TAG:Macro宏ID END*/

class testaioActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKRadioGroup::ICheckedChangeListener,
                     public ZKCheckBox::ICheckedChangeListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    testaioActivity();
    virtual ~testaioActivity();

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
    virtual void onCheckedChanged(ZKRadioGroup* pRadioGroup, int checkedID);
    virtual void onCheckedChanged(ZKCheckBox* pCheckBox, bool isChecked);

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