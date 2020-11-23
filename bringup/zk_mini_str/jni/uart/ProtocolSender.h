/*
 * ProtocolSender.h
 *
 *  Created on: Sep 8, 2017
 *      Author: guoxs
 */

#ifndef _PROTOCOL_SENDER_H_
#define _PROTOCOL_SENDER_H_

#include "CommDef.h"

bool sendProtocol(const UINT16 cmdID,const BYTE *pData, BYTE len);
bool sendRaw(const BYTE* pData, int len);
#endif /* _PROTOCOL_SENDER_H_ */
