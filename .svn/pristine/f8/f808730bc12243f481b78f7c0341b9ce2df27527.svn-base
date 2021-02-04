#ifndef __W_VIDEO_SETTING_H__
#define __W_VIDEO_SETTING_H__

#include "mWindow.h"
#include "mButton.h"
#include "mSeekBar.h"

#define MEASURE_TALK_BOX_MAX  16

class wVideoSetting: public mWindow {
public:
	wVideoSetting();
	virtual ~wVideoSetting();

	enum {
		CONTRAST = 0,
		LUMINOUS = 1,
		COLOR    = 2,
	};

	void doEvent(mEvent *e);
	void doTimer(void);
	int  getProgressNum();
	void setProgress(int dir);
	void loadLed(void);
	void loadNavigation(void);
private:
	mPixmap m_osd;
	mPixmap m_upper_area;
	mPixmap m_icon;

	mPixmap m_menu_area;
	mPixmap m_measure_icon;
	mSeekBar m_progress_bar;
	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_measureState;
	int m_cvbs_w;
	int m_cvbs_h;
	struct timeval m_cvbs_tv;
	struct timeval led_tv;

	int m_cvbs;
	uint8_t *m_cvbs_mm;
	int m_cvbs_sz;
};

#endif