#ifndef __JDEC_H__
#define __JDEC_H__

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif

#include <turbojpeg.h>

/**
 * Chrominance subsampling options.
 * When pixels are converted from RGB to YCbCr (see #TJCS_YCbCr) or from CMYK
 * to YCCK (see #TJCS_YCCK) as part of the JPEG compression process, some of
 * the Cb and Cr (chrominance) components can be discarded or averaged together
 * to produce a smaller image with little perceptible loss of image clarity
 * (the human eye is more sensitive to small changes in brightness than to
 * small changes in color.)  This is called "chrominance subsampling".
 */
typedef enum SAMP {
  /**
   * 4:4:4 chrominance subsampling (no chrominance subsampling).  The JPEG or
   * YUV image will contain one chrominance component for every pixel in the
   * source image.
   */
  SAMP_444 = 0,
  /**
   * 4:2:2 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 2x1 block of pixels in the source image.
   */
  SAMP_422,
  /**
   * 4:2:0 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 2x2 block of pixels in the source image.
   */
  SAMP_420,
  /**
   * Grayscale.  The JPEG or YUV image will contain no chrominance components.
   */
  SAMP_GRAY,
  /**
   * 4:4:0 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 1x2 block of pixels in the source image.
   *
   * @note 4:4:0 subsampling is not fully accelerated in libjpeg-turbo.
   */
  SAMP_440,
  /**
   * 4:1:1 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 4x1 block of pixels in the source image.
   * JPEG images compressed with 4:1:1 subsampling will be almost exactly the
   * same size as those compressed with 4:2:0 subsampling, and in the
   * aggregate, both subsampling methods produce approximately the same
   * perceptual quality.  However, 4:1:1 is better able to reproduce sharp
   * horizontal features.
   *
   * @note 4:1:1 subsampling is not fully accelerated in libjpeg-turbo.
   */
  SAMP_411
}SAMP;


/**
 * Transform operations for #tjTransform()
 */
typedef enum TANSFORM {
  /**
   * Do not transform the position of the image pixels
   */
  TANSFORM_NONE = 0,
  /**
   * Flip (mirror) image horizontally.  This transform is imperfect if there
   * are any partial MCU blocks on the right edge (see #TJXOPT_PERFECT.)
   */
  TANSFORM_HFLIP,
  /**
   * Flip (mirror) image vertically.  This transform is imperfect if there are
   * any partial MCU blocks on the bottom edge (see #TJXOPT_PERFECT.)
   */
  TANSFORM_VFLIP,
  /**
   * Transpose image (flip/mirror along upper left to lower right axis.)  This
   * transform is always perfect.
   */
  TANSFORM_TRANSPOSE,
  /**
   * Transverse transpose image (flip/mirror along upper right to lower left
   * axis.)  This transform is imperfect if there are any partial MCU blocks in
   * the image (see #TJXOPT_PERFECT.)
   */
  TANSFORM_TRANSVERSE,
  /**
   * Rotate image clockwise by 90 degrees.  This transform is imperfect if
   * there are any partial MCU blocks on the bottom edge (see
   * #TJXOPT_PERFECT.)
   */
  TANSFORM_ROT90,
  /**
   * Rotate image 180 degrees.  This transform is imperfect if there are any
   * partial MCU blocks in the image (see #TJXOPT_PERFECT.)
   */
  TANSFORM_ROT180,
  /**
   * Rotate image counter-clockwise by 90 degrees.  This transform is imperfect
   * if there are any partial MCU blocks on the right edge (see
   * #TJXOPT_PERFECT.)
   */
  TANSFORM_ROT270
}TANSFORM;

/**
 * Pixel formats
 */
typedef enum PIXELFMT {
  /**
   * RGB pixel format.  The red, green, and blue components in the image are
   * stored in 3-byte pixels in the order R, G, B from lowest to highest byte
   * address within each pixel.
   */
  PIXELFMT_RGB = 0,
  /**
   * BGR pixel format.  The red, green, and blue components in the image are
   * stored in 3-byte pixels in the order B, G, R from lowest to highest byte
   * address within each pixel.
   */
  PIXELFMT_BGR,
  /**
   * RGBX pixel format.  The red, green, and blue components in the image are
   * stored in 4-byte pixels in the order R, G, B from lowest to highest byte
   * address within each pixel.  The X component is ignored when compressing
   * and undefined when decompressing.
   */
  PIXELFMT_RGBX,
  /**
   * BGRX pixel format.  The red, green, and blue components in the image are
   * stored in 4-byte pixels in the order B, G, R from lowest to highest byte
   * address within each pixel.  The X component is ignored when compressing
   * and undefined when decompressing.
   */
  PIXELFMT_BGRX,
  /**
   * XBGR pixel format.  The red, green, and blue components in the image are
   * stored in 4-byte pixels in the order R, G, B from highest to lowest byte
   * address within each pixel.  The X component is ignored when compressing
   * and undefined when decompressing.
   */
  PIXELFMT_XBGR,
  /**
   * XRGB pixel format.  The red, green, and blue components in the image are
   * stored in 4-byte pixels in the order B, G, R from highest to lowest byte
   * address within each pixel.  The X component is ignored when compressing
   * and undefined when decompressing.
   */
  PIXELFMT_XRGB,
  /**
   * Grayscale pixel format.  Each 1-byte pixel represents a luminance
   * (brightness) level from 0 to 255.
   */
  PIXELFMT_GRAY,
  /**
   * RGBA pixel format.  This is the same as @ref TJPF_RGBX, except that when
   * decompressing, the X component is guaranteed to be 0xFF, which can be
   * interpreted as an opaque alpha channel.
   */
  PIXELFMT_RGBA,
  /**
   * BGRA pixel format.  This is the same as @ref TJPF_BGRX, except that when
   * decompressing, the X component is guaranteed to be 0xFF, which can be
   * interpreted as an opaque alpha channel.
   */
  PIXELFMT_BGRA,
  /**
   * ABGR pixel format.  This is the same as @ref TJPF_XBGR, except that when
   * decompressing, the X component is guaranteed to be 0xFF, which can be
   * interpreted as an opaque alpha channel.
   */
  PIXELFMT_ABGR,
  /**
   * ARGB pixel format.  This is the same as @ref TJPF_XRGB, except that when
   * decompressing, the X component is guaranteed to be 0xFF, which can be
   * interpreted as an opaque alpha channel.
   */
  PIXELFMT_ARGB,
  /**
   * CMYK pixel format.  Unlike RGB, which is an additive color model used
   * primarily for display, CMYK (Cyan/Magenta/Yellow/Key) is a subtractive
   * color model used primarily for printing.  In the CMYK color model, the
   * value of each color component typically corresponds to an amount of cyan,
   * magenta, yellow, or black ink that is applied to a white background.  In
   * order to convert between CMYK and RGB, it is necessary to use a color
   * management system (CMS.)  A CMS will attempt to map colors within the
   * printer's gamut to perceptually similar colors in the display's gamut and
   * vice versa, but the mapping is typically not 1:1 or reversible, nor can it
   * be defined with a simple formula.  Thus, such a conversion is out of scope
   * for a codec library.  However, the TurboJPEG API allows for compressing
   * CMYK pixels into a YCCK JPEG image (see #TJCS_YCCK) and decompressing YCCK
   * JPEG images into CMYK pixels.
   */
  PIXELFMT_CMYK,
  /**
   * Unknown pixel format.  Currently this is only used by #tjLoadImage().
   */
  PIXELFMT_UNKNOWN = -1
}PIXELFMT;


typedef struct jdecIMAGE
{
   unsigned long long phyAddr;
   char* virtAddr;
   unsigned short width;
   unsigned short height;
   unsigned int pitch;
   SAMP esubsamp;
}jdecIMAGE;
typedef struct jdecFILE
{
   FILE* fp;
   char* fBuf;
   unsigned long size;
}jdecFILE;

/**
 * decode a jpeg file to jdecIMAGE
 *
 * @param image jdecIMAGE handle to store decoded jpeg
 *        jdecIMAGE.phyAddr>0 use phyical memory allocated from MMA heap
 *        jdecIMAGE.phyAddr=0 use heap memory allocated from linux heap
 *
 **/
int SSTAR_TurboJpeg_OpenLibrary();
void SSTAR_TurbpJpeg_CloseLibrary();
int  jdec_decode(char *fileName, jdecIMAGE* image, TANSFORM eTranOpt, PIXELFMT ePixelFmt);
int  jdec_decode_yuv(char *fileName, jdecIMAGE *image, TANSFORM eTranOpt, SAMP esubsamp);
int  jdec_decode_yuv_from_buf(char* jpgBuf,unsigned long jpgSize, jdecIMAGE *image, TANSFORM eTranOpt, SAMP esubsamp);
void jdec_free(jdecIMAGE* image);

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
