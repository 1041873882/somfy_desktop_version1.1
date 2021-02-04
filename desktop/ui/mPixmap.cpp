
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <jpeg/jpeglib.h>
#include <png/png.h>

#include "mFB.h"
#include "mPixmap.h"
#include "mWindow.h"

mPixmap::mPixmap()
{
	m_name = "mPixmap";
	m_rgb565 = NULL;
	m_rgb_a = NULL;
	m_alpha = 0xFF;
	m_jpeg = 0;
}

mPixmap::~mPixmap()
{
	this->recycle();
}

void mPixmap::recycle(void)
{
	if (m_rgb565) {
		delete [] m_rgb565;
		m_rgb565 = NULL;
	}
	if (m_rgb_a) {
		delete [] m_rgb_a;
		m_rgb_a = NULL;
	}
	m_width = 0;
	m_height = 0;
	m_jpeg = 0;
}

int mPixmap::loadFile(const char *url)
{
	this->recycle();
	if (url != NULL)
		m_url = url;
	return 0;
}

int mPixmap::png(const char *url)
{
	FILE *fp = fopen(url, "rb");
	if (fp == NULL)
		return -1;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	m_width = png_get_image_width(png_ptr, info_ptr);
	m_height = png_get_image_height(png_ptr, info_ptr);
	int type = png_get_color_type(png_ptr, info_ptr);

	m_rgb565 = new uint16_t[m_width*m_height];
	if (m_rgb565) {
		int block = (type == 6 ? 4 : 3); //type == 6 有Alpha，其他无
		int n = 0;
		png_bytep *row = png_get_rows(png_ptr, info_ptr);
		if (block == 4) {
			m_rgb_a = new uint8_t[m_width*m_height];
			for (int i=0; i<m_height; i++) {
				for (int j=0; j<(block*m_width); j+=block) {
					uint8_t *p = (uint8_t *)&row[i][j];
					//转换成ARGB格式
					m_rgb565[n] = rgb_24_2_565(p[0]<<16 | p[1]<<8 | p[2]);
					m_rgb_a[n] = p[3];
					n++;
				}
			}
		} else {
			if (!fb.m_touch) {
				m_rgb_a = new uint8_t[m_width*m_height];
			}
			for (int i=0; i<m_height; i++) {
				for (int j=0; j<(block*m_width); j+=block) {
					uint8_t *p = (uint8_t *)&row[i][j];
					//转换成ARGB格式
					m_rgb565[n] = rgb_24_2_565(p[0]<<16 | p[1]<<8 | p[2]);
					if (!fb.m_touch) {
						m_rgb_a[n] = 0xFF;
					}
					n++;
				}
			}
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	return 0;
}

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

static void my_error_exit(j_common_ptr cinfo)
{
	struct my_error_mgr *myerr = (struct my_error_mgr *) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

int mPixmap::jpeg(const char *url)
{
	int result = -1;
	if (m_width != 0 && m_height != 0) { //需要缩放，使用硬件解码器
		result = this->loadJpegHw(url);
	}
	if (result) {
		result = this->loadJpegSw(url);
	}
	return result;
}

void mPixmap::load(dxml &p, const char *zone)
{
	const char *root = p.getText("/style/root");
	if (root) {
		char s[512], s2[512];
		sprintf(s, "/style/%s/url", zone);
		sprintf(s2, "%s/%s", root, p.getText(s));
		mObject::load(p, zone);
		this->loadFile(s2);
	}
}

void mPixmap::loadUrl(dxml &p, const char *zone)
{
	const char *root = p.getText("/style/root");
	if (root) {
		char s[512], s2[512];
		sprintf(s, "/style/%s/url", zone);
		sprintf(s2, "%s/%s", root, p.getText(s));
		this->loadFile(s2);
	}
}

void mPixmap::doPaint(void)
{
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
					for (int x=0; x<m_width; x++) {
						uint32_t a = (m_rgb_a[y*m_width+x]*m_alpha)>>8;
						uint32_t c = rgb_565_2_24(m_rgb565[y*m_width+x]) & 0xFFFFFF;
						fb_pixel(m_x+x, m_y+y, c | (a<<24));
					}
				}
			} else {
				for(int y=0; y<m_height; y++) {
					for (int x=0; x<m_width; x++) {
						uint32_t a = m_rgb_a[y*m_width+x];
						uint32_t c = rgb_565_2_24(m_rgb565[y*m_width+x]) & 0xFFFFFF;
						fb_pixel(m_x+x, m_y+y, c | (a<<24));
					}
				}
			}
		} else {
			int fb_offset = m_y*fb.m_width+m_x;
			int offset = 0, ww = m_width;
			if (ww+m_x > fb.m_width)
				ww = fb.m_width-m_x;
			for (int y=0; y<m_height; y++) {
				memcpy(&fb.m_rgb565[fb_offset], &m_rgb565[offset], 2*ww);
				offset += m_width;
				fb_offset += fb.m_width;
			}
		}
	}
	mObject::doPaint();
}

int mPixmap::loadJpegSw(const char *url)
{
	FILE *fp = fopen(url, "rb");
	if (fp == NULL)
		return -1;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	 /* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		fclose(fp);
		return -1;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	m_width = cinfo.output_width;
	m_height = cinfo.output_height;
	m_rgb565 = new uint16_t[m_width*m_height];
	if (m_rgb565) {
		int sz = cinfo.output_width * cinfo.output_components;
		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, sz, 1);
		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, buffer, 1);

			int n = 0;
			uint16_t *p = m_rgb565+m_width*(cinfo.output_scanline-1);
			uint8_t *d = buffer[0];
			for (int i=0; i<m_width; i++) {
				p[i] = rgb_24_2_565((d[n]<<16) | (d[n+1]<<8) | d[n+2]);
				n += 3;
			}
		}
		m_jpeg = 1;
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	return 0;
}

#define CMD_JPEG_DECODE 0x12345690
#define CMD_JPEG_WIDTH 0x12345691
#define CMD_JPEG_HEIGHT 0x12345692
#define CMD_JPEG_MMSZ 0x12345693

#define u32 uint32_t

typedef struct {
	u32 width; //图片输出宽 0:原始大小，其他：进行缩放处理
	u32 height; //图片输出高
	u32 length; //jpeg数据长度
} mjpeg_decoder_cmd_t;

int mPixmap::loadJpegHw(const char *url)
{
	int result = -1;
	FILE *fp = fopen(url, "rb");
	if (fp) {
		int fd = ::open("/dev/cedar_dev", O_RDWR, 0);
		if (fd  > 0) {
			int mm_sz = ::ioctl(fd, CMD_JPEG_MMSZ, 0);
			uint32_t *mm = (uint32_t *)::mmap(NULL, mm_sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

			mjpeg_decoder_cmd_t d;
			d.width = m_width ? m_width : 0;
			d.height = m_height ? m_height : 0;
			d.length = fread(mm, 1, mm_sz, fp);
			int r = ::ioctl(fd, CMD_JPEG_DECODE, &d);
			if (!r) {
				m_width = ::ioctl(fd, CMD_JPEG_WIDTH, 0);
				m_height = ::ioctl(fd, CMD_JPEG_HEIGHT, 0);
				m_rgb565 = new uint16_t[m_width*m_height];
				if (m_rgb565) {
					int offset = 0;
					for(int y=0; y<m_height; y++) {
						for(int x=0; x<m_width; x++) {
							m_rgb565[offset+x] = rgb_24_2_565(mm[offset+x]);
						}
						offset += m_width;
					}
					result = 0;
					m_jpeg = 1;
				}
			} else {
				fprintf(stderr, "mPixmap::loadJpegHw decode err!\n");
			}

			::munmap(mm, mm_sz);
			::close(fd);
		}
		fclose(fp);
	}
	return result;
}
