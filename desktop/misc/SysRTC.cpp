
#include "sys.h"
#include "sGpio.h"
#include "SysRTC.h"

static int bcd2bin(uint8_t val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}

static uint8_t bin2bcd(int val)
{
	return ((val / 10) << 4) + val % 10;
}

SysRTC rtc;

SysRTC::SysRTC()
{
	m_dev = NULL;
	m_synTime = 0;
	m_adjustState = 0;
}

SysRTC::~SysRTC()
{
}

int SysRTC::start(void)
{
	for (int i=0; sys_i2c[i] != NULL; i++) {
		i2c_dev i2c;
		i2c.open(sys_i2c[i]);
		i2c.addr(0x51);
		int vv = i2c.r8(0x00);
		if (vv != 0xFF) {
			uint8_t d[7];
			i2c.r8(0x02, d, 7);
			int vlow = d[0] & 0x80;
			if (vlow) { //芯片未初始化
				time_t ts = time(NULL);
				struct tm *tm = localtime(&ts);
				i2c.w8(0x02, bin2bcd(tm->tm_sec));
				i2c.w8(0x03, bin2bcd(tm->tm_min));
				i2c.w8(0x04, bin2bcd(tm->tm_hour));
				i2c.w8(0x05, bin2bcd(tm->tm_mday));
				i2c.w8(0x07, bin2bcd(tm->tm_mon+1));
				i2c.w8(0x08, bin2bcd(tm->tm_year % 100));
			} else {
				struct tm tm;
				tm.tm_year = bcd2bin(d[6]);
				tm.tm_mon = bcd2bin(d[5]&0x1F)-1;
				tm.tm_wday = bcd2bin(d[4]&0x7);
				tm.tm_mday = bcd2bin(d[3]&0x3F);
				tm.tm_hour = bcd2bin(d[2]&0x3F);
				tm.tm_min = bcd2bin(d[1]&0x7F);
				tm.tm_sec = bcd2bin(d[0]&0x7F);
				tm.tm_isdst = 0;
				if (tm.tm_year < 70) {
					tm.tm_year += 100;
				}
				struct timeval tv;
				tv.tv_sec = mktime(&tm);
				tv.tv_usec = 0;
				settimeofday(&tv, NULL);
				fprintf(stderr, "SysRTC::start rtc[%d-%02d-%02d %02d:%02d:%02d]\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			}
			m_dev = sys_i2c[i];
			fprintf(stderr, "SysRTC::start i2c[%s] ok.\n", m_dev);
			break;
		}
	}
	return 0;
}

//写入RTC时间
void SysRTC::hwclock(void)
{
	if (m_dev) {
		time_t ts = time(NULL);
		struct tm *tm = localtime(&ts);
		i2c_dev i2c;
		i2c.open(m_dev);
		i2c.addr(0x51);
		i2c.w8(0x02, bin2bcd(tm->tm_sec));
		i2c.w8(0x03, bin2bcd(tm->tm_min));
		i2c.w8(0x04, bin2bcd(tm->tm_hour));
		i2c.w8(0x05, bin2bcd(tm->tm_mday));
		i2c.w8(0x07, bin2bcd(tm->tm_mon+1));
		i2c.w8(0x08, bin2bcd(tm->tm_year % 100));
		fprintf(stderr, "SysRTC::hwclock\n");
	}
}

void SysRTC::correctSysTime()
{
	time_t nowTime = time(NULL);
	if (labs(nowTime - m_synTime) <= 30*60) { //30分钟校验一次
		return;
	}
	m_synTime = nowTime;
	for (int i=0; sys_i2c[i]!=NULL; i++) {
		i2c_dev i2c;
		i2c.open(sys_i2c[i]);
		i2c.addr(0x51);
		int vv = i2c.r8(0x00);
		if (vv != 0xFF) {
			uint8_t d[7];
			i2c.r8(0x02, d, 7);
			int vlow = d[0] & 0x80;
			if (!vlow) { //芯片已经初始化
				fprintf(stderr,"RTC init over \n");
				struct tm tm;
				tm.tm_year = bcd2bin(d[6]);
				tm.tm_mon = bcd2bin(d[5]&0x1F)-1;
				tm.tm_wday = bcd2bin(d[4]&0x7);
				tm.tm_mday = bcd2bin(d[3]&0x3F);
				tm.tm_hour = bcd2bin(d[2]&0x3F);
				tm.tm_min = bcd2bin(d[1]&0x7F);
				tm.tm_sec = bcd2bin(d[0]&0x7F);
				tm.tm_isdst = 0;
				if (tm.tm_year < 70) {
					tm.tm_year += 100;
				}
				time_t rtcTime = mktime(&tm);
				time_t now = time(NULL);
				if (labs(now - rtcTime) > 10) { //系统时间误差超过 10s,同步RTC时间
					struct timeval tv;
					tv.tv_sec = rtcTime;
					tv.tv_usec = 0;
					settimeofday(&tv, NULL);
					fprintf(stderr, "SysRTC::correct rtc[%d-%02d-%02d %02d:%02d:%02d]\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				}
			}
			m_dev = sys_i2c[i];
			fprintf(stderr, "SysRTC:: correct i2c[%s] ok.\n", m_dev);
			break;
		}
	}
}
