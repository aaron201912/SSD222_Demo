/*
 * IMEContext.h
 *
 *  Created on: Oct 30, 2017
 *      Author: guoxs
 */

#ifndef _IME_IMECONTEXT_H_
#define _IME_IMECONTEXT_H_

#include <string>

class IMEContext {
public:
	IMEContext();
	virtual ~IMEContext();

	typedef enum {
		E_IME_TEXT_TYPE_ALL,
		E_IME_TEXT_TYPE_NUMBER
	} EIMETextType;

	typedef struct {
		bool isPassword;
		char passwordChar;
		EIMETextType imeTextType;
		std::string text;
	} SIMETextInfo;

	class IIMETextUpdateListener {
	public:
		virtual ~IIMETextUpdateListener() { }
		virtual void onIMETextUpdate(const std::string &text) = 0;
	};

	void setIMETextUpdateListener(IIMETextUpdateListener *pListener) {
		mIMETextUpdateListenerPtr = pListener;
	}

	void initIME(const SIMETextInfo &info, IIMETextUpdateListener *pListener);

protected:
	virtual void onInitIME(const SIMETextInfo &info) { }

	void doneIMETextUpdate(const std::string &text);
	void cancelIMETextUpdate();

protected:
	IIMETextUpdateListener *mIMETextUpdateListenerPtr;
	SIMETextInfo mIMETextInfo;
};

#endif /* _IME_IMECONTEXT_H_ */
