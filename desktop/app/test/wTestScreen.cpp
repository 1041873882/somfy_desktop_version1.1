#include "SysCommand.h"
#include "TemporaryUtil.h"
#include "TestUtil.h"
#include "wTestChoose.h"
#include "wTestScreen.h"

wTestScreen::wTestScreen() : mWindow("test/screen"){
	
	m_bgId = 1;
	m_cycleTime = 0;
	temporary.m_testState = 0;
	
	m_bkg.setParent(this);
	m_bkg.load(m_style,"bkg_1");

	m_exit.setParent(this);
	m_exit.load(m_style,"exit");
	
	m_cycle.setParent(this);
	m_cycle.load(m_style,"cycle");
	
	
}

wTestScreen::~wTestScreen(){	

}

void wTestScreen::doEvent(mEvent *e){
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchEnd) {
		
		if(e->wParam == m_bkg.id()){
			
			if(!testUtil.m_colorCycle){
				
				m_bgId ++;
				if(m_bgId > 3){
					m_bgId = 1;
				}
				sprintf(m_buf,"bkg_%d",m_bgId);
				m_bkg.load(m_style,m_buf);
				
			}else{
				testUtil.m_colorCycle = 0;
			}
			
		}else if(e->wParam == m_exit.id()){
			testUtil.m_colorCycle = 0;
			wTestChoose *choose = new wTestChoose();
			choose->show();
		}else if(e->wParam == m_cycle.id()){
			testUtil.m_colorCycle = 1;
			testUtil.m_cycleTime = time(NULL);
		}
	}
}

void wTestScreen::doTimer(void){
	mWindow::doTimer();
	
	fb.enable(1);
	
	if(testUtil.m_colorCycle){
		time_t nowTime = time(NULL);
		if(labs(nowTime - m_cycleTime) > 2){
			m_bgId ++;
			if(m_bgId > 3)
				m_bgId = 1;
			
			sprintf(m_buf,"bkg_%d",m_bgId);
			m_bkg.load(m_style,m_buf);
			
			m_cycleTime = nowTime;
		}
	}

	
	if(testUtil.m_colorCycle){

		if(labs(time(NULL) - testUtil.m_cycleTime) > 120){
			temporary.m_testState = 1;
			testUtil.m_cycleTime = time(NULL);
			//门口机比120s多6s,避免门口机比分机同时超时
			command.sendMonitorReq(TIME_126S,command.ANSWER_MONITOR);
		}
	}
}




