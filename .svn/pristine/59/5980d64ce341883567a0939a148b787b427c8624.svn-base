#ifndef __SYS_MCU_H__
#define __SYS_MCU_H__

#include "sys.h"
#include "sFifo.h"


enum {
	MCU_POWER_ON     = 0x01, //供电，未使用
	MCU_POWER_OFF    = 0x03, //断电，未使用
	MCU_ACK          = 0x04, //回复
	MCU_QUERY        = 0x05, //参数读取
	MCU_SETUP        = 0x06, //参数设置
	MCU_EVENT        = 0x07, //事件通知
	MCU_CALL         = 0x11, //呼叫
	MCU_HANGUP       = 0x12, //挂机
	MCU_ANSWER       = 0x13, //摘机
	MCU_MONITOR      = 0x14, //监视
	MCU_LOCK         = 0x15, //开门/锁
	MCU_SWITCH       = 0x41, //智能开关
	MCU_MASTER_QUERY = 0xA0, //主机查询
	MCU_SLAVE_NOTICE = 0xB0, //副机回复
	MCU_SLAVE_QUERY  = 0xC0, //副机查询
	MCU_LED          = 0x51, //LED
	MCU_KEY          = 0x61, //按键状态
};

class SysMCU {
public:
	SysMCU();
	~SysMCU();
	
	int start(void);
	void process(void);
	int tx(uint8_t cmd, uint8_t ta, uint8_t sa, uint8_t *data, int length);
	void analyze(void);
	int filterData(const uint8_t *data, int length);
public:
	int m_tty;
	sFifo m_rx;
};

extern SysMCU mcu;

#endif
