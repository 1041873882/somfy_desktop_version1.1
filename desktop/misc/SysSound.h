
#ifndef __SYS_SOUND_H__
#define __SYS_SOUND_H__

#include <pthread.h>
#include <semaphore.h>
#include <queue>

#define VOLUME_LOUD_VALUE  0   //最大声
#define VOLUME_MUTE_VALUE  83 //静音

#define BELL_CHANNEL       0
#define AUDIO_CHANNEL      1
#define DUAL_CHANNEL       2

typedef struct {
	int channel;
	int value;
} sound_volume_t;

class SysSound {
public:
	
	SysSound();
	~SysSound();

	void load(void);
	void play(const char *url, int cycle);
	void press(void);
	void ringing(int time); //time:秒或毫秒
	void belling(int bellid, int time); //time:秒或毫秒
	void opening(void);
	void stop(void);
	//channel 0:通道1 1:通道2 其他:两个通道  val 0-83db
	void volume(int channel, int val);
	void volume2(int channel, int val);

	void process(void);
	void amplifierOnOff(int onOff); //onOff: 0 is off; 1 is on
	void microphoneOnOff(int onoff); // 0 is off; 1 is on
	int  getVolumeValue(int progress);
	void setVolumeValue(int channel);
	void testVol(int channel, int val);
public:
	const char *m_ringtone[7];
	//唤醒屏幕第一次播放铃声或进入对讲，打开功放
	int m_muteOnOff;
	std::queue<sound_volume_t> m_queue;
};

extern SysSound sound;

#endif
