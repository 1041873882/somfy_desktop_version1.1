#include "RTSUtil.h"
#include "sys.h"
#include "wMain.h"
#include "wRTSConfirm.h"
#include "wRTSRemove.h"
#include "SysCommand.h"
#include "wRTSProg.h"

static int select_idx = 0;

wRTSRemove::wRTSRemove(): mWindow("RTS/Remove")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_reaction = rTSUtil.getCurrentAction();
	m_icon.setParent(this);
	loadChannel();
	m_select_area.setParent(this);
	m_select_area.load(m_style, "select_area");
	if (m_reaction == RTSUtil::REMOVE) {
		m_icon.load(m_style, "icon");
		m_select_area.loadUrl(m_style, "select_area_1");
	} else {
		m_icon.load(m_style, "icon_1");
		m_select_area.loadUrl(m_style, "select_area_2");
	}

	loadDevice();
	selectChange();
	loadNavigation();
	fb_timeout = 120;
	gettimeofday(&led_tv, NULL);
}

wRTSRemove::~wRTSRemove()
{
}

void wRTSRemove::selectChange(void)
{
	int x = m_style.getInt("/style/select_area/x", 79);
	int x_offset = m_style.getInt("/style/select_area/x_offset", 120);
	m_select_area.move(x + x_offset*select_idx, m_select_area.y());
}

void wRTSRemove::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wRTSRemove::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CONFIRM) {
			if (rTSUtil.getChannelType(select_idx) != rTSUtil.TYPE_DEFAULT) {
				rTSUtil.setSelectedChnId(select_idx);
				rTSUtil.setCurrentType(rTSUtil.getChannelType(select_idx));
				if (m_reaction ==  RTSUtil::REMOVE) {
					rTSUtil.setCurrentNumber(rTSUtil.getChannelNumber(select_idx));
					wRTSConfirm *w = new wRTSConfirm();
					w->show();
				} else{
					wRTSProg *w = new wRTSProg();
					w->show();
				}
				
			}
		} else if (e->wParam == KEY_M_RIGHT) {
			if (++select_idx >= RTS_CHN_MAX) {
				select_idx = 0;
			}
			selectChange();
		} else if (e->wParam == KEY_M_LEFT) {
			if (--select_idx < 0) {
				select_idx = RTS_CHN_MAX - 1;
			}
			selectChange();
		} else if (e->wParam == KEY_M_HOME) {
			select_idx = 0;
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wRTSRemove::loadChannel(void)
{
	char buf[128];
	for (int i = 0; i < RTS_CHN_MAX; i++) {
		sprintf(buf, "channel_%d", i+1);
		m_channel[i].setParent(this);
		m_channel[i].load(m_style, buf);
	}
}

void wRTSRemove::loadDevice(void)
{
	char buf[128];
	int x_offset = m_style.getInt("/style/number/x_offset", 120);
	for (int i = 0; i < RTS_CHN_MAX; i++) {
		int channel_type = rTSUtil.getChannelType(i);
		sprintf(buf, "device_%d", channel_type);
		m_channel[i].setParent(this);
		m_channel[i].loadUrl(m_style, buf);
		int number = rTSUtil.getChannelNumber(i);
		if (number > 1) {
			m_number[i].setParent(this);
			m_number[i].load(m_style, "number");
			sprintf(buf, "%d", number);
			m_number[i].setText(buf);
			m_number[i].move(m_number[0].x() + x_offset*i, m_number[0].y());
		}
	}
}

void wRTSRemove::loadNavigation(void)
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

void wRTSRemove::loadLed(void)
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
