#ifndef __SYS_RTC_H__
#define __SYS_RTC_H__

#include <time.h>

class SysRTC {
public:
	SysRTC();
	~SysRTC();
	
	int start(void);
	void hwclock(void);
	void correctSysTime();
public:
	int m_adjustState;
private:
	const char *m_dev;
	time_t m_synTime;
};

extern SysRTC rtc;

#endif
