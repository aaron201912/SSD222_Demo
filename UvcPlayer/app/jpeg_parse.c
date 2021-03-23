#include <string.h>
#include "stdio.h"

typedef enum {                  /* JPEG marker codes */
  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,

  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,

  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,

  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,

  M_DHT   = 0xc4,

  M_DAC   = 0xcc,

  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,

  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,

  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,

  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,

  M_TEM   = 0x01,

  M_ERROR = 0x100
} JPEG_MARKER;

#define marker_len  2  //0xff 0xXX


int find_marker_index(char* buf, long buf_size)
{
    if(!buf || buf_size <= 0)
    {
        printf("find_marker_index error,please check buf or buf_size=%ld \n",buf_size);
        return -1;
    }

    char *tmp_buf = buf;

    for (long i = 0;i < buf_size;i++)
    {
        if(tmp_buf[i] == 0xff && (i + 1) < buf_size)
        {
            if(tmp_buf[i+1]!=0)
            {
                return i;;
            }
        }

    }

    return -1;//no marker,return -1
}

int parse_marker(char* buf, long buf_size, int *image_width, int *image_height)
{
    if(!buf  || buf_size <= 0 )
    {
        printf("parse_marker error,please check buf or buf_size=%ld \n",buf_size);
        return -1;
    }

    int index = 0;
    long cnt = 0;
    int segment_len = 0;
    char* tmp_buf = buf;

    for (;;)
    {
        index = find_marker_index(tmp_buf, buf_size - cnt);
        if(index < 0)
        {
            printf("parse_marker find no marker_index ,please check file\n");
            return -1;
        }
        switch(tmp_buf[++index])  //0xff 后一个非0字节
        {
            case M_SOI:
            {
                //printf("this is Mjpeg start_header tmp_buf[%d]=0x%x \n",index,tmp_buf[index]);
                break;
            }
            case M_SOS:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                //printf("M_SOS tmp_buf[%d]=0x%x segment_len=%d\n",index,tmp_buf[index],segment_len);
                break;
            }
            case M_DQT:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                //printf("M_DQT tmp_buf[%d]=0x%x segment_len=%d\n",index,tmp_buf[index],segment_len);
                break;
            }
            case M_SOF0:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                *image_height = tmp_buf[index+4]<<8|tmp_buf[index+5];
                *image_width = tmp_buf[index+6]<<8|tmp_buf[index+7];
                //printf("M_SOF0 tmp_buf[%d]=0x%x segment_len=%d image_width=%d image_height=%d\n",index,tmp_buf[index],segment_len,*image_width,*image_height);
                break;

            }
            case M_APP0:
            case M_APP1:
            case M_APP2:
            case M_APP3:
            case M_APP4:
            case M_APP5:
            case M_APP6:
            case M_APP7:
            case M_APP8:
            case M_APP9:
            case M_APP10:
            case M_APP11:
            case M_APP12:
            case M_APP13:
            case M_APP14:
            case M_APP15:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                //printf("M_APP0 tmp_buf[%d]=0x%x segment_len=%d\n",index,tmp_buf[index],segment_len);
                break;
            }
            case M_DHT:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                //printf("M_DHT tmp_buf[%d]=0x%x \n",index,tmp_buf[index],segment_len);
                if(tmp_buf[index+3]&0x80)
                {
                    //printf("this is DC_TABL tmp_buf[%d]=0x%x segment_len=%d\n",index,tmp_buf[index],segment_len);
                }
                else
                {
                    //printf("this is AC_TABL tmp_buf[%d]=0x%x segment_len=%d \n",index,tmp_buf[index],segment_len);
                }
                break;
            }
            case M_EOI:
            {
                //printf("M_EOI An whole Jpeg is Done tmp_buf[%d]=0x%x cnt=%ld \n",index,tmp_buf[index],cnt);
                return cnt+index+1;
            }
            case M_COM:
            case M_DRI:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                //printf("M_DRI/M_COM tmp_buf[%d]=0x%x segment_len=%d\n",index,tmp_buf[index],segment_len);
                break;
            }
            default:
            {
                segment_len = tmp_buf[index+1]<<8|tmp_buf[index+2];
                break;
            }
        }
        //printf("cnt=%ld tmp_buf[%d]=0x%x 0x%x  segment_len=%d\n",cnt,index,tmp_buf[index-1],tmp_buf[index],segment_len);
        if(buf_size < cnt + 2)
        {
            printf("Buf end with no M_EOI,return fail\n");
            return -1;
        }
        else if(buf_size < (cnt + segment_len + marker_len))
        {
            //printf("marker discrition is wrong,skip on marker_len,marker:0x%x 0x%x segment_len=%d\n",tmp_buf[index-1],tmp_buf[index],segment_len);//发现有写图片对于长度的描述是错误的
            cnt += marker_len;
            tmp_buf += marker_len;
        }
        else
        {
            cnt += (segment_len+marker_len);
            tmp_buf += (segment_len+marker_len);
        }
    }

    return 0;
}


