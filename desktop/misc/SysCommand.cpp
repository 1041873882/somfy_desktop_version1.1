#include "wMain.h"
#include "wTalk.h"
#include "wDateSetting.h"
#include "wTestChoose.h"

#include "mFB.h"
#include "DebugUtil.h"
#include "BeanUtil.h"
#include "PictureUtil.h" 
#include "TestUtil.h"
#include "SysMCU.h"
#include "SysCommand.h"
#include "SysSound.h"
#include "TemporaryUtil.h"

SysCommand command;

SysCommand::SysCommand()
{
	m_mode = NONE;
	m_callState = 0;
}

SysCommand::~SysCommand()
{
}

uint8_t SysCommand::getCurrentMonitor()
{
	uint8_t monitor = 0;
	int monitorState = beanUtil.getMeasureMonitorState();
	if (monitorState) {
		monitor = MCU_MONITOR_MASTER;
	} else {
		monitor = MCU_MONITOR_SLAVE;
	}
	return monitor;
}

uint8_t SysCommand::getAnotherMonitor()
{
	uint8_t monitor = 0;
	int monitorState = beanUtil.getMeasureMonitorState();
	if (monitorState) {
		monitor = MCU_MONITOR_SLAVE;
	} else {
		monitor = MCU_MONITOR_MASTER;
	}
	return monitor;
}

int SysCommand::sendAckRsp(uint8_t dstAddr, uint8_t ackState)
{
	uint8_t data[3];
	data[0] = ackState;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	uint8_t  monitor = getCurrentMonitor();
	return mcu.tx(MCU_ACK, dstAddr, monitor, data, 3);
}

int SysCommand::sendAckRsp(uint8_t dstAddr, uint8_t *data, int length)
{
	uint8_t  monitor = getCurrentMonitor();
	return mcu.tx(MCU_ACK, dstAddr, monitor, data, length);
}

int SysCommand::sendAckRsp(uint8_t srcAddr, uint8_t dstAddr, uint8_t ackState)
{
	uint8_t data[3];
	data[0] = ackState;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	return mcu.tx(MCU_ACK, dstAddr, srcAddr, data, 3);
}

int SysCommand::sendLockReq(uint8_t lockState, int answerState)
{
	uint8_t data[4];
	data[0] = lockState;
	int openTimeId = beanUtil.getOpenTimeId();
	if (openTimeId == 1) {
		data[1] = TIME_05S;
	} else if (openTimeId == 2) {
		data[1] = TIME_10S;
	} else {
		data[1] = TIME_02S;
	}
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;

	temporary.m_answerState = answerState;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_LOCK, MCU_DOOR_MACHINE, monitor, data, 4);
	return mcu.tx(MCU_LOCK, MCU_DOOR_MACHINE, monitor, data, 4);
}

//主副分机呼叫
int SysCommand::sendCallReq(uint8_t bellTime, uint8_t intercomTime)
{
	uint8_t data[4];
	data[0] = bellTime;
	data[1] = intercomTime;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;
	
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_CALL, MCU_MONITOR_ALL, monitor, data, 4);
	return mcu.tx(MCU_CALL, MCU_MONITOR_ALL, monitor, data, 4);
}

int SysCommand::sendMonitorReq(uint8_t intercomTime, int state)
{
	uint8_t data[4];
	data[0] = intercomTime;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;

	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_MONITOR, MCU_DOOR_ALL, monitor, data, 4);
	return mcu.tx(MCU_MONITOR, MCU_DOOR_ALL, monitor, data, 4);
}

int SysCommand::sendAnswerReq(uint8_t dstAddr, int state)
{
	uint8_t data[4];
	data[0] = DATA_RESERVED;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;
	
	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_ANSWER, dstAddr, monitor, data, 4);
	return mcu.tx(MCU_ANSWER, dstAddr, monitor, data, 4);
}

int SysCommand::sendHangupReq(uint8_t dstAddr, int state)
{
	uint8_t data[4];
	data[0] = DATA_RESERVED;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;

	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_HANGUP, dstAddr, monitor, data, 4);
	return mcu.tx(MCU_HANGUP, dstAddr, monitor, data, 4);
}

int SysCommand::sendEventAnswerReq(uint8_t intercomTime, int state)
{
	uint8_t data[2];
	data[0] = EVENT_ANSWER;
	data[1] = intercomTime;
	
	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
}

int SysCommand::sendEventHangupReq(int state)
{
	uint8_t data[2];
	data[0] = EVENT_HANGUP;
	data[1] = DATA_RESERVED;
	
	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
}

/*
*  解决振铃超时，主分机通知副分机，
*  副分机可能出现振铃延迟时间差问题
*/
int SysCommand::sendEventHangupReq(uint8_t timeout, int state)
{
	uint8_t data[2];
	data[0] = EVENT_HANGUP;
	data[1] = timeout;
	
	temporary.m_answerState = state;
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
}

int SysCommand::sendEventBackReq()
{
	uint8_t data[2];
	data[0] = EVENT_BACK;
	data[1] = DATA_RESERVED;
	
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
}

/*
* Only the master monitor sends the outdoor temperature to the slave monitor
*
* Will subtract the current temperature difference and pass it to another monitor
*
* 防止没有获取到温度，将初始化温度直接传递给副分机
*/
int SysCommand::sendEventTempReq()
{
	uint8_t data[4];
	int tempValue = temporary.getTempValue();
	if (tempValue >= 90) {
		PRINT_DEBUG("Did not get the temperature\n");
		return -1;
	}
	// tempValue = tempValue - beanUtil.getMeasureTempDeviValue();
	// if (tempValue >= 0) {
	// 	data[1] = 0x00;
	// } else {
	// 	data[1] = 0x01;
	// }
	data[2] = labs(tempValue);
	data[3] = 0x00;
	data[0] = EVENT_TEMP;
	temporary.setSendState(1);
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, MCU_MONITOR_MASTER, data, 4);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, MCU_MONITOR_MASTER, data, 4);
}

int SysCommand::sendQueryTemeratureReq()
{
	uint8_t data[2];
	data[0] = TIME_02S;
	data[1] = DATA_RESERVED;
	
	temporary.m_tempTime = time(NULL);
	temporary.setSendState(1);
	temporary.setSendData(MCU_QUERY, MCU_DOOR_ALL, MCU_MONITOR_MASTER, data, 2);
	return mcu.tx(MCU_QUERY, MCU_DOOR_ALL, MCU_MONITOR_MASTER, data, 2);
}

int SysCommand::sendMasterQueryReq()
{
	uint8_t data[2];
	data[0] = DATA_RESERVED;
	data[1] = DATA_RESERVED;
	
	temporary.setSendState(1);
	temporary.setSendData(MCU_MASTER_QUERY, MCU_MONITOR_ALL, MCU_MONITOR_MASTER, data, 2);
	return mcu.tx(MCU_MASTER_QUERY, MCU_MONITOR_ALL, MCU_MONITOR_MASTER, data, 2);
}

int SysCommand::sendSlaveNoticeRsp()
{
	uint8_t data[2];
	data[0] = DATA_RESERVED;
	data[1] = DATA_RESERVED;
	
	return mcu.tx(MCU_SLAVE_NOTICE, MCU_MONITOR_MASTER, MCU_MONITOR_SLAVE, data, 2);
}

int SysCommand::sendSlaveQueryReq()
{
	uint8_t data[2];
	data[0] = DATA_RESERVED;
	data[1] = DATA_RESERVED;
	
	temporary.setSendState(1);
	temporary.setSendData(MCU_SLAVE_QUERY, MCU_MONITOR_ALL, MCU_MONITOR_SLAVE, data, 2);
	return mcu.tx(MCU_SLAVE_QUERY, MCU_MONITOR_ALL, MCU_MONITOR_SLAVE, data, 2);
}

int SysCommand::sendEventBodyReq(int frequency)
{
	uint8_t data[2];
	data[0] = EVENT_BODY;
	data[1] = frequency;
	
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_LOCAL_PIC, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_LOCAL_PIC, monitor, data, 2);
}

int SysCommand::sendEventCallReq()
{
	uint8_t data[2];
	data[0] = EVENT_CALL;
	data[1] = DATA_RESERVED;
	
	temporary.setSendState(1);
	uint8_t monitor = getCurrentMonitor();
	temporary.setSendData(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
	return mcu.tx(MCU_EVENT, MCU_MONITOR_ALL, monitor, data, 2);
}

int SysCommand::sendSwitchMatch(uint8_t channel)
{
	uint8_t data[4];
	data[0] = SWITCH_MACTH;
	data[1] = channel;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;
	
	uint8_t monitor = getCurrentMonitor();
	return mcu.tx(MCU_SWITCH, MCU_LOCAL_PIC, monitor, data, 4);
}

int SysCommand::sendSwitchChange(uint8_t channel)
{
	uint8_t data[4];
	data[0] = SWITCH_CHANGE;
	data[1] = channel;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;
	
	uint8_t monitor = getCurrentMonitor();
	return mcu.tx(MCU_SWITCH, MCU_LOCAL_PIC, monitor, data, 4);
}

int SysCommand::sendSwitchControl(uint8_t channel, uint8_t position)
{
	uint8_t data[4];
	data[0] = SWITCH_CONTROL;
	data[1] = channel;
	data[2] = position;
	data[3] = DATA_RESERVED;

	uint8_t monitor = getCurrentMonitor();
	return mcu.tx(MCU_SWITCH, MCU_LOCAL_PIC, monitor, data, 4);
}

int SysCommand::sendLedControl(uint8_t *led)
{
	uint8_t data[5];
	data[0] = led[0] << 4 | led[1];
	data[1] = led[2] << 4 | led[3];
	data[2] = led[4] << 4 | led[5];
	data[3] = led[6] << 4 | led[7];
	data[4] = led[8] << 4;
	return mcu.tx(MCU_LED, MCU_LOCAL_PIC, MCU_MONITOR_ALL, data, 5);
}

int SysCommand::sendSwitchChnCheckTest()
{
	uint8_t data[4];
	data[0] = SWITCH_TEST;
	data[1] = DATA_RESERVED;
	data[2] = DATA_RESERVED;
	data[3] = DATA_RESERVED;
	
	uint8_t monitor = getCurrentMonitor();
	return mcu.tx(MCU_SWITCH, MCU_LOCAL_PIC, monitor, data, 4);
}

void SysCommand::recvAckHandle(uint8_t *data)
{
	int ack = 0;
	if (data[3] != SendData.sa) {
		PRINT_DEBUG("recv addr is not same currentMonitor addr \n");
		return;
	}
	PRINT_DEBUG("state: %d and cmd: %02x and srcAddr: %02x and command data[0]: %02x \n",
						command.SendData.state, command.SendData.cmd, data[4], command.SendData.data[0]);
	//Abnormal situation, first dealt with
	ack = temporary.ackByBusyOrError(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByCallMonitor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByHangupMonitor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByAnswerMonitor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByHangupDoor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByAnswerDoor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByMonitorDoor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByLockDoor(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByEventCall(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByEventAnswer(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByEventHangup(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByEventTemp(data);
	if (ack) {
		return;
	}
	ack = temporary.ackByEventBack(data);
	if (ack) {
		return;
	}
	ack = temporary.ackBySlaveQuery(data);
	if (ack) {
		return;
	}
}

void SysCommand::recvCallHandle(const uint8_t *data)
{
	//fprintf(stdout,"dst addr is %02x and src addr %02x \n",data[3],data[4]);
	switch (data[4]) {
	case MCU_DOOR_MACHINE:
		recvCallByDoorHandle(data);
		break;
	case MCU_MONITOR_MASTER:
	case MCU_MONITOR_SLAVE:
		recvCallByMonitorHandle(data);
		break;
	}
}

void SysCommand::recvAnswerHandle(const uint8_t *data)
{
	switch (data[4]) {
	case MCU_DOOR_MACHINE:
		//Will not receive the answer from the door machine
		break;
	case MCU_MONITOR_MASTER:
	case MCU_MONITOR_SLAVE:
		// recvAnswerByMonitorHandle(data);
		break;
	}
}

void SysCommand::recvAnswerByMonitorHandle(const uint8_t *data)
{
	uint8_t currentMonitor = getCurrentMonitor();
	uint8_t anotherMonitor = getAnotherMonitor();
	if (data[4] != anotherMonitor || data[3] != currentMonitor || data[2] != 4) {
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	if (m_mode > RINGING) {//避免数据重发
		PRINT_DEBUG("recv resend data \n");
		return;
	}
	//防止两台同时超时，同时发送挂机指令
	temporary.m_outSecond = TIME_120S + TIME_02S;
	temporary.m_timing = time(NULL);
	m_mode = TALKING;
	sound.microphoneOnOff(1);
	//延迟避免总线通信产生噪声
	usleep(100*1000);
	sound.setVolumeValue(AUDIO_CHANNEL);
}

void SysCommand::recvHangupHandle(const uint8_t *data)
{
	//PRINT_DEBUG("dst addr is %02x and src addr %02x \n",data[3],data[4]);
	switch (data[4]) {
	case MCU_DOOR_MACHINE:
		recvHangupByDoorHandle(data);
		break;
	case MCU_MONITOR_MASTER:
	case MCU_MONITOR_SLAVE:
		recvHangupByMonitorHandle(data);
		break;
	}
}

void SysCommand::recvHangupByMonitorHandle(const uint8_t *data)
{
	if (!(data[3] == MCU_MONITOR_MASTER || data[3] == MCU_MONITOR_SLAVE || data[3] == MCU_MONITOR_ALL)) {
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	if (m_mode >= RINGING) {
		if (m_mode == RINGING) {
			sound.stop();
		}
		if (m_mode == TALKING) {
			sound.microphoneOnOff(0);
		}
		sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
		temporary.m_interCall = temporary.INTER_DEFAULT;
		wMain *main = new wMain();
		main->show();
	}
	m_mode = NONE;
}

//This happens when the master monitor is offline or the monitor that has been answered suddenly goes offline.
void SysCommand::recvHangupByDoorHandle(const uint8_t *data)
{
	int currentMonitor = command.getCurrentMonitor();
	if (data[3] != currentMonitor) {
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	//避免分机间呼叫，分机没有响应，门口机发送挂机指令
	if (temporary.m_interCall == temporary.INTER_MTOM) {
		return;
	}
	if (m_mode == RINGING || m_mode >= TALKING) {
		if (m_mode == RINGING) {
			sound.stop();
		}
		if (m_mode == TALKING) {
			sound.microphoneOnOff(0);
		}
		temporary.m_interCall = temporary.INTER_DEFAULT;
		if (!temporary.m_testState) {
			sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
			wMain *main = new wMain();
			main->show();
		} else {
			wTestChoose *wChoose = new wTestChoose();
			wChoose->show();
			temporary.m_testState = 0;
		}
	}
	m_mode = NONE;
}

void SysCommand::recvCallByDoorHandle(const uint8_t *data)
{
	//主分机异常离线，门口机呼叫主分机未响应;但是如果分机与分机呼叫;
	int monitorState = beanUtil.getMeasureMonitorState();
	if (!monitorState) {
		PRINT_DEBUG("slave monitor not recv callEvent handle \n");
		//分机间通信时计数
		if (temporary.m_interCall == temporary.INTER_MTOM) {
			return;
		}
		m_callState ++;
		if (m_callState > 6) { //repeat
			sendSlaveQueryReq();
			m_callState = 0;
		}
		return;
	}
	//避免分机与分机通信，门口机呼叫进行干扰
	if (m_mode > NONE && temporary.m_interCall == temporary.INTER_MTOM) {
		PRINT_DEBUG("monitor are communicating with each other now \n");
		sendAckRsp(data[4], ACK_BUSY);
		return;
	} else if (m_mode == RINGING) { //避免数据重发
		PRINT_DEBUG("Resend data \n");
		sendAckRsp(data[4], ACK_NORMAL);
		return;
	} else if (m_mode > RINGING) { //避免已经接听，在进入振铃
		PRINT_DEBUG("undering answing\n");
		sendAckRsp(data[4], ACK_BUSY);
		return;
	}
	uint8_t sendData[3];
	sendData[0] = ACK_NORMAL;
	sendData[1] = TIME_51S;
	sendData[2] = TIME_126S;
	sendAckRsp(data[4], sendData, 3);
	//发送两条协议之间要进行间隔
	usleep(200*1000);
	int slaveState = beanUtil.getSlaveState();
	if (slaveState) {
		sendEventCallReq();
	}
	temporary.m_outSecond = TIME_45S;
	temporary.m_timing = time(NULL);
	temporary.m_interCall = temporary.INTER_MTOD;
	m_mode = RINGING;
	wTalk *talk = new wTalk();
	talk->show();
	//避免铃声播放时，门口机直接呼叫
	sound.stop();
	//等视频图像出现，在播放铃声
	sound.ringing(2);
}

void SysCommand::recvCallByMonitorHandle(const uint8_t *data)
{
	if (data[3] != MCU_MONITOR_ALL || data[2] != 4) {
		return;
	}
	//避免门口机与分机通信，进行干扰
	if (m_mode > NONE && temporary.m_interCall == temporary.INTER_MTOD) {
		PRINT_DEBUG("monitor intercom door and now is busy \n");
		sendAckRsp(data[4], ACK_BUSY);
		return;
	} else if (m_mode >= RINGING) { //避免数据重发
		PRINT_DEBUG("data resend \n");
		sendAckRsp(data[4], ACK_NORMAL);
		return;
	} else if (m_mode > RINGING) { //避免已经接听，在进入振铃
		PRINT_DEBUG("when Talking and now is busy \n");
		sendAckRsp(data[4], ACK_BUSY);
		return;
	}
	PRINT_DEBUG("start monitor intercome\n");
	sendAckRsp(data[4], ACK_NORMAL);
	temporary.m_outSecond = data[5];
	temporary.m_timing = time(NULL);
	temporary.m_interCall = temporary.INTER_MTOM;
	command.m_mode = command.RINGING;
	// wSpeakCall *call = new wSpeakCall();
	// call->show();
	sound.stop();
	sound.ringing(100);
}

void SysCommand::recvSetupHandle(const uint8_t *data)
{
	switch (data[5]) {
	case SETUP_TEMP:
		recvSetupOfTempHandle(data);
		break;
	}
}

/*
*  门口机上报的温度与实际温度差
*  程序上进行调整 6℃
*  -6℃
*/
void SysCommand::recvSetupOfTempHandle(const uint8_t *data)
{
	int currentMonitor = getCurrentMonitor();
	if (data[3] != MCU_MONITOR_MASTER || data[4] != MCU_DOOR_MACHINE 
									 || data[3] != currentMonitor
									 || data[2] != 4) {
		return;
	}
	//Avoid changing the address currently, but have not changed it yet
	if (m_mode == SWITCH) {
		return;
	}
	PRINT_DEBUG("recv door temperature\n");
	temporary.setSendState(0);
	int tmpValue = 0;
	tmpValue = data[7];
	if (data[8] >= 5) {
		tmpValue = tmpValue + 1;
	}
	if (data[6]) {
		tmpValue = 0 - tmpValue;
	}
	tmpValue = tmpValue - 6;
	// int tempDevi = beanUtil.getMeasureTempDeviValue();
	// tmpValue = tmpValue + tempDevi;
	temporary.setTempValue(tmpValue);
	temporary.setTempChange(1);
	int tempState = temporary.getTempState();
	if (!tempState) {
		temporary.setTempState(1);
	}
	sendMasterQueryReq();
}

void SysCommand::recvMonitorHandle(const uint8_t *data)
{
	//Temporarily will not receive the monitoring command of the door machine
}

void SysCommand::recvLockHandle(const uint8_t *data)
{
	/*
	*  Will receive the switch lock command of the slave monitor 
	*  but does not need to process
	*/
}

void SysCommand::recvMasterQueryHandle(const uint8_t *data)
{
	if (data[3] != MCU_MONITOR_ALL || data[4] != MCU_MONITOR_MASTER || data[2] != 2) {
		return;
	}
	int monitorState = beanUtil.getMeasureMonitorState();
	if (monitorState) {
		sendAckRsp(data[4], ACK_ERROR);
		return;
	}
	beanUtil.setMasterState(1);
	sendSlaveNoticeRsp();
	beanUtil.save();
}

void SysCommand::recvSlaveNoticeHandle(const uint8_t *data)
{
	if (data[3] != MCU_MONITOR_MASTER || data[4] != MCU_MONITOR_SLAVE || data[2] != 2) {
		return;
	}
	temporary.setSendState(0);
	beanUtil.setSlaveState(1);
	sendEventTempReq();
	beanUtil.save();
}

void SysCommand::recvSlaveQueryHandle(const uint8_t *data)
{
	if (data[3] != MCU_MONITOR_ALL || data[4] != MCU_MONITOR_SLAVE || data[2] != 2) {
		return;
	}
	uint8_t monitorState = getCurrentMonitor();
	if (monitorState != MCU_MONITOR_MASTER) {
		return;
	}
	sendAckRsp(data[4],ACK_NORMAL);
	beanUtil.setSlaveState(1);
	beanUtil.save();
	usleep(100*1000);
	sendEventTempReq();
}

void SysCommand::recvEventHandle(const uint8_t *data)
{
	switch (data[5]) {
	case EVENT_KEY:
		//not processing
		break;
	case EVENT_BODY:
		recvEventBodyHandle(data);
		break;
	case EVENT_CALL:
		recvEventCallHandle(data);
		break;
	case EVENT_ANSWER:
		recvEventAnswertHandle(data);
		break;
	case EVENT_HANGUP:
		recvEventHangupHandle(data);
		break;
	case EVENT_RTS:
		recvEventRTSHandle(data);
		break;
	case EVENT_TEMP:
		recvEventTempHandle(data);
		break;
	case EVENT_BACK:
		recvEventBackHandle(data);
		break;
	}
}

void SysCommand::recvEventCallHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 2, EVENT_DEFAULT, EVENT_DEFAULT);
	if (!result) {
		return;
	}
	if (m_mode > NONE) {
		sendAckRsp(data[4], ACK_BUSY);
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	//接收到主分机转发的门口机呼叫事件，复位门口机呼叫标志位
	if (m_callState) {
		m_callState = 0;
	}
	//避免主副分机同时超时，导致发送和接收消息混乱
	temporary.m_outSecond = TIME_45S + TIME_05S;
	temporary.m_timing = time(NULL);
	temporary.m_interCall = temporary.INTER_MTOD;
	m_mode = RINGING;
	wTalk *talk = new wTalk();
	talk->show();
	//避免铃声播放时，门口机直接呼叫
	sound.stop();
	//等视频图像出现，在播放铃声
	sound.ringing(2);
}

void SysCommand::recvEventBodyHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 2, EVENT_DEFAULT, MCU_LOCAL_PIC);
	if (!result) {
		return;
	}
	int screenEnable = fb.enabled();
	if (!temporary.m_ScreenState && data[6] == BODY_PIC_CLOSE && !screenEnable) {
		PRINT_DEBUG("off to on \n");
		temporary.setSendState(0);
		fb.enable(1);
		sendEventBodyReq(BODY_EXT_ZERO);
	} else if (data[6] == BODY_PIC_CLOSE && screenEnable) {
		PRINT_DEBUG("keep on 1 and Stop scanning \n");
		temporary.setSendState(0);
		fb.enable(1);
		sendEventBodyReq(BODY_EXT_ZERO);
	} else if (!temporary.m_ScreenState && data[6] == BODY_PIC_LEAVE && screenEnable) {
		PRINT_DEBUG("keep on 2 and Stop scanning \n");
		//temporary.setSendState(0);
		//fb.enable(1);
		sendEventBodyReq(BODY_EXT_ZERO);
	} else if (!temporary.m_ScreenState && data[6] == BODY_PIC_LEAVE && !screenEnable) {
		PRINT_DEBUG("no handle \n");
	} else if (temporary.m_ScreenState && data[6] == BODY_PIC_LEAVE && screenEnable) {
		PRINT_DEBUG("on to off and cycle scanning \n");
		temporary.setSendState(0);
		//避免振铃下，红外唤醒进行熄屏
		if (m_mode >= RINGING) {
			PRINT_DEBUG("when ringing, can not put off \n");
			return;
		}
		sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
		sound.microphoneOnOff(0);
		sound.stop();
		fb.m_touch = 0;
		fb.enable(0);
		sys.admin.login = 0;
		sys.user.login = 0;
		int resetState = beanUtil.getResetState();
		if (!resetState) {
			wMain *w = new wMain();
			w->show();
		} else {
			wDateSetting *initial = new wDateSetting();
			initial->show();
		}
		temporary.m_ScreenState = 0;
		sendEventBodyReq(BODY_EXT_CYCLE);
	} else if (data[6] == BODY_PIC_ACK) {
		PRINT_DEBUG("PIC ack \n");
		temporary.setSendState(0);
	}
}

void SysCommand::recvEventAnswertHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 2, EVENT_DEFAULT, EVENT_DEFAULT);
	if (!result) {
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	//Another monitor has already been answered and will hang up.
	if (m_mode == RINGING) {
		sound.stop();
		sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
		temporary.m_interCall = temporary.INTER_DEFAULT;
		wMain *main = new wMain();
		main->show();
		pictureUtil.removeCapturePicture();
	}
	temporary.m_outSecond = data[6];
	temporary.m_timing = time(NULL);
	m_mode = BUSY;
}

void SysCommand::recvEventTempHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 4, EVENT_DEFAULT, EVENT_DEFAULT);
	if (!result) {
		return;
	}
	sendAckRsp(data[4], ACK_NORMAL);
	int tmpValue = 0;
	tmpValue = data[7];
	if (data[8] >= 5) {
		tmpValue = tmpValue + 1;
	}
	if (data[6]) {
		tmpValue = 0 - tmpValue;
	}
	// int tempDevi = beanUtil.getMeasureTempDeviValue();
	// tmpValue = tmpValue + tempDevi;
	temporary.setTempValue(tmpValue);
	temporary.setTempChange(1);
	int tempState = temporary.getTempState();
	if (!tempState) {
		temporary.setTempState(1);
	}
}

void SysCommand::recvEventHangupHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 2, EVENT_DEFAULT, EVENT_DEFAULT);
	if (!result) {
		return;
	}
	if (m_mode >= RINGING) {
		if (m_mode == RINGING) {
			sound.stop();
		}
		if (m_mode == TALKING) {
			sound.microphoneOnOff(0);
		}
		/*
		* 主副分机振铃超时;
		* 时间低概率会有1-3秒的误差，不好准确定位时间
		* 故在协议中追加一个数据位，判断是正常挂机还是超时挂机
		*/
		int state = 0;
		if (m_mode == RINGING && data[6] == EVENT_HANGUP_TIMEOUT) {
			int unAnswer = beanUtil.getUnAnswer();
			unAnswer++;
			PRINT_DEBUG("unAnswer num: %d \n", unAnswer);
			beanUtil.setUnAnswer(unAnswer);
			beanUtil.save();
			PRINT_DEBUG("save over \n");
			state = 1;
		}
		temporary.m_interCall = temporary.INTER_DEFAULT;
		wMain *main = new wMain();
		main->show();
		if (!state) {
			pictureUtil.removeCapturePicture();
		}
		pictureUtil.m_captureState = 0;
		pictureUtil.m_captureName = "";
	}
	//避免界面还没有加载完，另一台分机已经发送挂机指令
	sendAckRsp(data[4], ACK_NORMAL);
	m_mode = NONE;
}

void SysCommand::recvEventRTSHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 3, EVENT_DEFAULT, MCU_LOCAL_PIC);
	if (!result) {
		return;
	}
	testUtil.checkChannelState(data[6]);
	testUtil.m_RTSCurrentChn = data[7];
	testUtil.m_inquireRTSAck = 1;
}


void SysCommand::recvEventBackHandle(const uint8_t *data)
{
	int result = judgeRecvEventMsg(data, 2, EVENT_DEFAULT, EVENT_DEFAULT);
	if (!result) {
		return;
	}
	if (m_mode == RINGING || m_mode >= TALKING) {
		if (m_mode == RINGING) {
			sound.stop();
		}
		if (m_mode == TALKING) {
			sound.microphoneOnOff(0);
		}
		sound.volume(AUDIO_CHANNEL, VOLUME_MUTE_VALUE);
		temporary.m_interCall = temporary.INTER_DEFAULT;
		wMain *main = new wMain();
		main->show();
		pictureUtil.removeCapturePicture();
	}
	//避免主分机发送完挂机指令，界面还没有加载完
	sendAckRsp(data[4],ACK_NORMAL);
	m_mode = NONE;
}

int SysCommand::judgeRecvEventMsg(const uint8_t *data, uint8_t data2, uint8_t data3, uint8_t data4)
{
	uint8_t dstAddr = 0x00;
	uint8_t srcAddr = 0x00;
	if (!data3) {
		dstAddr = getCurrentMonitor();
	} else {
		dstAddr = data3;
	}
	if (!data4) {
		srcAddr = getAnotherMonitor();
	} else {
		srcAddr = data4;
	}
	if (!(data[3] == MCU_MONITOR_ALL || data[3] == dstAddr)
									|| data[4] != srcAddr
									|| data[2] != data2) {
		return 0;
	}
	return 1;
}
