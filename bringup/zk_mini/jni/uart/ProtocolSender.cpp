/*
 * ProtocolSender.cpp
 *
 *  Created on: Sep 8, 2017
 *      Author: guoxs
 */

#include "uart/ProtocolSender.h"
#include "uart/UartContext.h"
#include "utils/Log.h"
#include <stdio.h>

bool sendProtocol(const UINT16 cmdID,const BYTE *pData, BYTE len) {

	BYTE dataBuf[7];

	dataBuf[0] = 0xFF;
	dataBuf[1] = 0xFF;

	dataBuf[2] = cmdID;
	if (len >= 2) {
        dataBuf[3] = pData[0];
        dataBuf[4] = pData[1];
	}

	dataBuf[5] = 0xFF;
	dataBuf[6] = 0xFF;


	return UARTCONTEXT->send(dataBuf, sizeof(dataBuf));
}

bool sendRaw(const BYTE* pData, int len) {
    return UARTCONTEXT->send(pData, len);
}
