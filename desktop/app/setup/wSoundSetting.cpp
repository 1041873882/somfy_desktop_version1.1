#include "BeanUtil.h"
#include "SysSound.h"
#include "wMain.h"
#include "wSetup.h"
#include "wSoundSetting.h"
#include "wCompletedSetting.h"
#include "SysCommand.h"

wSoundSetting::wSoundSetting(int channel): mWindow("setup/sound")
{
	m_channel = channel;

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	if (m_channel) {
		m_progress_num = beanUtil.getMeasureAudioNum();
		m_icon.load(m_style, "icon_1");
	} else {
		m_progress_num = beanUtil.getBellProgressBarNum();
		m_icon.load(m_style, "icon_0");
	}
	m_sound_area.setParent(this);
	m_sound_area.load(m_style, "sound_area");
	m_plus.setParent(this);
	m_plus.load(m_style, "plus");
	m_progress_bar.setParent(this);
	m_progress_bar.load(m_style, "progress_bar");
	m_progress_bar.setRange(0, 16);
	m_progress_bar.setValue(m_progress_num);
	m_less.setParent(this);
	m_less.load(m_style, "less");

	m_reset_state = beanUtil.getResetState();
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wSoundSetting::~wSoundSetting()
{
}

void wSoundSetting::doChange(int direction)
{
	if (direction == 1) {
		if (m_progress_num < PROGRESS_BAR_NUM) {
			m_progress_num++;
		}
		sound.testVol(BELL_CHANNEL, m_progress_num);
	} else if (direction == -1) {
		if (m_progress_num > 0) {
			m_progress_num--;
		}
		sound.testVol(BELL_CHANNEL, m_progress_num);
	}
	m_progress_bar.setValue(m_progress_num);
}

void wSoundSetting::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed(m_reset_state);
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wSoundSetting::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CONFIRM) {
			sound.stop();
			if (!m_reset_state) {
				if (m_channel) {
					beanUtil.setMeasureAudioNum(m_progress_num);
					int value = sound.getVolumeValue(m_progress_num);
					sound.volume(AUDIO_CHANNEL, value);
				} else {
					beanUtil.setBellProgressBarNum(m_progress_num);
					int value = sound.getVolumeValue(m_progress_num);
					sound.volume(BELL_CHANNEL, value);
				}
				beanUtil.save();
				wSetup *setup = new wSetup();
				setup->show();
			} else {
				beanUtil.setBellProgressBarNum(m_progress_num);
				beanUtil.save();
				int value = sound.getVolumeValue(m_progress_num);
				sound.volume(BELL_CHANNEL, value);
				wCompletedSetting *completed = new wCompletedSetting();
				completed->show();
			}
		} else if (e->wParam == KEY_M_UP) {
			doChange(1);
		} else if (e->wParam == KEY_M_DOWN) {
			doChange(-1);
		} else if (e->wParam == KEY_M_HOME) {
			if (!m_reset_state) {
				sound.stop();
				wMain *main = new wMain();
				main->show();
			}
		}
	}
}

void wSoundSetting::loadNavigation(void)
{
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	if (!m_reset_state) {
		m_home.load(m_style, "home_on");
	} else {
		m_home.load(m_style, "home_off");
		m_right.setParent(this);
		m_right.load(m_style, "right");
	}
}

void wSoundSetting::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		if (i >= LED5 && i < LED9 && i != LED7) {
			data[i] = LED_ON;
		} else {
			data[i] = LED_OFF;
		}
	}
	if (!mode) {
		data[LED9] = LED_ON;
	}
	command.sendLedControl(data);
}
