#include "TestUtil.h"
#include "wMain.h"
#include "wTestScreen.h"
//#include "wTestRTSControl.h"
#include "wTestChoose.h"

wTestChoose::wTestChoose() : mWindow("test/choose")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style,"bkg");

	m_bgScreen.setParent(this);
	m_bgScreen.load(m_style,"bg_screen");

	m_screebTxt.setParent(this);
	m_screebTxt.load(m_style,"screen_txt");

	m_screen.setParent(this);
	m_screen.load(m_style,"screen");

	m_bgRTS.setParent(this);
	m_bgRTS.load(m_style,"bg_RTS");

	m_RTSTxt.setParent(this);
	m_RTSTxt.load(m_style,"RTS_txt");

	m_RTS.setParent(this);
	m_RTS.load(m_style,"RTS");
	
	m_exit.setParent(this);
	m_exit.load(m_style,"exit");
	
	m_ts = time(NULL);
}

wTestChoose::~wTestChoose()
{
}

void wTestChoose::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_screen.id()) {
			testUtil.m_colorCycle = 0;
			wTestScreen *screen = new wTestScreen();
			screen->show();
		} else if (e->wParam == m_RTS.id()) {
			testUtil.m_colorCycle = 0;
			// wTestRTSControl *wControl = new wTestRTSControl();
			// wControl->show();
		} else if (e->wParam == m_exit.id()) {
			testUtil.m_colorCycle = 0;
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wTestChoose::doTimer(void)
{
	mWindow::doTimer();
	fb.enable(1);
	if (testUtil.m_colorCycle) {
		if (labs(time(NULL) - m_ts) > 3) {
			testUtil.m_cycleTime = time(NULL);
			wTestScreen *screen = new wTestScreen();
			screen->show();
		}
	}
}
