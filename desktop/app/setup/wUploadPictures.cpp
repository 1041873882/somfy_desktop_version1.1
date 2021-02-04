#include "wMain.h"
#include "wSetup.h"
#include "DebugUtil.h"
#include "SysSdcard.h"
#include "PictureUtil.h"
#include "wUploadPictures.h"
#include "SysCommand.h"

#define SD_CARD_PATH	"/var/sd/"

wUploadPictures::wUploadPictures(): mWindow("setup/upload")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_pictures.setParent(this);
	m_pictures.load(m_style, "pictures");
	m_arrow.setParent(this);
	m_arrow.load(m_style, "arrow");
	m_sd_card.setParent(this);
	m_sd_card.load(m_style, "sd_card");
	m_mark.setParent(this);
	m_mark.load(m_style, "mark");
	loadNavigation();
	gettimeofday(&led_tv, NULL);
}

wUploadPictures::~wUploadPictures()
{
}

void wUploadPictures::doTimer(void)
{
	mWindow::doTimer();
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wUploadPictures::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_F3) {
			if (!sdcard.used()) {
				PRINT_DEBUG("sd card not exist\n");
				return;
			}
			doUpload();
		} else if (e->wParam == KEY_M_CANCEL) {
			wSetup *setup = new wSetup();
			setup->show();
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wUploadPictures::doUpload(void)
{
	char cmd[128];
	sprintf(cmd, "cp -rf %s %s", PIC_SAVE_PATH, SD_CARD_PATH);
	system(cmd);
}

void wUploadPictures::loadNavigation(void)
{
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");		
	m_cancel.setParent(this);
	m_cancel.load(m_style, "cancel");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
	
	uint8_t active   = 0x80;	//透明度 50%
	if (!sdcard.used()) {
		m_confirm.hide();
		m_pictures.alpha(active);
		m_arrow.alpha(active);
		m_sd_card.alpha(active);
		m_mark.alpha(active);
		gettimeofday(&led_tv, NULL);
	}
}

void wUploadPictures::loadLed(void)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}

	if (sdcard.used()) {
		data[LED7] = data[LED8] = LED_ON;
	} else {
		data[LED8] = LED_ON;
	}
	data[LED9] = LED_ON;
	command.sendLedControl(data);
}