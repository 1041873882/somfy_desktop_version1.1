#include "TestUtil.h"
#include "BeanUtil.h"
#include "DebugUtil.h"

#include "mFB.h"
#include "SysCommand.h"
#include "sys.h"
#include "wMain.h"
//#include "wResetWelcome.h"
#include "wTestRTSControl.h"


wTestRTSControl::wTestRTSControl() : mWindow("test/RTSControl"){

	m_step = 1;
	m_autoCheck = 0;
	m_chnOk = 0;
	
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_topbar.setParent(this);
	m_topbar.load(m_style, "toolbar");
	
	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
	
	m_title.setParent(this);
	m_title.load(m_style, "title_1");

	m_descriptionOne.setParent(this);
	m_descriptionOne.load(m_style, "complete_1");

	m_descriptionTwo.setParent(this);
	m_descriptionTwo.load(m_style, "complete_2");

	m_oneControl.setParent(this);
	m_oneControl.load(m_style, "restart");

	m_oneName.setParent(this);
	m_oneName.load(m_style, "restart_name");
		
	m_twoControl.setParent(this);
	m_twoControl.load(m_style, "reset");

	
	m_twoName.setParent(this);
	m_twoName.load(m_style, "reset_name");

	int x_offset = m_style.getInt("/style/step_one_3_on/x_offset",0);
	for(int i = 0; i < 5; i++){
		m_channel[i].setParent(this);
		m_channel[i].load(m_style, "step_one_3_un");
		m_channel[i].move(m_channel[i].x()+x_offset*i, m_channel[i].y());
		m_channel[i].hide();
		//通道恢复默认
		testUtil.m_chnState[i] = 0;
	}

	m_light.setParent(this);
	m_light.load(m_style, "step_three_2_on");
	m_light.hide();

	loadPageComplete();
	
}

wTestRTSControl::~wTestRTSControl(){
	
}

void wTestRTSControl::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchEnd) {
		
		if(e->wParam == m_exit.id()){
			
			wMain *main = new wMain();
			main->show();
		
		}else if(e->wParam == m_oneControl.id()){

			//PRINT_DEBUG("step : %d \n", m_step);
			if(m_step == 0){ //重置设置选项

				beanUtil.setResetState(1);
				beanUtil.measureReset();
				//选中SD卡中的音乐,恢复默认
				// musicUtil.setShowPage(1);
				// musicUtil.setShowPagePosition(0);
				sys.lcd.bright(20);
				sys.lcd.contrast(100);
				sys.lcd.saturation(128);
				sys.lcd.hue(0);
				int unAnswer = beanUtil.getUnAnswer();
				if(unAnswer){
					beanUtil.setUnAnswer(0);
				}
				//wakeup turn off 
				command.sendEventBodyReq(BODY_EXT_ZERO);
				// wResetWelcome *welcom = new wResetWelcome();
				// welcom->show();
				beanUtil.save();
				sys.save();
			
			}else if(m_step == 1){
				
				m_step = 2;
				loadPageOne_1();
			
			}else if(m_step == 2){
				
				m_step = 3;
				loadPageOne_2();
				command.sendSwitchChnCheckTest();
				m_autoCheck = time(NULL);
				
			}else if(m_step == 3){
				
				//没有按键，自动跳转
				
			}else if(m_step == 4){
			
				m_step = 5;
				loadPageTwo_1();
				
			}else if(m_step == 5){
				
				m_step = 6;
				command.sendSwitchMatch(testUtil.m_RTSCurrentChn);
				loadPageTwo_2();
				
			}else if(m_step == 6){
				
				m_step = 7;
				loadPageThreeControl(1);
				
			}else if(m_step == 7){
				
				m_step = 8;
				command.sendSwitchControl(testUtil.m_RTSCurrentChn, 1);
				loadPageThreeShow(2);
				
			}else if(m_step == 8){
				
				m_step = 9;
				loadPageThreeControl(3);
				
			}else if(m_step == 9){
				
				m_step = 10;
				command.sendSwitchControl(testUtil.m_RTSCurrentChn, 2);
				loadPageThreeShow(4);
				
			}else if(m_step == 10){
				
				m_step = 11;
				loadPageThreeControl(5);
				
			}else if(m_step == 11){
				
				m_step = 12;
				command.sendSwitchControl(testUtil.m_RTSCurrentChn, 3);
				loadPageThreeShow(6);
				
			}else if(m_step == 12){
				
				m_step = 13;
				loadPageFour_1();
				
			}else if(m_step == 13){
				
				m_step = 14;
				command.sendSwitchMatch(testUtil.m_RTSCurrentChn);
				loadPageFour_2();
				
			}else if(m_step == 14){
				
				m_step = 1;
				loadPageComplete();
				
			}
			
		}else if(e->wParam == m_twoControl.id()){

			//PRINT_DEBUG("step : %d \n", m_step);
			if(m_step == 0){

				m_step = 1;
				loadPageComplete();
			
			}else if(m_step == 1){
				
				m_step = 0;
				loadPageReset();
				
			}else if(m_step == 4){

				m_step = 2;
				loadPageOne_1();
				
			}else if(m_step == 6){
				
				m_step = 5;
				loadPageTwo_1();
				
			}else if(m_step == 8){
				
				m_step = 7;
				loadPageThreeControl(1);
				
			}else if(m_step == 10){
				
				m_step = 9;
				loadPageThreeControl(3);
				
			}else if(m_step == 12){ // step 13, 没有返回按钮
				
				m_step = 11;
				loadPageThreeControl(5);
				
			}else if(m_step == 14){
				m_step = 13;
				loadPageFour_1();
			}
		}
		
	}
}

void wTestRTSControl::doTimer(void){
	mWindow::doTimer();
    fb.enable(1);
	
	if(m_step == 3 && labs(time(NULL) - m_autoCheck) > 5){
		m_step = 4;
		loadPageOne_3();
	}

	//停留2s,让这个界面加载明显一些
	if(m_step == 3 && testUtil.m_inquireRTSAck && labs(time(NULL) - m_autoCheck) > 2){
		m_step = 4;
		loadPageOne_3();
	}
}


void wTestRTSControl::loadPageComplete(){

	m_title.load(m_style, "title_1");
	m_title.show();
	
	m_descriptionOne.load(m_style, "complete_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "complete_2");
	m_descriptionTwo.show();
	
	m_oneControl.load(m_style, "restart");
	m_oneControl.show();

	m_oneName.load(m_style, "restart_name");
	m_oneName.show();
	
	m_twoControl.load(m_style, "reset");
	m_twoControl.show();
	
	m_twoName.load(m_style, "reset_name");
	m_twoName.show();
}


void wTestRTSControl::loadPageReset(){

	m_title.load(m_style, "title_2");
	m_title.show();
	
	m_descriptionOne.load(m_style, "reset_description");
	m_descriptionOne.show();
	
	m_descriptionTwo.hide();
	
	m_oneControl.load(m_style, "reset_yes");
	m_oneControl.show();
	
	m_twoControl.load(m_style, "reset_no");
	m_twoControl.show();
	
	m_oneName.load(m_style, "reset_yes_name");
	m_oneName.show();
	
	m_twoName.load(m_style, "reset_no_name");
	m_twoName.show();
}


void wTestRTSControl::loadPageOne_1(){

	m_title.load(m_style, "title_2");
	m_title.show();
	
	m_descriptionOne.load(m_style, "step_one_1_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_one_1_2");
	m_descriptionTwo.show();
	
	for(int i = 0; i < 5; i++)
		m_channel[i].hide();
	
	m_oneControl.load(m_style, "step_one_1_test");
	m_oneControl.show();

	m_oneName.load(m_style, "step_one_1_test_name");
	m_oneName.show();
	
	m_twoControl.hide();
	m_twoName.hide();
}

void wTestRTSControl::loadPageOne_2(){

	m_descriptionOne.load(m_style, "step_one_2_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_one_2_2");
	m_descriptionTwo.show();

	m_oneControl.hide();
	m_oneName.hide();

	m_twoControl.hide();
	m_twoName.hide();

	m_exit.hide();
	
	for(int i = 0; i < 5; i++)
		testUtil.m_chnState[i] = 0;
	
	testUtil.m_RTSCurrentChn = 0;
	testUtil.m_inquireRTSAck = 0;
	
}

void wTestRTSControl::loadPageOne_3(){

	m_title.load(m_style, "title_2");
	m_title.show();
	
	m_descriptionOne.load(m_style, "step_one_3_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_one_3_2");
	m_descriptionTwo.show();

	m_exit.show();
	
	int x_offset = m_style.getInt("/style/step_one_3_on/x_offset", 0);
	for(int i = 0; i < 5; i++){

		if(testUtil.m_chnState[i]){
			m_channel[i].load(m_style, "step_one_3_on");
			m_chnOk = 1;
		}else{
			m_channel[i].load(m_style, "step_one_3_un");
		}
		m_channel[i].move(m_channel[i].x()+ x_offset*i, m_channel[i].y());
		m_channel[i].show();
	}
	
	m_oneControl.load(m_style, "step_one_3_yes");
	m_oneControl.show();

	m_oneName.load(m_style, "step_one_3_yes_name");
	m_oneName.show();
	
	m_twoControl.load(m_style, "step_one_3_no");
	m_twoControl.show();
	
	m_twoName.load(m_style, "step_one_3_no_name");
	m_twoName.show();

	//如果没有一个通道是正常，无法接下去测试
	if(!m_chnOk)
		m_oneControl.setEnable(0);
	
}

void wTestRTSControl::loadPageTwo_1(){

	m_descriptionOne.load(m_style, "step_two_1_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_two_1_2");
	m_descriptionTwo.show();
	
	for(int i = 0; i < 5; i++){
		m_channel[i].hide();
	}
	
	m_oneControl.load(m_style, "step_two_1_pairing");
	m_oneControl.show();

	m_oneName.load(m_style, "step_two_1_pairing_name");
	m_oneName.show();
	
	m_twoControl.hide();
	m_twoName.hide();
	
}


void wTestRTSControl::loadPageTwo_2(){

	m_descriptionOne.load(m_style, "step_two_2_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_two_2_2");
	m_descriptionTwo.show();
	
	m_oneControl.load(m_style, "step_two_2_yes");
	m_oneControl.show();

	m_oneName.load(m_style, "step_two_2_yes_name");
	m_oneName.show();
	
	m_twoControl.load(m_style, "step_two_2_no");
	m_twoControl.show();

	m_twoName.load(m_style, "step_two_2_no_name");
	m_twoName.show();
	
}


void wTestRTSControl::loadPageThreeControl(int num){

	char buf[128];
	sprintf(buf, "step_three_%d_1", num);
	m_descriptionOne.load(m_style, buf);
	m_descriptionOne.show();

	sprintf(buf, "step_three_%d_2", num);
	m_descriptionTwo.load(m_style, buf);
	m_descriptionTwo.show();

	m_light.hide();

	sprintf(buf, "step_three_%d_on", num);
	m_oneControl.load(m_style, buf);
	m_oneControl.show();

	sprintf(buf, "step_three_%d_on_name", num);
	m_oneName.load(m_style, buf);
	m_oneName.show();
	
	m_twoControl.hide();
	m_twoName.hide();
	
}

void wTestRTSControl::loadPageThreeShow(int num){

	char buf[128];
	sprintf(buf, "step_three_%d_1", num);
	m_descriptionOne.load(m_style, buf);
	m_descriptionOne.show();

	sprintf(buf, "step_three_%d_2", num);
	m_descriptionTwo.load(m_style, buf);
	m_descriptionTwo.show();

	sprintf(buf, "step_three_%d_on", num);
	m_light.load(m_style, buf);
	m_light.show();

	sprintf(buf, "step_three_%d_yes", num);
	m_oneControl.load(m_style, buf);
	m_oneControl.show();

	sprintf(buf, "step_three_%d_yes_name", num);
	m_oneName.load(m_style, buf);
	m_oneName.show();

	sprintf(buf, "step_three_%d_no", num);
	m_twoControl.load(m_style, buf);
	m_twoControl.show();

	sprintf(buf, "step_three_%d_no_name", num);
	m_twoName.load(m_style, buf);
	m_twoName.show();
	
}

void wTestRTSControl::loadPageFour_1(){
	
	m_descriptionOne.load(m_style, "step_four_1_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_four_1_2");
	m_descriptionTwo.show();

	m_light.hide();
	
	m_oneControl.load(m_style, "step_four_1_unpairing");
	m_oneControl.show();

	m_oneName.load(m_style, "step_four_1_unpairing_name");
	m_oneName.show();

	m_twoControl.hide();
	m_twoName.hide();
	
}


void wTestRTSControl::loadPageFour_2(){
	
	m_descriptionOne.load(m_style, "step_four_2_1");
	m_descriptionOne.show();
	
	m_descriptionTwo.load(m_style, "step_four_2_2");
	m_descriptionTwo.show();
	
	m_oneControl.load(m_style, "step_four_2_end");
	m_oneControl.show();

	m_oneName.load(m_style, "step_four_2_end_name");
	m_oneName.show();
	
	m_twoControl.load(m_style, "step_four_2_no");
	m_twoControl.show();

	m_twoName.load(m_style, "step_four_2_no_name");
	m_twoName.show();
	
}











