
#ifndef __TEMPORARY_UTIL_H__
#define __TEMPORARY_UTIL_H__

#include "types.h"


class TemporaryUtil{
	
public:
	
	TemporaryUtil();
	virtual ~TemporaryUtil();
	
	enum {
	
		INTER_DEFAULT  = 0x00,
		INTER_MTOD     = 0x01, //分机与门口机通信
		INTER_MTOM     = 0x02, //分机与分机通信
	};
	
	void setTempValue(int value);
	int  getTempValue();

	void setTempState(int state);
	int  getTempState();

	void setTempChange(int state);
	int  getTempChange();
	
	void ledLighting();
	void setMonitorOffLine();
	void setMonitorToMaster();
	int  compareMonitorAddr(uint8_t *addr);
	void monitorStart();
	
	void prepareSwitchingMonitor(uint8_t *data);
	void cancelBusy();

	//Get the temperature every 30 minutes
	void getTemperature();

	void resendData();
	int  sendDataIsQueryMonitor();
	void sendMonitorQuery();
	
	void setSendData(uint8_t cmd,uint8_t ta,uint8_t sa,uint8_t *data,uint8_t length);
	void setSendState(int state);

	//recv ack handle
	int ackByCallMonitor(uint8_t *data);
	int ackByHangupMonitor(uint8_t *data);
	int ackByAnswerMonitor(uint8_t *data);
	int ackBySlaveQuery(uint8_t *data);
	
	//int ackByCallDoor(uint8_t *data);
	int ackByHangupDoor(uint8_t *data);
	int ackByAnswerDoor(uint8_t *data);
	int ackByMonitorDoor(uint8_t *data);
	int ackByLockDoor(uint8_t *data);
	
	int ackByEventCall(uint8_t *data);
	int ackByEventAnswer(uint8_t *data);
	int ackByEventHangup(uint8_t *data);
	int ackByEventTemp(uint8_t *data);
	int ackByEventBack(uint8_t *data);
	
	int ackByBusyOrError(uint8_t *data);
	int judgeAckMsg(uint8_t *data, uint8_t cmd, uint8_t ta, uint8_t data0, uint8_t data3, uint8_t data4);
	
	int m_ScreenState;
	int m_answerState;
	int m_callState; // 0 is default; 1 is loadCall
	int m_talkState;
	int m_testState;//隐藏测试功能下进入，监视界面
	int m_interCall;
	int m_firstCall;
	
	uint8_t m_outSecond; //对讲超时时长
	time_t m_timing; //对讲（振铃/通话）开始时间点
	time_t m_tempTime; //获取温度时间点
	time_t m_ledTime; // led灯闪烁时间点
private:
	int tempValue;
	int tempState;
	int tempChange;

	int resendCount;
	struct timeval send_tv;
};

extern TemporaryUtil temporary;

#endif