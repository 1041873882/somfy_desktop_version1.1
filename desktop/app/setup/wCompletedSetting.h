#ifndef __W_COMPLETED_SETTING_H__
#define __W_COMPLETED_SETTING_H__

#include "mWindow.h"
#include "mButton.h"

class wCompletedSetting: public mWindow {
public:
	wCompletedSetting();
	virtual ~wCompletedSetting();

	void doTimer(void);
	void loadLed(void);
	void doEvent(mEvent *e);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_check_area;

	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_right;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	struct timeval led_tv;
};

#endif