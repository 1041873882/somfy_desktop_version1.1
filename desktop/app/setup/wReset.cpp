#include "wMain.h"
#include "wSetup.h"
#include "wReset.h"
#include "SysCommand.h"

wReset::wReset(): mWindow("setup/reset")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_factory.setParent(this);
	m_factory.load(m_style, "factory");
	m_arrow.setParent(this);
	m_arrow.load(m_style, "arrow");
	m_device.setParent(this);
	m_device.load(m_style, "device");
	m_mark.setParent(this);
	m_mark.load(m_style, "mark");
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wReset::~wReset()
{
}

void wReset::doReset(void)
{
	system("rm -rf /dnake/data/*");
	sleep(2);
	system("reboot");
}

void wReset::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wReset::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_F3) {
			doReset();
		} else if (e->wParam == KEY_M_CANCEL) {
			wSetup *setup = new wSetup();
			setup->show();
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wReset::loadNavigation(void)
{
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");
	m_cancel.setParent(this);
	m_cancel.load(m_style, "cancel");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wReset::loadLed()
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		if (i >= LED7) {
			data[i] = LED_ON;
		} else {
			data[i] = LED_OFF;
		}
	}
	command.sendLedControl(data);
}
