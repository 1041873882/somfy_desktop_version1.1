#include "wMain.h"
#include "wSetup.h"
#include "wUpgrade.h"
#include "FileUtil.h"
#include "DebugUtil.h"
#include "SysUpgrade.h"
#include "UpgradeUtil.h"
#include "SysCommand.h"
#include "SysSdcard.h"

wUpgrade::wUpgrade(): mWindow("setup/upgrade")
{
	char s[128];

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

	m_fw_version.setParent(this);
	m_fw_version.load(m_style, "version");
	sys.version(s);
	m_fw_version.setText(s);

	m_sd_card.setParent(this);
	m_sd_card.load(m_style, "sd_card");
	m_arrow.setParent(this);
	m_arrow.load(m_style, "arrow");
	m_device.setParent(this);
	m_device.load(m_style, "device");
	m_mark.setParent(this);
	m_mark.load(m_style, "mark");
	loadNavigation();

	m_timer = 0;
	m_start = 0;
	m_successState = 0;
	gettimeofday(&led_tv, NULL);
}

wUpgrade::~wUpgrade()
{
}

void wUpgrade::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (m_start) {
			return;
		}
		if (e->wParam == KEY_M_F3) {
			if (!sdcard.used()) {
				PRINT_DEBUG("sd card not exist\n");
				return;
			}
			doUpgrade();
		} else if (e->wParam == KEY_M_CANCEL) {
			wSetup *setup = new wSetup();
			setup->show();
		} else if (e->wParam == KEY_M_HOME) {
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wUpgrade::doUpgrade(void)
{
	FileUtil fileUtil;
	fileUtil.loadFile(SD_CARD_UPDATE_PATH, fileUtil.STATE_IMG);
	if (!upgradeUtil.imgState) {
		PRINT_DEBUG("img not exist\n");
	} else {
		system("rm -rf /dnake/data/*");
		char buf[128];
		sprintf(buf, "%s/%s", SD_CARD_UPDATE_PATH, upgradeUtil.imgName.c_str());
		SysUprade sysUpgrade;
		sysUpgrade.start(buf);
		m_timer = time(NULL);
		m_start = 1;
	}
}

void wUpgrade::doTimer(void)
{
	mWindow::doTimer();
	//刷新开屏时间，防止进入待机
	fb.enable(1);
	time_t nowTime = time(NULL);
	if (m_timer && labs(nowTime - m_timer) > 1) {
		SysUprade sysUpgrade;
		int progress = sysUpgrade.query();
		//避免升级成功的瞬间 progress 是 -1
		if (progress >= 90 || progress == -1) {
			m_successState = 1;
		}
		char buf[128];
		if (m_successState && progress == -1) {
			m_mark.setText("100%");
		} else {
			sprintf(buf, "%d%s", progress, "%");
			m_mark.setText(buf);
		}
	}
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wUpgrade::loadNavigation(void)
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
		m_sd_card.alpha(active);
		m_arrow.alpha(active);
		m_device.alpha(active);
		m_mark.alpha(active);
		gettimeofday(&led_tv, NULL);
	}
}

void wUpgrade::loadLed(void)
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