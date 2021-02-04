#include "sys.h"
#include "wMain.h"
#include "BeanUtil.h"
#include "wCompletedSetting.h"
#include "SysCommand.h"

wCompletedSetting::wCompletedSetting(): mWindow("setup/completed")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_check_area.setParent(this);
	m_check_area.load(m_style, "check_area");

	fb_timeout = 20;
	loadNavigation();
	beanUtil.setResetState(0);
	beanUtil.save();
	gettimeofday(&led_tv, NULL);
}

wCompletedSetting::~wCompletedSetting()
{
}

void wCompletedSetting::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wCompletedSetting::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wCompletedSetting::loadNavigation(void)
{
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_right.setParent(this);
	m_right.load(m_style, "right");
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wCompletedSetting::loadLed(void)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}
	data[LED9] = LED_ON;
	command.sendLedControl(data);
}