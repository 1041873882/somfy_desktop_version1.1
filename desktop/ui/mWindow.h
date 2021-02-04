
#ifndef __M_WINDOWS_H__
#define __M_WINDOWS_H__

#include "dxml.h"
#include "AutoMutex.h"
#include "mObject.h"
#include "mEvent.h"

class mWindow : public mObject {
public:
	mWindow(const char *name);
	virtual ~mWindow();

	void doTouch(int x, int y, int m);
	void show(void);
	void translate(int direction);//1-从左到右  2-从右到左   3-从上到下  4-从下到上
	void doPaint(void);

public:
	static void process(void);

	static pthread_mutex_t m_mutex;
	static mWindow *window;

protected:
	dxml m_style;
	int m_direction;
};

#endif