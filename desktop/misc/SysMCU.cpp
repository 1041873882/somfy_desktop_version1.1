
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <strings.h>
#include <sys/time.h>
#include "mEvent.h"
#include "SysCommand.h"
#include "DebugUtil.h"
#include "TemporaryUtil.h"
#include "SysMCU.h"

SysMCU mcu;

//单条数据最长字节数
#define DATA_SIZE_MAX    15

static void *dnake_mcu_thread(void *)
{
	mcu.process();
	return NULL;
}

SysMCU::SysMCU()
{
	m_tty = -1;
	m_rx.realloc(512);
}

SysMCU::~SysMCU()
{
}

int SysMCU::start(void)
{
	m_tty = ::open("/dev/ttyS2", O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_tty < 0) {
		fprintf(stderr, "SysMCU::start open /dev/ttyS2 err.\n");
		return -1;
	}
	
	struct termios opt;
	fcntl(m_tty, F_SETFL, 0);
	bzero(&opt, sizeof(opt));
	opt.c_cflag |= CLOCAL | CREAD;
	opt.c_cflag &= ~CSIZE;
	opt.c_cflag |= CS8;
	opt.c_cflag &= ~PARENB;
	opt.c_cflag &= ~CSTOPB;
	opt.c_cc[VTIME] = 0;
	opt.c_cc[VMIN]  = 0;

	cfsetispeed(&opt, B9600);
	cfsetospeed(&opt, B9600);

	tcflush(m_tty, TCIFLUSH);
	tcsetattr(m_tty, TCSANOW, &opt);

	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_mcu_thread, NULL) != 0) {
		perror("pthread_create dnake_mcu_thread!\n");
	}
	fprintf(stderr, "SysMCU::start open /dev/ttyS2 ok.\n");
	return 0;
}

void SysMCU::process(void)
{
	while (1) {
		struct timeval tv;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(m_tty, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 100*1000;
		select(m_tty+1, &rfds, NULL, NULL, &tv);
		if (FD_ISSET(m_tty, &rfds)) {
			uint8_t d[128];
			int r = ::read(m_tty, d, sizeof(d));
			if (r > 0) {
#if 1
				fprintf(stderr, "<--\n");
				for (int i=0; i<r; i++)
					fprintf(stderr, "%02X ", d[i]);
				fprintf(stderr, "\n");
#endif
				m_rx.put(d, r);
				m_rx.copy(d, 1);
				int filter = this->filterData(d, r);
				if (d[0] != 0xA5 || filter) {//同步头错误
					m_rx.flush();
				}
			}
		}
		this->analyze();
		temporary.resendData();
	}
}

int SysMCU::tx(uint8_t cmd, uint8_t ta, uint8_t sa, uint8_t *data, int length)
{
	uint8_t d[128];
	d[0] = 0xA5;
	d[1] = cmd;
	d[2] = length;
	d[3] = ta;
	d[4] = sa;
	for (int i=0; i<length; i++) {
		d[5+i] = data[i];
	}
	d[5+length] = 0;
	for (int i=1; i<(5+length); i++) {
		d[5+length] += d[i];
	}
	d[5+length] ^= cmd;
#if 1
	fprintf(stderr, "-->\n");
	for (int i=0; i<(5+length+1); i++) {
		fprintf(stderr, "%02X ", d[i]);
	}
	fprintf(stderr, "\n");
#endif
	return ::write(m_tty, d, 5+length+1);
}

void SysMCU::analyze(void)
{
	uint8_t d[128];
	if (!m_rx.copy(d, 6)) {
		return;
	}
	int sz = d[2]+6;
	if (sz <= m_rx.used()) {
		m_rx.get(d, sz);
		uint8_t ck = 0;
		for (int i=1; i<(sz-1); i++) {
			ck += d[i];
		}
		ck ^= d[1];
		// if (ck != d[sz-1]) { //校验错误
		// 	m_rx.flush();
		// 	return;
		// }
		//PRINT_DEBUG("start analyze data \n");
		//修改地址期间，接收到另一台分机发送的指令
		if (command.m_mode == command.SWITCH) {
			temporary.prepareSwitchingMonitor(d);
		}
		//If the monitor address is the same, modify the current monitor address
		int isSame = temporary.compareMonitorAddr(d);
		if (isSame) {
			PRINT_DEBUG("the same addr by error\n");
			return;
		}
		//处理MCU发往主CPU命令
		switch (d[1]) {
		case MCU_ACK:
			command.recvAckHandle(d);
			break;
		case MCU_SETUP:
			command.recvSetupHandle(d);
			break;
		case MCU_EVENT:
			command.recvEventHandle(d);
			break;
		case MCU_CALL:
			command.recvCallHandle(d);
			break;
		case MCU_HANGUP:
			command.recvHangupHandle(d);
			break;
		case MCU_ANSWER:
			command.recvAnswerHandle(d);
			break;
		case MCU_MONITOR:
			command.recvMonitorHandle(d);
			break;
		case MCU_LOCK:
			//The monitor cannot receive the command
			command.recvLockHandle(d);
			break;
		case MCU_SWITCH:
			//The data is only sent,
			//and will not receive the message sent by the RTS device.
			break;
		case MCU_MASTER_QUERY:
			command.recvMasterQueryHandle(d);
			break;
		case MCU_SLAVE_NOTICE:
			command.recvSlaveNoticeHandle(d);
			break;
		case MCU_SLAVE_QUERY:
			command.recvSlaveQueryHandle(d);
			break;
		case MCU_KEY:
			sys_key_report(d[6], d[5]);
			break;
		}
	}
}

//过滤一条数据中包含多条指令或且指令长度超出
int SysMCU::filterData(const uint8_t *data, int length)
{
	int startData = 0;
	for (int i = 0; i < length; i++) {
		if (data[i] == 0xA5) {
			startData++;
		}
	}
	if (startData > 1 && length > DATA_SIZE_MAX) {
		fprintf(stderr,"---> Filter out this data\n");
		return 1;
	}
	return 0;
}
