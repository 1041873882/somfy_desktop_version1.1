#include "wMain.h"
#include "RTSUtil.h"
#include "SysCommand.h"
#include "wRTSControl.h"

wRTSControl::wRTSControl(): mWindow("RTS/Control")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_time_area.setParent(this);
	m_time_area.load(m_style, "time_area");
	m_time.setParent(this);
	m_time.load(m_style, "time");
	m_date.setParent(this);
	m_date.load(m_style, "date");
	m_am.setParent(this);
	m_pm.setParent(this);

	active = 0x33;		//透明度 20%
	deactive = 0XFF;
	m_minutes = -1;
	m_noStop_action = 0;
	m_deactive_select = 0;

	loadTime();
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wRTSControl::~wRTSControl()
{
}

void wRTSControl::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		int channel_id = rTSUtil.getSelectedChnId();
		if (e->wParam == KEY_M_ACT_UP) {
			controlAction(UP_DEACTIVE);
			command.sendSwitchControl(channel_id+1, 1);
		} else if(e->wParam == KEY_M_ACT_STOP) {
			controlAction(STOP_DEACTIVE);
			command.sendSwitchControl(channel_id+1, 2);
		} else if (e->wParam == KEY_M_ACT_DOWN) {
			controlAction(DOWN_DEACTIVE);
			command.sendSwitchControl(channel_id+1, 3);
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wRTSControl::controlAction(int mode)
{
	if (!m_noStop_action) {
		if (mode == UP_DEACTIVE) {
			m_up.alpha(active);
			m_stop.alpha(deactive);
			m_down.alpha(deactive);
		} else if (mode == STOP_DEACTIVE) {
			m_up.alpha(deactive);
			m_stop.alpha(active);
			m_down.alpha(deactive);
		} else {
			m_up.alpha(deactive);
			m_stop.alpha(deactive);
			m_down.alpha(active);
		}
	} else {
		m_stop.alpha(active);
		if (mode == UP_DEACTIVE) {
			m_up.alpha(active);
			m_down.alpha(deactive);			
		} else if (mode == DOWN_DEACTIVE) {
			m_up.alpha(deactive);
			m_down.alpha(active);
		}
	}


	this->paint();
	m_deactive_select = mode;
	gettimeofday(&led_tv, NULL);
}

void wRTSControl::doTimer()
{
	mWindow::doTimer();
	loadTime();
	//LED切换设置为100ms， 防止RTS设备控制时LED切换与图标切换出现明显延迟
	if (__val(led_tv) && __ts(led_tv) > 100) {
		memset(&led_tv, 0, sizeof(led_tv));
		loadLed(m_deactive_select);
	}
}

void wRTSControl::loadTime(void)
{
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (m_minutes != tm->tm_min) {
		m_minutes = tm->tm_min;
		char timeBuf[32];		
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

void wRTSControl::loadNavigation(void)
{
	m_menu_area.setParent(this);
	m_menu_area.load(m_style, "menu_area");
	char buf[128];
	int select_channel = rTSUtil.getSelectedChnId();
	for (int i = 0; i < CTRL_CHN_MAX; i++) {
		sprintf(buf, "channel_%d", i+1);
		m_channel[i].setParent(this);
		m_channel[i].load(m_style, buf);
		if (select_channel == i) {
			sprintf(buf, "device_%d", rTSUtil.getChannelType(i));
		} else {
			sprintf(buf, "device_%d_1", rTSUtil.getChannelType(i));
		}
		m_channel[i].loadUrl(m_style, buf);
	}
	m_select_func.setParent(this);
	m_select_func.load(m_style, "select_func");
	m_selected = rTSUtil.getChannelType(select_channel);
	sprintf(buf, "device_%d_2", m_selected);
	m_select_func.loadUrl(m_style, buf);
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_stop.setParent(this);
	m_stop.load(m_style, "stop");
	if (m_selected == rTSUtil.PLUG ||  m_selected == rTSUtil.LIGHT_BULB) {
		m_noStop_action = 1;
		controlAction(STOP_DEACTIVE);
		gettimeofday(&led_tv, NULL);
	}
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wRTSControl::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}

	switch (mode)
	{
		case DEFAULT:
			for (int i = LED6; i <= LED8; i++) {
				data[i] = LED_ON;
			}
			break;
		case UP_DEACTIVE:
			data[LED7] = data[LED8] = LED_ON;
			break;
		case STOP_DEACTIVE:
			data[LED6] = data[LED8] = LED_ON;
			break;
		case DOWN_DEACTIVE:
			data[LED6] = data[LED7] = LED_ON;
			break;
		default:
			break;
	}

	if (m_noStop_action) {
		data[LED7] = LED_OFF;
	}

	data[LED9] = LED_ON;


	command.sendLedControl(data);
}
