
#ifndef __M_EVENT_H__
#define __M_EVENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/input.h>
#include <sys/time.h>

#include "types.h"

class mObject;

#define mEventMonitor	(mEvent::Type)(mEvent::User+2001)
#define mEventJpeg		(mEvent::Type)(mEvent::User+2002)
#define mEventElev		(mEvent::Type)(mEvent::User+2003)
#define mEventBusy		(mEvent::Type)(mEvent::User+2004)
#define mEventHook		(mEvent::Type)(mEvent::User+2005)

#define KEY_M_CALL		0x01
#define KEY_M_UNLOCK1	0x02
#define KEY_M_UNLOCK2	0x03
#define KEY_M_PICTURE	0x04
#define KEY_M_F1		0x05
#define KEY_M_F2		0x06
#define KEY_M_F3		0x07
#define KEY_M_F4		0x08
#define KEY_M_F5		0x09
#define KEY_M_F6		0x0A
#define KEY_M_UP		KEY_M_F1
#define KEY_M_RIGHT		KEY_M_F1
#define KEY_M_LEFT		KEY_M_F2
#define KEY_M_ACT_UP	KEY_M_F2
#define KEY_M_DOWN		KEY_M_F2
#define KEY_M_DEL		KEY_M_F3
#define KEY_M_ACT_STOP	KEY_M_F3
#define KEY_M_ACT_DOWN	KEY_M_F4
#define KEY_M_CANCEL	KEY_M_F4
#define KEY_M_CONFIRM	KEY_M_F4
#define KEY_M_DELALL	KEY_M_F4
#define KEY_M_HOME		KEY_M_F5
#define KEY_M_SETTINGS	KEY_M_F6

class mEvent {
public:
	enum Type {
		None = 0,
		Timer = 1,
		TouchBegin = 2,
		TouchUpdate = 3,
		TouchEnd = 4,
		TouchRaw = 5,
		KeyPress = 6,
		KeyRelease = 7,
		FocusIn = 8,
		FocusOut = 9,
		EditEnd = 10,
		AlertEnd = 11,
		ComboxEnd = 12,
		KeyboardEnd = 13,
		ScrollEnd = 14,
		User = 1000, // first user event id
		MaxUser = 65535 // last user event id
	};

	mEvent(Type t);
	~mEvent();

	static void process(void);
	static void post(mEvent *e);
	static void doKeyEvent(struct input_event *ev);
	static void doTouchEvent(struct input_event *ev);
	static void doTouchData(void);
	static void doInput(int fd);
	static void doFifo(mObject *obj);
	static void doFlush(void);
	static void mute(void);

public:
	Type type;
	uint32_t wParam;
	uint32_t lParam;
	uint32_t lParam2;
};

void sys_key_report(int value, int code);

#endif
