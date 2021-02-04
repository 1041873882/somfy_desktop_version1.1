#include "SysCommand.h"

#include "wTestChoose.h"
#include "wTestRTS.h"


wTestRTS::wTestRTS() : mWindow("test/RTS"){
	
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	
	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
	
	m_match.setParent(this);
	m_match.load(m_style, "match");

	m_delete.setParent(this);
	m_delete.load(m_style, "delete");

	m_matchTxt.setParent(this);
	m_matchTxt.load(m_style, "match_txt");

	m_deleteTxt.setParent(this);
	m_deleteTxt.load(m_style, "delete_txt");
		
	m_top.setParent(this);
	m_top.load(m_style, "top");
	m_top.hide();
	
	m_middle.setParent(this);
	m_middle.load(m_style, "middle");
	m_middle.hide();
	
	m_bottom.setParent(this);
	m_bottom.load(m_style, "bottom");
	m_bottom.hide();
	
}

wTestRTS::~wTestRTS(){
	
}

void wTestRTS::doEvent(mEvent *e){
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchEnd) {
		
		if(e->wParam == m_exit.id()){
			wTestChoose *choose = new wTestChoose();
			choose->show();
		}else if(e->wParam == m_match.id()){
			command.sendSwitchMatch(1);
			setChannelEnable(1);
		}else if(e->wParam == m_delete.id()){
			command.sendSwitchMatch(1);
			setChannelEnable(0);
		}else if(e->wParam == m_top.id()){
			
			command.sendSwitchControl(1,1);
			loadControl(1);
			
		}else if(e->wParam == m_middle.id()){
			
			command.sendSwitchControl(1,2);
			loadControl(2);
			
		}else if(e->wParam == m_bottom.id()){
			
			command.sendSwitchControl(1,3);
			loadControl(3);
		}
		
	}
}

void wTestRTS::doTimer(void){
	mWindow::doTimer();
}

void wTestRTS::loadControl(int position){
	char buf[128];
	for(int i = 0; i < 3; i++){
		if(position == (i + 1)){
			sprintf(buf,"ctl_on_%d",position);
		}else{
			sprintf(buf,"ctl_un_%d",i+1);
		}
		
		if(i == 0)
			m_top.load(m_style,buf);
		else if(i == 1)
			m_middle.load(m_style,buf);
		else if(i == 2)
			m_bottom.load(m_style,buf);
	}
	
}

void wTestRTS::setChannelEnable(int enable){

	if(enable){
		m_top.show();
		m_middle.show();
		m_bottom.show();
		
	}else{
		m_top.hide();
		m_middle.hide();
		m_bottom.hide();
	}

}

