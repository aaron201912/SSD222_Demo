/*
 * usbhotplug.h
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */

#ifndef JNI_HOTPLUGDETECT_USBDETECT_USBHOTPLUG_H_
#define JNI_HOTPLUGDETECT_USBDETECT_USBHOTPLUG_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#define PATH_BUFFER_SIZE 	512

typedef struct UsbParam_s
{
	int action;
	char udisk_path[PATH_BUFFER_SIZE];
}UsbParam_t;

typedef void (*UsbHotplugCallback)(UsbParam_t *pstUsbParam);

int USB_CheckCurrentStatus();
void USB_GetUsbPath(char *pUsbPath, int nLen);
int USB_StartCheckHotplug();
void USB_StopCheckHotplug();
int USB_RegisterCallback(UsbHotplugCallback pfnCallback);
void USB_UnRegisterCallback(UsbHotplugCallback pfnCallback);

#ifdef  __cplusplus
}
#endif

#endif /* JNI_HOTPLUGDETECT_USBDETECT_USBHOTPLUG_H_ */
