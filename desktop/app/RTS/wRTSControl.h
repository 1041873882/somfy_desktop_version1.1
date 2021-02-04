#ifndef __W_RTS_CONTROL_H__
#define __W_RTS_CONTROL_H__

#include "mWindow.h"
#include "mButton.h"

#define CTRL_CHN_MAX	5

class wRTSControl: public mWindow {
public:
	wRTSControl();
	virtual ~wRTSControl();

	enum {
		DEFAULT			= 0,
		UP_DEACTIVE		= 1,
		STOP_DEACTIVE	= 2,
		DOWN_DEACTIVE	= 3
	};

	void loadLed(int mode);
	void doEvent(mEvent *e);
	void controlAction(int mode);
	void doTimer(void);
	void loadTime(void);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_time_area;
	mText   m_time;
	mText   m_date;
	mText	m_am;
	mText	m_pm;

	mPixmap m_menu_area;
	mPixmap m_channel[CTRL_CHN_MAX];

	mPixmap m_select_func;
	mPixmap m_up;
	mPixmap m_stop;
	mPixmap m_down;
	mPixmap m_separator;
	mPixmap m_home;
private:
	uint8_t active;
	uint8_t deactive;

	int m_noStop_action;
	int m_deactive_select;
	int m_selected;
	int m_minutes;
	struct timeval led_tv;
};

#endif