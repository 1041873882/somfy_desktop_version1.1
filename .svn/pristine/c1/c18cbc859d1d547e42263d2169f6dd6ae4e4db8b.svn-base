#ifndef __UPGRADE_UTIL_H__
#define __UPGRADE_UTIL_H__

#include <string>

#define SD_CARD_UPDATE_PATH		"/var/sd/update"
#define IMG_NAME_LENGTH			30

class UpgradeUtil {
public:
	UpgradeUtil();
	~UpgradeUtil();
	
	int checkImgName(std::string name);
	int addImgName(std::string name);
	void clearImgName();
	
	std::string imgName;
	int imgState;
};

extern UpgradeUtil upgradeUtil;

#endif