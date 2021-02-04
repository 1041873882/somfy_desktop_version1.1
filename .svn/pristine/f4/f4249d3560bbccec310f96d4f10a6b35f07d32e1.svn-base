
#include "wSetup.h"
#include "wTalk.h"
#include "wVideoSetting.h"

#include "DebugUtil.h"
#include "BeanUtil.h"
#include "TestUtil.h"

#include "sys.h"
#include "SysMCU.h"
#include "SysCommand.h"
#include "SysSound.h"
#include "TemporaryUtil.h"

TemporaryUtil temporary;

TemporaryUtil::TemporaryUtil()
{
	tempValue      = 100;
	tempState      = 0;
	m_ScreenState  = 0;
	m_callState    = 0;
	m_answerState  = 0;
	m_talkState    = 0;
	m_testState    = 0;
	m_outSecond    = 0;
	m_interCall    = 0;
	m_timing       = 0;
	m_tempTime     = 0;
	m_ledTime      = 0;
	m_firstCall    = 1;
}

TemporaryUtil::~TemporaryUtil()
{
}

void TemporaryUtil::setTempValue(int value)
{
	tempValue = value;
}

int TemporaryUtil::getTempValue()
{
	return tempValue;
}

void TemporaryUtil::setTempState(int state)
{
	tempState = state;
}

int TemporaryUtil::getTempState()
{
	return tempState;
}

void TemporaryUtil::setTempChange(int state)
{
	tempChange = state;
}

int TemporaryUtil::getTempChange()
{
	return tempChange;
}

void TemporaryUtil::ledLighting()
{
	int unAnsewer = beanUtil.getUnAnswer();
	if (!unAnsewer) {
		return;
	}
	if (labs(time(NULL) - m_ledTime) > 1) {
		sys_led(1);
		usleep(1000*30);
		sys_led(0);
		m_ledTime = time(NULL);
	}
}

void TemporaryUtil::setMonitorOffLine()
{
	PRINT_DEBUG("\n");
	int monitorState = beanUtil.getMeasureMonitorState();
	if (monitorState) {
		beanUtil.setSlaveState(0);
	} else {
		setMonitorToMaster();
	}
	beanUtil.save();
}

//修改地址期间，接收到一台分机发送指令
void TemporaryUtil::prepareSwitchingMonitor(uint8_t *data)
{
	PRINT_DEBUG(" ");
	uint8_t currentMonitor = command.getCurrentMonitor();
	uint8_t anotherMonitor = command.getAnotherMonitor();
	if (data[1] == MCU_MASTER_QUERY) {
		command.sendSlaveNoticeRsp();
	} else if((data[3] == MCU_MONITOR_ALL || data[3] == currentMonitor) && data[1] != MCU_ACK) {
		command.sendAckRsp(anotherMonitor, data[4], ACK_BUSY);
	}
}

//避免分机异常离线或分机长时间处于忙碌下
void TemporaryUtil::cancelBusy()
{
	if (command.m_mode >= command.BUSY) {
		//防止对讲超时时间和这里一起超时及开锁/开门恢复正常延迟2S
		if (labs(time(NULL) - m_timing) > (m_outSecond + TIME_02S)) {
			PRINT_DEBUG("Cancel Buszy \n");
			command.m_mode = command.NONE;
			if (m_interCall) {
				m_interCall = 0;
			}
		}
	}
}

void TemporaryUtil::getTemperature()
{
	int monitor = command.getCurrentMonitor();
	if (monitor != MCU_MONITOR_MASTER) {
		return;
	}
	time_t nowTmer = time(NULL);
	if (labs(nowTmer - m_tempTime) > 30*60) { //30分钟获取一次门口机温度
		//避免在对讲时，对协议发送进行干扰
		m_tempTime = nowTmer;
		if (command.m_mode != command.NONE) {
			PRINT_DEBUG("mode: %d,can not get temperature \n",command.m_mode);
			return;
		}
		PRINT_DEBUG("get Temperature \n");
		command.sendQueryTemeratureReq();
	}
}

// 1 is same 0 different
int TemporaryUtil::compareMonitorAddr(uint8_t *data)
{
	int monitorState = beanUtil.getMeasureMonitorState();
	int slaveState = beanUtil.getSlaveState();
	if (monitorState && data[4] == MCU_MONITOR_MASTER) {//将主分机改成副分机
		prepareSwitchingMonitor(data);
		command.m_mode = command.SWITCH;
		beanUtil.setSlaveState(1);
		beanUtil.setMeasureMonitorState(0);
		beanUtil.save();
		command.m_mode = command.NONE;
		return 1;
	} else if(!monitorState && data[4] == MCU_MONITOR_SLAVE) {//将副分机改成主分机
		prepareSwitchingMonitor(data);
		command.m_mode = command.SWITCH;
		beanUtil.setMasterState(1);
		beanUtil.setMeasureMonitorState(1);
		beanUtil.save();
		command.m_mode = command.NONE;
		return 1;
	} else if(!slaveState && data[4] == MCU_MONITOR_SLAVE) {//副分机不存在，再次接收到副分机的指令
		//Write operations are time consuming
		//command.sendAckRsp(data[4],ACK_BUSY);
		beanUtil.setSlaveState(1);
		beanUtil.save();
	}
	return 0;
}

void TemporaryUtil::setMonitorToMaster()
{
	command.m_mode = command.SWITCH;
	beanUtil.setMasterState(1);
	beanUtil.setMeasureMonitorState(1);
	beanUtil.save();
	command.m_mode = command.NONE;
}

/*
* If the master monitor is powered on, 
* first obtain the outdoor temperature, 
* send the inquiry slave monitor  
* and send the master monitor temperature value 
* to the secondary monitor.
*/
void TemporaryUtil::monitorStart()
{
	int monitorState = beanUtil.getMeasureMonitorState();	
	if (monitorState) {
		command.sendQueryTemeratureReq();
	} else {
		command.sendSlaveQueryReq();
	}
}

void TemporaryUtil::resendData()
{
	if (__val(send_tv) == 0) {
		return;
	}
	//重发5次，间隔700;因为概率性出现3s内，另一台分机接收不到任何数据
	if (__ts(send_tv) >= 700) {
		mcu.tx(command.SendData.cmd, command.SendData.ta, command.SendData.sa,
									command.SendData.data, command.SendData.length);
		gettimeofday(&send_tv, NULL);
		resendCount++;
	}
	if (resendCount >= 5) {
		setSendState(0);
		if (command.SendData.cmd != MCU_MASTER_QUERY 
			&& command.SendData.cmd != MCU_SLAVE_QUERY
			&& ( command.SendData.ta == MCU_MONITOR_MASTER
			  || command.SendData.ta == MCU_MONITOR_SLAVE
			  || command.SendData.ta == MCU_MONITOR_ALL) ) {
			sendMonitorQuery();
		} else if (command.SendData.cmd == MCU_MASTER_QUERY
				|| command.SendData.cmd == MCU_SLAVE_QUERY) {
			setMonitorOffLine();
		}
	}
}

void TemporaryUtil::sendMonitorQuery()
{
	uint8_t monitorAddr = command.getCurrentMonitor();
	if (monitorAddr == MCU_MONITOR_SLAVE) {
		command.sendSlaveQueryReq();
	} else {
		command.sendMasterQueryReq();
	}
}

void TemporaryUtil::setSendData(uint8_t cmd, uint8_t ta, uint8_t sa, uint8_t *data, uint8_t length)
{
	command.SendData.cmd = cmd;
	command.SendData.ta = ta;
	command.SendData.sa = sa;
	command.SendData.length = length;
	for (int i = 0; i < length; i++) {
		command.SendData.data[i] = data[i];
	}
}

void TemporaryUtil::setSendState(int state)
{
	command.SendData.state = state;
	resendCount = state;
	if (state) {
		gettimeofday(&send_tv, NULL);
	} else {
		memset(&send_tv, 0, sizeof(send_tv));
	}
}

int TemporaryUtil::ackByCallMonitor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_CALL, MCU_MONITOR_ALL, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	m_outSecond = TIME_45S;
	m_timing = time(NULL);
	m_interCall = INTER_MTOM;
	command.m_mode = command.RINGING;
	// wSpeakAnswer *speakAnswer = new wSpeakAnswer();
	// speakAnswer->show();

	return 1;
}

int TemporaryUtil::ackByHangupMonitor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_HANGUP, data[4], EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	command.m_mode = command.NONE;

	return 1;
}

int TemporaryUtil::ackByAnswerMonitor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_ANSWER, data[4], EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	m_outSecond = TIME_120S;
	m_timing = time(NULL);
	command.m_mode = command.TALKING;
	sound.setVolumeValue(AUDIO_CHANNEL);
	sound.microphoneOnOff(1);
	m_interCall = 1;
	// wSpeakAnswer *speakAnswer = new wSpeakAnswer();
	// speakAnswer->show();

	return 1;
}

int TemporaryUtil::ackBySlaveQuery(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_SLAVE_QUERY, MCU_MONITOR_ALL, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	temporary.setSendState(0);
	beanUtil.setMasterState(1);
	beanUtil.save();
	return 1;
}

int TemporaryUtil::ackByHangupDoor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_HANGUP, MCU_DOOR_MACHINE, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DOOR_MACHINE);
	if (!result) {
		return 0;
	}
	setSendState(0);
	command.m_mode = command.NONE;
	int masterState = beanUtil.getMasterState();
	int slaveState = beanUtil.getSlaveState();
	if (masterState && slaveState && m_answerState >= command.ANSWER_CALL) {
		command.sendEventHangupReq(command.NONE);
	}
	return 1;
}

int TemporaryUtil::ackByAnswerDoor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_ANSWER, MCU_DOOR_MACHINE, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DOOR_MACHINE);
	if (!result) {
		return 0;
	}
	setSendState(0);
	if (m_answerState == command.ANSWER_MONITOR) {//分机监视门口
		PRINT_DEBUG("monitor\n");
		//分机监视门口机，可以听到门口声音;测试屏幕老化，是静音状态
		if (!temporary.m_testState) {
			sound.setVolumeValue(AUDIO_CHANNEL);
		}
		m_callState = 1;
		m_answerState = command.ANSWER_NONE;
		m_outSecond = TIME_120S;
		m_timing = time(NULL);
		m_interCall = INTER_MTOD;
		command.m_mode = command.MONITORING;
		wTalk *talk = new wTalk();
		talk->show();
	} else if (m_answerState == command.ANSWER_CALL) {//分机接听门口机呼叫
		PRINT_DEBUG("call \n");
		sound.setVolumeValue(AUDIO_CHANNEL);
		sound.microphoneOnOff(1);
		m_callState = 1;
		m_answerState = command.ANSWER_NONE;
		m_outSecond = TIME_120S;
		m_timing = time(NULL);
		command.m_mode = command.TALKING;
	}
	return 1;
}

int TemporaryUtil::ackByMonitorDoor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_MONITOR, MCU_DOOR_ALL, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DOOR_MACHINE);
	if (!result) {
		return 0;
	}
	PRINT_DEBUG(" \n");
	setSendState(0);
	if (m_answerState == command.ANSWER_MONITOR) {
		command.sendAnswerReq(MCU_DOOR_MACHINE, command.ANSWER_MONITOR);
	} else if (m_answerState == command.ANSWER_MEASURE) {
		// videoSetting 只是设置视频参数，不进行对讲，不发送摘机指令
		m_outSecond = TIME_120S;
		m_timing = time(NULL);
		m_interCall = INTER_MTOD;
		command.m_mode = command.TALKING;
		wVideoSetting *videoSetting = new wVideoSetting();
		videoSetting->show();
	}
	return 1;
}

int TemporaryUtil::ackByLockDoor(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_LOCK, MCU_DOOR_MACHINE, EVENT_DEFAULT, MCU_DEFAULT_ADDR, MCU_DOOR_MACHINE);
	if(!result)  return 0;
	setSendState(0);
	if (m_answerState == command.ANSWER_GATE || m_answerState == command.ANSWER_DOOR) {
		int masterState = beanUtil.getMasterState();
		int slaveState = beanUtil.getSlaveState();
		if (masterState && slaveState) {
			command.sendEventHangupReq(command.NONE);
		}
	}
	
	/*
	* 通话下，发送开锁指令前会将通话音量设置成静音，
	* 避免总线通信噪声，指令发送结束恢复正常
	*/
	if (command.m_mode >= command.TALKING && !temporary.m_testState) {
		sound.setVolumeValue(AUDIO_CHANNEL);
	}
	m_answerState = command.ANSWER_NONE;
	return 1;
}

int TemporaryUtil::ackByEventCall(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_EVENT, MCU_MONITOR_ALL, EVENT_CALL, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	return 1;
}

int TemporaryUtil::ackByEventAnswer(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_EVENT, MCU_MONITOR_ALL, EVENT_ANSWER, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	PRINT_DEBUG(" \n");
	setSendState(0);
	switch(m_answerState){
	case command.ANSWER_CALL:
		command.sendAnswerReq(MCU_DOOR_MACHINE, command.ANSWER_CALL);
		break;
	case command.ANSWER_MONITOR:
		command.sendMonitorReq(DATA_RESERVED, command.ANSWER_MONITOR);
		break;
	case command.ANSWER_GATE:
		command.sendLockReq(LOCK_GATE, command.NONE);
		break;
	case command.ANSWER_DOOR:
		command.sendLockReq(LOCK_DOOR, command.NONE);
		break;
	case command.ANSWER_MEASURE:
		command.sendMonitorReq(DATA_RESERVED, command.ANSWER_MEASURE);
	default:
		//doing nothing
		break;
	}

	return 1;
}

int TemporaryUtil::ackByEventHangup(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_EVENT, MCU_MONITOR_ALL, EVENT_HANGUP, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	if (m_answerState == command.ANSWER_CALL) {
		command.sendHangupReq(MCU_DOOR_MACHINE,command.NONE);
	}
	if (command.m_mode >= command.TALKING) {
		sound.setVolumeValue(AUDIO_CHANNEL);
	}
	return 1;
}

int TemporaryUtil::ackByEventTemp(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_EVENT, MCU_MONITOR_ALL, EVENT_TEMP, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	return 1;
}

int TemporaryUtil::ackByEventBack(uint8_t *data)
{
	int result = judgeAckMsg(data, MCU_EVENT, MCU_MONITOR_ALL, EVENT_BACK, MCU_DEFAULT_ADDR, MCU_DEFAULT_ADDR);
	if (!result) {
		return 0;
	}
	setSendState(0);
	command.sendHangupReq(MCU_DOOR_MACHINE,command.NONE);
	return 1;
}

//Respond to busy or error, do not resend
int TemporaryUtil::ackByBusyOrError(uint8_t *data)
{
	uint8_t anotherMonitor = command.getAnotherMonitor();
	if (!command.SendData.state || !(data[5] == ACK_BUSY
							   ||   data[5] == ACK_ERROR) 
							   || data[4] != anotherMonitor) {
		return 0;
	}
	setSendState(0);
	//自动呼叫测试
	PRINT_DEBUG("delay send,beacause of busy \n");
	testUtil.m_busy = 1;
	testUtil.m_delayTime = time(NULL);
	return 1;
}

int TemporaryUtil::judgeAckMsg(uint8_t *data, uint8_t cmd, uint8_t ta, uint8_t data0, uint8_t data3, uint8_t data4)
{
	uint8_t dstAddr = 0x00;
	uint8_t srcAddr = 0x00;
	if (!data3) {
		dstAddr = command.getCurrentMonitor();
	} else {
		dstAddr = data3;
	}
	if (!data4) {
		srcAddr = command.getAnotherMonitor();
	} else {
		srcAddr = data4;
	}
	if (!command.SendData.state || command.SendData.cmd != cmd 
							   || command.SendData.ta != ta
					           || ( data0 && command.SendData.data[0] != data0)
					           || data[3] != dstAddr
							   || data[4] != srcAddr) {
		return 0;
	}
	return 1;
}
