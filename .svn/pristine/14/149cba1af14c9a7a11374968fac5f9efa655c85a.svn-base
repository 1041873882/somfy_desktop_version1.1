#ifndef __W_RESET_WELCOME_H__
#define __W_RESET_WELCOM_H__

#include "mWindow.h"
#include "mButton.h"
#include "mSeekBar.h"

#define  PROGRESS_BAR_NUM	16

class wSoundSetting: public mWindow {
public:
	wSoundSetting(int channel);
	virtual ~wSoundSetting();

	void doTimer(void);
	void doEvent(mEvent *e);
	void loadLed(int mode);
	void loadNavigation(void);
	void doChange(int direction);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap  m_sound_area;
	mPixmap  m_plus;
	mSeekBar m_progress_bar;
	mPixmap  m_less;

	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_right;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_channel;
	int m_progress_num;
	int m_reset_state;
	struct timeval led_tv;
};

#endif