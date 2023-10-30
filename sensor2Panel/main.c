#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <sys/prctl.h>
#include "sstardisp.h"


#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)



static int g_exit = 0;

void handle_signal(int sig)
{
    printf("get signal: Ctrl+C, exit app\n");
    g_exit = 1;
}


/* Main function */
int main(int argc, const char *argv[])
{
    signal(SIGINT, handle_signal);

    sstar_disp_init();


    while (g_exit)
    {
        sleep(1);
    }

    sstar_disp_deinit();

    return 0;
}
