
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "mFB.h"
#include "SysSdcard.h"

SysSdcard sdcard;

//sdcard挂载目录 /var/sd
SysSdcard::SysSdcard()
{
	m_used = 0;
}

SysSdcard::~SysSdcard()
{
}

void SysSdcard::process(void)
{
	static time_t ts = time(NULL);
	if (labs(time(NULL)-ts) < 2 || !fb.enabled()) {
		return;
	}
	ts = time(NULL);
	if (!m_used) {
		struct stat st;
		if (!stat("/var/sd", &st)) {
			system("umount /var/sd");
			system("rmdir /var/sd");
		}
	}
	FILE *fp = fopen("/proc/partitions", "r");
	if (fp) {
		int found = 0;
		char m[4][64];
		while (!feof(fp)) {
			fscanf(fp, "%s %s %s %s\n", m[0], m[1], m[2], m[3]);
			if (!strncmp(m[3], "mmcblk", 6)) {
				found = 1;
				if (!m_used) {
					char s[128];
					system("mkdir /var/sd");
					sprintf(s, "mount /dev/%s /var/sd", m[3]);
					pid_t r = system(s);
					if (WEXITSTATUS(r) == 0) {
						m_used = 1;
						break;
					} else {
						system("rmdir /var/sd");
					}
				}
			}
		}
		fclose(fp);
		if (!found && m_used) { //没有sd卡信息
			system("umount -lf /var/sd");
			system("rmdir /var/sd");
			m_used = 0;
		}
	}
}
