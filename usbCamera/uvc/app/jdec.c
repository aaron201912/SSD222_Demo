#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include "jdec.h"

#include "mi_sys.h"

typedef struct
{
	void *pHandle;
	tjhandle (*pfntjInitTransform)(void);
	int (*pfntjTransform)(tjhandle handle, const unsigned char *jpegBuf,
						  unsigned long jpegSize, int n,
						  unsigned char **dstBufs, unsigned long *dstSizes,
						  tjtransform *transforms, int flags);
	tjhandle (*pfntjInitDecompress)(void);
	int (*pfntjDecompressHeader3)(tjhandle handle,
								  const unsigned char *jpegBuf,
								  unsigned long jpegSize, int *width,
								  int *height, int *jpegSubsamp,
								  int *jpegColorspace);
	unsigned long (*pfntjBufSizeYUV2)(int width, int pad, int height, int subsamp);
	int (*pfntjDecompressToYUV2)(tjhandle handle, const unsigned char *jpegBuf,
								 unsigned long jpegSize, unsigned char *dstBuf, int width,
								 int pad, int height, int flags);
	int (*pfntjDestroy)(tjhandle handle);
	unsigned char* (*pfntjAlloc)(int bytes);
	void (*pfntjFree)(unsigned char *buffer);
	int (*pfntjDecompress)(tjhandle handle, unsigned char *jpegBuf,
						   unsigned long jpegSize, unsigned char *dstBuf,
						   int width, int pitch, int height, int pixelSize,
						   int flags);
	int (*pfntjDecodeYUV)(tjhandle handle, const unsigned char *srcBuf,
						  int pad, int subsamp, unsigned char *dstBuf,
						  int width, int pitch, int height, int pixelFormat,
						  int flags);
	int (*pfntjDecompress2)(tjhandle handle, const unsigned char *jpegBuf,
						  unsigned long jpegSize, unsigned char *dstBuf,
						  int width, int pitch, int height, int pixelFormat,
						  int flags);
	char *(*pfntjGetErrorStr2)(tjhandle handle);
} LibtjpegAssembly_t;

static LibtjpegAssembly_t g_stLibtjpegAssembly;

#define THROW(action, message)                                                 \
    {                                                                          \
        printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
        retval = -1;                                                           \
        goto bailout;                                                          \
    }

#define THROW_TJ(action) THROW(action, g_stLibtjpegAssembly.pfntjGetErrorStr2(tjInstance))

#define THROW_UNIX(action) THROW(action, strerror(errno))

const char *subsampName[TJ_NUMSAMP] = {
    "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"};

const char *colorspaceName[TJ_NUMCS] = {
    "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"};

int SSTAR_TurboJpeg_OpenLibrary()
{
	memset(&g_stLibtjpegAssembly, 0, sizeof(LibtjpegAssembly_t));

	g_stLibtjpegAssembly.pHandle = dlopen("libturbojpeg.so", RTLD_NOW);
	if (NULL == g_stLibtjpegAssembly.pHandle)
	{
		printf(" %s: Can not load libturbojpeg.so!\n", __func__);
		return -1;
	}

	g_stLibtjpegAssembly.pfntjInitTransform = (tjhandle (*)(void))dlsym(g_stLibtjpegAssembly.pHandle, "tjInitTransform");
	if(NULL == g_stLibtjpegAssembly.pfntjInitTransform)
	{
		printf(" %s: dlsym tjInitTransform failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjTransform = (int (*)(tjhandle handle, const unsigned char *jpegBuf,
						  	  	  	  	   unsigned long jpegSize, int n,
										   unsigned char **dstBufs, unsigned long *dstSizes,
										   tjtransform *transforms, int flags))dlsym(g_stLibtjpegAssembly.pHandle, "tjTransform");
	if(NULL == g_stLibtjpegAssembly.pfntjTransform)
	{
		printf(" %s: dlsym tjTransform failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjInitDecompress = (tjhandle (*)(void))dlsym(g_stLibtjpegAssembly.pHandle, "tjInitDecompress");
	if(NULL == g_stLibtjpegAssembly.pfntjInitDecompress)
	{
		printf(" %s: dlsym pfntjInitDecompress failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDecompressHeader3 = (int (*)(tjhandle handle,
												  const unsigned char *jpegBuf,
												  unsigned long jpegSize, int *width,
												  int *height, int *jpegSubsamp,
												  int *jpegColorspace))dlsym(g_stLibtjpegAssembly.pHandle, "tjDecompressHeader3");
	if(NULL == g_stLibtjpegAssembly.pfntjDecompressHeader3)
	{
		printf(" %s: dlsym tjDecompressHeader3 failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjBufSizeYUV2 = (unsigned long (*)(int width, int pad, int height, int subsamp))dlsym(g_stLibtjpegAssembly.pHandle, "tjBufSizeYUV2");
	if(NULL == g_stLibtjpegAssembly.pfntjBufSizeYUV2)
	{
		printf(" %s: dlsym tjBufSizeYUV2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDecompressToYUV2 = (int (*)(tjhandle handle, const unsigned char *jpegBuf,
												 unsigned long jpegSize, unsigned char *dstBuf, int width,
												 int pad, int height, int flags))dlsym(g_stLibtjpegAssembly.pHandle, "tjDecompressToYUV2");
	if(NULL == g_stLibtjpegAssembly.pfntjDecompressToYUV2)
	{
		printf(" %s: dlsym tjDecompressToYUV2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDestroy = (int (*)(tjhandle handle))dlsym(g_stLibtjpegAssembly.pHandle, "tjDestroy");
	if(NULL == g_stLibtjpegAssembly.pfntjDestroy)
	{
		printf(" %s: dlsym tjDestroy failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjAlloc = (unsigned char* (*)(int bytes))dlsym(g_stLibtjpegAssembly.pHandle, "tjAlloc");
	if(NULL == g_stLibtjpegAssembly.pfntjAlloc)
	{
		printf(" %s: dlsym tjAlloc failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjFree = (void (*)(unsigned char *buffer))dlsym(g_stLibtjpegAssembly.pHandle, "tjFree");
	if(NULL == g_stLibtjpegAssembly.pfntjFree)
	{
		printf(" %s: dlsym tjFree failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDecompress = (int (*)(tjhandle handle, unsigned char *jpegBuf,
										   unsigned long jpegSize, unsigned char *dstBuf,
										   int width, int pitch, int height, int pixelSize,
										   int flags))dlsym(g_stLibtjpegAssembly.pHandle, "tjDecompress");
	if(NULL == g_stLibtjpegAssembly.pfntjDecompress)
	{
		printf(" %s: dlsym tjDecompress failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDecodeYUV = (int (*)(tjhandle handle, const unsigned char *srcBuf,
										  int pad, int subsamp, unsigned char *dstBuf,
										  int width, int pitch, int height, int pixelFormat,
										  int flags))dlsym(g_stLibtjpegAssembly.pHandle, "tjDecodeYUV");
	if(NULL == g_stLibtjpegAssembly.pfntjDecodeYUV)
	{
		printf(" %s: dlsym tjDecodeYUV failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjDecompress2 = (int (*)(tjhandle handle, const unsigned char *jpegBuf,
											  unsigned long jpegSize, unsigned char *dstBuf,
											  int width, int pitch, int height, int pixelFormat,
											  int flags))dlsym(g_stLibtjpegAssembly.pHandle, "tjDecompress2");
	if(NULL == g_stLibtjpegAssembly.pfntjDecompress2)
	{
		printf(" %s: dlsym tjDecompress2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	g_stLibtjpegAssembly.pfntjGetErrorStr2 = (char* (*)(tjhandle handle))dlsym(g_stLibtjpegAssembly.pHandle, "tjGetErrorStr2");
	if(NULL == g_stLibtjpegAssembly.pfntjGetErrorStr2)
	{
		printf(" %s: dlsym tjGetErrorStr2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_TurbpJpeg_CloseLibrary()
{
	if(g_stLibtjpegAssembly.pHandle)
	{
		dlclose(g_stLibtjpegAssembly.pHandle);
		g_stLibtjpegAssembly.pHandle = NULL;
	}
	memset(&g_stLibtjpegAssembly, 0, sizeof(LibtjpegAssembly_t));
}

//struct timeval time_start, time_end;
//int64_t time;

int jdec_decode_yuv_from_buf(char* jpgBuf, unsigned long jpgSize, jdecIMAGE *image, TANSFORM eTranOpt, SAMP esubsamp)
{
    //char *inFormat;
    tjhandle tjInstance = NULL;
    int retval = 0;
    int width, height;
    unsigned char *jpegBuf = (unsigned char *)jpgBuf;

    if (NULL!=jpegBuf)
    {
        unsigned long jpegSize = jpgSize;
        tjtransform xform;
        int inSubsamp, inColorspace;
        memset(&xform, 0, sizeof(tjtransform));
        /* Read the JPEG file into memory. */

        if (jpegSize == 0)
            THROW("determining input file size", "Input file contains no data");

        if (eTranOpt != TANSFORM_NONE)
        {
            /* Transform it. */
            unsigned char *dstBuf = NULL; /* Dynamically allocate the JPEG buffer */
            unsigned long dstSize = 0;

            if ((tjInstance = g_stLibtjpegAssembly.pfntjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options = (TJXOPT_TRIM);

            if (g_stLibtjpegAssembly.pfntjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize, &xform, 0) < 0)
                THROW_TJ("transforming input image");

            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
        	if ((tjInstance = g_stLibtjpegAssembly.pfntjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }
        if (g_stLibtjpegAssembly.pfntjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height, &inSubsamp, &inColorspace) < 0)
            THROW_TJ("reading JPEG header");

        unsigned long yuvSize = g_stLibtjpegAssembly.pfntjBufSizeYUV2(width, 1, height, inSubsamp);

        if (yuvSize == (unsigned long)-1)
            THROW_TJ("allocating YUV buffer");
        if (image == NULL)
        {
            THROW_TJ("param image=NULL");
        }
        else
        {
            image->width = width;
            image->height = height;
            image->pitch = width;
            image->esubsamp = (enum SAMP)inSubsamp;
        }

        //gettimeofday(&time_start, NULL);
        if (g_stLibtjpegAssembly.pfntjDecompressToYUV2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr,
                               width, 1, height, TJFLAG_FASTUPSAMPLE|TJFLAG_FASTDCT) == -1)
            THROW_TJ("executing tjDecompressToYUV2()");

        //gettimeofday(&time_end, NULL);
        //time = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //printf("tjDecompressToYUV2 Time = %lld\n", time);

        if (MI_SUCCESS != MI_SYS_FlushInvCache(image->virtAddr, yuvSize))
        {
            THROW_TJ("MI_SYS_FlushInvCache");
        }
    }
    else
    {
        THROW_TJ("need JPEG image");
    }
bailout:
    if (tjInstance)
    	g_stLibtjpegAssembly.pfntjDestroy(tjInstance);
    return retval;
}

int jdec_decode_yuv(char *fileName, jdecIMAGE *image, TANSFORM eTranOpt, SAMP esubsamp)
{
    char *inFormat;
    FILE *jpegFile = NULL;
    tjhandle tjInstance = NULL;
    int retval = 0;
    inFormat = strrchr(fileName, '.');
    inFormat = &inFormat[1];
    int width, height;
    unsigned char *jpegBuf = NULL;

    if (!strcasecmp(inFormat, "jpg"))
    {
        unsigned long jpegSize;
        tjtransform xform;
        long size;
        int inSubsamp, inColorspace;
        memset(&xform, 0, sizeof(tjtransform));
        /* Read the JPEG file into memory. */
        if ((jpegFile = fopen(fileName, "rb")) == NULL)
            THROW_UNIX("opening input file");
        if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) ||
            fseek(jpegFile, 0, SEEK_SET) < 0)
            THROW_UNIX("determining input file size");
        if (size == 0)
            THROW("determining input file size", "Input file contains no data");
        jpegSize = (unsigned long)size;
        if ((jpegBuf = (unsigned char *)g_stLibtjpegAssembly.pfntjAlloc(jpegSize)) == NULL)
            THROW_UNIX("allocating JPEG buffer");
        if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1)
            THROW_UNIX("reading input file");
        fclose(jpegFile);
        jpegFile = NULL;
        if (eTranOpt != TANSFORM_NONE)
        {
            /* Transform it. */
            unsigned char *dstBuf = NULL; /* Dynamically allocate the JPEG buffer */
            unsigned long dstSize = 0;

            if ((tjInstance = g_stLibtjpegAssembly.pfntjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options |= TJXOPT_TRIM;

            if (g_stLibtjpegAssembly.pfntjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize, &xform, 0) < 0)
                THROW_TJ("transforming input image");

            g_stLibtjpegAssembly.pfntjFree(jpegBuf);
            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
        	if ((tjInstance = g_stLibtjpegAssembly.pfntjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }

        if (g_stLibtjpegAssembly.pfntjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height, &inSubsamp, &inColorspace) < 0)
            THROW_TJ("reading JPEG header");

        unsigned long yuvSize = g_stLibtjpegAssembly.pfntjBufSizeYUV2(width, 1, height, inSubsamp);

        if (yuvSize == (unsigned long)-1)
            THROW_TJ("allocating YUV buffer");
        if (image == NULL)
        {
            THROW_TJ("param image=NULL");
        }
        else
        {
            image->width = width;
            image->height = height;
            image->pitch = width;
            printf("%s %d %d %ld\n", __FUNCTION__, __LINE__, inSubsamp, yuvSize);
        }

        if (image->phyAddr==0)
        {
            if (MI_SUCCESS != MI_SYS_MMA_Alloc((MI_U8 *)"#jdec",
                                               yuvSize, &image->phyAddr))
            {
                THROW_TJ("MI_SYS_MMA_Alloc");
            }
            if (MI_SUCCESS != MI_SYS_Mmap(image->phyAddr,
                                          yuvSize, (void **)&image->virtAddr, TRUE))
            {
                THROW_TJ("MI_SYS_Mmap");
            }
        }
        else if(image->phyAddr<0)
        {
        	if ((image->virtAddr = (char *)g_stLibtjpegAssembly.pfntjAlloc(yuvSize)) == NULL)

                THROW_UNIX("allocating uncompressed image buffer");
        }

        //if (tjDecompress(tjInstance, jpegBuf, jpegSize, image->virtAddr, 0, 0, 0, 0, TJ_YUV) == -1)
        //    THROW_TJ("executing tjDecompress()");

        if (g_stLibtjpegAssembly.pfntjDecompressToYUV2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr,
                               width, 1, height, 0) == -1)
            THROW_TJ("executing tjDecompressToYUV2()");
        //if (tjDecodeYUV(tjInstance, image->virtAddr, 1, esubsamp, dstPtr2, width,
        //                pitch, height, pf, 0) == -1)

        if (image->phyAddr)
        {
            if (MI_SUCCESS != MI_SYS_FlushInvCache(image->virtAddr, yuvSize))
            {
                THROW_TJ("MI_SYS_FlushInvCache");
            }
        }
    }
    else
    {
        THROW_TJ("need JPEG image");
    }
bailout:
    if (tjInstance)
    	g_stLibtjpegAssembly.pfntjDestroy(tjInstance);
    if (jpegBuf)
    	g_stLibtjpegAssembly.pfntjFree(jpegBuf);
    if (jpegFile)
        fclose(jpegFile);
    return retval;
}

int jdec_decode(char *fileName, jdecIMAGE *image, TANSFORM eTranOpt, PIXELFMT ePixelFmt)
{
    char *inFormat;
    FILE *jpegFile = NULL;
    tjhandle tjInstance = NULL;
    int retval = 0;
    inFormat = strrchr(fileName, '.');
    inFormat = &inFormat[1];
    int width, height;
    unsigned char *jpegBuf = NULL;

    if (!strcasecmp(inFormat, "jpg"))
    {
        unsigned long jpegSize;
        tjtransform xform;
        long size;
        int inSubsamp, inColorspace;
        memset(&xform, 0, sizeof(tjtransform));
        /* Read the JPEG file into memory. */
        if ((jpegFile = fopen(fileName, "rb")) == NULL)
            THROW_UNIX("opening input file");
        if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) ||
            fseek(jpegFile, 0, SEEK_SET) < 0)
            THROW_UNIX("determining input file size");
        if (size == 0)
            THROW("determining input file size", "Input file contains no data");
        jpegSize = (unsigned long)size;
        if ((jpegBuf = (unsigned char *)g_stLibtjpegAssembly.pfntjAlloc(jpegSize)) == NULL)
            THROW_UNIX("allocating JPEG buffer");
        if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1)
            THROW_UNIX("reading input file");
        fclose(jpegFile);
        jpegFile = NULL;
        if (eTranOpt != TANSFORM_NONE)
        {
            /* Transform it. */
            unsigned char *dstBuf = NULL; /* Dynamically allocate the JPEG buffer */
            unsigned long dstSize = 0;

            if ((tjInstance = g_stLibtjpegAssembly.pfntjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options |= TJXOPT_TRIM;

            if (g_stLibtjpegAssembly.pfntjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize, &xform, 0) < 0)
                THROW_TJ("transforming input image");

            g_stLibtjpegAssembly.pfntjFree(jpegBuf);
            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
        	if ((tjInstance = g_stLibtjpegAssembly.pfntjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }

        if (g_stLibtjpegAssembly.pfntjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height, &inSubsamp, &inColorspace) < 0)
            THROW_TJ("reading JPEG header");
        if (image == NULL)
        {
            THROW_TJ("param image=NULL");
        }
        else
        {
            image->width = width;
            image->height = height;
            image->pitch = width * tjPixelSize[ePixelFmt];
        }

        if (image->phyAddr==0)
        {
            if (MI_SUCCESS != MI_SYS_MMA_Alloc((MI_U8 *)"#jdec",
                                               width * height * tjPixelSize[ePixelFmt], &image->phyAddr))
            {
                THROW_TJ("MI_SYS_MMA_Alloc");
            }
            if (MI_SUCCESS != MI_SYS_Mmap(image->phyAddr,
                                          width * height * tjPixelSize[ePixelFmt], (void **)&image->virtAddr, TRUE))
            {
                THROW_TJ("MI_SYS_Mmap");
            }
        }
        else if(image->phyAddr<0)
        {
        	if ((image->virtAddr = (char *)g_stLibtjpegAssembly.pfntjAlloc(width * height * tjPixelSize[ePixelFmt])) == NULL)
                THROW_UNIX("allocating uncompressed image buffer");
        }

        if (g_stLibtjpegAssembly.pfntjDecompress2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr, width, 0, height,
                          ePixelFmt, 0) < 0)
            THROW_TJ("decompressing JPEG image");
        if (image->phyAddr)
        {
            if (MI_SUCCESS != MI_SYS_FlushInvCache(image->virtAddr, width * height * tjPixelSize[ePixelFmt]))
            {
                THROW_TJ("MI_SYS_FlushInvCache");
            }
        }

        g_stLibtjpegAssembly.pfntjFree(jpegBuf);
        jpegBuf = NULL;

        g_stLibtjpegAssembly.pfntjDestroy(tjInstance);
        tjInstance = NULL;
    }
    else
    {
        THROW_TJ("need JPEG image");
    }
bailout:
    if (tjInstance)
    	g_stLibtjpegAssembly.pfntjDestroy(tjInstance);
    if (jpegBuf)
    	g_stLibtjpegAssembly.pfntjFree(jpegBuf);
    if (jpegFile)
        fclose(jpegFile);
    return retval;
}

void jdec_free(jdecIMAGE *image)
{
    if (image->phyAddr)
    {
        MI_SYS_Munmap(image->virtAddr, image->height * image->pitch);
        MI_SYS_MMA_Free(image->phyAddr);
    }
    else
    {
        free(image->virtAddr);
    }
}
