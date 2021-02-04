#ifndef __W_RESET_H__
#define __W_RESET_H__

#include "mWindow.h"
#include "mButton.h"

class wReset: public mWindow {
public:
	wReset();
	virtual ~wReset();

	void loadLed(void);
	void doTimer(void);
	void doReset(void);
	void doEvent(mEvent *e);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;
	mPixmap m_factory;
	mPixmap m_arrow;
	mPixmap m_device;
	mButton m_mark;

	mPixmap m_confirm;
	mPixmap m_cancel;
	mPixmap m_separator;
	mPixmap m_home;
private:
	struct timeval led_tv;
};

#endif