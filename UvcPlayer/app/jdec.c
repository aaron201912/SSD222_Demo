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

#include "jdec.h"

#include "mi_sys.h"

#define THROW(action, message)                                                 \
    {                                                                          \
        printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
        retval = -1;                                                           \
        goto bailout;                                                          \
    }

#define THROW_TJ(action) THROW(action, tjGetErrorStr2(tjInstance))

#define THROW_UNIX(action) THROW(action, strerror(errno))

const char *subsampName[TJ_NUMSAMP] = {
    "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"};

const char *colorspaceName[TJ_NUMCS] = {
    "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"};

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

            if ((tjInstance = tjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options = (TJXOPT_TRIM);

            if (tjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize,
                            &xform, 0) < 0)
                THROW_TJ("transforming input image");

            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
            if ((tjInstance = tjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }
        if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height,
                                &inSubsamp, &inColorspace) < 0)
            THROW_TJ("reading JPEG header");
        unsigned long yuvSize = tjBufSizeYUV2(width, 1, height, inSubsamp);

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
            image->esubsamp = inSubsamp;
        }

        //gettimeofday(&time_start, NULL);
        if (tjDecompressToYUV2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr,
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
        tjDestroy(tjInstance);
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
        if ((jpegBuf = (unsigned char *)tjAlloc(jpegSize)) == NULL)
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

            if ((tjInstance = tjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options |= TJXOPT_TRIM;

            if (tjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize,
                            &xform, 0) < 0)
                THROW_TJ("transforming input image");

            tjFree(jpegBuf);
            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
            if ((tjInstance = tjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }
        if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height,
                                &inSubsamp, &inColorspace) < 0)
            THROW_TJ("reading JPEG header");

        unsigned long yuvSize = tjBufSizeYUV2(width, 1, height, inSubsamp);

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
            if ((image->virtAddr = (char *)tjAlloc(yuvSize)) == NULL)

                THROW_UNIX("allocating uncompressed image buffer");
        }

        //if (tjDecompress(tjInstance, jpegBuf, jpegSize, image->virtAddr, 0, 0, 0, 0, TJ_YUV) == -1)
        //    THROW_TJ("executing tjDecompress()");

        if (tjDecompressToYUV2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr,
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
        tjDestroy(tjInstance);
    if (jpegBuf)
        tjFree(jpegBuf);
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
        if ((jpegBuf = (unsigned char *)tjAlloc(jpegSize)) == NULL)
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

            if ((tjInstance = tjInitTransform()) == NULL)
                THROW_TJ("initializing transformer");
            xform.op = eTranOpt;
            xform.options |= TJXOPT_TRIM;

            if (tjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize,
                            &xform, 0) < 0)
                THROW_TJ("transforming input image");

            tjFree(jpegBuf);
            jpegBuf = dstBuf;
            jpegSize = dstSize;
        }
        else
        {
            if ((tjInstance = tjInitDecompress()) == NULL)
                THROW_TJ("initializing decompressor");
        }
        if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height,
                                &inSubsamp, &inColorspace) < 0)
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
            if ((image->virtAddr = (char *)tjAlloc(width * height *
                                                            tjPixelSize[ePixelFmt])) == NULL)

                THROW_UNIX("allocating uncompressed image buffer");
        }

        if (tjDecompress2(tjInstance, jpegBuf, jpegSize, (unsigned char *)image->virtAddr, width, 0, height,
                          ePixelFmt, 0) < 0)
            THROW_TJ("decompressing JPEG image");
        if (image->phyAddr)
        {
            if (MI_SUCCESS != MI_SYS_FlushInvCache(image->virtAddr, width * height * tjPixelSize[ePixelFmt]))
            {
                THROW_TJ("MI_SYS_FlushInvCache");
            }
        }
        tjFree(jpegBuf);
        jpegBuf = NULL;
        tjDestroy(tjInstance);
        tjInstance = NULL;
    }
    else
    {
        THROW_TJ("need JPEG image");
    }
bailout:
    if (tjInstance)
        tjDestroy(tjInstance);
    if (jpegBuf)
        tjFree(jpegBuf);
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
