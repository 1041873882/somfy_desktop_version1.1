#ifndef __W_TEST_CHOOSE_H__
#define __W_TEST_CHOOSE_H__

#include "mWindow.h"
#include "mButton.h"

class wTestChoose: public mWindow {
public:
	wTestChoose();
	virtual ~wTestChoose();

	void doEvent(mEvent *e);
	void doTimer(void);
private:
	mPixmap m_bkg;
	mPixmap m_bgScreen;
	mText   m_screebTxt;
	mButton m_screen;
	mPixmap m_bgRTS;
	mText   m_RTSTxt;
	mButton m_RTS;
	mButton m_exit;

	time_t m_ts;
};

#endif

