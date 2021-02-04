#include "SysRTC.h"
#include "BeanUtil.h"
#include "DebugUtil.h"
#include "wMain.h"
#include "wSetup.h"
#include "wBellSetting.h"
#include "wTimeSetting.h"
#include "SysCommand.h"

wTimeSetting::wTimeSetting(): mWindow("setup/time")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_time_area1.setParent(this);
	m_time_area1.load(m_style, "time_area1");
	m_small_inactive.setParent(this);
	m_small_select.setParent(this);
	m_a24_hour.setParent(this);
	m_a24_hour.load(m_style, "a24_hour");
	m_a12_hour.setParent(this);
	m_a12_hour.load(m_style, "a12_hour");
	m_time_area2.setParent(this);
	m_time_area2.load(m_style, "time_area2");
	m_big_select.setParent(this);
	m_hour.setParent(this);
	m_hour.load(m_style, "hour");
	m_separator1.setParent(this);
	m_separator1.load(m_style, "separator1");
	m_minute.setParent(this);
	m_minute.load(m_style, "minute");
	m_ampm_inactive.setParent(this);
	m_ampm_select.setParent(this);
	m_am.setParent(this);
	m_am.load(m_style, "am");
	m_pm.setParent(this);
	m_pm.load(m_style, "pm");

	m_select_idx  	  = 0;
	m_ampm_select_idx = 0;
	a24_hour 	      = beanUtil.get24Hour();
	m_reset_state     = beanUtil.getResetState();
	if (a24_hour) {
		m_small_inactive.load(m_style, "small_inactive_area1");
	} else {
		m_small_inactive.load(m_style, "small_inactive_area2");
	}
	loadTime();
	selectChange();
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wTimeSetting::~wTimeSetting()
{
}

void wTimeSetting::loadTime(void)
{
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	nYear  = tm->tm_year + 1900;
	nMonth = tm->tm_mon + 1;
	nDay   = tm->tm_mday;
	if (m_reset_state) {
		nHour   = 12;
		nMinute = 0;
	} else {
		nHour   = tm->tm_hour;
		nMinute = tm->tm_min;
	}
	doUpdate();
}

void wTimeSetting::selectChange(void)
{
	int sel = 1;
	char buf[128];
	if (m_select_idx == 0) {
		m_small_select.show();
		if (a24_hour) {
			m_a24_hour.setFocus(1);
			m_a12_hour.setFocus(0);
			loadAMPM(0);
		} else {
			sel++;
			m_a24_hour.setFocus(0);
			m_a12_hour.setFocus(1);
			loadAMPM(1);
			doUpdate();	
		}
		m_am.setFocus(0);
		m_pm.setFocus(0);
		m_minute.setFocus(0);
		m_ampm_select.hide();
		sprintf(buf, "small_select_area%d", sel);
		m_small_select.load(m_style, buf);
		m_big_select.load(m_style, "big_select_area1");
		m_big_select.loadUrl(m_style, "big_inactive_area");
	} else if (m_select_idx == 1) {
		if (a24_hour) {
			m_a24_hour.setFocus(0);
		} else {
			sel++;
			m_a12_hour.setFocus(0);
		}
		m_hour.setFocus(1);
		m_small_select.hide();
		sprintf(buf, "small_inactive_area%d", sel);
		m_small_inactive.load(m_style, buf);
		m_big_select.load(m_style, "big_select_area1");
	} else if (m_select_idx == 2) {
		loadAMPM(0);
		m_hour.setFocus(0);
		m_minute.setFocus(1);
		m_big_select.load(m_style, "big_select_area2");
	} else {
		loadAMPM(1);
		m_minute.setFocus(0);
		if (m_ampm_select_idx) {
			if (nHour < 12) {
				nHour += 12;
			}
			m_am.setFocus(0);
			m_pm.setFocus(1);
			sel++;
		} else {
			if (nHour >= 12) {
				nHour -= 12;
			}
			m_am.setFocus(1);
			m_pm.setFocus(0);
		}
		sprintf(buf, "ampm_select_area%d", sel);
		m_ampm_select.load(m_style, buf);
		m_big_select.load(m_style, "big_select_area1");
		m_big_select.loadUrl(m_style, "big_inactive_area");
	}
}

void wTimeSetting::loadAMPM(int mode)
{
	if (mode) {
		m_am.show();
		m_pm.show();
		m_ampm_select.show();
		m_ampm_inactive.show();	
	} else {
		m_am.hide();
		m_pm.hide();
		m_ampm_select.hide();	
		m_ampm_inactive.hide();	
	}
}

void wTimeSetting::doUpdate(void)
{
	char minBuf[128];
	char hourBuf[128];

	if (a24_hour) {
		sprintf(hourBuf, "%02d", nHour);
	} else {
		int hour = nHour;
		if (hour < 12) {
			if (hour == 0) {
				hour += 12;
			}
			m_ampm_inactive.load(m_style, "ampm_inactive_area1");
		} else {
			if (hour > 12) {
				hour -= 12;
			}
			m_ampm_inactive.load(m_style, "ampm_inactive_area2");
		}
		sprintf(hourBuf, "%02d", hour);
	}

	sprintf(minBuf, "%02d", nMinute);
	m_minute.setText(minBuf);
	m_hour.setText(hourBuf);
}

void wTimeSetting::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed(m_reset_state);
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wTimeSetting::doChange(int direction)
{
	if (m_select_idx == 0) {
		a24_hour = !a24_hour;
		selectChange();
	} else if (m_select_idx == 1) {
		if (direction) {
			if (++nHour > 23) {
				nHour = 0;
			}
		} else {
			if (--nHour < 0) {
				nHour = 23;
			}
		}
	} else if (m_select_idx == 2) {
		if (direction) {
			if (++nMinute > 59) {
				nMinute = 0;
			}
		} else {
			if (--nMinute < 0) {
				nMinute = 59;
			}
		}
	} else {
		m_ampm_select_idx = !m_ampm_select_idx;	
		selectChange();
	}
	doUpdate();
}

void wTimeSetting::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CONFIRM) {
			syncTime();
			beanUtil.set24Hour(a24_hour);
			beanUtil.save();
			if (!m_reset_state) {
				wSetup *setup = new wSetup();
				setup->show();
			} else {
				wBellSetting *bell = new wBellSetting();
				bell->show();
			}
		} else if (e->wParam == KEY_M_F3) {
			int mode;
			a24_hour ? mode = 2 : mode = 3;	
			if (++m_select_idx > mode) {
				m_select_idx = 0;
			}
			selectChange();
		} else if (e->wParam == KEY_M_UP) {
			doChange(1);
		} else if (e->wParam == KEY_M_DOWN) {
			doChange(0);
		} else if (e->wParam == KEY_M_HOME) {
			if (!m_reset_state) {
				wMain *main = new wMain();
				main->show();
			}
		}
	}
}

void wTimeSetting::syncTime(void)
{
	int ret = 0;
	struct tm tm;
	struct timeval tv;
	memset(&tm, 0, sizeof(struct tm));
	tm.tm_year  = nYear - 1900;
	tm.tm_mon   = nMonth - 1;
	tm.tm_mday  = nDay;
	tm.tm_hour  = nHour;
	tm.tm_min   = nMinute;
	tm.tm_isdst = 0;
	tv.tv_sec   = mktime(&tm);
	tv.tv_usec  = 0;

	ret = settimeofday(&tv, NULL);
	if (ret != 0) {
		fprintf(stderr, "settimeofday failed\n");
	}

	rtc.hwclock();
}

void wTimeSetting::loadNavigation(void)
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
	if (!m_reset_state) {
		m_home.load(m_style, "home_on");
	} else {
		m_home.load(m_style, "home_off");
	}
}

void wTimeSetting::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		if (i >= LED5 && i < LED9) {
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