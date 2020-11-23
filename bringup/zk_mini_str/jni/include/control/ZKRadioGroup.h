/*
 * ZKRadioGroup.h
 *
 *  Created on: Oct 23, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKRADIOGROUP_H_
#define _CONTROL_ZKRADIOGROUP_H_

#include "ZKCheckBox.h"

class ZKRadioGroupPrivate;

class ZKRadioGroup : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKRadioGroup)

public:
	ZKRadioGroup(HWND hParentWnd);
	virtual ~ZKRadioGroup();

	void setCheckedID(int id);
	int getCheckedID() const { return mCheckedID; }

	void clearCheck();

public:
	class ICheckedChangeListener {
	public:
		virtual ~ICheckedChangeListener() { }
		virtual void onCheckedChanged(ZKRadioGroup *pRadioGroup, int checkedID) = 0;
	};

	void setCheckedChangeListener(ICheckedChangeListener *pListener) {
		mCheckedChangeListenerPtr = pListener;
	}

protected:
	ZKRadioGroup(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_RADIOGROUP; }

	virtual BOOL onTouchEvent(const MotionEvent &ev);

private:
	void parseRadioGroupAttributeFromJson(const Json::Value &json);
	void parseRadioButtonsAttributeFromJson(const Json::Value &json);

private:
	ICheckedChangeListener *mCheckedChangeListenerPtr;
	int mCheckedID;

	vector<ZKCheckBox*> mCheckBoxsList;
};

#endif /* _CONTROL_ZKRADIOGROUP_H_ */
