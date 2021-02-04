#ifndef __W_TIME_SETTING_H__
#define __W_TIME_SETTING_H__

#include "mWindow.h"
#include "mButton.h"

class wTimeSetting: public mWindow {
public:
	wTimeSetting();
	virtual ~wTimeSetting();

	void doTimer(void);
	void doEvent(mEvent *e);
	void doUpdate(void);
	void loadTime(void);
	void syncTime(void);
	void selectChange(void);
	void loadNavigation(void);
	void loadAMPM(int mode);
	void loadLed(int mode);
	void doChange(int direction);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_time_area1;
	mText   m_a24_hour;
	mText   m_a12_hour;
	mPixmap m_small_select;
	mPixmap m_small_inactive;
	mPixmap m_time_area2;
	mText   m_hour;
	mText   m_separator1;
	mText   m_minute;
	mPixmap m_big_select;
	mText 	m_am;
	mText 	m_pm;
	mPixmap m_ampm_select;
	mPixmap m_ampm_inactive;

	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_right;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int a24_hour;
	int nDay;
	int nMonth;
	int nYear;
	int nHour;
	int nMinute;
	int m_select_idx;
	int m_ampm_select_idx;		// 0 is am, 1 is pm
	int m_reset_state;
	struct timeval led_tv;
};

#endif