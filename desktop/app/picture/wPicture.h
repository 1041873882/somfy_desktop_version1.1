#ifndef __W_PICTURE_H__
#define __W_PICTURE_H__

#include "mWindow.h"
#include "mButton.h"

#define PICTURE_WIDTH	800
#define PICTURE_HEIGHT	480

enum {
	DEFAULT,
	DELETE_ONE_PICTURE,
	DELETE_ALL_PICTURE
};

class wPicture: public mWindow {
public:
	wPicture();
	virtual ~wPicture();

	void loadLed(int mode);
	void doEvent(mEvent *e);
	void doTimer(void);
	void doConfirm(void);
	void modeChange(int mode);
	void loadPicure(int position);
	void loadNavigation(void);
	void doChange(int direction);
	void loadInfos(std::string name);
	void updateUnreadPicNum(void);
private:
	mPixmap m_picture;
	mPixmap m_newer_selected;
	mPixmap m_upper_area;
	mPixmap m_icon;

	mPixmap m_bin_icon;
	mButton m_offset;
	mPixmap m_infos;
	mText   m_date;
	mText   m_time;
	mButton m_number;

	mPixmap m_menu_area;
	mPixmap m_up;
	mPixmap m_down;
	mPixmap m_func1;
	mPixmap m_func2;
	mPixmap m_separator;
	mPixmap m_home;
private:
	int m_mode;
	int m_pictureNum;
	int m_position;
	int m_unReadPictureNum;
	struct timeval led_tv;
};

#endif