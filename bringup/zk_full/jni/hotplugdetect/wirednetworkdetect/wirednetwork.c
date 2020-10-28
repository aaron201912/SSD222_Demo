/*
 * wirednetwork.c
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
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include "hotplug_common.h"
#include "wirednetwork.h"
#include "list.h"


#define INTERFACE_NAME_LEN 32

typedef struct
{
	list_t callbackList;
	NetHotplugCallback pfnCallback;
}WiredNetworkCallbackListData_t;

static pthread_t g_checkNetHotplugThread = 0;
static int g_bCheckNetThreadRun = 0;
static list_t g_wiredNetworkCallbackListHead;
static pthread_mutex_t g_callbackListMutex;

static int InitNetHotplugSock()
{
    struct sockaddr_nl snl;
    const int buffersize = 2 * 1024 * 1024;
    int retval;

    memset(&snl, 0, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = RTNLGRP_LINK;

    int nethotplug_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (nethotplug_sock == -1)
    {
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }

    /* set receive buffersize */
    setsockopt(nethotplug_sock, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
    retval = bind(nethotplug_sock, (struct sockaddr*)&snl, sizeof(struct sockaddr_nl));
    if (retval < 0)
    {
        printf("bind failed: %s", strerror(errno));
        close(nethotplug_sock);
        nethotplug_sock = -1;
        return -1;
    }

    pthread_mutex_init(&g_callbackListMutex, NULL);
    INIT_LIST_HEAD(&g_wiredNetworkCallbackListHead);

    return nethotplug_sock;
}

static void DeinitNetHotplugSock(int sock)
{
	WiredNetworkCallbackListData_t *pstWiredNetworkCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_callbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_wiredNetworkCallbackListHead)
	{
		pstWiredNetworkCallbackData = list_entry(pListPos, WiredNetworkCallbackListData_t, callbackList);
		list_del(pListPos);
		free(pstWiredNetworkCallbackData);
	}
	pthread_mutex_unlock(&g_callbackListMutex);

	pthread_mutex_destroy(&g_callbackListMutex);

	if (sock != -1)
		close(sock);
}

static void *CheckNetHotPlugProc(void *pdata)
{
	int nethotplug_sock = InitNetHotplugSock();
	//NetHotplugCallback pfnCallbak = (NetHotplugCallback)pdata;
    struct nlmsghdr *nh;
    struct ifinfomsg *ifinfo;
    struct rtattr *attr;
    unsigned int ifindex;
    int status;
    char buf[UEVENT_BUFFER_SIZE*2] = {0};
    char szIfName[INTERFACE_NAME_LEN] = {0};

	while(g_bCheckNetThreadRun)
	{
		WiredNetworkCallbackListData_t *pstWiredNetworkCallbackData = NULL;
		list_t *pListPos = NULL;
	    ifindex = 0;
        status = 0;
        memset(buf, 0, UEVENT_BUFFER_SIZE*2);
        memset(szIfName, 0, INTERFACE_NAME_LEN);

		int len = recv(nethotplug_sock, &buf, sizeof(buf), 0);

		//printf("wired net: %s, len=%d\n", buf, len);

        for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len))
        {
            if (nh->nlmsg_type == NLMSG_DONE)
                break;
            else if (nh->nlmsg_type == NLMSG_ERROR)
                goto msg_error;
            else if (nh->nlmsg_type != RTM_NEWLINK)
                continue;

            ifinfo = (struct ifinfomsg *)NLMSG_DATA(nh);
            //printf("%u: %s", ifinfo->ifi_index, (ifinfo->ifi_flags & IFF_LOWER_UP) ? "up" : "down" );
            ifindex = ifinfo->ifi_index;
            if (ifinfo->ifi_flags & IFF_LOWER_UP)
            {
                status = 1;
            }
            else
            {
                status = 0;
            }
            attr = (struct rtattr*)(((char*)nh) + NLMSG_SPACE(sizeof(*ifinfo)));
            len = nh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
            for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
            {
                if (attr->rta_type == IFLA_IFNAME)
                {
                    //printf(" %s", (char*)RTA_DATA(attr));
                    if (strlen((char*)RTA_DATA(attr)) < 32)
                    {
                        strcpy(szIfName, (char*)RTA_DATA(attr));
                    }
                    break;
                }
            }
            //printf("\n");

            pthread_mutex_lock(&g_callbackListMutex);
			list_for_each(pListPos, &g_wiredNetworkCallbackListHead)
			{
				pstWiredNetworkCallbackData = list_entry(pListPos, WiredNetworkCallbackListData_t, callbackList);
				pstWiredNetworkCallbackData->pfnCallback(ifindex, status, szIfName);
			}
			pthread_mutex_unlock(&g_callbackListMutex);
        }

msg_error:
		usleep(50000);
	}

	printf("close net sock\n");
	DeinitNetHotplugSock(nethotplug_sock);

	printf("exit thread proc\n");
	return NULL;
}

int WiredNetwork_StartCheckHotplug()
{
	g_bCheckNetThreadRun = 1;

	pthread_create(&g_checkNetHotplugThread, NULL, CheckNetHotPlugProc, NULL);

	if (!g_checkNetHotplugThread)
	{
		printf("create check net hotplug thread failed\n");
		return -1;
	}

	return 0;
}

void WiredNetwork_StopCheckHotplug()
{
	g_bCheckNetThreadRun = 0;

	if (g_checkNetHotplugThread)
	{
		printf("check thread is exiting\n");
        pthread_join(g_checkNetHotplugThread, NULL);
		g_checkNetHotplugThread = 0;
		printf("check thread exit\n");
	}
}

int WiredNetwork_RegisterCallback(NetHotplugCallback pfnCallback)
{
	WiredNetworkCallbackListData_t *pstWiredNetworkCallbackData = NULL;

	if (!pfnCallback)
		return -1;

	pstWiredNetworkCallbackData = (WiredNetworkCallbackListData_t*)malloc(sizeof(WiredNetworkCallbackListData_t));
	memset(pstWiredNetworkCallbackData, 0, sizeof(WiredNetworkCallbackListData_t));
	pstWiredNetworkCallbackData->pfnCallback = pfnCallback;

	pthread_mutex_lock(&g_callbackListMutex);
	WiredNetworkCallbackListData_t *pos = NULL;

	list_for_each_entry(pos, &g_wiredNetworkCallbackListHead, callbackList)
	{
		if (pos->pfnCallback == pfnCallback)
		{
			printf("have registered wired network callback\n");
			pthread_mutex_unlock(&g_callbackListMutex);
			return 0;
		}
	}

	list_add_tail(&pstWiredNetworkCallbackData->callbackList, &g_wiredNetworkCallbackListHead);
	pthread_mutex_unlock(&g_callbackListMutex);

	return 0;
}

void WiredNetwork_UnRegisterCallback(NetHotplugCallback pfnCallback)
{
	WiredNetworkCallbackListData_t *pstWiredNetworkCallbackData = NULL;
	list_t *pListPos = NULL;
	list_t *pListPosN = NULL;

	pthread_mutex_lock(&g_callbackListMutex);
	list_for_each_safe(pListPos, pListPosN, &g_wiredNetworkCallbackListHead)
	{
		pstWiredNetworkCallbackData = list_entry(pListPos, WiredNetworkCallbackListData_t, callbackList);

		if (pstWiredNetworkCallbackData->pfnCallback == pfnCallback)
		{
			list_del(pListPos);
			free(pstWiredNetworkCallbackData);
		}
	}
	pthread_mutex_unlock(&g_callbackListMutex);
}
