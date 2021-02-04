#include "wMain.h"
#include "wPicture.h"
#include "BeanUtil.h"
#include "PictureUtil.h"
#include "SysCommand.h"

wPicture::wPicture(): mWindow("picture")
{
	m_picture.setParent(this);
	m_picture.load(m_style, "picture");
	m_newer_selected.setParent(this);
	m_newer_selected.load(m_style, "newer_selected");
	m_newer_selected.hide();
	m_upper_area.setParent(this);
	m_upper_area.load(m_style, "upper_area");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");
	m_bin_icon.setParent(this);
	m_bin_icon.load(m_style, "bin_icon");

	m_offset.setParent(this);
	m_offset.load(m_style, "offset");
	m_infos.setParent(this);
	m_infos.load(m_style, "infos");
	m_date.setParent(this);
	m_date.load(m_style, "date");
	m_time.setParent(this);
	m_time.load(m_style, "time");

	loadNavigation();
	updateUnreadPicNum();
	if (beanUtil.getMissedCall()) {
		beanUtil.setMissedCall(0);
	}
	m_position = 0;
	modeChange(DEFAULT);
	loadPicure(m_position);
	gettimeofday(&led_tv, NULL);
}

wPicture::~wPicture()
{
	std::string name("");
	for (int i = 0; i < m_pictureNum; i++) {
		pictureUtil.loadPictureName(i, &name);
		if (pictureUtil.isUnReadPicture(name)) {
			pictureUtil.changeSysFileName(i, &name);
		}
	}
}

void wPicture::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_UP) {
			doChange(-1);
		} else if (e->wParam == KEY_M_DOWN) {
			doChange(1);
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		} else if (e->wParam == KEY_M_DEL) {
			if (!m_pictureNum) {
				return;
			}
			if (m_mode) {
				doConfirm();
			} else {
				modeChange(DELETE_ONE_PICTURE);
			}
		} else if (e->wParam == KEY_M_DELALL) {
			if (!m_pictureNum) {
				return;
			}
			if (m_mode == DEFAULT) {
				modeChange(DELETE_ALL_PICTURE);
			} else {
				modeChange(DEFAULT);
			}
		} else if (e->wParam == KEY_M_PICTURE) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wPicture::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed(0);
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wPicture::updateUnreadPicNum(void)
{
	m_unReadPictureNum = 0;
	m_pictureNum = pictureUtil.getPictureNum();
	for (int i = 0; i < m_pictureNum; i++) {
		std::string name("");
		pictureUtil.loadPictureName(i, &name);
		if (pictureUtil.isUnReadPicture(name)) {
			m_unReadPictureNum++;
		}
	}
}

void wPicture::doConfirm(void)
{
	if (m_mode == DELETE_ONE_PICTURE) {
		pictureUtil.deleteOnePicture(m_position);
		doChange(-1);
	} else if (m_mode == DELETE_ALL_PICTURE) {
		m_position = 0;
		pictureUtil.deleteAllPictures();
	}
	updateUnreadPicNum();
	modeChange(0);
	loadPicure(m_position);
}

void wPicture::loadPicure(int position)
{
	char buf[128];
	std::string name("");
	pictureUtil.loadPictureName(position, &name);
	if (!name.empty()
		&& name.length() >= PIC_NAME_SHOW_SIZE) {
		sprintf(buf, "%s/%s", PIC_SAVE_PATH, name.c_str());
		m_picture.loadFile(buf);
		m_picture.setWidth(PICTURE_WIDTH);
		m_picture.setHeight(PICTURE_HEIGHT);
		m_picture.show();
		loadInfos(name);
		if (pictureUtil.isUnReadPicture(name)) {
			snprintf(buf, sizeof(buf), "(%d/%d)", position+1, m_unReadPictureNum);
			m_offset.setText(buf);
			m_offset.show();
			m_newer_selected.show();
		} else {
			m_offset.hide();
			m_newer_selected.hide();
		}
	} else {
		m_offset.hide();
		loadInfos("");
		m_picture.load(m_style, "picture");
		m_newer_selected.hide();
	}
}

void wPicture::loadInfos(std::string name)
{
	if (name.empty()) {
		m_date.hide();
		m_time.hide();
		m_infos.hide();
	} else {
		char buf[128];
		snprintf(buf, sizeof(buf), "%s/%s/%s", name.substr(6, 2).c_str(),
				name.substr(4, 2).c_str(), name.substr(0, 4).c_str());
		m_date.setText(buf);
		snprintf(buf, sizeof(buf), "%s:%s", name.substr(8, 2).c_str(), name.substr(10, 2).c_str());
		m_time.setText(buf);
		m_date.show();
		m_time.show();
		m_infos.show();
	}
}

void wPicture::doChange(int direction)
{
	if (direction == -1) {
		if (m_position <= 0) {
			return;
		}
		m_position--;
	} else if (direction == 1) {
		if (m_position >= m_pictureNum - 1) {
			return;
		}
		if (m_position+1 <= m_unReadPictureNum) {
			std::string name("");
			pictureUtil.loadPictureName(m_position, &name);
			if (pictureUtil.isUnReadPicture(name)) {
				pictureUtil.changeSysFileName(m_position, &name);
			}
		}
		m_position++;
	}

	if (m_position < 1) {
		m_up.load(m_style, "up_off");
	} else {
		m_up.load(m_style, "up_on");
	}
	if (m_position < m_pictureNum - 1) {
		m_down.load(m_style, "down_on");
	} else {
		m_down.load(m_style, "down_off");
	}

	loadPicure(m_position);
	loadLed(m_mode);
}

void wPicture::modeChange(int mode)
{
	if (mode) {
		m_up.hide();
		m_down.hide();
		m_func1.load(m_style, "confirm");
		m_func2.load(m_style, "cancel");
		m_bin_icon.show();
	} else {
		m_up.show();
		m_down.show();
		if (m_pictureNum > 0) {
			m_func1.load(m_style, "del_on");
			m_func2.load(m_style, "delall_on");
		} else {
			m_func1.load(m_style, "del_off");
			m_func2.load(m_style, "delall_off");
		}
		m_bin_icon.hide();
	}
	char buf[128];
	if (mode == DELETE_ALL_PICTURE) {
		m_number.show();
		m_number.load(m_style, "del_num");
		m_number.loadUrl(m_style, "number");
	} else if (mode == DEFAULT) {
		m_number.show();
		m_number.load(m_style, "all_num");
		if (m_pictureNum == 0) {
			m_number.hide();
		} else {
			m_number.loadUrl(m_style, "number1");
		}
	} else {
		m_number.hide();
	}
	snprintf(buf, sizeof(buf), "%d", m_pictureNum);
	m_number.setText(buf);
	m_mode = mode;
	doChange(0);
}

void wPicture::loadNavigation(void)
{
	m_menu_area.setParent(this);
	m_menu_area.load(m_style, "menu_area");
	m_up.setParent(this);
	m_up.load(m_style, "up_off");
	m_down.setParent(this);
	m_down.load(m_style, "down_off");
	m_func1.setParent(this);
	m_func1.load(m_style, "del_off");
	m_func2.setParent(this);
	m_func2.load(m_style, "delall_off");
	m_number.setParent(this);
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wPicture::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}
	data[LED4] = data[LED9] = LED_ON;
	if (!mode) {
		if (m_position > 0) {
			data[LED5] = LED_ON;
		}
		if (m_position < m_pictureNum - 1) {
			data[LED6] = LED_ON;
		}
		if (m_pictureNum > 0) {
			data[LED7] = data[LED8] = LED_ON;
		}
	} else {
		data[LED7] = data[LED8] = LED_ON;
	}
	command.sendLedControl(data);
}
