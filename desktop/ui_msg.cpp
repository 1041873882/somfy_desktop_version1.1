
#include "dxml.h"
#include "dmsg.h"
#include "sys.h"

static void ui_run(const char *body)
{
	dmsg_ack(200, NULL);
}

int ui_msg_init(void)
{
	if (dmsg_init("/ui")) {
		exit(-1);
	}
	dmsg_setup("/ui/run", ui_run);
	return 0;
}
