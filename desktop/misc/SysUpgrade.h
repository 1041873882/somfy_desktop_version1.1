#ifndef __SYS_UPGRADE_H__
#define __SYS_UPGRADE_H__

#include "dmsg.h"

class SysUprade {
public:
	SysUprade();
	~SysUprade();
	
	void start(const char *url);
	int query(void);
};

#endif
