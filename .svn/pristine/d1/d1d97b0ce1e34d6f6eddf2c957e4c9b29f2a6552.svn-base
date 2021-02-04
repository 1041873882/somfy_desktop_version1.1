#include "sys.h"
#include "wMain.h"
#include "wSetup.h"
#include "BeanUtil.h"
#include "wKeySetting.h"
#include "SysCommand.h"

wKeySetting::wKeySetting(): mWindow("setup/key")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	int offset = m_style.getInt("/style/x_offset", 203);
	for (int i = 0; i < MAX_CHOICE_NUM; i++) {
		m_choice_area[i].setParent(this);
		m_choice_area[i].load(m_style, "choice_area");
		m_choice_area[i].move(m_choice_area[i].x()+offset*i, m_choice_area[i].y());
	}
	m_old_select.setParent(this);
	m_old_select.load(m_style, "old_select");
	int openTimeId = beanUtil.getOpenTimeId();
	m_old_select.move(m_old_select.x()+openTimeId*offset, m_old_select.y());
	char buf[128];
	for (int i = 0; i < MAX_CHOICE_NUM; i++) {
		sprintf(buf, "choice_%d", i+1);
		m_choice[i].setParent(this);
		m_choice[i].load(m_style, buf);
		if (i == openTimeId) {
			m_choice[i].setFocus(1);
		}
	}
	m_select_area.setParent(this);
	m_select_area.load(m_style, "select_area");
	m_select_idx = 0;
	selectChange();
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wKeySetting::~wKeySetting()
{
}

void wKeySetting::selectChange(void)
{
	char buf[128];
	sprintf(buf, "/style/choice_%d/x", m_select_idx+1);
	int x = m_style.getInt(buf, 93);
	sprintf(buf, "/style/choice_%d/y", m_select_idx+1);
	int y = m_style.getInt(buf, 163);
	m_select_area.move(x, y);
}

void wKeySetting::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wKeySetting::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_RIGHT) {
			if (++m_select_idx >= MAX_CHOICE_NUM) {
				m_select_idx = 0;
			}
			selectChange();
		} else if (e->wParam == KEY_M_LEFT) {
			if (--m_select_idx < 0) {
				m_select_idx = MAX_CHOICE_NUM - 1;
			}
			selectChange();
		} else if (e->wParam == KEY_M_CONFIRM) {
			beanUtil.setOpenTimeId(m_select_idx);
			beanUtil.save();
			wSetup *setup = new wSetup();
			setup->show();
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wKeySetting::loadNavigation(void)
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

void wKeySetting::loadLed(void)
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