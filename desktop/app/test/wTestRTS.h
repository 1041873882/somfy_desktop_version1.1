#ifndef __W_TEST_RTS_H__
#define __W_TEST_RTS_H__

#include "mWindow.h"
#include "mButton.h"


class wTestRTS : public mWindow {

public:

	wTestRTS();
	virtual ~wTestRTS();

	void doEvent(mEvent *e);
	void doTimer(void);
	
	void loadControl(int position);
	void setChannelEnable(int enable);
	
private:

	mPixmap m_bkg;
	mButton m_exit;
	mButton m_match;
	mButton m_delete;
	mText   m_matchTxt;
	mText   m_deleteTxt;

	mButton m_top;
	mButton m_middle;
	mButton m_bottom;
	
};

#endif