#ifndef __W_UPGRADE_H__
#define __W_UPGRADE_H__

#include "mWindow.h"
#include "mButton.h"

class wUpgrade: public mWindow {
public:
	wUpgrade();
	virtual ~wUpgrade();

	void loadLed(void);
	void doTimer(void);
	void doUpgrade(void);
	void doEvent(mEvent *e);
	void loadNavigation(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;
	mText	m_fw_version;
	mPixmap m_sd_card;
	mPixmap m_arrow;
	mPixmap m_device;
	mButton m_mark;

	mPixmap m_confirm;
	mPixmap m_cancel;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_start;
	int m_successState;
	time_t m_timer;
	struct timeval led_tv;
};

#endif