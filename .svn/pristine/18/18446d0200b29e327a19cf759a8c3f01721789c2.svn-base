#ifndef __W_TEST_SCREEN_H__
#define __W_TEST_SCREEN_H__

#include "mWindow.h"
#include "mButton.h"

class wTestScreen : public mWindow {
public:
	wTestScreen();
	virtual ~wTestScreen();

	void doEvent(mEvent *e);
	void doTimer(void);
	
private:
	
	mButton m_bkg;
	mButton m_cycle;
	mButton m_exit;

	int m_monitor;
	int m_bgId;
	time_t m_cycleTime;
	char m_buf[128];

};

#endif


