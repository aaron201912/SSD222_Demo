/*
 * SecurityManager.h
 *
 *  Created on: Jan 23, 2018
 *      Author: guoxs
 */

#ifndef _SECURITY_SECURITY_MANAGER_H_
#define _SECURITY_SECURITY_MANAGER_H_

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

	int writeSecData(unsigned char *pData, int len, ESecDataPage page = E_SECDATA_PAGE_USER);
	int readSecData(unsigned char *pData, int len, ESecDataPage page = E_SECDATA_PAGE_USER);

	/**
	 * @brief 获取机器唯一ID码
	 */
	bool getDevID(unsigned char devID[8]);

	void forbidExecute(const char *pPara = NULL);
	void permitExecute(const char *pPara = NULL);

private:
	SecurityManager();
};

#define SECURITYMANAGER			SecurityManager::getInstance()

#endif /* _SECURITY_SECURITY_MANAGER_H_ */
