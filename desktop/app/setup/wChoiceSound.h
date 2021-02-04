#ifndef __W_CHOICE_SOUND_H__
#define __W_CHOICE_SOUND_H__

#include "mWindow.h"
#include "mButton.h"

class wChoiceSound: public mWindow {
public:
	wChoiceSound();
	virtual ~wChoiceSound();

	void doTimer(void);
	void doEvent(mEvent *e);
	void selectChange(void);
	void loadNavigation(void);
	void loadLed(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_talk;
	mPixmap m_ringtone;
	mPixmap m_select_area;

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