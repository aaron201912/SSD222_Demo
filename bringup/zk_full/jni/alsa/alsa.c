/*
 * alsa.c
 *
 *  Created on: 2023年5月8日
 *      Author: koda.xu
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <pthread.h>
#include <errno.h>
#include "alsa.h"

int SSTAR_ALSA_StartRecord(char *pRecordFile)	// 录制音频
{
	/*
	./amixer -c 1 sget "ADC01_ANOLG_GAIN"
	./amixer -c 1 sget "ADC01_L DPGA"
	./amixer -c 1 sget "ADC01_R DPGA"
	./amixer -c 1 sset "ADC01_ANOLG_GAIN" 10
	./amixer -c 1 sset "ADC01_L DPGA" 140
	./amixer -c 1 sset "ADC01_R DPGA" 100
	./amixer -c 1 sset "AI_MCH_01_SEL" "AI_DETACH"
	./amixer -c 1 sset "AI_MCH_01_SEL" "ADC_A"
	./arecord -D hw:1,0 -c 2 -r16000 -f S16_LE -d 15 test.wav
	./amixer -c 1 sset "AI_MCH_01_SEL" "AI_DETACH"
	*/

	int iRet;
	char sysCmd[256] = {0};
	sprintf(sysCmd, "%s %s", ALSA_RECORD_SHELL, pRecordFile);

	iRet=system(sysCmd);
	printf("iRet=%d\n", iRet);
	if (iRet == -1)
		printf("%d:%s\n", errno, strerror(errno));

	return 0;
}

int SSTAR_ALSA_StopRecord()
{
	return 0;
}

int SSTRR_ALSA_StartPlayRecord(char *pRecordFile)
{
	/*
	./amixer -c 1 sget "MMC1_L DPGA"
	./amixer -c 1 sset "MMC1_L DPGA" 90
	./amixer -c 1 sset "DAC_A_SEL" "AO_DETACH"
	./amixer -c 1 sset "DAC_A_SEL" DMA_L
	./aplay -D hw:1,0 --period-size=4400 --buffer-size=8800 /customer/res/8K_16bit_STERO_30s.wav
	./amixer -c 1 sset "DAC_A_SEL" "AO_DETACH"
	*/

	int iRet;
	char sysCmd[256] = {0};
	sprintf(sysCmd, "%s %s", ALSA_PLAY_SHELL, pRecordFile);

	iRet=system(sysCmd);
	printf("iRet=%d\n", iRet);
	if (iRet == -1)
		printf("%d:%s\n", errno, strerror(errno));

	return 0;
}

int SSTRR_ALSA_StopPlayRecord()
{

	return 0;
}
