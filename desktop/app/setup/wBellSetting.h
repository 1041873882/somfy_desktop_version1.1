#ifndef __W_RESET_WELCOME_H__
#define __W_RESET_WELCOM_H__

#include "mWindow.h"
#include "mButton.h"

#define MAX_MUSIC_NUM	7
#define PER_LINE_NUM	5

class wBellSetting: public mWindow {
public:
	wBellSetting();
	virtual ~wBellSetting();

	void doTimer(void);
	void doEvent(mEvent *e);
	void selectChange(void);
	void loadNavigation(void);
	void doChange(int direction);
	void loadLed(int mode);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mButton m_music[MAX_MUSIC_NUM];
	mPixmap m_select_area;

	mPixmap m_right;
	mPixmap m_left;
	mPixmap m_down;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_select_idx;
	int m_reset_state;
	struct timeval led_tv;
};

#endif