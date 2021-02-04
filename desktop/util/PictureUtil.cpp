#include "mFB.h"
#include "SysSdcard.h"

#include "DebugUtil.h"
#include "FileUtil.h"
#include "PictureUtil.h"

#define  LOAD_SEQUENCE   1  // 0 is reverse; 1 is direction

PictureUtil pictureUtil;

PictureUtil::PictureUtil()
{
	pictureNum = 0;
	m_changeState = 1;
	m_captureState = 0;
	m_captureName = "";
}

PictureUtil::~PictureUtil()
{
}

void PictureUtil::setPictureNum(int num)
{
	pictureNum = num;
}

int PictureUtil::getPictureNum()
{
	return pictureNum;
}

void PictureUtil::setCurrentPosition(int position)
{
	currentPosition = position;
}

int PictureUtil::getCurrentPosition()
{
	return currentPosition;
}

int PictureUtil::isUnReadPicture(std::string name)
{
	if (name.empty()) {
		return 0;
	}
	if ((name.length() == PIC_NAME_SUF_SIZE) || (name.length() == PIC_NAME_LENGTH)) {
		return 1;
	}
	return 0;
}

void PictureUtil::changeSysFileName(int currentPosition, std::string *name)
{
	int index = 0;
	std::list<std::string>::iterator itor;
	for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
		if (index == currentPosition) {
			std::string tmpName = (*itor);
			if (!tmpName.empty() && tmpName.length() == PIC_NAME_LENGTH) {
				char buf[128];
				std::string newFileName = tmpName.substr(0, PIC_NAME_SHOW_SIZE);
				sprintf(buf,"mv %s/%s %s/%s.jpg", PIC_SAVE_PATH, name[0].c_str(),
						PIC_SAVE_PATH, newFileName.c_str());
				PRINT_DEBUG("cmd: %s\n", buf);
				system(buf);
				sprintf(buf, "%s.jpg", newFileName.c_str());
				name[0] = buf;
				*itor = newFileName.c_str();
				return;
			}
		}
		index++;
	}
}

//name 带后缀
int PictureUtil::changeSysFileName(std::string name, std::string *newName)
{
	if (name.empty()) {
		PRINT_DEBUG("name is empty\n");
		return -1;
	}
	if (name.length() != PIC_NAME_SUF_SIZE) {
		PRINT_DEBUG("name not change\n");
		return 0;
	}
	char buf[128];
	std::string newPicName = name.substr(0, PIC_NAME_SHOW_SIZE);
	sprintf(buf,"mv %s/%s %s/%s.jpg", PIC_SAVE_PATH, name.c_str(),
			PIC_SAVE_PATH, newPicName.c_str());
	PRINT_DEBUG("cmd: %s\n",buf);
	system(buf);
	std::string showName = name.substr(0,PIC_NAME_LENGTH);
	std::list<std::string>::iterator itor;
	for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
		std::string tmpName = (*itor);
		if (!tmpName.empty() && tmpName == showName) {
			pictureNameList.insert(itor, newPicName);
			pictureNameList.erase(itor++);
			sprintf(buf, "%s.jpg", newPicName.c_str());
			newName[0] = buf;
			return 1;
		}
	}
	return 0;
}

void PictureUtil::removeSysPicture(std::string name)
{
	if (name.empty()) {
		PRINT_DEBUG("name is empty \n");
		return;
	}
	char buf[128];
	if (name.find(".jpg", 0) == name.npos) {
		sprintf(buf,"%s.jpg", name.c_str());
		name = buf;
	}
	sprintf(buf,"rm -f %s/%s", PIC_SAVE_PATH, name.c_str());
	PRINT_DEBUG("cmd: %s\n", buf);
	system(buf);
}

void PictureUtil::deleteOnePicture(int currentPosition)
{
	if (currentPosition < 0 || pictureNameList.empty()) {
		PRINT_DEBUG("pictureNameList is empty or currentPosition is <  0, and position is %d\n", currentPosition);
		return;
	}
	int index = 0;
	std::list<std::string>::iterator itor;
	for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
		if (index == currentPosition) {
			char buf[128];
			std::string tmpName = (*itor);
			sprintf(buf,"rm -f %s/%s.jpg", PIC_SAVE_PATH, tmpName.c_str());
			PRINT_DEBUG("cmd:%s\n", buf);
			system(buf);
			pictureNameList.erase(itor);
			pictureNum--;
			break;
		}
		index++;
	}
}

#define TMP_PIC_PATH  "/var/sd/tmp_pic"
void PictureUtil::deleteAllPictures(void)
{
	if (pictureNameList.empty()) {
		PRINT_DEBUG("pictureNameList is empty\n");
		return;
	}
	char buf[64];
	sprintf(buf, "rm -rf %s/*", PIC_SAVE_PATH);
	system(buf);
	clearPictureName();
}

int PictureUtil::removeCapturePicture()
{
	if (!m_captureName.empty() && m_captureState) {
		char buf[128];
		sprintf(buf,"rm -rf %s/%s", PIC_SAVE_PATH, m_captureName.c_str());
		PRINT_DEBUG("remove buf is %s\n", buf);
		system(buf);
		removePictureName(m_captureName);
		m_captureName = "";
		m_captureState = 0;
		PRINT_DEBUG("pic num: %d \n", pictureNum);
	}
	return 0;
}

void PictureUtil::loadFilePicture()
{
	if (!fb.enabled()) {
		return;
	}
	if (m_changeState) {
		m_changeState = 0;
	} else {
		return;
	}
	clearPictureName();
	FileUtil fileUtil;
	int picNum = fileUtil.loadFile(PIC_SAVE_PATH, fileUtil.STATE_JPG);
	if (picNum < 0) {
		char buf[128];
		sprintf(buf, "mkdir %s", PIC_SAVE_PATH);
		int isOk = system(buf);
		if (isOk < 0) {
			PRINT_DEBUG("create picture path error\n");
		}
		return;
	}
	while (pictureNum > MAX_PICTURE_NUM) {
		removeFinalPicture();
	}
	//sortPictureName();
	sortByStatusAndName();
}

//start from 0
int PictureUtil::loadPictureName(int currentPosition, std::string *name)
{
	if (currentPosition < 0 || currentPosition > (pictureNum -1)) {
		PRINT_DEBUG("currentPosition %d ,and out of range\n", currentPosition);
		return -1;
	}
	int index = 0;
	char buf[128];
	std::list<std::string>::iterator itor;
	for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
		if (index == currentPosition) {
			sprintf(buf, "%s.jpg", (*itor).c_str());
			name[0] = buf;
			return 1;
		}
		index++;
	}
	return 0;
}

//带后缀
int PictureUtil::addPictureName(std::string name, int sort)
{
	if (name.empty()) {
		PRINT_DEBUG("name is empty \n");
		return -1;
	}
	int length = name.length();
	std::string tmpName = name.substr(0, length-4);
	// Avoid the same name
	int found = 0;
	if (!pictureNameList.empty()) {
		std::list<std::string>::iterator itor;
		for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
			if (*itor == tmpName) {
				found = 1;
				pictureNameList.erase(itor);
				break;
			}
		}
	}
	if (found) {
		removeSysPicture(name);
	} else {
		pictureNum++;
	}
	if (pictureNum > MAX_PICTURE_NUM) {
		removeFinalPicture();
	}
#if 0
	pictureNameList.push_back(tmpName);
#else
	pictureNameList.push_front(tmpName);
	if (sort) {
		sortByStatusAndName();
	}
#endif
	return pictureNum;
}

//不带后缀
int PictureUtil::removePictureName(std::string name)
{
	std::string tmpName;
	if (pictureNameList.empty()) {
		PRINT_DEBUG("picture name list is empty \n");
		return -1;
	}
	tmpName = name;
	if (tmpName.find(".jpg", 0) != tmpName.npos) {
		std::string subString = tmpName;
		int size = tmpName.length();
		tmpName = subString.substr(0, (size - 4));
	}
	PRINT_DEBUG("remove name is %s\n", tmpName.c_str());
	pictureNameList.remove(tmpName);
	pictureNum--;
	return 0;
}

int PictureUtil::removeFinalPicture()
{
	if (pictureNum > MAX_PICTURE_NUM) {
		if (!pictureNameList.empty()) {
			std::string oldName = pictureNameList.back();
			char buf[128];
			sprintf(buf,"rm %s/%s.jpg", PIC_SAVE_PATH, oldName.c_str());
			PRINT_DEBUG("cmd: %s \n ", buf);
			system(buf);
			pictureNameList.pop_back();
			pictureNum--;
		}
	}
	return 0;
}

//时间排序
void PictureUtil::sortPictureName()
{
	pictureNameList.sort();
#if LOAD_SEQUENCE
	pictureNameList.reverse();
#endif
#if 0
	std::list<std::string>::iterator itor;
	for(itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++){
		std::string tmp = *itor;
		PRINT_DEBUG("name is %s \n",tmp.c_str());
	}
#endif
}

void PictureUtil::sortByStatusAndName(void)
{
	std::list<std::string> oldList;
	std::list<std::string> newList;
	std::list<std::string>::iterator itor;
	for (itor = pictureNameList.begin(); itor != pictureNameList.end(); itor++) {
		const char *name = (*itor).c_str();
		if (isUnReadPicture(name)) {
			newList.push_front(name);
		} else {
			oldList.push_front(name);
		}
	}
	oldList.sort();
	oldList.reverse();
	newList.sort();
	newList.reverse();
	pictureNameList.clear();
	pictureNameList.splice(pictureNameList.begin(), oldList);
	pictureNameList.splice(pictureNameList.begin(), newList);
}

void PictureUtil::clearPictureName()
{
	pictureNameList.clear();
	pictureNum = 0;
}
