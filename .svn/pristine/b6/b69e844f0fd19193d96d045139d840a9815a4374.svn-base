#ifndef __SYS_H__
#define __SYS_H__

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>

#include "types.h"
#include "dxml.h"
#include "dmsg.h"

#define SYS_MAJOR			1
#define SYS_MINOR			0
#define SYS_MINOR2			1

#define PCB_VERSION   "316M_SF4"
//正式发布软件版本号
#define SOFT_VERSION  "v1.0.1"
//测试软件版本号
#define TEST_VERSION  "t1.0.2"

enum {
	LANG_CHS = 0,
	LANG_EN,
	LANG_FR, //French
	LANG_DE, //German
	LANG_ES, //Spanish
	LANG_IT, //Italian
	LANG_DU, //Dutch
	LANG_GR, //Greek
	LANG_PL, //Polish
	LANG_CS, //Czech
	LANG_PT, //Portuguese
};

class __sys {
public:
	__sys();
	~__sys();

	void load(void);
	void save(void);

	class __mute {
	public:
		int m_enable;
		time_t m_ts;
	} mute;

	class __user {
	public:
		int login;
		char m_passwd[16];
		int m_language;
		std::string m_tz; //时区

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void tz(const char *val) { m_tz = val; };
		const char *tz(void) { return m_tz.c_str(); };
		void language(int val) { m_language = val; };
		int language(void) { return m_language; };
	} user;

	class __admin {
	public:
		int login;
		char m_passwd[16];

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
	} admin;

	class __volume {
	public:
		int m_music;
		int m_talk;

		void music(int val) { m_music = val; };
		int music(void) { return m_music; };
		void talk(int val) { m_talk = val; };
		int talk(void) { return m_talk; };
	} volume;

	class __net {
	public:
		char m_mac[20];

		void mac(const char *val) { if (val) strcpy(m_mac, val); };
		const char *mac(void) { return m_mac; };
	} net;
	
	class __lcd {
	public:
		int m_bright; //0<=亮度<=255 默认32
		int m_saturation; //0<=饱和度<=255 默认128
		int m_contrast; //0<=对比度<=255 默认128
		int m_hue; //0<=色调<=255 默认0

		void bright(int val) { m_bright = val; };
		int bright(void) { return m_bright; };
		void saturation(int val) { m_saturation = val; };
		int saturation(void) { return m_saturation; };
		void contrast(int val) { m_contrast = val; };
		int contrast(void) { return m_contrast; };
		void hue(int val) { m_hue = val; };
		int hue(void) { return m_hue; };
	} lcd;

	void version(char *s);
	int factoryTime();
	const char *style(void);

	void setTZ(void);

	void process(void);
	//set hwclock
	void setHwclock(int state, struct tm &stm);

public:
	int m_limit;
	int m_year;
};

extern __sys sys;

extern int sys_factory;

extern int fb_timeout;

extern const char *sys_i2c[];

void sys_led(int onoff);

int ui_msg_init(void);

#endif
