#ifndef __BARCODELIB_H__
#define __BARCODELIB_H__
#ifdef __cplusplus
extern "C" {
#endif

unsigned int Initial_Decoder(void); //初始化，只要调用一次
extern int Decoding_Image(unsigned char* img_buffer, int width, int height);	//解码主函数
extern unsigned int GetResultLength();//获取解码结果长度
extern int GetDecoderResult(unsigned char * result);	//获取解码结果

#ifdef __cplusplus
}
#endif
#endif
