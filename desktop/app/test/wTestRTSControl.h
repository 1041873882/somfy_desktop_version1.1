#ifndef __W_TEST_RTS_CONTROL_H__
#define __W_TEST_RTS_CONTROL_H__

#include "mWindow.h"
#include "mButton.h"

/*
*  这是一个测试流程，包含多个界面，全部集合到一个界面
*  根据具体界面和Step 步骤命名界面
*  命令名称不能直接说明具体功能，可以结合实际操作步骤去区分界面
*  对应一个界面一个函数
*/

class wTestRTSControl : public mWindow {

public:
	
	wTestRTSControl();
	virtual ~wTestRTSControl();

	void doEvent(mEvent *e);
	void doTimer(void);

	void loadPageComplete();
	void loadPageReset();
	void loadPageOne_1();
	void loadPageOne_2();
	void loadPageOne_3();
	void loadPageTwo_1();
	void loadPageTwo_2();
	void loadPageThreeControl(int num);
	void loadPageThreeShow(int num);
	void loadPageFour_1();
	void loadPageFour_2();

private:

	mPixmap m_bkg;

	mButton m_topbar;
	mButton m_exit;
	mText   m_title;

	mText   m_descriptionOne;
	mText   m_descriptionTwo;
	mButton m_oneControl;
	mButton m_twoControl;
	mText   m_oneName;
	mText   m_twoName;

	mPixmap m_channel[5];
	mPixmap m_light;
	
	int m_step;
	time_t m_autoCheck;

	int m_chnOk;
	
};

#endif

