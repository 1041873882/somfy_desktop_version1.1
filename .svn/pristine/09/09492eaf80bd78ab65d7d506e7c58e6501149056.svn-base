#ifndef __SYS_COMMAND_H__
#define __SYS_COMMAND_H__

#include "types.h"
#include <time.h>

#define DATA_RESERVED  0xAA
#define DATA_BYTE_MAX  64

#define LED_ON         0x03
#define LED_OFF        0x01
#define LED_BLINK      0x0B
#define LED1           0x00
#define LED2           0x01
#define LED3           0x02
#define LED4           0x03
#define LED5           0x04
#define LED6           0x05
#define LED7           0x06
#define LED8           0x07
#define LED9           0x08

enum {
	MCU_DEFAULT_ADDR   = 0x00,
	MCU_DOOR_MACHINE   = 0x01,
	MCU_DOOR_ALL       = 0x0F,
	MCU_MONITOR_MASTER = 0x10,
	MCU_MONITOR_SLAVE  = 0x20,
	MCU_LOCAL_PIC      = 0x90,
	MCU_MONITOR_ALL    = 0xF0,
};

enum {
	QUERY_VERSION = 0x01, //旧版本程序， X1L不在使用
	QUERY_TEMP    = 0x02, 
};


enum {
	SETUP_TEMP   = 0x02,
};

enum {
	
	BODY_PIC_CLOSE  = 0x00,
	BODY_PIC_LEAVE  = 0x01,
	BODY_PIC_ACK    = 0xAA,
};

enum {
	
	BODY_EXT_ZERO  = 0x00,
	BODY_EXT_ONE   = 0x01,
	BODY_EXT_CYCLE = 0x02,
};

enum {
	LOCK_DOOR = 0x01,
	LOCK_GATE = 0x02,
};

enum {
	ACK_NONE   = 0x00,
	ACK_NORMAL = 0x01,
	ACK_BUSY   = 0x02,
	ACK_ERROR  = 0x03,
};

enum {
	EVENT_DEFAULT    = 0x00,
	EVENT_KEY        = 0x01, //Temporarily not processing
	EVENT_BODY       = 0x02,
	EVENT_CALL       = 0x03,
	EVENT_ANSWER     = 0x04,
	EVENT_HANGUP     = 0x05,
	EVENT_RTS        = 0x06,
	EVENT_TEMP       = 0x07,
	EVENT_BACK       = 0x08,
};

enum{
	EVENT_HANGUP_DEFAULT = 0x00,
	EVENT_HANGUP_TIMEOUT = 0x01,
};

enum {
	SWITCH_MACTH    = 0x01,
	SWITCH_CHANGE   = 0x02,
	SWITCH_CONTROL  = 0x03,
	SWITCH_TEST     = 0x04,
};


enum {
	TIME_01S   = 0x01,
	TIME_02S   = 0x02,
	TIME_05S   = 0x05,
	TIME_10S   = 0x0A,
	TIME_21S   = 0x15,
	TIME_45S   = 0x2d,
	TIME_51S   = 0x33,
	TIME_120S  = 0x78,
	TIME_121S  = 0x79,
	TIME_123S  = 0x7B,
	TIME_126S  = 0x7E,
};

class SysCommand {
public:
	SysCommand();
	~SysCommand();
	
	enum Type {
		NONE,
		SWITCH,
		BUSY,
		QUERY,
		RINGING,
		TALKING,
		MONITORING,
	};
	
	enum {
		ANSWER_NONE     = 0x00,
		ANSWER_CALL     = 0x01,
		ANSWER_MONITOR  = 0x02,
		ANSWER_GATE     = 0x03,
		ANSWER_DOOR     = 0x04,
		ANSWER_MEASURE  = 0x05,
	};
	
	struct {
		int state; // 0：without response; 1：response
		uint8_t cmd;
		uint8_t ta;
		uint8_t sa;
		uint8_t data[DATA_BYTE_MAX];
		int length;
	} SendData;

	uint8_t getCurrentMonitor();
	uint8_t getAnotherMonitor();
	
	int sendAckRsp(uint8_t dstAddr,uint8_t ackState);
	int sendAckRsp(uint8_t dstAddr,uint8_t *data,int length);
	int sendAckRsp(uint8_t srcAddr,uint8_t dstAddr,uint8_t ackState);
	
	int sendLockReq(uint8_t lockState, int answerState);
	int sendEventAck();

	int sendCallReq(uint8_t bellTime,uint8_t intercomTime);
	int sendHangupReq(uint8_t dstAddr,int state);
	int sendAnswerReq(uint8_t dstAddr,int state); 
	int sendMonitorReq(uint8_t intercomTime,int state);
	int sendQueryTemeratureReq();
	int sendMasterQueryReq();
	int sendSlaveNoticeRsp();
	int sendSlaveQueryReq();
	
	int sendEventBodyReq(int frequency);
	int sendEventCallReq();
	int sendEventTempReq();
	int sendEventHangupReq(int state);
	int sendEventHangupReq(uint8_t timeout, int state);
	int sendEventAnswerReq(uint8_t intercomTime,int state);
	int sendEventBackReq();

	//RTS
	int sendSwitchMatch(uint8_t channel);
	int sendSwitchChange(uint8_t channel);
	int sendSwitchControl(uint8_t channel,uint8_t position);
	int sendSwitchChnCheckTest();

	int sendLedControl(uint8_t *led);

	void recvAckHandle(uint8_t *data);
	void recvCallHandle(const uint8_t *data);
	void recvHangupHandle(const uint8_t *data);
	void recvSetupHandle(const uint8_t *data);
	void recvLockHandle(const uint8_t *data);
	void recvMonitorHandle(const uint8_t *data);
	void recvAnswerHandle(const uint8_t *data);
	void recvMasterQueryHandle(const uint8_t *data);
	void recvSlaveNoticeHandle(const uint8_t *data);
	void recvSlaveQueryHandle(const uint8_t *data);
	
	void recvHangupByMonitorHandle(const uint8_t *data);
	void recvHangupByDoorHandle(const uint8_t *data);
	void recvCallByDoorHandle(const uint8_t *data);
	void recvCallByMonitorHandle(const uint8_t *data);
	void recvAnswerByMonitorHandle(const uint8_t *data);
	void recvHandupByMoniotAllHandle(const uint8_t *data);
	
	void recvSetupOfTempHandle(const uint8_t *data);
	
	void recvEventHandle(const uint8_t *data);
	void recvEventBodyHandle(const uint8_t *data);
	void recvEventCallHandle(const uint8_t *data);
	void recvEventTempHandle(const uint8_t *data);
	void recvEventBackHandle(const uint8_t *data);
	void recvEventHangupHandle(const uint8_t *data);
	void recvEventRTSHandle(const uint8_t *data);
	void recvEventAnswertHandle(const uint8_t *data);
	int  judgeRecvEventMsg(const uint8_t *data, uint8_t data2, uint8_t data3, uint8_t data4);
	
	Type m_mode;
private:
	int m_callState;
};

extern SysCommand command;

#endif
