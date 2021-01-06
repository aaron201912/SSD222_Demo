/*
 * ZKCheckBox.h
 *
 *  Created on: Oct 22, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKCHECKBOX_H_
#define _CONTROL_ZKCHECKBOX_H_

#include "ZKButton.h"

class ZKCheckBoxPrivate;

class ZKCheckBox : public ZKButton {
	ZK_DECLARE_PRIVATE(ZKCheckBox)

public:
	ZKCheckBox(ZKBase *pParent);
	virtual ~ZKCheckBox();

	void setChecked(bool isChecked);
	bool isChecked() const;

public:
	class ICheckedChangeListener {
	public:
		virtual ~ICheckedChangeListener() { }
		virtual void onCheckedChanged(ZKCheckBox *pCheckBox, bool isChecked) = 0;
	};

	void setCheckedChangeListener(ICheckedChangeListener *pListener);

protected:
	ZKCheckBox(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CHECKBOX; }

	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseCheckBoxAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKCHECKBOX_H_ */
