
#include "mSeekBar.h"

mSeekBar::mSeekBar()
{
	m_name = "mSeekBar";
	m_min = 0;
	m_max = 10;
	m_value = 0;
	m_seek.setParent(this);
}

mSeekBar::~mSeekBar()
{
}

void mSeekBar::load(dxml &p, const char *zone)
{
	mPixmap::load(p, zone);

	char s[512];
	sprintf(s, "/style/%s/seek", zone);
	const char *url = p.getText(s);
	const char *root = p.getText("/style/root");
	if (root) {
		sprintf(s, "%s/%s", root, url);
		m_seek.loadFile(s);
	}
}

void mSeekBar::doTouch(int x, int y, int m)
{
	int yy = m_seek.height()/2;
	if (m && m_visible && x >= m_x && y >= (m_y-yy) && x <= (m_x+m_width) && y <= (m_y+m_height+yy)) {
		m_tp_x = x;
		mEvent::doFifo(this);
	}
	mPixmap::doTouch(x, y, m);
}

void mSeekBar::doPaint(void)
{
	int m = m_max-m_min;
	if (m <= 0) {
		m = 1;
	}
	int v = m_value-m_min;
	int sh = m_seek.width()/2;
	int offset = m_height-m_height*v/m-m_seek.height()/2;
	m_seek.move(m_x-sh+m_width/2, m_y+offset);

	if (m_visible) {
		if (m_url.length() > 0 && m_rgb565 == NULL) {
			const char *url = m_url.c_str();
			const char *ft = url+strlen(url)-4;
			if (!strcasecmp(ft, ".png"))
				this->png(url);
			else if (!strcasecmp(ft, ".jpg"))
				this->jpeg(url);
			m_url = "";
		}

		if (m_rgb_a != NULL) {
			if (m_alpha != 0xFF) {
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_width; x++) {
						uint32_t a = (m_rgb_a[y*m_width+x]*m_alpha)>>8;
						uint32_t c = rgb_565_2_24(m_rgb565[y*m_width+x]) & 0xFFFFFF;
						fb_pixel(m_x+x, m_y+y,  c | (a<<24));
					}
				}
			} else {
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_width; x++) {
						uint32_t a = m_rgb_a[y*m_width+x];
						uint32_t c = rgb_565_2_24(m_rgb565[y*m_width+x]) & 0xFFFFFF;
						if (y < offset) {
							fb_pixel(m_x+x, m_y+y, c | (a<<24));
						} else {
							fb_pixel(m_x+x, m_y+y, 0xFFF7B500);
						}
					}
				}
			}
		} else {
			int fb_offset = m_y*fb.m_width+m_x;
			int offset = 0, ww = m_width;
			if (ww+m_x > fb.m_width)
				ww = fb.m_width-m_x;
			for(int y=0; y<m_height; y++) {
				memcpy(&fb.m_rgb565[fb_offset], &m_rgb565[offset], 2*ww);
				offset += m_width;
				fb_offset += fb.m_width;
			}
		}
	}
	mObject::doPaint();
}

void mSeekBar::setFocus(int val)
{
	if (val) {
		m_value = 1.1*(m_max-m_min)*(m_tp_x-m_x)/m_width+m_min;
		if (m_value > m_max) {
			m_value = m_max;
		} else if (m_value < m_min) {
			m_value = m_min;
		}
		this->paint();
	}
	mPixmap::setFocus(val);
}
