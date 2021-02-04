#ifndef __PICTURE_UTIL_H__
#define __PICTURE_UTIL_H__

#include <list>
#include <string>

#define MAX_PICTURE_NUM		100
#define PIC_SAVE_PATH		"/dnake/data/PIC"

#define PIC_NAME_SUF_SIZE   20  //带后缀(20190910121212_1.jpg)
#define PIC_NAME_OLD_SIZE   18  //带后缀(20190910121212.jpg)
#define PIC_NAME_LENGTH     16  //不带后缀(20190910121212_1)
#define PIC_NAME_SHOW_SIZE  14  //不带后缀(20190910121212)

class PictureUtil {
public:
	PictureUtil();
	~PictureUtil();

	void setPictureNum(int num);
	int  getPictureNum();

	void setCurrentPosition(int position);
	int  getCurrentPosition(void);

	void changeSysFileName(int currentPosition, std::string *name);
	int  changeSysFileName(std::string name, std::string *newName);
	void removeSysPicture(std::string name);
	void deleteOnePicture(int currentPosition);
	void deleteAllPictures(void);
	int  removeCapturePicture();

	void loadFilePicture();
	int loadPictureName(int currentPosition, std::string *name);

	int addPictureName(std::string name, int sort=0);
	int removePictureName(std::string name);
	int removeFinalPicture();
	int isUnReadPicture(std::string name);

	void clearPictureName();
	void sortPictureName();
	void sortByStatusAndName(void);
public:
	int m_changeState;
	int m_captureState;
	std::string m_captureName;
	std::list<std::string> oldPictureList;
	std::list<std::string> newPictureList;
	std::list<std::string> pictureNameList;

	int pictureNum;
	int currentPosition;
};

extern PictureUtil pictureUtil;

#endif
