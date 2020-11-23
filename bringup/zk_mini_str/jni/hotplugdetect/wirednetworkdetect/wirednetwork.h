/*
 * wirednetwork.h
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */

#ifndef JNI_HOTPLUGDETECT_WIREDNETWORKDETECT_WIREDNETWORK_H_
#define JNI_HOTPLUGDETECT_WIREDNETWORKDETECT_WIREDNETWORK_H_

#ifdef  __cplusplus
extern "C"
{
#endif

typedef void (*NetHotplugCallback)(unsigned int index, int status, char *pstIfName);

int WiredNetwork_StartCheckHotplug();
void WiredNetwork_StopCheckHotplug();
int WiredNetwork_RegisterCallback(NetHotplugCallback pfnCallback);
void WiredNetwork_UnRegisterCallback(NetHotplugCallback pfnCallback);

#ifdef  __cplusplus
}
#endif

#endif /* JNI_HOTPLUGDETECT_WIREDNETWORKDETECT_WIREDNETWORK_H_ */
