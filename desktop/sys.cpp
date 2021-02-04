#include <fcntl.h>

#include "DebugUtil.h"
#include "BeanUtil.h"
#include "SysSound.h"
#include "sMisc.h"
#include "sys.h"

#define SYS_CFG		"/dnake/data/sys.xml"

int fb_timeout = 20;
int sys_factory = 0; //生产模式

__sys sys;

__sys::__sys()
{
	mute.m_enable = 0;
	user.login = 0;
	admin.login = 0;
	m_year = 0;

	int fd = ::open("/dev/urandom", 0);
	if (fd < 0) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		::srand(clock()+tv.tv_usec);
	} else {
		unsigned long seed;
		::read(fd, &seed, sizeof(seed));
		::srand(seed);
		::close(fd);
	}

	m_limit = 0;
	FILE *fp = fopen("/dnake/etc/limit", "r");
	if (fp) {
		fscanf(fp, "%d", &m_limit);
		fclose(fp);
	}
}

__sys::~__sys()
{
}

void __sys::load(void)
{
	dxml p;
	bool ok = p.loadFile(SYS_CFG);

	user.passwd(p.getText("/sys/user/passwd", "1234"));
	user.tz("+00:00");
	user.language(p.getInt("/sys/user/language", 0));

	admin.passwd(p.getText("/sys/admin/passwd", "123456"));

	volume.music(p.getInt("/sys/volume/music", 80));
	volume.talk(p.getInt("/sys/volume/talk", 80));

	//视频跟旧分机差异，没有采用默认
	lcd.bright(p.getInt("/sys/lcd/bright", 20));
	lcd.saturation(p.getInt("/sys/lcd/saturation", 128));
	lcd.contrast(p.getInt("/sys/lcd/contrast", 100));
	lcd.hue(p.getInt("/sys/lcd/hue", 0));

	char s[128];
	sprintf(s, "BC:F8:11:%02X:%02X:%02X", (uint8_t)(rand()%15), (uint8_t)(rand()%255), (uint8_t)(rand()%255));
	net.mac(p.getText("/sys/network/mac", s));

	if (!ok) {
		this->save();
	} else {
		sprintf(s, "ifconfig eth0 hw ether %s", net.mac());
		system(s);
	}
}

void __sys::save(void)
{
	dxml p;
	p.setText("/sys/user/passwd", user.passwd());
	p.setText("/sys/user/tz", user.tz());
	p.setInt("/sys/user/language", user.language());

	p.setText("/sys/admin/passwd", admin.passwd());

	p.setInt("/sys/volume/music", volume.music());
	p.setInt("/sys/volume/talk", volume.talk());
	
	p.setInt("/sys/lcd/bright", lcd.bright());
	p.setInt("/sys/lcd/saturation", lcd.saturation());
	p.setInt("/sys/lcd/contrast", lcd.contrast());
	p.setInt("/sys/lcd/hue", lcd.hue());

	p.setText("/sys/network/mac", net.mac());

	p.saveFile(SYS_CFG);
	system("sync");

	this->setTZ();
	char s[128];
	sprintf(s, "ifconfig eth0 hw ether %s", net.mac());
	system(s);
}

static const char *gdate_table[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void __sys::setTZ(void)
{
	char tz[64];
	const char *p = user.tz();
	if (*p == '-')
		sprintf(tz, "GMT+%s", p+1);
	else
		sprintf(tz, "GMT-%s", p+1);
	setenv("TZ", tz, 1);
}

static const char *GCC_DATE(void)
{
	static char s[128];
	char m[32];
	int y, d;

	sscanf(__DATE__, "%s %d %d", m, &d, &y);
	for (int i=0; i<12; i++) {
		if (!strcasecmp(m, gdate_table[i])) {
			sprintf(s, "%04d_%02d_%02d", y, i+1, d);
			sys.m_year = y;
			return s;
		}
	}
	return "err";
}

void __sys::version(char *s)
{
	sprintf(s, "%s_%s_%s", PCB_VERSION, TEST_VERSION, GCC_DATE());
}

int __sys::factoryTime()
{
	char m[32];
	int y, d;

	sscanf(__DATE__, "%s %d %d", m, &d, &y);
	for (int i=0; i<12; i++) {
		if (!strcasecmp(m, gdate_table[i])) {
			sys.m_year = y;
			return 1;
		}
	}
	return 0;
}

const char *__sys::style(void)
{
	return "style_en.xml";
}

static int VmRSS(void)
{
	int result = 0;

	char s[2*1024];
	sprintf(s, "/proc/%d/status", getpid());
	FILE *fp = fopen(s, "r");
	if (fp) {
		fread(s, 1, sizeof(s), fp);
		fclose(fp);

		char *p = strstr(s, "VmRSS:");
		if (p) {
			p += strlen("VmRSS:");
			result = atoi(p);
		}
	}
	return result;
}

void __sys::process(void)
{
	static time_t ts = 0;
	if (time(NULL) != ts) {
		if (VmRSS() >= 12*1024) {//内存占用太大
			exit(-1);
		}
		ts = time(NULL);
	}
}
