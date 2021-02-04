#include <sys/stat.h>
#include <sys/types.h>

#include "mFB.h"
#include "DebugUtil.h"
#include "BeanUtil.h"
#include "PictureUtil.h"
#include "UpgradeUtil.h"
#include "FileUtil.h"

#define FILTER_FILE_SIZE    1024 //过滤掉小于 1024 字节的文件

FileUtil::FileUtil()
{
}

FileUtil::~FileUtil()
{
}

/*
* 路径下会有
* .  ..  两个文件
* 
*/
int FileUtil::loadFile(const char *path, int state)
{
	if (!fb.enabled()) {
		return -1;
	}
	DIR *dirp = NULL;
	struct dirent *dir_entry = NULL;
	PRINT_DEBUG("loadFile path: %s\n", path);
	if (!(dirp = opendir(path))) {
		PRINT_DEBUG("file load error \n");
		return -1;
	}
	//先清缓存
	if (state == STATE_JPG) {
		pictureUtil.clearPictureName();
	} else if (state == STATE_MP3) {
		//musicUtil.clearMusicName();
	} else if (state == STATE_IMG) {
		upgradeUtil.clearImgName();
	}
	int index = 0;
	std::string name;
	struct stat st;
	while ((dir_entry = readdir(dirp)) != NULL) {
		index++;
		name = dir_entry->d_name;
		char buf[128];
		sprintf(buf, "%s/%s", path, name.c_str());
		//PRINT_DEBUG("file name: %s \n",buf);
		if (stat(buf, &st) < 0) {
			PRINT_DEBUG("get file info error\n");
			continue;
		}
		//主要是避免抓怕过程中，图片还没有保存完整，直接拔出SD卡，导致图片大小是 0 kb的问题
		if (st.st_size < FILTER_FILE_SIZE) {
			PRINT_DEBUG("filter name: %s and file size: %ld\n", name.c_str(), st.st_size);
			continue;
		}
#if  0
		PRINT_DEBUG("index: %d and name: %s \n",index,name.c_str());
#endif
		if (state == STATE_JPG && name.find(".jpg", 0) != name.npos) {
			//需要在这里加上验证
			pictureUtil.addPictureName(name);
		} else if (state == STATE_MP3 && name.find(".mp3", 0) != name.npos) {
			// int isOk = musicUtil.checkMusicName(name);
			// if (isOk) {
			// 	musicUtil.addMusicName(name);
			// }
		} else if (state == STATE_IMG && name.find(".img", 0) != name.npos) {
			int isOk = upgradeUtil.checkImgName(name);
			if (isOk == 1) {
				upgradeUtil.addImgName(name);
			}
		} else {
			//PRINT_DEBUG("index: %d, invalid file: %s \n", index, name.c_str());
		}
	}
	closedir(dirp);
	//PRINT_DEBUG("all file: %d \n", index);
	return index;
}
