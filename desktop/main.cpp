#include "DebugUtil.h"
#include "TemporaryUtil.h"
#include "PictureUtil.h"
#include "BeanUtil.h"
#include "RTSUtil.h"
#include "TestUtil.h"

#include "sys.h"
#include "sGpio.h"
#include "SysSound.h"
#include "SysRTC.h"
#include "SysSdcard.h"
#include "SysCommand.h"
#include "SysUpgrade.h"
#include "SysMCU.h"
#include "mFT.h"
#include "mFB.h"
#include "wMain.h"
#include "wDateSetting.h"

const char *sys_i2c[] = {
	"/dev/i2c-2",
	NULL
};

static void *dnake_misc_thread(void *)
{
	sleep(1);
	rtc.start();
	sound.stop();
	sound.load();
	//音量在使用时正常音量，其他情况是静音的
	sound.volume(DUAL_CHANNEL, VOLUME_MUTE_VALUE);
	//防止上电第一次播放没有声音
	sound.opening();
	//上电重新确认分机是否在线
	//temporary.monitorStart();
	while (1) {
		sys.process();
		sdcard.process();

		//temporary.ledLighting();
		temporary.cancelBusy();
		pictureUtil.loadFilePicture();
		rtc.correctSysTime();

		//分机自动呼叫分机，测试是否会出现丢包
		testUtil.monitorCallMonitor(testUtil.m_monitorCall, 10000);
		testUtil.monitorAnswerMonitor(testUtil.m_monitorAnswer, 10000);
		testUtil.monitorHangupMonitor(testUtil.m_monitorAnswer, 10000);
		usleep(200*1000);
	}
	return NULL;
}

void led_off(void)
{
	uint8_t data[9] = {0};
	for (int i = LED1; i <= LED9; i++) {
		data[i] = LED_OFF;
	}
	if (beanUtil.getMissedCall()) {
		data[LED4] = LED_ON;
	}
	command.sendLedControl(data);
}

void ui_main_process(void)
{
	if (fb.enabled()) {
		if (fb.timeout() >= 8*60*60) {
			//对时导致时间偏差太大,刷新开屏时间
			fb.enable(1);
		} else if (fb_timeout && (fb.timeout() >= fb_timeout)) { //熄屏
			//避免调整时间直接熄屏
			if (rtc.m_adjustState) {
				rtc.m_adjustState = 0;
				fb.enable(1);
				return;
			}
			PRINT_DEBUG("put off \n");
			sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
			sound.microphoneOnOff(0);
			sound.stop();
			fb.enable(0);
			fb.m_touch = 0;
			sys.admin.login = 0;
			sys.user.login = 0;
			led_off();
			int resetState = beanUtil.getResetState();
			PRINT_DEBUG("reset: %d\n", resetState);
			if (resetState) {
				wDateSetting *initial = new wDateSetting();
				initial->show();
			} else {
				wMain *w = new wMain();
				w->show();
			}
		}
	}
	//静音超过8小时自动取消
	// if (sys.mute.m_enable && labs(time(NULL)-sys.mute.m_ts) >= 8*60*60) {
	// 	sys.mute.m_enable = 0;
	// }
}

void sys_led(int onoff)
{
	sGpio io;
	io.set("aw", "led0", onoff ? 0 : 1);
}

static void board_init(void)
{
	sGpio io;
	//指示灯
	io.start("aw", "led0");
	io.dir("aw", "led0", 1);
	io.set("aw", "led0", 1);

	//喇叭
	io.start("aw", "spk_mute");
	io.dir("aw", "spk_mute", 1);
	io.set("aw", "spk_mute", 1);

	//MIC
	io.start("aw", "mic_mute");
	io.dir("aw", "mic_mute", 1);
	io.set("aw", "mic_mute", 1);
}

static void sys_signal(int s)
{
	exit(-1);
}

int main(int argc, const char *argv[])
{
	signal(SIGINT, sys_signal); // Interrupt from keyboard
	signal(SIGQUIT, sys_signal); // Quit from keyboard

	board_init();
	ui_msg_init();

	//data load
	sys.load();
	beanUtil.load();
	rTSUtil.load();

	ft.start();
	fb.start();
	mcu.start();

	int resetState = beanUtil.getResetState();
	if (!resetState) {
		wMain *w = new wMain();
		w->show();
	} else {
		wDateSetting *initial = new wDateSetting();
		initial->show();
	}
	//分机呼叫分机丢包测试
	for (int i=1; i<argc; i++) {
		testUtil.deliveryParamter(argv[i]);
	}
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_misc_thread, NULL) != 0) {
		perror("pthread_create dnake_misc_thread!\n");
	}
	fprintf(stderr, "desktop start...\n");
	mEvent::process();
	return 0;
}
