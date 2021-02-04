#ifndef __W_MAIN_H__
#define __W_MAIN_H__

#include "mWindow.h"
#include "mButton.h"
#include "BeanUtil.h"

#define MAX_CUSTOM_BTN	5

class wMain: public mWindow {
public:
	wMain();
	virtual ~wMain();

	void loadLed(int mode);
	void doEvent(mEvent *e);
	void doTimer(void);
	void loadTime(void);
	void loadCustomBtn(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;
	mPixmap m_menu_area;
	mPixmap m_time_area;
	mText   m_time;
	mText   m_date;
	mText   m_am;
	mText   m_pm;
	mButton m_channel_number[MAX_CUSTOM_BTN];
	mPixmap m_custom_func[MAX_CUSTOM_BTN];
private:
	int m_led_off;
	struct timeval led_tv;
	uint32_t m_minutes;
};

extern mWindow *SysMain;

#endif
