/*
 * SecurityManager.h
 *
 *  Created on: Jan 23, 2018
 *      Author: guoxs
 */

#ifndef _SECURITY_SECURITY_MANAGER_H_
#define _SECURITY_SECURITY_MANAGER_H_

#include <stdint.h>

typedef enum {
	E_SECDATA_PAGE_RESERVE = 2,
	E_SECDATA_PAGE_USER
} ESecDataPage;

class SecurityManager {
public:
	~SecurityManager();

	static SecurityManager* getInstance();

	bool isSecDataLock(ESecDataPage page = E_SECDATA_PAGE_USER) const;
	bool lockSecData(ESecDataPage page = E_SECDATA_PAGE_USER);

	int writeSecData(uint8_t *pData, int len, ESecDataPage page = E_SECDATA_PAGE_USER);
	int readSecData(uint8_t *pData, int len, ESecDataPage page = E_SECDATA_PAGE_USER);

	/**
	 * @brief 获取机器唯一ID码
	 * @return devID长度
	 */
	int getDevID(uint8_t devID[16]);

private:
	SecurityManager();
};

#define SECURITYMANAGER			SecurityManager::getInstance()

#endif /* _SECURITY_SECURITY_MANAGER_H_ */
