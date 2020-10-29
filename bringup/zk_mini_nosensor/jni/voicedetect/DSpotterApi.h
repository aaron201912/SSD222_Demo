#ifndef __DSPOTTER_API_H
#define __DSPOTTER_API_H

#if defined(_WIN32)
	#ifdef DSPDLL_EXPORTS
		#define DSPDLL_API __declspec(dllexport)
	#endif
#endif

#ifndef DSPDLL_API
#define DSPDLL_API
#endif

#include "base_types.h"
#include "DSpotterApi_Const.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _VerInfo
{
	const char *pchSDKName;
	const char *pchSDKVersion;
	const char *pchSDKType;
	const char *pchReleaseDate;
	const char *pchLicenseType;
	BOOL  bTrialVersion;
} VerInfo;

/** Main API */
DSPDLL_API HANDLE DSpotterInitMultiWithPackBin(char *lpchPackBin, BOOL *lpbEnableGroup, INT nMaxTime, BYTE *lpbyState, INT nStateSize, INT *lpnErr, char *lpchLicenseFile, char *lpchServerFile);

DSPDLL_API HANDLE DSpotterInitMultiWithMod(char *lpchCYBaseFile, char **lppchGroupFile, INT nNumGroupFile, INT nMaxTime, BYTE *lpbyState, INT nStateSize, INT *lpnErr, char *lpchLicenseFile, char *lpchServerFile);

DSPDLL_API INT DSpotterReset(HANDLE hDSpotter);

DSPDLL_API INT DSpotterRelease(HANDLE hDSpotter);

DSPDLL_API INT DSpotterGetCommandNumber(HANDLE hDSpotter);

DSPDLL_API INT DSpotterGetUTF8Command(HANDLE hDSpotter, INT nCmdIdx, BYTE *lpbyCommand);

DSPDLL_API INT DSpotterGetUTF16Command(HANDLE hDSpotter, INT nCmdIdx, UNICODE *lpwcCommand);

DSPDLL_API INT DSpotterGetSampleRate(HANDLE hDSpotter);

DSPDLL_API INT DSpotterGetNumGroup(char *lpchPackBin);

DSPDLL_API const char *DSpotterVerInfo(char *lpchLicenseFile, VerInfo *lpVerInfo, INT *lpnErr);

DSPDLL_API INT DSpotterAddSample(HANDLE hDSpotter, SHORT *lpsSample, INT nNumSample);

DSPDLL_API BOOL DSpotterIsKeywordAlive(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetUTF8Result(HANDLE hDSpotter, INT *lpnCmdIdx, BYTE *lpbyResult, INT *lpnWordDura, INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL);

DSPDLL_API INT DSpotterGetUTF16Result(HANDLE hDSpotter, INT *lpnCmdIdx, UNICODE *lpwcResult, INT *lpnWordDura, INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL);

DSPDLL_API INT DSpotterGetUTF8ResultNoWait(HANDLE hDSpotter, INT *lpnCmdIdx, BYTE *lpbyResult, INT *lpnWordDura, INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL);

DSPDLL_API INT DSpotterGetUTF16ResultNoWait(HANDLE hDSpotter, INT *lpnCmdIdx, UNICODE *lpwcResult, INT *lpnWordDura, INT *lpnEndSilDura, INT *lpnNetworkLatency, INT *lpnGMM, INT *lpnSG, INT *lpnFIL);

DSPDLL_API INT DSpotterSetEnableNBest(HANDLE hDSpotter, BOOL bEnable);

DSPDLL_API INT DSpotterGetNBestUTF8ResultScore(HANDLE hDSpotter, INT *lpnCmdIdx, BYTE **lppbyResult, INT *lpnResultLength, INT *lpnScore, INT nMaxNBest);

DSPDLL_API INT DSpotterGetNBestUTF16ResultScore(HANDLE hDSpotter, INT *lpnCmdIdx, UNICODE **lppwcResult, INT *lpnResultLength, INT *lpnScore, INT nMaxNBest);

DSPDLL_API INT DSpotterGetCmdEnergy(HANDLE hDSpotter);

/** Threshold API */
DSPDLL_API INT DSpotterSetRejectionLevel(HANDLE hDSpotter, INT nRejectionLevel);

DSPDLL_API INT DSpotterSetSgLevel(HANDLE hDSpotter, INT nSgLevel);

DSPDLL_API INT DSpotterSetFilLevel(HANDLE hDSpotter, INT nFilLevel);

DSPDLL_API INT DSpotterSetResponseTime(HANDLE hDSpotter, INT nResponseTime);

DSPDLL_API INT DSpotterSetCmdResponseTime(HANDLE hDSpotter, INT nCmdIdx, INT nResponseTime);

DSPDLL_API INT DSpotterSetEnergyTH(HANDLE hDSpotter, INT nEnergyTH);

DSPDLL_API INT DSpotterSetCmdReward(HANDLE hDSpotter, INT nCmdIdx, INT nReward);

DSPDLL_API INT DSpotterGetRejectionLevel(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetSgLevel(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetFilLevel(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetCmdResponseTime(HANDLE hDSpotter, INT nCmdIdx, INT *lpnErr);

DSPDLL_API INT DSpotterGetEnergyTH(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetCmdReward(HANDLE hDSpotter, INT nCmdIdx, INT *lpnErr);

DSPDLL_API INT DSpotterGetModelRejectionLevel(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetModelSgLevel(HANDLE hDSpotter, INT *lpnErr);

DSPDLL_API INT DSpotterGetModelFilLevel(HANDLE hDSpotter, INT *lpnErr);

#ifdef __cplusplus
}
#endif

#endif // __DSPOTTER_API_H
