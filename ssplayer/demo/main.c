#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h> /* sysconf */
#include <stdlib.h> /* exit */
#include <pthread.h>

#include "player.h"
#include "interface.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    char cmd;
    int ret, width, height;

    uint32_t luma = 50, contrast = 50;
    int volumn = 30;
    bool b_exit = false, mute = false;
    double duration, position;

    int i, nrcpus;
    cpu_set_t mask;
    unsigned long bitmask = 0;

    if (!argv[1]) {
        printf("please input a file!\n");
        return -1;
    }

    printf("welcome to test ssplayer!\n");

    /* get logical cpu number */
    nrcpus = sysconf(_SC_NPROCESSORS_CONF);
    printf("processor logical cpu number #%d\n", nrcpus);

    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    CPU_SET(1, &mask);

    /* Set the CPU affinity for a pid */
    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    {
        perror("sched_setaffinity");
        return -1;
    }

    CPU_ZERO(&mask);

    /* Get the CPU affinity for a pid */
    if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    {
        perror("sched_getaffinity");
        return -1;
    }

    for (i = 0; i < nrcpus; i++)
    {
        if (CPU_ISSET(i, &mask))
        {
            bitmask |= (unsigned long)0x01 << i;
            printf("processor #%d is set\n", i);
        }
    }
    printf("after set bitmask = %#lx\n", bitmask);

    sstar_sys_init();

    sstar_panel_init();

    sstar_getpanel_wh(&width, &height);

    printf("try playing %s ...\n", argv[1]);

    mm_player_set_opts("audio_layout", "", AV_CH_LAYOUT_MONO);
    mm_player_set_opts("video_only", "", 0);
    mm_player_set_opts("video_rotate", "", AV_ROTATE_NONE);
    mm_player_set_opts("resolution", "384000", 0);//限制视频分辨率800x480

    ret = mm_player_open(argv[1], 0, 0, width, height);
    if (ret < 0) {
        goto exit;
    }

    mm_player_getduration(&duration);

    while (!b_exit)
    {
        fflush(stdin);
        cmd = getchar();
        //printf("receive char = %c\n", cmd);
        switch (cmd) 
        {
            case 's':
                mm_player_open(argv[1], 0, 0, width, height);
            break;

            case 't':
                mm_player_close();
            break;

            case 'p':
                mm_player_pause();    // 暂停
            break;

            case 'c':
                mm_player_resume();   // 恢复播放
            break;

            case 'f':
                mm_player_getposition(&position);
                position += 10.0;
                position = (position >= duration) ? duration : position;
                mm_player_seek2time(position);
            break;

            case 'b':
                mm_player_getposition(&position);
                position -= 10.0;
                position = (position <= 0) ? 0 : position;
                mm_player_seek2time(position);
            break;

            case 'd':
                mm_player_getduration(&duration);
                printf("get video duration = [%.3lf]!\n", duration);
            break;

            case 'g':
                mm_player_getposition(&position);
                printf("get video current time = [%.3lf]!\n", position);
            break;

            case 'u': {
                mute = !mute;
                mm_player_set_mute(mute);
                break;
            }

            case '+': {
                volumn += 5;
                if (volumn > 100) {
                    volumn = 100;
                    printf("audio volumn is over max!\n");
                }
                mm_player_set_volumn(volumn);
                break;
            }

            case '-': {
                volumn -= 5;
                if (volumn < 0) {
                    volumn = 0;
                    printf("audio volumn is lower min!\n");
                }
                mm_player_set_volumn(volumn);
                break;
            }

            case 'q': 
                b_exit = true;        // 退出
            break;

            default : 
                //printf("invalid cmd!\n");
            break;
        }
        fflush(stdout);
    }
exit:
    mm_player_close();

    sstar_panel_deinit();

    sstar_sys_deinit();

    return 0;
}


