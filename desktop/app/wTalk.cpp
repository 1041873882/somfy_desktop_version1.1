#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "DebugUtil.h"
#include "TemporaryUtil.h"
#include "BeanUtil.h"
#include "PictureUtil.h"
#include "SysCommand.h"
#include "RTSUtil.h"

#include "SysSdcard.h"
#include "SysSound.h"
#include "sys.h"
#include "sun4i_v_inf.h"

#include "RTSHeader.h"
#include "wTestChoose.h"
#include "wMain.h"
#include "wTalk.h"

mWindow *SysTalk = NULL;

#define PHOTO_WIDTH		772
#define PHOTO_HEIGHT	454

wTalk::wTalk(): mWindow("talk")
{
	SysTalk = this;

	//视频显示区域
	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

	m_action_bkg.setParent(this);
	m_action_bkg.load(m_style, "action_bkg");
	m_action_bkg.hide();

	m_picture.setParent(this);
	m_picture.load(m_style, "picture");
	m_picture.hide();

	m_arrow_action = 0;
	for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
		if (rTSUtil.getChannelType(i)) {
			m_arrow_action = 1;
			break;
		}
	}

	m_control_noStop = 0;
	m_unAnswerPic_action = 0;
	m_menu_select = MENU_CAPTRUE;
	m_led_select = LED_DEFAULT;
	m_cvbs = -1;
	m_action = AC_DEFAULT;
	gettimeofday(&m_cvbs_tv, NULL);
	gettimeofday(&led_tv, NULL);
	fb_timeout = 120;
	loadNavigation();
}

wTalk::~wTalk()
{
	SysTalk = NULL;
	closeOsd();
}

void wTalk::closeOsd(void)
{
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

void wTalk::touchManage(int key)
{
	if (m_menu_select == MENU_CHANNEL) {
		rTSUtil.setSelectedChnId(key);
		if (!ISTYPEDEFAULT(rTSUtil.getChannelType(key))) {
			m_menu_select = MENU_CONTROL;
			m_led_select = LED_CONTRL;
			gettimeofday(&firstUI_tv, NULL);
			gettimeofday(&led_tv, NULL);
			loadNavigation();
		}		
	} else if (m_menu_select == MENU_CONTROL) {
		int channel_id = rTSUtil.getSelectedChnId();
		if (key) {
			gettimeofday(&firstUI_tv, NULL);
			if (key < TALK_KEY_9) {
				controlAction(key);
				command.sendSwitchControl(channel_id+1, key);
			} else {
				m_menu_select = MENU_CHANNEL;
				m_led_select = LED_CHANNEL;
				gettimeofday(&led_tv, NULL);
				loadNavigation();
			}
		}
	} else {
		if (key == TALK_KEY_6) {
			if (m_action >= AC_GATE && m_action <= AC_CAMERA) {
				return;
			}
			if (command.m_mode == command.RINGING) {
				sound.stop();
			}
			m_action = AC_CAMERA;
			pictureUtil.removeCapturePicture();
			if (doCaturePicture() != 0) {
				m_action = AC_DEFAULT;
				return;
			}
			loadActionBkg();
			//避免抓拍的图片再次被删掉
			pictureUtil.m_captureState = 0;
			pictureUtil.m_captureName = "";
			m_actionTime = time(NULL);
			sendBothMonitorMsg(AC_CAMERA);
		} else if (key == TALK_KEY_8) {
			if (!m_arrow_action) {
				return;
			}
			m_menu_select = MENU_CHANNEL;
			m_led_select = LED_CHANNEL;
			loadNavigation();
			gettimeofday(&firstUI_tv, NULL);
			gettimeofday(&led_tv, NULL);
		}
	}
}

void wTalk::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_UNLOCK1) {
			m_action = AC_DOOR;
			if (command.m_mode == command.RINGING) {
				sound.stop();
			}
			loadActionBkg();
			m_actionTime = time(NULL);
			//将通话声道调为静音，避免总线噪声
			if (command.m_mode >= command.TALKING) {
				sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
			}
			pictureUtil.removeCapturePicture();
			if (sendBothMonitorMsg(AC_DOOR)) {
				return;
			}
			m_led_select = LED_DOOR;
			gettimeofday(&led_tv, NULL);
			command.sendLockReq(LOCK_DOOR, command.NONE);
		} else if (e->wParam == KEY_M_UNLOCK2) {
			m_action = AC_GATE;
			if (command.m_mode == command.RINGING) {
				sound.stop();
			}
			loadActionBkg();
			m_actionTime = time(NULL);
			//将通话声道调为静音，避免总线噪声
			if (command.m_mode >= command.TALKING) {
				sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
			}
			pictureUtil.removeCapturePicture();
			if (sendBothMonitorMsg(AC_GATE)) {
				return;
			}
			m_led_select = LED_GATE;
			gettimeofday(&led_tv, NULL);
			command.sendLockReq(LOCK_GATE, command.NONE);
		} else if (e->wParam == KEY_M_CALL) {
			if (command.m_mode == command.RINGING) {
				sound.stop();
				pictureUtil.removeCapturePicture();
				if (m_unAnswerPic_action) {
					beanUtil.setMissedCall(0);
				}
				//主副分机同时存在,转发接听事件
				if (sendBothMonitorMsg(AC_ANSWER)) {
					return;
				}
				m_menu_select = MENU_CAPTRUE;
				m_led_select = LED_ANSWER;
				loadNavigation();
				gettimeofday(&led_tv, NULL);
				command.sendAnswerReq(MCU_DOOR_MACHINE, command.ANSWER_CALL);
			} else if (command.m_mode >= command.TALKING) {
				doHungup();
			}
		} else {
			int key = e->wParam - KEY_M_F1;
			touchManage(key);
		}
	}
}

void wTalk::loadActionBkg(void)
{
	if (m_action >= AC_GATE && m_action <= AC_CAMERA) {
		PRINT_DEBUG("show action is %d\n", m_action);
		m_action_bkg.show();
		if (m_action == AC_CAMERA) {
			char buf[128];
			sprintf(buf, "%s/%s", PIC_SAVE_PATH, pictureUtil.m_captureName.c_str());
			m_camera.loadUrl(m_style, "camera_2");
			closeOsd();
			m_led_select = LED_CAMERA;
			gettimeofday(&led_tv, NULL);
			m_picture.loadFile(buf);
			m_picture.setWidth(PHOTO_WIDTH);
			m_picture.setHeight(PHOTO_HEIGHT);
			m_picture.show();
		}
	} else if (m_action == AC_END) {
		if (m_menu_select == 0) {
			m_led_select = LED_DEFAULT;
			gettimeofday(&led_tv, NULL);
		}
		m_camera.loadUrl(m_style, "camera_1");
		PRINT_DEBUG("show action hide\n");
		m_action_bkg.hide();
		m_picture.hide();
	}
}

int wTalk::doCaturePicture(void)
{
	char buf[128];
	std::string pictureName;
	int result = -1;
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	sprintf(buf, "%02d%02d%02d%02d%02d%02d_1.jpg", (tm->tm_year+1900), (tm->tm_mon+1), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	pictureName = buf;
	sprintf(buf, "%s/%s", PIC_SAVE_PATH, pictureName.c_str());
	if (m_cvbs > 0) {
		int offset = ::ioctl(m_cvbs, TVD_PREV_OFFSET, 0);
	
		if (offset > 0) {
			//原始数据为NV21，需要转换成YV21再编码
			uint8_t yuv[m_cvbs_w*m_cvbs_h*3/2], *u, *v, *uv;
			int ySize = m_cvbs_w*m_cvbs_h;
			memcpy(yuv, m_cvbs_mm+offset, ySize);
			u = yuv+ySize;
			v = u+ySize/4;
			uv = m_cvbs_mm+offset+ySize;
			for (int i=0; i<(ySize/2); i+=2) {
				u[i/2] = uv[i];
				v[i/2] = uv[i+1];
			}
			result = this->doJpegYv21(yuv, m_cvbs_w, m_cvbs_h, buf);
			printf("doJpegYv21 end.\n");
		}
	}
	PRINT_DEBUG("capture picture name is %s and result: %d\n", pictureName.c_str(), result);
	if (!result) {
		pictureUtil.addPictureName(pictureName, 1);
		pictureUtil.removeFinalPicture();
		pictureUtil.m_captureState = 1;
		pictureUtil.m_captureName = pictureName;
		return 0;
	}
	return -1;
}

void wTalk::doHungup(void)
{
	if (command.m_mode == command.RINGING) {
		sound.stop();
	}
	sound.microphoneOnOff(0);
	sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
	temporary.m_interCall = temporary.INTER_DEFAULT;
	pictureUtil.m_captureState = 0;
	pictureUtil.m_captureName = "";
	PRINT_DEBUG("testState : %d\n", temporary.m_testState);
	if (!temporary.m_testState) {
		wMain *main = new wMain();
		main->show();
	} else {
		wTestChoose *wChoose = new wTestChoose();
		wChoose->show();
		temporary.m_testState = 0;
	}
	PRINT_DEBUG("doHungup mode: %d and action: %d\n", command.m_mode, m_action);
	int masterState = beanUtil.getMasterState();
	int slaveState = beanUtil.getSlaveState();
	if (command.m_mode == command.RINGING && !m_action && masterState && slaveState) {
		PRINT_DEBUG("ringing timeout\n");
		command.sendEventHangupReq(EVENT_HANGUP_TIMEOUT, command.ANSWER_CALL);
	} else {
		command.sendHangupReq(MCU_DOOR_MACHINE, command.ANSWER_CALL);
	}
	command.m_mode = command.NONE;
}

int wTalk::sendBothMonitorMsg(int action)
{
	int masterState = beanUtil.getMasterState();
	int slaveState = beanUtil.getSlaveState();
	if (!masterState || !slaveState) {
		PRINT_DEBUG("master or slave no exist\n");
		return 0;
	}
	int dif = 0;
	if (command.m_mode == command.RINGING) {
		dif = time(NULL) - temporary.m_timing;
		dif = labs(TIME_45S - dif);
	}
	if (action == AC_EXIT && command.m_mode == command.RINGING) {
		PRINT_DEBUG("exit\n");
		command.sendEventBackReq();
		return 1;
	} else if (action == AC_ANSWER) {
		PRINT_DEBUG("answer\n");
		command.sendEventAnswerReq(TIME_120S, command.ANSWER_CALL);
		return 1;
	} else if ((action == AC_DOOR || action == AC_GATE) && command.m_mode == command.RINGING) {
		PRINT_DEBUG("lock: %d\n", action);
		if (action == AC_DOOR) {
			command.sendEventAnswerReq(dif, command.ANSWER_DOOR);
		} else {
			command.sendEventAnswerReq(dif, command.ANSWER_GATE);
		}
		return 1;
	} else if ((action == AC_CAMERA || action == AC_RTS) && command.m_mode == command.RINGING) {
		PRINT_DEBUG("action: %d\n", action);
		command.sendEventAnswerReq(dif, command.ANSWER_NONE);
		return 1;
	}
	return 0;
}

void wTalk::doTimer(void)
{
	mWindow::doTimer();
	//LED切换设置为100ms， 防止Rts控制时LED切换与图标切换出现明显延迟
	if (__val(led_tv) && __ts(led_tv) > 100) {
		memset(&led_tv, 0, sizeof(led_tv));
		if (m_led_select == LED_DOOR || m_led_select == LED_GATE) {
			loadLedLock(m_led_select);
		} else {
			loadLed(m_led_select);
		}
	}

	if (m_menu_select != 0 && __val(firstUI_tv) && __ts(firstUI_tv) > 15*1000) {
		m_menu_select = MENU_CAPTRUE;
		memset(&firstUI_tv, 0, sizeof(firstUI_tv));
		loadLed(LED_DEFAULT);
		loadNavigation();
	}

	if (__val(lockLed_tv) && __ts(lockLed_tv) > 1000) {
		memset(&lockLed_tv, 0, sizeof(lockLed_tv));
		if (m_menu_select == 1) {
			loadLed(LED_CHANNEL);
		} else if (m_menu_select == 2) {
			loadLed(LED_CONTRL);
		} else {
			loadLed(LED_DEFAULT);
		}
	}


	//刷新开屏时间，防止进入待机
	if (!fb.enabled() &&__ts(m_cvbs_tv) > 1000) { //避免低概率，屏幕已唤醒，界面还没有加载完成
		fb.enable(1);
	} else if (fb.enabled()) {
		fb.enable(1);
	}
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
	if (!sound.m_muteOnOff) {
		sound.amplifierOnOff(1);
	}
	/*
	* The master and slave monitor exist at the same time. 
	* The master monitor first sends an hangup request and sends an hangup event.
	*/
	if (command.m_mode >= command.RINGING) {
		if (labs(time(NULL) - temporary.m_timing) > temporary.m_outSecond) {
			PRINT_DEBUG("talk timeout\n");
			if (command.m_mode == command.RINGING && !m_action) {
				/*
				*出现一次超时，铃声打开异常，导致线程一直卡住，无法准时切换线程导致时间异常
				*目前猜测是振铃超时，此时开始重复铃声，打开铃声操作和写文件造成阻塞，
				*故这里直接关闭铃声，去掉不必要打印
				*/
				sound.stop();
				int unAnswer = beanUtil.getUnAnswer();
				unAnswer++;
				beanUtil.setUnAnswer(unAnswer);
				PRINT_DEBUG("unAnswer num: %d \n", unAnswer);
			}
			doHungup();
		}
	}
	if (m_action >= AC_GATE && m_action <= AC_CAMERA) {
		if (labs(time(NULL) - m_actionTime) > 2) {
			PRINT_DEBUG("aciton timeout\n");
			m_action = AC_END;
			loadActionBkg();
		}
	}
	//接听/挂断键切换
	if (temporary.m_callState) {
		temporary.m_callState = 0;
	}
	//振铃下6s未接听自动抓拍，后续接听等操作将被删除
	if (command.m_mode == command.RINGING && !pictureUtil.m_captureState && !m_action) {
		if (labs(time(NULL) - temporary.m_timing) > 6) {
			PRINT_DEBUG("start capture picture doing\n");
			if (m_unAnswerPic_action) {
				return;
			}
			if (doCaturePicture() != 0) {
				return;
			}
			m_unAnswerPic_action = 1;
			beanUtil.setMissedCall(1);
			beanUtil.save();
		}
	}	
}

void wTalk::loadNavigation(void)
{
	char buf[128];

	m_menu_area.setParent(this);
	for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
		m_custom_func[i].setParent(this);
		m_channel_number[i].setParent(this);
		m_channel[i].setParent(this);
	}
	m_menu_area.setParent(this);
	m_camera.setParent(this);
	m_arrow.setParent(this);
	m_select_func.setParent(this);
	m_up.setParent(this);
	m_stop.setParent(this);
	m_down.setParent(this);
	m_separator.setParent(this);
	m_back.setParent(this);

	if (m_menu_select == MENU_CHANNEL) {
		m_camera.hide();
		m_arrow.hide();
		loadControl(0);
		m_menu_area.load(m_style, "menu_area_1");
		int y_offset = m_style.getInt("/style/custom_func/y_offset", 96);
		for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
			m_channel[i].hide();
			m_custom_func[i].show();
			m_custom_func[i].load(m_style, "custom_func");
			sprintf(buf, "device_%d", rTSUtil.getChannelType(i));
			m_custom_func[i].loadUrl(m_style, buf);
			m_custom_func[i].move(m_custom_func[0].x(), m_custom_func[0].y() + y_offset*i);
			
			int number = rTSUtil.getChannelNumber(i);
			m_channel_number[i].show();
			sprintf(buf, "%d", number);
			if (number > 1) {
				m_channel_number[i].load(m_style, "channel_number");
				m_channel_number[i].setText(buf);
				m_channel_number[i].move(m_channel_number[i].x(), m_channel_number[i].y() + y_offset*i);
			}
		}
	} else if (m_menu_select == MENU_CONTROL) {
		loadControl(1);
		int select_channel = rTSUtil.getSelectedChnId();
		int y_offset = m_style.getInt("/style/m_channel/y_offset", 96);
		for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
			m_channel_number[i].hide();
			m_custom_func[i].hide();
			m_channel[i].show();
			m_channel[i].load(m_style, "channel");
			if (select_channel == i) {
				sprintf(buf, "device_%d", rTSUtil.getChannelType(i));
			} else {
				sprintf(buf, "device_%d_1", rTSUtil.getChannelType(i));
			}
			m_channel[i].loadUrl(m_style, buf);
			m_channel[i].move(m_channel[0].x(), m_channel[0].y() + y_offset*i);
		}
		int channel_type = rTSUtil.getChannelType(select_channel);
		if (channel_type == rTSUtil.PLUG || channel_type == rTSUtil.LIGHT_BULB) {
			m_control_noStop = 1;
			controlAction(STOP_DEACTIVE);
		} else {
			m_control_noStop = 0;
			controlAction(CONTROL_DEFAULT);
		}
		m_select_func.load(m_style, "select_func");
		sprintf(buf, "device_%d_2",channel_type);
		m_select_func.loadUrl(m_style, buf);
	} else {
		m_camera.show();
		m_arrow.show();
		loadControl(0);
		for (int i = 0; i < MAX_CUSTOM_BTN; i++) {
			m_channel_number[i].hide();
			m_custom_func[i].hide();
			m_channel[i].hide();
		}
		m_menu_area.load(m_style, "menu_area");
		m_camera.load(m_style, "camera");
		m_camera.loadUrl(m_style, "camera_1");
		if (m_arrow_action) {
			m_arrow.load(m_style, "arrow");
		} else {
			m_arrow.load(m_style, "arrow_1");
		}
	}
}

void wTalk::loadControl(int val)
{
	if (val) {
		m_select_func.show();
		m_up.show();
		m_stop.show();
		m_down.show();
		m_separator.show();
		m_back.show();
		m_menu_area.load(m_style, "menu_area_2");
		m_up.load(m_style, "up");
		m_stop.load(m_style, "stop");
		m_down.load(m_style, "down");
		m_separator.load(m_style, "separator");
		m_back.load(m_style, "back");
	} else {
		m_select_func.hide();
		m_up.hide();
		m_stop.hide();
		m_down.hide();
		m_separator.hide();
		m_back.hide();
	}
}


void wTalk::controlAction(int mode)
{

	uint8_t active = 0x33;		//透明度 20%
	uint8_t deactive = 0XFF;

	if (!m_control_noStop) {
		if (mode == UP_DEACTIVE) {
			m_up.alpha(active);
			m_stop.alpha(deactive);
			m_down.alpha(deactive);
		} else if (mode == STOP_DEACTIVE) {
			m_up.alpha(deactive);
			m_stop.alpha(active);
			m_down.alpha(deactive);
		} else if (mode == DOWN_DEACTIVE) {
			m_up.alpha(deactive);
			m_stop.alpha(deactive);
			m_down.alpha(active);
		} else {
			m_up.alpha(deactive);
			m_stop.alpha(deactive);
			m_down.alpha(deactive);
		}
	} else {
		m_stop.alpha(active);
		m_up.alpha(deactive);
		m_down.alpha(deactive);			
		if (mode == UP_DEACTIVE) {
			m_up.alpha(active);
			m_down.alpha(deactive);			
		} else if (mode == DOWN_DEACTIVE) {
			m_up.alpha(deactive);
			m_down.alpha(active);
		}
	}

	this->paint();
	m_led_select = LED_CONTRL;
	m_control_deactive = mode;
	gettimeofday(&led_tv, NULL);
} 

void wTalk::loadLedLock(int mode) {
	uint8_t data[9] = {0};
	if (mode == LED_DOOR) {
		data[LED2] = LED_ON;
		gettimeofday(&lockLed_tv, NULL);
	} else if (mode == LED_GATE) {
		data[LED3] = LED_ON;
		gettimeofday(&lockLed_tv, NULL);
	}
	command.sendLedControl(data);
}

void wTalk::loadLed(int mode)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}
	if (mode != LED_ANSWER) {
		if (command.m_mode == command.RINGING) {
			data[LED1] = LED_BLINK;
		} else {
			data[LED1] = LED_ON;
		}
	}

	switch (mode) {
		case LED_DEFAULT:
			data[LED6] = LED_ON;
			if (m_arrow_action) {
				data[LED8] = LED_ON;
			}
			break;
		case LED_ANSWER:
			data[LED1] = data[LED6] = LED_ON;
			if (m_arrow_action) {
				data[LED8] = LED_ON;
			}
			break;
		case LED_CAMERA:
			if (m_arrow_action) {
				data[LED8] = LED_ON;
			}
			break;	
		case LED_CHANNEL:
			for (int i = 0, led = LED5; i < MAX_CUSTOM_BTN; i++, led++) {
				if (rTSUtil.getChannelType(i)) {
					data[led] = LED_ON;
				}
			}
			break;	
		case LED_CONTRL:
			if (m_control_deactive == UP_DEACTIVE) {
				data[LED7] = data[LED8] = LED_ON;
			} else if (m_control_deactive == STOP_DEACTIVE) {
				data[LED6] = data[LED8] = LED_ON;
			} else if (m_control_deactive == DOWN_DEACTIVE) {
				data[LED6] = data[LED7] = LED_ON;
			} else {
				for (int i = LED6; i <= LED8; i++) {
					data[i] = LED_ON;
				}
			}
			if (m_control_noStop) {
				data[LED7] = LED_OFF;
			}
			data[LED9] = LED_ON;
			break;
		default:
			break;
	}
	command.sendLedControl(data);
}

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
}

int wTalk::doJpegYv21(void *yuv, int w, int h, const char *url) {
	AVFormatContext *ctx = NULL;
	AVOutputFormat *fmt = NULL;
	AVCodecContext *avc = NULL;
	AVStream *stream = NULL;
	AVCodec *codec = NULL;
	AVPacket pkt;
	AVFrame *frame = NULL;
	int result = -1;

	avformat_alloc_output_context2(&ctx, NULL, "mjpeg", url);
	fmt = ctx->oformat;
	if (avio_open(&ctx->pb, url, AVIO_FLAG_READ_WRITE) < 0) {
		fprintf(stderr, "wTalk::doJpegYv21 avio_open %s err.\n", url);
		goto err;
	}
	if(!(stream = avformat_new_stream(ctx, 0))) {
		fprintf(stderr, "wTalk::doJpegYv21 avformat_new_stream err.\n");
		goto err;
	}
	stream->time_base = (AVRational){ 1, 25 };
	if (!(avc = avcodec_alloc_context3(NULL))) {
		fprintf(stderr, "wTalk::doJpegYv21 avcodec_alloc_context3 err.\n");
		goto err;
	}
	avcodec_parameters_to_context(avc, stream->codecpar);
	avc->codec_id = fmt->video_codec;
	avc->codec_type = AVMEDIA_TYPE_VIDEO;
	avc->pix_fmt = AV_PIX_FMT_YUVJ420P;
	avc->width = w;
	avc->height = h;
	avc->time_base.num = 1;
	avc->time_base.den = 25;
	av_dump_format(ctx, 0, url, 1);

	if (!(codec = avcodec_find_encoder(avc->codec_id))) {
		fprintf(stderr, "wTalk::doJpegYv21 avcodec_find_encoder err!\n");
		goto err;
	}
	if (avcodec_open2(avc, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		goto err;
	}

	avc->codec_id = fmt->video_codec;
	avc->codec_type = AVMEDIA_TYPE_VIDEO;
	avc->pix_fmt = AV_PIX_FMT_YUVJ420P;
	avc->width = w;
	avc->height = h;
	avc->time_base.num = 1;
	avc->time_base.den = 25;
	av_dump_format(ctx, 0, url, 1);

	if (!(codec = avcodec_find_encoder(avc->codec_id))) {
		fprintf(stderr, "wTalk::doJpegYv21 avcodec_find_encoder err!\n");
		goto err;
	}
	if (avcodec_open2(avc, codec, NULL) < 0) {
		fprintf(stderr, "wTalk::doJpegYv21 avcodec_open2 err!\n");
		goto err;
	}

	frame = av_frame_alloc();
	av_image_fill_arrays(frame->data, frame->linesize, (uint8_t *)yuv, AV_PIX_FMT_YUV420P, avc->width, avc->height, 1);

	ctx->streams[0]->codecpar->width = w;
	ctx->streams[0]->codecpar->height = h;
	ctx->streams[0]->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	printf("avformat_write_header = %d\n", avformat_write_header(ctx, NULL));

	av_new_packet(&pkt, w*h*3);
	frame->data[0] = (uint8_t *)yuv;               // Y
	frame->data[1] = (uint8_t *)yuv + w*h;         // U
	frame->data[2] = (uint8_t *)yuv  + (w*h*5/4);  // V
	frame->width = avc->width;
	frame->height = avc->height;
	frame->format = AV_PIX_FMT_YUV420P;
	frame->pts = 0;

	if ((avcodec_send_frame(avc, frame) == 0) && (avcodec_receive_packet(avc, &pkt) == 0)) {
		pkt.stream_index = stream->index;
		printf("av_write_frame = %d ", av_write_frame(ctx, &pkt));
	}
	av_packet_unref(&pkt);
	printf("av_write_trailer = %d \n", av_write_trailer(ctx));
	result = 0;
err:
	if (ctx) {
		if (ctx->pb)
			avio_close(ctx->pb);
		if (stream) {
			avcodec_close(avc);
			av_free(frame);
		}
		avformat_free_context(ctx);
	}
	return result;
}
