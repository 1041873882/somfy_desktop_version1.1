#ifndef __W_KEY_SETTING_H__
#define __W_KEY_SETTING_H__

#include "mWindow.h"
#include "mButton.h"

#define MAX_CHOICE_NUM	3

class wKeySetting: public mWindow {
public:
	wKeySetting();
	virtual ~wKeySetting();

	void doTimer(void);
	void loadLed(void);
	void doEvent(mEvent *e);
	void selectChange(void);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_choice_area[MAX_CHOICE_NUM];
	mText   m_choice[MAX_CHOICE_NUM];
	mPixmap m_select_area;
	mPixmap m_old_select;

	mPixmap m_right;
	mPixmap m_left;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_select_idx;
	struct timeval led_tv;
};

#endif