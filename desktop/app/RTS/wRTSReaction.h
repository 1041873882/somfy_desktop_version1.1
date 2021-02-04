
#ifndef __W_RTS_REACTION_H__
#define __W_RTS_REACTION_H__

#include "RTSHeader.h"
#include "mWindow.h"
#include "mButton.h"

class wRTSReaction: public mWindow {
public:
	wRTSReaction();
	virtual ~wRTSReaction();

	void doTimer(void);
	void loadLed(int mode);
	void doReaction(void);
	void doEvent(mEvent *e);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_micromodule;
	mPixmap m_device;
	mPixmap m_op_dir;
	mPixmap m_test_dir;
	mPixmap m_mark_dir;
	mButton m_test;
	mButton m_mark;

	mPixmap m_menu_area1;
	mPixmap m_func1;
	mPixmap m_func2;
	mPixmap m_menu_area2;
	mPixmap m_confirm;
	mPixmap m_cancel;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_action;
	int m_reaction;
	struct timeval led_tv;
};

#endif