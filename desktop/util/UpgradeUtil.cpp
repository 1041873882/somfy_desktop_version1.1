#include "sys.h"
#include "DebugUtil.h"

#include "UpgradeUtil.h"

UpgradeUtil upgradeUtil;

UpgradeUtil::UpgradeUtil()
{
	imgName = "";
	imgState = 0;
}

UpgradeUtil::~UpgradeUtil()
{
}

int UpgradeUtil::checkImgName(std::string name)
{
	if (name.empty()) {
		PRINT_DEBUG("name is empty\n");
		return -1;
	}
	int length = name.length();
	if (length < IMG_NAME_LENGTH || length > (IMG_NAME_LENGTH + 3)) {
		PRINT_DEBUG("name length is error, length: %d \n", length);
		return -2;
	}
	std::string versionMsg = "";
	versionMsg = name.substr(0, 8);
	if (!versionMsg.empty()) {
		if (versionMsg != PCB_VERSION) {
			PRINT_DEBUG("pcb version is error, name is %s \n", name.c_str());
			return 0;
		}
	} else {
		PRINT_DEBUG("get name 0-7 is error \n");
		return -3;
	}
	versionMsg = name.substr(8, 2);
	if (!versionMsg.empty()) {
		if (!(versionMsg == "_t" || versionMsg == "_v")) {
			PRINT_DEBUG("Name does not contain \"_t\" or \"_v\", and error\n");
			return -4;
		}
	}
	return 1;
}

int UpgradeUtil::addImgName(std::string name)
{
	if (!name.empty()) {
		imgName = name;
		imgState = 1;
	}
	return 1;
}

void UpgradeUtil::clearImgName()
{
	imgName = "";
	imgState = 0;
}
