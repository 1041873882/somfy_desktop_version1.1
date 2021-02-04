#ifndef __SYS_SDCARD_H__
#define __SYS_SDCARD_H__

#include "sys.h"

class SysSdcard {
public:
	SysSdcard();
	~SysSdcard();
	
	int used(void) { return m_used; };
	void process(void);
public:
	int m_used;
};

extern SysSdcard sdcard;

#endif
