
#include "mFB.h"
#include "SysSound.h"
#include "SysHighThread.h"

static void *sys_high_thread(void *)
{
	struct sched_param sp;
	sp.sched_priority = sched_get_priority_max(SCHED_RR) - 1;
	pthread_setschedparam(pthread_self(), SCHED_RR, &sp);
	while (1) {
		hThread.process();
	}
	return NULL;
}

SysHighThread hThread;

SysHighThread::SysHighThread()
{
	sem_init(&m_wait, 0, 0);
}

SysHighThread::~SysHighThread()
{
}

void SysHighThread::process(void)
{
	sem_wait(&m_wait);
	sound.process();
	fb.process();
}

void SysHighThread::post(void)
{
	static int iThread = 1;
	if (iThread) {
		iThread = 0;
		pthread_t pid;
		if (pthread_create(&pid, NULL, sys_high_thread, this) != 0) {
			perror("SysHighThread::wakeup pthread_create\n");
		}
	}
	sem_post(&m_wait);
}
