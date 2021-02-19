
#ifndef __W_TALK_H__
#define __W_TALK_H__

#include "mWindow.h"
#include "mButton.h"

#define MAX_CUSTOM_BTN	5

class wTalk: public mWindow {
public:
	wTalk();
	virtual ~wTalk();

	enum {
		AC_DEFAULT = 0,
		AC_EXIT    = 1,
		AC_ANSWER  = 2,
		AC_GATE    = 3,
		AC_DOOR    = 4,
		AC_CAMERA  = 5,
		AC_RTS     = 6,
		AC_END     = 7,
	};

	enum {
		LED_DEFAULT = 0,
		LED_ANSWER  = 1,
		LED_DOOR	= 2,
		LED_GATE	= 3,
		LED_CAMERA	= 4,
		LED_CHANNEL	= 5,
		LED_CONTRL	= 6,
	};

	enum {
		CONTROL_DEFAULT = 0,
		UP_DEACTIVE		= 1,
		STOP_DEACTIVE	= 2,
		DOWN_DEACTIVE	= 3
	};

	enum {
		TALK_KEY_5,
		TALK_KEY_6,
		TALK_KEY_7,
		TALK_KEY_8,
		TALK_KEY_9
	};


	enum {
		MENU_CAPTRUE = 0,
		MENU_CHANNEL = 1,
		MENU_CONTROL = 2
	};

	void closeOsd(void);
	void loadLed(int mode);
	void loadLedLock(int mode);
	void doEvent(mEvent *e);
	void loadNavigation(void);
	void touchManage(int key);
	void controlAction(int mode);
	void loadControl(int val);
	void doTimer(void);
	int  doJpegYv21(void *yuv, int w, int h, const char *url);

	int  doCaturePicture(void);
	void doHungup(void);
	void loadActionBkg(void);
	int  sendBothMonitorMsg(int action);
private:
	mPixmap m_osd;
	mPixmap m_action_bkg;
	mPixmap m_picture;
	mPixmap m_menu_area;
	mPixmap m_camera;
	mPixmap m_arrow;
	mPixmap m_custom_func[MAX_CUSTOM_BTN];
	mButton m_channel_number[MAX_CUSTOM_BTN];
	mPixmap m_channel[MAX_CUSTOM_BTN];
	mPixmap m_select_func;
	mPixmap m_up;
	mPixmap m_stop;
	mPixmap m_down;
	mPixmap m_separator;
	mPixmap m_back;
private:
	int m_cvbs_w;
	int m_cvbs_h;
	struct timeval m_cvbs_tv;
	struct timeval led_tv;
	struct timeval firstUI_tv;
	struct timeval lockLed_tv;
	
	int m_cvbs;
	uint8_t *m_cvbs_mm;
	int m_cvbs_sz;

	int m_control_noStop;
	int m_control_deactive;
	int m_unAnswerPic_action;
	int m_arrow_action;
	int m_led_select;
	int m_menu_select;
	int m_action;
	time_t m_actionTime;
	int m_switchState;
	int m_toorbarState;
};

#endif
