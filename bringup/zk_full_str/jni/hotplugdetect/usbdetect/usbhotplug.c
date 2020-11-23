/*
 * usbhotplug.c
 *
 *  Created on: 2020年2月14日
 *      Author: koda.xu
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/bitypes.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include "hotplug_common.h"
#include "usbhotplug.h"
#include "list.h"


#define CMD_BUFFER_SIZE 		512
#define MULTI_DEV_NAME_LEN_MIN  7     // sda/sda*
#define DEV_NAME_LEN_MIN    	2     // sd*
#define USB_PARTTITION_CHECK    "/proc/partitions"
#define USB_MOUNTS_CHECK    	"/proc/mounts"
#define USB_MOUNT_DEFAULT_DIR   "/vendor"

typedef struct
{
    list_t callbackList;
    UsbHotplugCallback pfnCallback;
}UsbCallbackListData_t;


static char g_line[CMD_BUFFER_SIZE];
static pthread_t g_checkHotplugThread = 0;
static int g_bCheckUsbThreadRun = 0;
static list_t g_usbCallbackListHead;
static pthread_mutex_t g_callbackListMutex;
static int g_usbStart = 0;

static int InitUsbHotplugSock()
{
    struct sockaddr_nl snl;
    const int buffersize = 2 * 1024 * 1024;
    int retval;
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);

    if (hotplug_sock == -1)
    {
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }

    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
    retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));

    if (retval < 0)
    {
        printf("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }

    return hotplug_sock;
}

static void DeinitUsbHotplugSock(int sock)
{
    if (sock != -1)
        close(sock);
}

static void *CheckUsbHotPlugProc(void *pdata)
{
    int hotplug_sock = InitUsbHotplugSock();
    char umountCmd[CMD_BUFFER_SIZE] = {0};
    char rmCmd[CMD_BUFFER_SIZE] = {0};
    int nRet = 0;

    printf("Exec CheckUsbHotPlugProc\n");

    while(g_bCheckUsbThreadRun)
    {
        char buf[UEVENT_BUFFER_SIZE*2] = {0};
        UsbCallbackListData_t *pstUsbCallbackData = NULL;
        list_t *pListPos = NULL;
        int len = recv(hotplug_sock, &buf, sizeof(buf), 0);

        if (strstr(buf, "sd") && (strlen(strstr(buf, "sd")) > DEV_NAME_LEN_MIN))
        {
        	//printf("\n******** start ********\n\n");
			//printf("%s\n", buf);
			//printf("\n******** end ********\n\n");

			//char *pstmsg = strstr(buf, "sd") + strlen("sda/");
			char *pstmsg = buf;

			if (strlen(strstr(buf, "sd")) > MULTI_DEV_NAME_LEN_MIN)
				pstmsg = strstr(buf, "sd") + strlen("sda/");
			else
				pstmsg = strstr(buf, "sd");

			printf("pstmsg is %s\n", pstmsg);

			UsbParam_t stUsbParam;
			memset(&stUsbParam, 0, sizeof(UsbParam_t));

			if (strstr(buf, "add"))
			{
				printf("usb add\n");

				// usb sda~sdh
				if (strlen(pstmsg) > 2 && (pstmsg[2] >= 'a' && pstmsg[2] <= 'h'))
				{
					stUsbParam.action = 1;
					sprintf(stUsbParam.udisk_path, "/vendor/udisk_%s", pstmsg);

					pthread_mutex_lock(&g_callbackListMutex);
					list_for_each(pListPos, &g_usbCallbackListHead)
					{
						pstUsbCallbackData = list_entry(pListPos, UsbCallbackListData_t, callbackList);
						pstUsbCallbackData->pfnCallback(&stUsbParam);
					}
					pthread_mutex_unlock(&g_callbackListMutex);
				}
			}

			if (strstr(buf, "remove"))
			{
				printf("usb remove\n");
				stUsbParam.action = 0;
				sprintf(stUsbParam.udisk_path, "/vendor/udisk_%s", pstmsg);

				pthread_mutex_lock(&g_callbackListMutex);
				list_for_each(pListPos, &g_usbCallbackListHead)
				{
					pstUsbCallbackData = list_entry(pListPos, UsbCallbackListData_t, callbackList);
					pstUsbCallbackData->pfnCallback(&stUsbParam);
				}
				pthread_mutex_unlock(&g_callbackListMutex);

				if (access(stUsbParam.udisk_path, F_OK) == 0)
				{
					//printf("access path:%s \n", stUsbParam.udisk_path);
					memset(umountCmd, 0, CMD_BUFFER_SIZE);
					sprintf(umountCmd, "umount %s", stUsbParam.udisk_path);
					nRet = system(umountCmd);
					if (nRet)
					{
						printf("umount  %s failed\n", stUsbParam.udisk_path);
						continue;
					}

					memset(rmCmd, 0, CMD_BUFFER_SIZE);
					sprintf(rmCmd, "rm -rf %s", stUsbParam.udisk_path);
					system(rmCmd);
				}
			}
        }

        usleep(50000);
    }

    printf("close socket\n");
    DeinitUsbHotplugSock(hotplug_sock);

    printf("exit thread proc\n");
    return NULL;
}

static char *freadline(FILE *stream)
{
    int count = 0;

    while(!feof(stream) && (count < CMD_BUFFER_SIZE) && ((g_line[count++] = getc(stream)) != '\n'));
    if (!count)
        return NULL;

    g_line[count - 1] = '\0';

    return g_line;
}

int USB_CheckCurrentStatus()
{
	FILE *pFile = fopen(USB_PARTTITION_CHECK, "r");		// usb mount成功前已经获取了状态，所以通过proc/partitions是否存在节点来判断
	//FILE *pFile = fopen(USB_MOUNTS_CHECK, "r");
	char *pCurLine = NULL;
	char *pSeek = NULL;
	int nRet = 0;

	if (pFile)
	{
		while((pCurLine = freadline(pFile)) != NULL)
		{
			pSeek = strstr(pCurLine, "sd");
			if (pSeek)
			{
				// usb: sda~sdh
				if (strlen(pSeek) > 2 && (pSeek[2] >= 'a' && pSeek[2] <= 'h'))
				{
					nRet = 1;
					break;
				}
			}
		}

		fclose(pFile);
		pFile = NULL;
	}

	if (nRet)
		printf("Exist usb device\n");
	else
		printf("No usb devices\n");

	return nRet;
}

void USB_GetUsbPath(char *pUsbPath, int nLen)
{
	strncpy(pUsbPath, USB_MOUNT_DEFAULT_DIR, nLen);
}

//int USB_StartCheckHotplug(UsbHotplugCallback pfnCallback)
int USB_StartCheckHotplug()
{
	pthread_mutex_init(&g_callbackListMutex, NULL);
	INIT_LIST_HEAD(&g_usbCallbackListHead);
	g_bCheckUsbThreadRun = 1;

	pthread_create(&g_checkHotplugThread, NULL, CheckUsbHotPlugProc, NULL);

	if (!g_checkHotplugThread)
	{
		printf("create check hotplug thread failed\n");
		return -1;
	}

	g_usbStart = 1;

	return 0;
}

void USB_StopCheckHotplug()
{
	g_usbStart = 0;
	g_bCheckUsbThreadRun = 0;

	if (g_checkHotplugThread)
	{
		printf("check thread is exiting\n");
		pthread_join(g_checkHotplugThread, NULL);
		g_checkHotplugThread = 0;
		printf("check thread exit\n");
	}

	UsbCallbackListData_t *pstUsbCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_callbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_usbCallbackListHead)
	{
		pstUsbCallbackData = list_entry(pListPos, UsbCallbackListData_t, callbackList);
		list_del(pListPos);
		free(pstUsbCallbackData);
	}
	pthread_mutex_unlock(&g_callbackListMutex);

	pthread_mutex_destroy(&g_callbackListMutex);
}

int USB_RegisterCallback(UsbHotplugCallback pfnCallback)
{
	UsbCallbackListData_t *pstUsbCallbackData = NULL;

	printf("Enter USB_RegisterCallback\n");

	if (g_usbStart)
	{
		if (!pfnCallback)
		{
			printf("usb callback is NULL\n");
			return -1;
		}

		pstUsbCallbackData = (UsbCallbackListData_t*)malloc(sizeof(UsbCallbackListData_t));
		memset(pstUsbCallbackData, 0, sizeof(UsbCallbackListData_t));
		pstUsbCallbackData->pfnCallback = pfnCallback;

		pthread_mutex_lock(&g_callbackListMutex);
		UsbCallbackListData_t *pos = NULL;

		list_for_each_entry(pos, &g_usbCallbackListHead, callbackList)
		{
			if (pos->pfnCallback == pfnCallback)
			{
				printf("have registered usb callback\n");
				pthread_mutex_unlock(&g_callbackListMutex);
				return 0;
			}
		}

		list_add_tail(&pstUsbCallbackData->callbackList, &g_usbCallbackListHead);
		pthread_mutex_unlock(&g_callbackListMutex);
	}

	printf("Leave USB_RegisterCallback\n");

	return 0;
}

void USB_UnRegisterCallback(UsbHotplugCallback pfnCallback)
{
	UsbCallbackListData_t *pstUsbCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	printf("Enter USB_UnRegisterCallback\n");

	if (g_usbStart)
	{
		pthread_mutex_lock(&g_callbackListMutex);
		list_for_each_safe(pListPos, pListPosN, &g_usbCallbackListHead)
		{
			pstUsbCallbackData = list_entry(pListPos, UsbCallbackListData_t, callbackList);

			if (pstUsbCallbackData->pfnCallback == pfnCallback)
			{
				list_del(pListPos);
				free(pstUsbCallbackData);
			}
		}
		pthread_mutex_unlock(&g_callbackListMutex);
	}

	printf("Leave USB_UnRegisterCallback\n");
}
