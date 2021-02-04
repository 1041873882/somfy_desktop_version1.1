#ifndef __W_RTS_CONFIRM_H__
#define __W_RTS_CONFIRM_H__

#include "mWindow.h"
#include "mButton.h"

class wRTSConfirm: public mWindow {
public:
	wRTSConfirm();
	virtual ~wRTSConfirm();

	void doTimer(void);
	void doEvent(mEvent *e);
	void loadLed(void);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mButton m_number;
	mPixmap m_channel;
	mPixmap m_arrow;
	mPixmap m_gestures;

	mPixmap m_confirm;
	mPixmap m_cancel;
	mPixmap m_separator;
	mPixmap m_home;
private:
	struct timeval led_tv;
};

#endif