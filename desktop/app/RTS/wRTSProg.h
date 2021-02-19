
#ifndef __W_RTS_PROG_H__
#define __W_RTS_PROG_H__

#include "RTSHeader.h"
#include "mWindow.h"
#include "mButton.h"

class wRTSProg: public mWindow {
public:
	wRTSProg();
	virtual ~wRTSProg();

	void doTimer(void);
	void doEvent(mEvent *e);
	void loadLed(void);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_micromodule;
	mPixmap m_remote;
	mButton m_mark;
	mButton m_mark_smoove;
	mButton m_mark_situo;

	mPixmap m_right;
	mPixmap m_left;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_action;
	struct timeval led_tv;
};

#endif