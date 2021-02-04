#include "sys.h"
#include "wMain.h"
#include "wSetup.h"
#include "SysSound.h"
#include "wSoundSetting.h"
#include "wChoiceSound.h"
#include "SysCommand.h"

wChoiceSound::wChoiceSound(): mWindow("setup/sound/choice")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_talk.setParent(this);
	m_talk.load(m_style, "talk");
	m_ringtone.setParent(this);
	m_ringtone.load(m_style, "ringtone");
	m_select_area.setParent(this);
	m_select_area.load(m_style, "select_area");

	m_select_idx = 0;
	selectChange();
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wChoiceSound::~wChoiceSound()
{
}

void wChoiceSound::selectChange(void)
{
	int x, y;
	if (m_select_idx) {
		x = m_ringtone.x();
		y = m_ringtone.y();
	} else {
		x = m_talk.x();
		y = m_talk.y();
	}
	m_select_area.move(x, y);
}

void wChoiceSound::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wChoiceSound::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_RIGHT) {
			if (++m_select_idx >= 2) {
				m_select_idx = 0;
			}
			selectChange();
		} else if (e->wParam == KEY_M_LEFT) {
			if (--m_select_idx < 0) {
				m_select_idx = 1;
			}
			selectChange();
		} else if (e->wParam == KEY_M_CONFIRM) {
			if (m_select_idx) {
				wSoundSetting *soundSetting = new wSoundSetting(BELL_CHANNEL);
				soundSetting->show();
			} else {
				wSoundSetting *soundSetting = new wSoundSetting(AUDIO_CHANNEL);
				soundSetting->show();
			}
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wChoiceSound::loadNavigation(void)
{
	m_right.setParent(this);
	m_right.load(m_style, "right");
	m_left.setParent(this);
	m_left.load(m_style, "left");
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wChoiceSound::loadLed(void)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		if (i >= LED5 && i != LED7) {
			data[i] = LED_ON;
		} else {
			data[i] = LED_OFF;
		}
	}
	command.sendLedControl(data);
}