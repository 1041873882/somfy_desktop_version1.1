#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <time.h>
#include "types.h"



class TestUtil{

public:
	
	TestUtil();
	~TestUtil();
	
	int monitorCallMonitor(int state, int testNum);
	int monitorHangupMonitor(int state, int testNum);
	int monitorAnswerMonitor(int state, int testNum);
	int doorCallMonitor(int state, int testNum);
	int monitorAnswerDoor(int state, int testNum);

	int delaySendOfBusy();
	int deliveryParamter(const char *argv);

	//test RTS Channel
	int checkChannelState(uint8_t data);
	int m_busy;

	//屏幕老化测试
	int m_colorCycle;
	time_t m_cycleTime;
	
	//分机呼叫分机
	time_t m_delayTime;
	int m_monitorCall;
	int m_monitorAnswer;

	//test temperature
	int m_temperature;
	
	int m_chnState[5];
	int m_RTSCurrentChn;
	int m_inquireRTSAck;
	
private:

	time_t m_callTime;
	time_t m_hangupTime;
	time_t m_answerTime;
	
	int m_noneState;
	int m_hangupState;
	int m_answerState;

	int m_callNum;
	int m_hangupNum;
	int m_answerNum;
	
};

extern TestUtil testUtil;


#endif


