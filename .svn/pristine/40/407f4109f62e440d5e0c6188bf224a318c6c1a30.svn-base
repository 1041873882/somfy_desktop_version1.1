#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__

#include <dirent.h>
#include <sys/types.h>
#include "sys.h"

/*
* If the number of pictures is large, it may cause long reading time.
*  
* The file is loaded from the back to the front
*/
class FileUtil {
public:
	FileUtil();
	~FileUtil();
	
	enum {
		STATE_DEF = 0x00,
		STATE_JPG = 0x01,
		STATE_MP3 = 0x02,
		STATE_IMG = 0x03,
	} STATE;
	
	int loadFile(const char *path, int state);
};

#endif
