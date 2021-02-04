#include <stdio.h>
#include "sys.h"
#include "mFB.h"
#include "sMisc.h"
#include "SysMCU.h"
#include "SysCommand.h"
#include "RTSUtil.h"
#include "TemporaryUtil.h"
#include "PictureUtil.h"
#include "DebugUtil.h"

#include "wTalk.h"
#include "wSetup.h"
#include "wMain.h"
#include "wPicture.h"
#include "RTSHeader.h"
#include "wRTSControl.h"
#include "wRTSDevice.h"

mWindow *SysMain = NULL;

wMain::wMain(): mWindow("main")
{
	SysMain = this;

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_menu_area.setParent(this);
	m_menu_area.load(m_style, "menu_area");

	m_time_area.setParent(this);
	m_time_area.load(m_style, "time_area");
	m_time.setParent(this);
	m_time.load(m_style, "time");
	m_date.setParent(this);
	m_date.load(m_style, "date");
	for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
		m_custom_func[i].setParent(this);
		m_custom_func[i].load(m_style, "custom_func");
	}
	m_am.setParent(this);
	m_pm.setParent(this);

	m_led_off  = 200;
	m_minutes  = -1;
	fb_timeout = 20;
	loadTime();
	loadCustomBtn();
	gettimeofday(&led_tv, NULL);
}

wMain::~wMain()
{
	SysMain = NULL;
}

void wMain::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CALL) {
			if (command.m_mode > command.NONE) {
				PRINT_DEBUG("mode is not NONE, and mode: %d\n", command.m_mode);
				return;
			}
			int masterState = beanUtil.getMasterState();
			int slaveState = beanUtil.getSlaveState();
			if (!masterState) {
				PRINT_DEBUG("master monitor is not exist\n");
				return;
			}
			if (masterState && slaveState) {
				command.sendEventAnswerReq(TIME_120S, command.ANSWER_MONITOR);
				return;
			}
			command.sendMonitorReq(TIME_120S, command.ANSWER_MONITOR);
		} else if (e->wParam == KEY_M_UNLOCK1) {
			if (command.m_mode > command.NONE) {
				PRINT_DEBUG("mode is not NONE, and mode: %d\n", command.m_mode);
				return;
			}
			int masterState = beanUtil.getMasterState();
			if (!masterState) {
				return;
			}
			command.sendLockReq(LOCK_DOOR, command.NONE);
			loadLed(1);
		} else if (e->wParam == KEY_M_UNLOCK2) {
			if (command.m_mode > command.NONE) {
				PRINT_DEBUG("mode is not NONE, and mode: %d\n", command.m_mode);
				return;
			}
			int masterState = beanUtil.getMasterState();
			if (!masterState) {
				return;
			}
			command.sendLockReq(LOCK_GATE, command.NONE);
			loadLed(2);
		} else if (e->wParam == KEY_M_PICTURE) {
			pictureUtil.setCurrentPosition(0);
			wPicture *picture = new wPicture();
			picture->show();
		} else if (e->wParam == KEY_M_SETTINGS) {
			wSetup *setup = new wSetup();
			setup->show();
		} else {
			if (!fb.enabled()) {
				return;
			}
			int key = e->wParam - KEY_M_F1;
			rTSUtil.setSelectedChnId(key);
			if (ISTYPEDEFAULT(rTSUtil.getChannelType(key))) {
				rTSUtil.setCurrentAction(RTSUtil::ADD);
				wRTSDevice *rtsDevice = new wRTSDevice();
				rtsDevice->show();
			} else {
				wRTSControl *rtsControl = new wRTSControl();
				rtsControl->show();
			}
		}
	}
}

void wMain::doTimer(void)
{
	mWindow::doTimer();
	loadTime();
	if (fb.enabled() && __val(led_tv) && __ts(led_tv) > m_led_off) {
		memset(&led_tv, 0, sizeof(led_tv));
		loadLed(0);
	}
}

void wMain::loadTime(void)
{
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (m_minutes != tm->tm_min) {
		m_minutes = tm->tm_min;
		char timeBuf[32];
		uint8_t active   = 0x33;		//透明度 20%
		uint8_t deactive = 0XFF;		
		if (!beanUtil.get24Hour()) {
			m_am.load(m_style, "am");
			m_pm.load(m_style, "pm");			
			strftime(timeBuf, sizeof(timeBuf), "%I:%M", tm);
			if (tm->tm_hour < 12) {
				m_am.alpha(deactive);
				m_pm.alpha(active);
			} else {
				m_am.alpha(active);
				m_pm.alpha(deactive);
			}
		} else {
			strftime(timeBuf, sizeof(timeBuf), "%H:%M", tm);
		}
		m_time.setText(timeBuf);
		strftime(timeBuf, sizeof(timeBuf), "%d/%m/%Y", tm);
		m_date.setText(timeBuf);
	}
}

void wMain::loadCustomBtn(void)
{
	char buf[128];
	for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
		sprintf(buf, "device_%d", rTSUtil.getChannelType(i));
		int y_offset = m_style.getInt("/style/custom_func/y_offset", 96);
		m_custom_func[i].loadUrl(m_style, buf);
		m_custom_func[i].move(m_custom_func[0].x(), m_custom_func[0].y() + y_offset*i);
		int number = rTSUtil.getChannelNumber(i);
		if (number > 1) {
			sprintf(buf, "%d", number);
			m_channel_number[i].setParent(this);
			m_channel_number[i].load(m_style, "channel_number");
			m_channel_number[i].setText(buf);
			m_channel_number[i].move(m_channel_number[0].x(), m_channel_number[0].y() + y_offset*i);
		}
	}
}

void wMain::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		if (i >= LED5) {
			data[i] = LED_ON;
		} else {
			data[i] = LED_OFF;
		}
	}

	if ( beanUtil.getMissedCall()) {
		data[LED4] = LED_ON;	
	}

	if (mode == 1) {
		data[LED2] = LED_ON;
		m_led_off = 1000;
		gettimeofday(&led_tv, NULL);
	} else if (mode == 2) {
		data[LED3] = LED_ON;
		m_led_off = 1000;
		gettimeofday(&led_tv, NULL);
	}
	command.sendLedControl(data);
}
