#ifndef __W_RTS_DEVICE_H__
#define __W_RTS_DEVICE_H__

#include "mWindow.h"
#include "mButton.h"

#define ADD_RTS_DEVICE_MAX	9
#define MAX_NUM_PER_LINE	5

class wRTSDevice : public mWindow {
public:
	wRTSDevice();
	virtual ~wRTSDevice();

	void doTimer(void);
	void selectChange(void);
	void doEvent(mEvent *e);
	void loadNavigation(void);
	void doChange(int direction);
	void loadLed(void);
private:
	mPixmap m_bkg;
	mPixmap m_icon;

	mPixmap m_devices[ADD_RTS_DEVICE_MAX];
	mPixmap m_select_area;

	mPixmap m_right;
	mPixmap m_left;
	mPixmap m_confirm;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_select_idx;
	uint32_t m_minutes;
	struct timeval led_tv;
};

#endif