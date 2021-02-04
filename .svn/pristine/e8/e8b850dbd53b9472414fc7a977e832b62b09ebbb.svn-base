#include "RTSUtil.h"
#include "sys.h"
#include "wMain.h"
#include "SysCommand.h"
#include "wRTSConfirm.h"
#include "wRTSDevice.h"
#include "wRTSReaction.h"
#include "wRTSProg.h"
#include "SysCommand.h"
#include "wRTSRemove.h"

wRTSProg::wRTSProg() : mWindow("RTS/Prog")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_icon.setParent(this);
	m_action = rTSUtil.getCurrentAction();
	if (m_action == rTSUtil.ADD || m_action == rTSUtil.ADD_GROUP) {
		char s[64];
		sprintf(s, "icon_%d", rTSUtil.getCurrentType());
		m_icon.load(m_style, s);
	} else {
		m_icon.load(m_style, "icon");
	}
	m_micromodule.setParent(this);
	m_micromodule.load(m_style, "micromodule");
	m_remote.setParent(this);
	m_remote.load(m_style, "remote");
	m_mark.setParent(this);
	m_mark.load(m_style, "mark");

	loadNavigation();
	fb_timeout = 0;
	gettimeofday(&led_tv, NULL);
}

wRTSProg::~wRTSProg()
{
}

void wRTSProg::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wRTSProg::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_RIGHT) {
			int channel_id = rTSUtil.getSelectedChnId();
			command.sendSwitchMatch(channel_id+1);
			wRTSReaction *rtsReaction = new wRTSReaction();
			rtsReaction->show();
		} else if (e->wParam == KEY_M_LEFT) {
			if (m_action == rTSUtil.ADD) {
				wRTSDevice *rtsDevice = new wRTSDevice();
				rtsDevice->show();
			} else if (m_action == rTSUtil.REMOVE) {
				wRTSConfirm *rtsConfirm = new wRTSConfirm();
				rtsConfirm->show();
			} else {
				wRTSRemove *rtsRemove = new wRTSRemove();
				rtsRemove->show();
			}
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wRTSProg::loadNavigation(void)
{
	m_right.setParent(this);
	m_right.load(m_style, "right");
	m_left.setParent(this);
	m_left.load(m_style, "left");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wRTSProg::loadLed(void)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}
	data[LED5] = data[LED6] = data[LED9] = LED_ON;
	command.sendLedControl(data);
}
