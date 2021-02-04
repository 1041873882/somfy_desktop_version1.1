#ifndef __BEAN_UTIL_H__
#define __BEAN_UTIL_H__

#include <string>

#define BELL_NAME_MAX_SIZE  16

class BeanUtil {
public:
	BeanUtil();
	~BeanUtil();

	void setBellId(int id);
	int getBellId();

	void setBellProgressBarNum(int id);
	int getBellProgressBarNum();

	void setMeasureAudioNum(int num);
	int getMeasureAudioNum();

	void setMeasureMonitorState(int state);
	int getMeasureMonitorState();

	void setMeasureColorNum(int num);
	int getMeasureColorNum();
	
	void setMeasureLuminosityNum(int num);
	int getMeasureLuminosityNum();

	void setMeasureContrastNum(int num);
	int getMeasureContrastNum();

	void setSlaveState(int state);
	int  getSlaveState();

	void setMasterState(int state);
	int  getMasterState();

	void setUnAnswer(int num);
	int  getUnAnswer();

	void setResetState(int state);
	int  getResetState();
	
	void setOpenTimeId(int id);
	int getOpenTimeId();

	void set24Hour(int val);
	int get24Hour(void);

	void setVideoSetType(int val);
	int getVideoSetType(void);

	void setMissedCall(int val);
	int getMissedCall(void);

	const char *dir();
	void load();
	void save();
	void measureReset();
	void start();
private:
	int bellId;
	int bellProgressBarNum;
	int measureAudioNum;
	int measureMonitorState; //master is 1 , slaver is 0
	int measureColorNum;
	int measureLuminosityNum;
	int measureContrastNum;
	int measureResetState;
	int openTimeId;
	int a24_hour;

	//talking
	int masterState;
	int slaveState;
	int unAnswerNum;
	int videoSetType;
	int missedCall;			//1 is have missed call, 0 is no have missed call
};

extern BeanUtil beanUtil;

#endif
