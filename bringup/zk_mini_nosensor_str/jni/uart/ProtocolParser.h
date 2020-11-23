/*
 * ProtocolParser.h
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */
#ifndef _PROTOCOL_PARSER_H_
#define _PROTOCOL_PARSER_H_

#include "ProtocolData.h"

typedef void (*OnProtocolDataUpdateFun)(const SProtocolData&);

void registerProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener);
void unregisterProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener);

SProtocolData& getProtocolData();

#endif /* _PROTOCOL_PARSER_H_ */
