
#ifndef __M_PIXMAP_H__
#define __M_PIXMAP_H__

#include "mObject.h"

class mPixmap : public mObject {
public:
	mPixmap();
	virtual ~mPixmap();

	int loadFile(const char *url);
	int png(const char *url);
	int jpeg(const char *url);
	void recycle(void);

	int loadJpegSw(const char *url);
	int loadJpegHw(const char *url);

	void load(dxml &p, const char *zone);
	void loadUrl(dxml &p, const char *zone);
	int width(void) { return m_width; };
	void setWidth(int width) { m_width = width; };
	int height(void) { return m_height; };
	void setHeight(int height) { m_height = height; };
	void alpha(uint8_t val) { m_alpha = val; };

	void doPaint(void);

protected:
	std::string m_url;
	uint16_t *m_rgb565;
	uint8_t *m_rgb_a;
	uint8_t m_alpha;
	int m_jpeg;
};

#endif
