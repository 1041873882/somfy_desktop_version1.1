#ifndef __W_RESET_WELCOME_H__
#define __W_RESET_WELCOM_H__

#include "mWindow.h"
#include "mButton.h"

class wDateSetting: public mWindow {
public:
	wDateSetting();
	virtual ~wDateSetting();

	void doTimer(void);
	void loadLed(int mode);
	void doEvent(mEvent *e);
	void loadTime(void);
	void syncTime(void);
	void doUpdate(void);
	void selectChange(void);
	void loadNavigation(void);
	void doChange(int direction);
private:
	int isLeapYear(int year);
	int getDaysOfMonth(int year, int month);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_date_area;
	mText   m_day;
	mText   m_separator_1;
	mText   m_month;
	mText   m_separator_2;
	mText   m_year;
	mPixmap m_select_area;

	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_right;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int nDay;
	int nMonth;
	int nYear;
	int nHour;
	int nMinute;
	int m_select_idx;
	int m_reset_state;
	struct timeval led_tv;
};

#endif