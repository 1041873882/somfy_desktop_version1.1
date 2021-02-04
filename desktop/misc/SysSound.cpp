#include "DebugUtil.h"
#include "BeanUtil.h"
#include "sys.h"
#include "sGpio.h"
#include "MediaPlayer.h"
#include "SysHighThread.h"
#include "SysSound.h"

SysSound sound;

#define DELAY_TIME_RING   20

SysSound::SysSound()
{
	m_ringtone[0] = "/dnake/sound/Bell.wav";
	m_ringtone[1] = "/dnake/sound/Mobile.wav";
	m_ringtone[2] = "/dnake/sound/Ringtone1.wav";
	m_ringtone[3] = "/dnake/sound/Ringtone2.wav";
	m_ringtone[4] = "/dnake/sound/Smartphone.wav";
	m_ringtone[5] = "/dnake/sound/Door_Bell.wav";
	m_ringtone[6] = "/dnake/sound/Standard.wav";

	m_muteOnOff = 0;
	
	sGpio io;
	io.start("aw", "vol_clk");
	io.dir("aw", "vol_clk", 1);
	io.set("aw", "vol_clk", 1);

	io.start("aw", "vol_data");
	io.dir("aw", "vol_data", 1);
	io.set("aw", "vol_data", 1);
}

SysSound::~SysSound()
{
}

void SysSound::load(void)
{
	if (sys.user.language()) {
		//非简体中文
	} else {
	
	}
}

void SysSound::play(const char *url, int cycle)
{
	player.start(url, cycle);
}

void SysSound::press(void)
{
	//if (!player.used())
		//player.start(m_press, 0);
}

void SysSound::ringing(int time)
{
	if (time > 0 && time < DELAY_TIME_RING) {
		sleep(time);
	} else if (time >= DELAY_TIME_RING) {
		usleep(time*1000);
	}
	if (!m_muteOnOff) {
		amplifierOnOff(1);
	}
	setVolumeValue(BELL_CHANNEL);
	int bellId = beanUtil.getBellId();
	if (bellId < 0 || bellId >= 7) {
		return;
	}
	player.start(m_ringtone[bellId], 1);
}

void SysSound::belling(int bellid, int time)
{
	if (bellid < 0 || bellid >= 7) {
		return;
	}
	if (time > 0 && time < DELAY_TIME_RING) {
		sleep(time);
	} else if (time >= DELAY_TIME_RING) {
		usleep(time*1000);
	}
	if (!m_muteOnOff) {
		amplifierOnOff(1);
	}
	setVolumeValue(BELL_CHANNEL);
	player.start(m_ringtone[bellid], 0);
}

void SysSound::testVol(int channel, int val)
{
	player.stop();
	if (!m_muteOnOff) {
		amplifierOnOff(1);
	}
	int bellId = beanUtil.getBellId();
	if (bellId < 0 || bellId >= 7) {
		return;
	}
	int value = getVolumeValue(val);
	volume(channel, value);
	player.start(m_ringtone[bellId], 0);
}

void SysSound::opening(void)
{
	PRINT_DEBUG("opening ...\n");
	player.start(m_ringtone[6], 0);
}

void SysSound::stop(void)
{
	//主要是避免同时关视频，导致声音停止播放延迟卡顿
	volume(BELL_CHANNEL, VOLUME_MUTE_VALUE);
	player.stop();
}

//channel 0:通道1 1:通道2 其他:两个通道
//val 0-83db
void SysSound::volume(int channel, int val)
{
	sound_volume_t d;
	d.channel = channel;
	d.value = val;
	m_queue.push(d);
	hThread.post();
}

void SysSound::volume2(int channel, int val)
{
	int d = 0;
	switch (channel) {
	case 0:
		d |= 0x01<<1;
		break;
	case 1:
		d |= 0x01 | (0x01<<1);
		break;
	}
	if (val > 83) {
		val = 83;
	}
	uint8_t db = (21-(val/4)) | (((~(val%4))&0x03)<<5);
	d |= db<<2;
	d |= (1<<9);

	//d = 0b1011000111;
	sGpio io;
	for (int i=0; i<10; i++) {
		int v = (d>>i) & 0x01;
		io.set("aw", "vol_clk", 0);
		io.set("aw", "vol_data", v);
		io.set("aw", "vol_clk", 1);
		io.set("aw", "vol_data", 0);
	}

	io.set("aw", "vol_clk", 0);
	io.set("aw", "vol_data", 1);
	io.set("aw", "vol_clk", 1);
	io.set("aw", "vol_clk", 0);

	usleep(10*1000);
	fprintf(stderr, "SysSound::volume: channel: %d val: %d data: 0x%04X\n", channel, val, d);
}

void SysSound::process(void)
{
	if (!m_queue.empty()) {
		sound_volume_t d = m_queue.front();
		this->volume2(d.channel, d.value);
		m_queue.pop();
	}
}

void SysSound::amplifierOnOff(int onOff)
{
	sGpio io;
	if (onOff) {
		m_muteOnOff = 1;
		io.set("aw", "spk_mute", 0);
	} else {
		m_muteOnOff = 0;
		io.set("aw", "spk_mute", 1);
	}
}

void SysSound::microphoneOnOff(int onOff)
{
	sGpio io;
	io.set("aw", "mic_mute", onOff ? 0 : 1);
	PRINT_DEBUG("mic: %s \n", onOff ? "on" : "off");
}

/*
* -83db -> 0db  
*  
*  -83 至 -5 db 音量变化小
*   -4 至   0 db 音量变化明显
*
*/
#define VOLUME_PROGRESS_MAX  16   //音量调节刻度数
#define VOLUME_ONE_START     11  // 第N格后音量，每格刻度对应值进行，由原来 1:N 改成 1:1
#define VOLUME_VALUE_MAX     83  //音量最大值
#define VOLUME_VALUE_FILL    28  //音量值低于 -Ndb 音量调节不明显，音量补齐值
#define VOLUME_PER_SCALE     4  //低于N格时，每格刻度对应的音量值 1:N

int SysSound::getVolumeValue(int progress)
{
	int value = VOLUME_VALUE_MAX;
	if (progress < 0 || progress > VOLUME_PROGRESS_MAX) {
		PRINT_DEBUG("proress beyoud\n");
		return -1;
	}
	if (progress > VOLUME_ONE_START) {
		value = VOLUME_PROGRESS_MAX - progress;
	} else if (progress > 0 && progress <= VOLUME_ONE_START) {
		value = (VOLUME_VALUE_MAX - VOLUME_VALUE_FILL) - VOLUME_PER_SCALE*progress;
	} else if (progress == 0) {
		value = VOLUME_VALUE_MAX;
	}
	if (value < 0) {
		value = 0;
	} else if (value > VOLUME_VALUE_MAX) {
		value = VOLUME_VALUE_MAX;
	}
	PRINT_DEBUG("volume value is %d \n",value);
	return value;
}

void SysSound::setVolumeValue(int channel)
{
	int value = 0;
	int progressNum = 0;
	if (channel == 0) {
		progressNum = beanUtil.getBellProgressBarNum();
	} else if (channel == 1) {
		progressNum = beanUtil.getMeasureAudioNum();
	} else {
		int bellNum = beanUtil.getBellProgressBarNum();
		int audioNum = beanUtil.getMeasureAudioNum();
		if (bellNum != audioNum) {
			value = getVolumeValue(bellNum);
			volume(BELL_CHANNEL, value);
			usleep(50*1000);
			value = getVolumeValue(audioNum);
			volume(AUDIO_CHANNEL, value);
			return;
		}
	}
	value = getVolumeValue(progressNum);
	volume(channel, value);
}
