#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "sys.h"
#include "wMain.h"
#include "wSetup.h"
#include "SysSound.h"
#include "DebugUtil.h"
#include "sun4i_v_inf.h"
#include "SysCommand.h"
#include "BeanUtil.h"
#include "TemporaryUtil.h"
#include "wVideoSetting.h"

#define  COLOR_SCALE_VALUE      16
#define  CONTRAST_SCALE_VALUE   10
#define  BRIGHT_SCALE_VALUE     2

mWindow *SysMeasureTalk = NULL;

wVideoSetting::wVideoSetting(): mWindow("setup/video")
{
	SysMeasureTalk = this;

	m_osd.setParent(this);
	m_osd.load(m_style, "osd");
	m_upper_area.setParent(this);
	m_upper_area.load(m_style, "upper_area");
	char buf[128];
	m_measureState = beanUtil.getVideoSetType();
	sprintf(buf, "icon_%d", m_measureState);
	m_icon.setParent(this);
	m_icon.load(m_style, buf);

	loadNavigation();
	m_measure_icon.setParent(this);
	sprintf(buf, "measure_icon_%d", m_measureState);
	m_measure_icon.load(m_style, buf);
	m_progress_bar.setParent(this);
	m_progress_bar.load(m_style, "progress_bar");
	m_progress_bar.setRange(0, MEASURE_TALK_BOX_MAX);
	setProgress(0);
	//Capture
	m_cvbs = -1;
	gettimeofday(&m_cvbs_tv, NULL);
	gettimeofday(&led_tv, NULL);
}

wVideoSetting::~wVideoSetting()
{
	SysMeasureTalk = NULL;
	if (m_cvbs > 0) {
		if (m_cvbs_mm) {
			::munmap(m_cvbs_mm, m_cvbs_sz);
			m_cvbs_mm = NULL;
		}
		::close(m_cvbs);
		m_cvbs = -1;
		fb.osd(0, 0, 0, 0);
	}
}

void wVideoSetting::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_UP) {
			setProgress(1);
		} else if (e->wParam == KEY_M_DOWN) {
			setProgress(-1);
		} else if (e->wParam == KEY_M_CONFIRM) {
			beanUtil.save();
			sys.save();
			sound.microphoneOnOff(0);
			temporary.m_interCall = temporary.INTER_DEFAULT;
			command.sendHangupReq(MCU_DOOR_MACHINE, command.ANSWER_MEASURE);
			wSetup *setup = new wSetup();
			setup->show();
		} else if (e->wParam == KEY_M_HOME) {
			sound.microphoneOnOff(0);
			temporary.m_interCall = temporary.INTER_DEFAULT;
			command.sendHangupReq(MCU_DOOR_MACHINE, command.ANSWER_MEASURE);
			wMain *main = new wMain();
			main->show();
		}
	}
}

void wVideoSetting::doTimer()
{
	mWindow::doTimer();
	//刷新开屏时间，防止进入待机
	fb.enable(1);
	if (m_cvbs == -1 && __ts(m_cvbs_tv) > 1.5*1000) { //1.5秒延时
		//打开模拟摄像头采集
		m_cvbs = ::open("/dev/tvd_dev", O_RDWR | O_NONBLOCK, 0);
		if (m_cvbs > 0) {
			//设置显示区域
			ShowInfo si;
			si.point.x = m_osd.x();
			si.point.y = m_osd.y();
			si.size.cx = m_osd.width();
			si.size.cy = m_osd.height();
			::ioctl(m_cvbs, TVD_PREV_START, &si);

			m_cvbs_w = ::ioctl(m_cvbs, TVD_PREV_WIDTH, 0);
			m_cvbs_h = ::ioctl(m_cvbs, TVD_PREV_HEIGHT, 0);
			m_cvbs_sz = (m_cvbs_w*m_cvbs_h*3/2)*4;
			m_cvbs_mm = (uint8_t *)::mmap(NULL, m_cvbs_sz, PROT_READ | PROT_WRITE, MAP_SHARED, m_cvbs, 0);
			
			uint32_t val = sys.lcd.bright();
			::ioctl(m_cvbs, TVD_PREV_BRIGHT, &val); //亮度

			val = sys.lcd.contrast();
			::ioctl(m_cvbs, TVD_PREV_CONTRAST, &val); //对比度

			val = sys.lcd.hue();
			::ioctl(m_cvbs, TVD_PREV_HUE, &val); //色调

			val = sys.lcd.saturation();
			::ioctl(m_cvbs, TVD_PREV_SATURATION, &val); //饱和度

			fb.osd(m_osd.x(), m_osd.y(), m_osd.width(), m_osd.height());
			this->paint();

			fprintf(stderr, "wTalk::doTimer tvd_dev open[%dx%d] ok...\n", m_cvbs_w, m_cvbs_h);
		}
	}
	if (command.m_mode > command.NONE) {
		time_t nowTime = time(NULL);
		if (labs(nowTime - temporary.m_timing) > temporary.m_outSecond) {
			PRINT_DEBUG("talk timeout, mode: %d\n", command.m_mode);
			if (command.m_mode == command.TALKING) {
				command.sendHangupReq(MCU_DOOR_MACHINE, command.ANSWER_MEASURE);
				command.m_mode = command.NONE;
				temporary.m_interCall = temporary.INTER_DEFAULT;
				wSetup *setup = new wSetup();
				setup->show();
			}
		}
	}
	if (__val(led_tv) && __ts(led_tv) > 200) {
		loadLed();
		memset(&led_tv, 0, sizeof(led_tv));
	}
}

void wVideoSetting::setProgress(int dir)
{
	int progressNum = getProgressNum();
	if (dir == -1) {
		if (progressNum <= 0) {
			return;
		}
		progressNum--;
	} else if (dir == 1) {
		if (progressNum >= MEASURE_TALK_BOX_MAX) {
			return;
		}
		progressNum++;
	}
	int value = 0;
	if (m_measureState == COLOR) {
		beanUtil.setMeasureColorNum(progressNum);
		value = progressNum * COLOR_SCALE_VALUE;
		//将色度用饱和度替代
		::ioctl(m_cvbs, TVD_PREV_SATURATION, &value);
		sys.lcd.saturation(value);
	} else if (m_measureState == LUMINOUS) {
		beanUtil.setMeasureLuminosityNum(progressNum);
		value = progressNum * BRIGHT_SCALE_VALUE;
		::ioctl(m_cvbs, TVD_PREV_BRIGHT, &value);
		sys.lcd.bright(value);
	} else if (m_measureState == CONTRAST) {
		beanUtil.setMeasureContrastNum(progressNum);
		value = progressNum * CONTRAST_SCALE_VALUE;
		::ioctl(m_cvbs, TVD_PREV_CONTRAST, &value);
		sys.lcd.contrast(value);
	}
	m_progress_bar.setValue(progressNum);
}

int wVideoSetting::getProgressNum(void)
{
	int progressNum = 0;
	if (m_measureState == COLOR) {
		progressNum = beanUtil.getMeasureColorNum();
	} else if (m_measureState == LUMINOUS) {
		progressNum = beanUtil.getMeasureLuminosityNum();
	} else if (m_measureState == CONTRAST) {
		progressNum = beanUtil.getMeasureContrastNum();
	}
	return progressNum;
}

void wVideoSetting::loadNavigation(void)
{
	m_menu_area.setParent(this);
	m_menu_area.load(m_style, "menu_area");
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_confirm.setParent(this);
	m_confirm.load(m_style, "confirm");
	m_separator.setParent(this);
	m_separator.load(m_style, "separator");
	m_home.setParent(this);
	m_home.load(m_style, "home");
}

void wVideoSetting::loadLed()
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