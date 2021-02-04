#ifndef __SYS_HIGH_THREAD_H__
#define __SYS_HIGH_THREAD_H__

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>

class SysHighThread {
public:
	SysHighThread();
	~SysHighThread();

	void process(void);
	void post(void);

public:
	sem_t m_wait;
};

extern SysHighThread hThread;

#endif
