/*
 * SysAppFactory.h
 *
 *  Created on: Oct 25, 2017
 *      Author: guoxs
 */

#ifndef _APP_SYSAPP_FACTORY_H_
#define _APP_SYSAPP_FACTORY_H_

#include <vector>

#include "AppTypeDef.h"
#include "BaseApp.h"

class SysAppFactory {
public:
	static SysAppFactory *getInstance();

	bool registerSysApp(int appType, BaseApp* (*create)());
	BaseApp* create(int appType);

private:
	SysAppFactory() { }

private:
	typedef struct {
		int appType;
		BaseApp* (*create)();
	} SSysAppInfo;

	std::vector<SSysAppInfo> mSysAppInfos;
};

#define SYSAPPFACTORY		SysAppFactory::getInstance()
#define REGISTER_SYSAPP(appType, _class) \
    static struct _SysAppFactory_##_class { \
        static BaseApp* create() { \
            return new _class(); \
        } \
        _SysAppFactory_##_class() { \
        	SYSAPPFACTORY->registerSysApp(appType, create); \
        } \
    } _autoRegister_SysApp_##_class

#endif /* _APP_SYSAPP_FACTORY_H_ */
