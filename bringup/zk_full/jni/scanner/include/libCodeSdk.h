#pragma once
#pragma once
#ifndef SVTCLIENT_EXTERN_C
#ifdef __cplusplus
#define SVTCLIENT_EXTERN_C extern "C"
#else
#define SVTCLIENT_EXTERN_C
#endif
#endif

#define SVTCLIENT_API_EXPORTS

#ifndef SVTCLIENT_STDCALL
#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64) && (defined _MSC_VER)
#define SVTCLIENT_STDCALL __stdcall
#else
#define SVTCLIENT_STDCALL
#endif
#endif

#define SVTCLIENT_IMPL SVTCLIENT_EXTERN_C
#define SVTCLIENT_API(rettype) SVTCLIENT_EXTERN_C SVTCLIENT_API_EXPORTS rettype SVTCLIENT_STDCALL



enum ImageFormats
{
	None = 0,
	Lum = 1,
	RGB = 2,
	BGR = 3,
	RGBX = 4,
	XRGB = 5,
	BGRX = 6,
	XBGR = 7,
};
#ifdef __cplusplus
extern "C" {
#endif
	namespace CodeDeCode {
		/*****************
		*功能：实现对图像的解码功能
		*输入： data图像buffer
		*输入： width图像宽
		*输入： height图像高度
		*输入： imgformat图像格式参考ImageFormats
		*输入： codeformat二维码指定格式比如QR_CODE   所有支持格式{AZTEC,CODABAR,CODE_39,CODE_93,CODE_128,DATA_MATRIX,EAN_8,EAN_13,ITF,MAXICODE,PDF_417,QR_CODE,RSS_14,RSS_EXPANDED,UPC_AUPC_E}
		*输入： faster，更快解码，但是解码精度会有所下降
		*输入： ispure是否是纯净的，一般为false
		*输入： norotate是否旋转，一般选择false
		*******************/
		SVTCLIENT_API(int) ImageDecode(const char* data, int width, int height, int imgformat,const char* codeformat, char* output, bool faster = false, bool ispure = false, bool norotate = false);
	}
#ifdef __cplusplus
}
#endif
