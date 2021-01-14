#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h> /* sysconf */
#include <stdlib.h> /* exit */
#include <pthread.h>

#include "interface.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    char cmd;
    int ret, width, height;

    uint32_t luma = 50, contrast = 50;
    int volumn = 30;
    bool b_exit = false, mute = false;

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

    //sstar_player_setopts("video_only", "1", 0);   //设置是否只播视频
    //sstar_player_setopts("rotate", "0", 0);       //设置是否旋转

    ret = sstar_player_open(argv[1], 0, 0, width, height);
    if (ret < 0) {
        goto exit;
    }

    while (!b_exit)
    {
        fflush(stdin);
        cmd = getchar();
        //printf("receive char = %c\n", cmd);
        switch (cmd) 
        {
            case 's':
                sstar_player_open(argv[1], 0, 0, width, height);
            break;

            case 't':
                sstar_player_close();
            break;

            case 'p':
                sstar_player_pause();    // 暂停
            break;

            case 'c':
                sstar_player_resume();   // 恢复播放
            break;

            case 'f':
                sstar_player_seek(10);   // forward
            break;

            case 'b':
                sstar_player_seek(-10);  // backward
            break;

            case 'd': {
                double duration;
                if (0 == (ret = sstar_player_getduration(&duration))) {
                    printf("get video duration = [%.3lf]!\n", duration);
                }
                break;
            }

            case 'g': {
                double current_time;
                if (0 == (ret = sstar_player_gettime(&current_time))) {
                    printf("get video current time = [%.3lf]!\n", current_time);
                }
                break;
            }

            case 'u': {
                mute = !mute;
                sstar_player_set_mute(mute);
                break;
            }

            case '+': {
                volumn += 5;
                if (volumn > 100) {
                    volumn = 100;
                    printf("audio volumn is over max!\n");
                }
                sstar_player_set_volumn(volumn);
                break;
            }

            case '-': {
                volumn -= 5;
                if (volumn < 0) {
                    volumn = 0;
                    printf("audio volumn is lower min!\n");
                }
                sstar_player_set_volumn(volumn);
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
    sstar_player_close();

    sstar_panel_deinit();

    sstar_sys_deinit();

    return 0;
}


