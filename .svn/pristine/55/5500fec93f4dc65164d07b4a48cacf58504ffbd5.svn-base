
#ifndef __M_FB_H__
#define __M_FB_H__

#include <time.h>
#include "types.h"

#define RGB565_MASK_RED		0xF800
#define RGB565_MASK_GREEN	0x07E0
#define RGB565_MASK_BLUE	0x001F

static inline uint16_t rgb_24_2_565(uint8_t *c)
{
	return (uint16_t)(((c[2] << 8) & 0xF800) | ((c[1] << 3) & 0x7E0) | (c[0] >> 3)); 
}

static inline uint16_t rgb_24_2_565(uint32_t c)
{
	return rgb_24_2_565((uint8_t *)&c);
}

static inline uint32_t rgb_565_2_24(uint16_t c)
{
	uint32_t d;
	uint8_t *p = (uint8_t *)&d;

	p[3] = 0xFF;
	p[2] = ((c & RGB565_MASK_RED) >> 11) << 3;
	p[1] = ((c & RGB565_MASK_GREEN) >> 5) << 2;
	p[0] = ((c & RGB565_MASK_BLUE)) << 3;
 
	return d;
}

class mFB {
public:
	mFB();
	~mFB();

	int start(void);

	int width(void) { return m_width; };
	int height(void) { return m_height; };
	void clean(void);
	void sync(int direction);
	void sync2(void);

	void enable(int val);
	int enabled(void) { return m_enabled; };

	int timeout(void) { return labs(time(NULL)-m_ts); };
	void bright(int val);

	void osd(int x, int y, int w, int h);

	void process(void);

public:
	
	int m_fd;
	int m_width;
	int m_height;
	uint32_t *m_bits;
	uint16_t *m_rgb565;

	int m_sync;
	int m_direction;//0-无动画  1-从左到右  2-有右到左   3-从上到下  4-从下到上
	int m_osd_sx, m_osd_sy;
	int m_osd_ex, m_osd_ey;

	int m_touch;// 0:正常触控效果 1:不需要触控效果
	time_t m_ts;
	int m_enabled;
};

extern mFB fb;

static inline void fb_pixel(int x, int y, uint32_t c)
{
	if (fb.m_rgb565 == NULL || x < 0 || y < 0 || x >= fb.m_width || y >= fb.m_height) {
		return;
	}
	uint8_t *d = (uint8_t *)&c;
	if (d[3] != 0xFF && d[3] < 0xFA) {
		if (d[3] < 0x05) {
		} else { //ARGB叠加
			uint32_t rgb888 = rgb_565_2_24(fb.m_rgb565[y*fb.m_width+x]);
			uint8_t *s = (uint8_t *)&rgb888;
			s[0] = (((d[0]-s[0]) * d[3]) >> 8) + s[0];
			s[1] = (((d[1]-s[1]) * d[3]) >> 8) + s[1];
			s[2] = (((d[2]-s[2]) * d[3]) >> 8) + s[2];
			fb.m_rgb565[y*fb.m_width+x] = rgb_24_2_565(rgb888);
		}
	} else {
		fb.m_rgb565[y*fb.m_width+x] = rgb_24_2_565(c);
	}
}

#endif
