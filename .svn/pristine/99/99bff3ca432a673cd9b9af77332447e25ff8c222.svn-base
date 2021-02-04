#include "FileUtil.h"
#include "SysUpgrade.h"

SysUprade::SysUprade()
{
}

SysUprade::~SysUprade()
{
}

//开始升级，url为升级镜像完整路径
void SysUprade::start(const char *url)
{
	dmsg req;
	dxml p;
	p.setText("/params/url", url);
	req.request("/upgrade/local", p.data());
}

//升级状态查询，小于0:升级失败 0-100:升级进度
int SysUprade::query(void)
{
	dmsg req;
	int r = req.request("/upgrade/query", NULL);
	if (r == 200) {
		dxml p(req.body());
		int d = p.getInt("/params/percent", 0);
		return d;
	}
	return -1;
}
