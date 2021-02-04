#include "wMain.h"

#include "SysSound.h"
#include "DebugUtil.h"
#include "SysCommand.h"
#include "TestUtil.h"

TestUtil testUtil;

TestUtil::TestUtil()
{
	m_callTime = 0;
	m_hangupTime = 0;
	m_answerTime = 0;
	m_delayTime = 0;

	m_noneState = 0;
	m_hangupState = 0;
	m_answerState = 0;

	m_callNum = 0;
	m_hangupNum = 0;
	m_answerNum = 0;
	m_busy  = 0;
	m_monitorCall = 0;
	m_monitorAnswer = 0;

	m_colorCycle = 0;
	m_cycleTime = 0;

	for (int i = 0; i < 5; i++) {
		m_chnState[i] = 0;
	}
	m_RTSCurrentChn = 0;
	m_inquireRTSAck = 0;
}

TestUtil::~TestUtil()
{
}

int TestUtil::monitorCallMonitor(int state, int testNum)
{
	if (!state || m_callNum > testNum) {
		return 0;
	}
	if (m_monitorCall && m_monitorAnswer) {
		return 0;
	}
	if (command.m_mode == command.NONE) {
		if (!m_noneState) {
			m_noneState = 1;
			m_callTime = time(NULL);
		}
		if (labs( time(NULL)- m_callTime) > 20 && m_noneState) {
			int isBusy = delaySendOfBusy();
			if (!isBusy) {
				PRINT_DEBUG("delay \n");
				return 0;
			}
			m_callNum++;
			PRINT_DEBUG("---> call num: %d \n",m_callNum);
			command.sendCallReq((TIME_45S + TIME_01S), TIME_120S);
		}
	} else {
		m_noneState = 0;
	}
	sleep(5);
	return 1;
}

//呼叫时挂断还是接听时挂断
int TestUtil::monitorHangupMonitor(int state, int testNum)
{
	if (!state || m_hangupNum > testNum) {
		return 0;
	}
	if (m_monitorCall && m_monitorAnswer) {
		return 0;
	}
	if (command.m_mode >= command.RINGING) {
		if (!m_hangupState) {
			m_hangupState = 1;
			m_hangupTime = time(NULL);
		}
		if (labs( time(NULL) - m_hangupTime) > 15 && m_hangupState) {
			int isBusy = delaySendOfBusy();
			if (!isBusy) {
				PRINT_DEBUG("delay\n");
				return 0;
			}
			m_hangupNum ++;
			PRINT_DEBUG("---> hangup num: %d \n",m_hangupNum);
			if (command.m_mode == command.TALKING) {
				sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
			}
			sound.stop();
			wMain *main = new wMain();
			main->show();
			uint8_t dstAddr = command.getAnotherMonitor();
			command.sendHangupReq(dstAddr,command.NONE);
		}
	} else {
		m_hangupState = 0;
	}
	sleep(5);
	return 1;
}

int TestUtil::monitorAnswerMonitor(int state, int testNum)
{
	if (!state || m_answerNum > testNum) {
		return 0;
	}
	if (m_monitorCall && m_monitorAnswer) {
		return 0;
	}
	if (command.m_mode == command.RINGING) {
		if (!m_answerState) {
			m_answerState = 1;
			m_answerTime = time(NULL);
		}
		if (labs(time(NULL) - m_answerTime) > 10 && m_answerState) {
			int isBusy = delaySendOfBusy();
			if (!isBusy) {
				PRINT_DEBUG("delay \n");
				return 0;
			}
			m_answerNum++;
			PRINT_DEBUG("---> answer num: %d \n", m_hangupNum);
			sound.stop();
			uint8_t dstAddr = command.getAnotherMonitor();
			command.sendAnswerReq(dstAddr,command.ANSWER_NONE);
		}
	} else {
		m_answerState = 0;
	}
	sleep(5);
	return 1;
}

int TestUtil::delaySendOfBusy()
{
	if (m_busy && labs(time(NULL) - m_delayTime) > 60) {
		m_busy = 0;
		return 0;
	}
	return 1;
}

int TestUtil::deliveryParamter(const char *argv)
{
	if (!strcmp(argv, "-a")) { //分机呼叫分机，分机呼叫
		PRINT_DEBUG("-a\n");
		testUtil.m_monitorCall = 1;
	} else if (!strcmp(argv, "-b")) { //分机呼叫分机，分机接听并挂机
		PRINT_DEBUG("-b\n");
		testUtil.m_monitorAnswer = 1;
	}
	return 0;
}

int TestUtil::checkChannelState(uint8_t data)
{
	for (int i = 0; i < 5; i++) {
		m_chnState[i] = (data && (1 << i)) ? 1 : 0;
		PRINT_DEBUG("channel %d and state: %d \n", i+1, m_chnState[i]);
	}
	return 1;
}
