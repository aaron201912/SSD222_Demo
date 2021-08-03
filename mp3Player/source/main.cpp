#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "user_audio.h"


static bool g_bExit = false;

void signalHandler(int signo)
{
    switch (signo)
    {
        case SIGINT:
            printf("catch exit signal\n");
            g_bExit = true;
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    int ret;
    char cmd;
    signal(SIGINT, signalHandler);

    if (!argv[1]) {
        printf("please input a mp3 file!\n");
        printf("eg: ./Mp3Player file.mp3\n");
        return -1;
    }

    printf("### Mp3Player Start ###\n");

    ret = mp3_codec(argv[1]);
    if (ret != 0) {
        printf("mp3_codec init failed!\n");
        return -1;
    }

    while (1)
    {
        if (is_play_done())
            break;
        if (g_bExit) {
            Mp3PlayStopDec();
            break;
        }
    }

    printf("### Exit Mp3Player ###\n");

    return 0;
}
