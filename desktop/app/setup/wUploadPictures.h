#ifndef __W_UPLOAD_PICTURES_H__
#define __W_UPLOAD_PICTURES_H__

#include "mWindow.h"
#include "mButton.h"

class wUploadPictures: public mWindow {
public:
	wUploadPictures();
	virtual ~wUploadPictures();

	void loadLed(void);
	void doTimer(void);
	void doUpload(void);
	void doEvent(mEvent *e);
	void loadNavigation();
private:
	mPixmap m_bkg;
	mPixmap m_icon;
	mPixmap m_pictures;
	mPixmap m_arrow;
	mPixmap m_sd_card;
	mButton m_mark;

	mPixmap m_confirm;
	mPixmap m_cancel;
	mPixmap m_separator;
	mPixmap m_home;
private:
	struct timeval led_tv;
};

#endif