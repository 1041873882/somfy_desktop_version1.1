
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "SysSound.h"
#include "mEvent.h"
#include "mFB.h"

#define ANIMATION_STEP_NUM 40

mFB fb;

mFB::mFB()
{
	m_fd = -1;
	m_bits = NULL;
	m_rgb565 = NULL;
	m_width = 0;
	m_height = 0;
	m_enabled = 0;
	m_osd_sx = 0;
	m_osd_sy = 0;
	m_osd_ex = 0;
	m_osd_ey = 0;
	m_sync = 0;
	m_direction = 0;
	m_touch = 0;
}

mFB::~mFB()
{
}

int mFB::start(void)
{
	m_fd = ::open("/dev/fb0", O_RDWR);
	if (m_fd > 0) {
		struct fb_var_screeninfo vi;
		::ioctl(m_fd, FBIOGET_VSCREENINFO, &vi);

		struct fb_fix_screeninfo fi;
		::ioctl(m_fd, FBIOGET_FSCREENINFO, &fi);

		m_width = vi.xres;
		m_height = vi.yres;
		m_bits = (uint32_t *)::mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
		m_rgb565 = new uint16_t[m_width*m_height];
		LOGE("mFB::start open /dev/fb0 [%dx%d] ok.\n", m_width, m_height);
	} else {
		LOGE("mFB::start open /dev/fb0 error!\n");
	}
	this->enable(1);
	return 0;
}

void mFB::clean(void)
{
	if (m_rgb565) {
		memset(m_rgb565, 0x00, 2*m_width*m_height);
	}
}

#include "SysHighThread.h"
void mFB::sync(int direction)
{
	m_direction = direction;
	m_sync = 1;

	hThread.post();
}

void mFB::sync2(void)
{
	if (m_bits == NULL || m_rgb565 == NULL)
		return;

	int offset = 0;
	uint32_t *tmp = NULL;
	uint32_t *p = NULL;

	if (m_direction != 0) {
		tmp = new uint32_t[m_width*m_height];
	}
	p = (tmp != NULL) ? tmp : m_bits;

	for(int y=0; y<m_height; y++) {
		for(int x=0; x<m_width; x++) {
			if (x > m_osd_sx && x < m_osd_ex && y > m_osd_sy && y < m_osd_ey) {
				if (m_rgb565[offset+x]) {
					p[offset+x] = rgb_565_2_24(m_rgb565[offset+x]);
				} else {
					p[offset+x] = 0x00000000;
				}
			} else {
				p[offset+x] = rgb_565_2_24(m_rgb565[offset+x]);
			}
		}
		offset += m_width;
	}

	if (tmp != NULL) {
		for (int i=0; i<m_width*m_height; i++)
			m_bits[i] = 0xFF000000;
		for (int i=1; i<=ANIMATION_STEP_NUM; i++) {
			if (m_direction == 1) {
				int step = m_width/ANIMATION_STEP_NUM*i;
				uint32_t *dst = m_bits;
				uint32_t *src = p + m_width - step;
				for (int j=0; j<m_height; j++) {
					memcpy(dst, src, step*sizeof(uint32_t));
					src += m_width;
					dst += m_width;
				}
			} else if (m_direction == 2) {
				int step = m_width/ANIMATION_STEP_NUM*i;
				uint32_t *dst = m_bits + m_width - step;
				uint32_t *src = p;
				for (int j=0; j<m_height; j++) {
					memcpy(dst, src, step*sizeof(uint32_t));
					src += m_width;
					dst += m_width;
				}
			} else if (m_direction == 3) {
				int step = m_height/ANIMATION_STEP_NUM*i;
				uint32_t *dst = m_bits;
				uint32_t *src = p + (m_height - step)*m_width;
				memcpy(dst, src, step*m_width*sizeof(uint32_t));
			} else if (m_direction == 4) {
				int step = m_height/ANIMATION_STEP_NUM*i;
				uint32_t *dst = m_bits + (m_height - step)*m_width;
				uint32_t *src = p;
				memcpy(dst, src, step*m_width*sizeof(uint32_t));
			}
		}
		m_direction = 0;
		delete [] tmp;
	}
}

#define SUNXI_FB_CONTROL _IOWR('C', 0x55, int)

void mFB::enable(int val)
{
	if (!val) {
		sound.amplifierOnOff(0);
	}
	if (m_enabled != val) {
		//mEvent::mute();
		if (m_fd > 0) {
			ioctl(m_fd, SUNXI_FB_CONTROL, val);
		}
	}
	m_ts = time(NULL);
	m_enabled = val;
}

void mFB::osd(int x, int y, int w, int h)
{
	m_osd_sx = x;
	m_osd_sy = y;
	m_osd_ex = x+w;
	m_osd_ey = y+h;
}

//UI实际刷新不使用mEvent主线程
//使用SysHighThread最高优先级线程处理

void mFB::process(void)
{
	if (m_sync) {
		this->sync2();
		m_sync = 0;
	}
}
