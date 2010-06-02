
#include <stdlib.h>
#include <stdio.h> /* for additional logging */
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "imgConverter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "logging.h"

/* YUV bytes in frame (not blocked):
         0                         w*64
       0 +----------------------------+
         |DDDDDDDDDDDDDDDDD..... |0..0|
         |                       |0..0|
         |                       |0..0|
         |                       |0..0|
         |                       |0..0|
         |                       |0..0|
         |                       |0..0|
         |                    ..D|0..0|
         +-----------------------|0..0|
         |0..........................0|
         |0..........................0|
(h)*32   +----------------------------+


if width not divided into 64 or height not divided into 32 (in any Y, U or V frame), then offset filled by zero.

Blocks position in frame:
             0      32      64      128     256                 w*64
      +-  0  +-------+-------+-------+-------+-----//-----+------+
      |      |   1   |   2   |   5   |   6   |   9 || n-3 | n-2  |
slice |   16 +-------+-------+-------+-------+-----||-----+------+
      |      |   3   |   4   |   7   |   8   |  11 || n-1 |  n   |
      +-  32 +-------+-------+-------+-------+-----||-----+------+
             |  81   |  82   |   85  |  86   |  89 ||2n-3 |2n-2  |
          48 +-------+-------+-------+-------+-----||-----+------+
             |  83   |  84   |  87   |  88   |  91 ||2n-1 |  2n  |
             . . . . . . . . . . . . . . . . . . . || . . . . . .
      (h)*32 +-------+-------+-------+-------+-----//-----+------+

1,2,3,4... - macroblocks (size: 32x16 pixels)

1..80 - slice (size 40x2 macroblocks) */
static int blockFrame(uint8_t *ptr_out, uint8_t *ptr_in, uint8_t rows, uint8_t cols);
static int copySlice(uint8_t *ptr_out, uint8_t *ptr_in, uint8_t cols);
static int copy4x16x32(uint8_t *ptr_out, uint8_t *ptr_in, uint16_t Stride);
static int copy16x32(uint8_t *ptr_out, uint8_t *ptr_in, uint16_t Stride);
static void interleaveBytes(uint8_t *src1, uint8_t *src2, uint8_t *dest,
                            long width, long height, long src1Stride,
                            long src2Stride, long dstStride);



#if defined(__amd64__)
#define HAVE_3DNOW
#endif

#if defined(__x86_64__)
#define REGa  rax
#define REGc  rcx
#define REGd  rdx
#define REG_a  "rax"
#define REG_c  "rcx"
#define REG_d  "rdx"
#define REG_SP "rsp"
#define ALIGN_MASK "$0xFFFFFFFFFFFFFFF8"
//MMX2 versions
//#define HAVE_MMX
//#define HAVE_MMX2
//#define HAVE_SSE2
#else
#define REGa  eax
#define REGc  ecx
#define REGd  edx
#define REG_a  "eax"
#define REG_c  "ecx"
#define REG_d  "edx"
#define REG_SP "esp"
#define ALIGN_MASK "$0xFFFFFFF8"
#endif //ARCH_X86 || ARCH_X86_64


#ifdef HAVE_3DNOW
/* On K6 femms is faster of emms. On K7 femms is directly mapped on emms. */
#define EMMS     "femms"
#else
#define EMMS     "emms"
#endif

#ifdef HAVE_3DNOW
#define PREFETCH  "prefetch"
#define PREFETCHW "prefetchw"
#define PAVGB	  "pavgusb"
#elif defined ( HAVE_MMX2 )
#define PREFETCH "prefetchnta"
#define PREFETCHW "prefetcht0"
#define PAVGB	  "pavgb"
#endif

#ifdef HAVE_MMX2
#define MOVNTQ "movntq"
#define SFENCE "sfence"
#else
#define MOVNTQ "movq"
#define SFENCE " # nop"
#endif

/**************************************************************
 * IMAGE CONVERTER Functions
 **************************************************************/
int imgConverter_init(imgConverter_t *imgConv)
{
	int IUVSizeD;

	logwrite(LOG_INFO, "Optimisations:");
#ifdef HAVE_3DNOW
	logwrite(LOG_INFO, "3DNOW");
#endif
#ifdef HAVE_MMX
	logwrite(LOG_INFO, "MMX");
#endif
#ifdef HAVE_MMX2
	logwrite(LOG_INFO, "MMX2");
#endif
#ifdef HAVE_SSE2
	logwrite(LOG_INFO, "SSE2");
#endif

	imgConv->y_cols = imgConv->y_width/64;
	imgConv->y_rows = imgConv->y_height/32;
	if (imgConv->y_width % 64 > 0)
		imgConv->y_cols++;

	if (imgConv->y_height % 32 > 0)
		imgConv->y_rows++;

	imgConv->y_width_d = 64 * imgConv->y_cols;
	imgConv->y_height_d = 32 * imgConv->y_rows;
	imgConv->y_frame_size_d = imgConv->y_width_d * imgConv->y_height_d;


	imgConv->iuv_rows = imgConv->y_rows/2;
	if(imgConv->y_rows%2)
		imgConv->iuv_rows++;

	IUVSizeD = imgConv->y_cols * imgConv->iuv_rows * (64*32);

	logwrite(LOG_DEBUG, "YFrame=%dx%d (%dx%d), iUVFrame=%dx%d (%dx%d).",
								imgConv->y_width, imgConv->y_height,
								imgConv->y_width_d, imgConv->y_height_d,
								imgConv->y_cols*64, imgConv->iuv_rows*32,
								imgConv->y_cols, imgConv->iuv_rows);

	/* blocked frames */
	imgConv->iyuv_frame_size_d = 	imgConv->y_frame_size_d + IUVSizeD;

	/* frame with interleaved UV */
	imgConv->iyuv = malloc(imgConv->iyuv_frame_size_d);
	if(!imgConv->iyuv)
		return 0;

	memset(imgConv->iyuv, 0, imgConv->iyuv_frame_size_d);

	imgConv->iyuv_blocked = malloc(imgConv->iyuv_frame_size_d);
	return 1;
}



int imgConverter_convert(imgConverter_t *imgConv)
{
	int i;
	logwrite(LOG_XTREME, "Readed YUV frame...");

	for(i=0; i<imgConv->y_height;i++)
		memcpy(imgConv->iyuv + imgConv->y_width_d*i, imgConv->y + imgConv->y_stride*i,
				imgConv->y_width);

	logwrite(LOG_XTREME, "Interleave UV...");
	interleaveBytes(imgConv->u, imgConv->v, imgConv->iyuv + imgConv->y_frame_size_d,
			imgConv->uv_width, imgConv->uv_height,
			imgConv->uv_stride, imgConv->uv_stride, imgConv->y_width_d);

	logwrite(LOG_XTREME, "Block...");
	if(!blockFrame(imgConv->iyuv_blocked, imgConv->iyuv, imgConv->y_rows + imgConv->iuv_rows,
			imgConv->y_cols))
		return CONVERTER_FAIL;

	logwrite(LOG_XTREME, "Frame blocked...\n");

	return CONVERTER_FINISHED;
};

void imgConverter_free(imgConverter_t *imgConv)
{
	if(imgConv->iyuv)
		free(imgConv->iyuv);
	if(imgConv->iyuv_blocked)
		free(imgConv->iyuv_blocked);
};







static int blockFrame(uint8_t *ptr_out, uint8_t *ptr_in, uint8_t rows, uint8_t cols)
{

	int sliceSize = (32*16) * 2 * 2 * cols;
	int slice;
//	XTREME("Blocking frame: size=%dx%d.", rows, cols);
	for(slice = 0; slice < rows; slice++){
//		fprintf(stderr, "DEBUG: copy_row(%d).\n",slice);
		copySlice(ptr_out + sliceSize*slice, ptr_in + sliceSize*slice, cols);
	}
	return 1;
}

static int copySlice(uint8_t *ptr_out, uint8_t *ptr_in, uint8_t cols)
{
	int fourBlockNum;
	uint16_t Stride = cols*(2*32);

	for(fourBlockNum = 0; fourBlockNum < cols; fourBlockNum++){
//		fprintf(stderr, "DEBUG: copy_4blocks(%d):",fourBlockNum);
		copy4x16x32(ptr_out + (32*16)*4*fourBlockNum, ptr_in + (32*2)*fourBlockNum, Stride);
	}
	return 1;
}


static int copy4x16x32(uint8_t *ptr_out, uint8_t *ptr_in, uint16_t Stride)
{
	int counter = 0;
/*                32       64
 * ptr -> +--------+--------+ <stride = WidthD
 *        |   1    |    2   |
 *     16 +--------+--------+
 *        |   3    |    4   |
 *     32 +--------+--------+
 * */

	/* 1 */
	counter += copy16x32(ptr_out, ptr_in, Stride);
	counter += copy16x32(ptr_out + (32*16), ptr_in + 32, Stride);
	counter += copy16x32(ptr_out + (32*16)*2, ptr_in + (16 * Stride), Stride);
	counter += copy16x32(ptr_out + (32*16)*3, ptr_in + (16 * Stride) + 32, Stride);
//	fprintf(stderr, ":%d\n", counter);
	return counter;
}

static int copy16x32(uint8_t *ptr_out, uint8_t *ptr_in, uint16_t Stride)
{
/*                32
 * ptr -> +--------+ <stride=WidthD
 *        |   x    |
 *     16 +--------+
 * */
	int line;
	for (line=0; line < 16; line++) {
//		fprintf(stderr, "%u>%u (line:%d, Stride:%d)\n", (imgConv->blockedFrames + imgConv->blockedFrameSizeD * imgConv->blockedFramesNum) ,(ptr_out + (line*32) + 32), line, Stride);
//		assert(	(imgConv->blockedFrames + imgConv->blockedFrameSizeD * imgConv->blockedFramesNum) >= (ptr_out + (line*32) + 32) );
		memcpy (ptr_out + (line*32), ptr_in + (line*Stride), 32);
	}
	return line;
}

static void interleaveBytes(uint8_t *src1, uint8_t *src2, uint8_t *dest,
                            long width, long height, long src1Stride,
                            long src2Stride, long dstStride)
{
        long h;

        for(h=0; h < height; h++)
        {
                long w;

#ifdef HAVE_MMX
#ifdef HAVE_SSE2
                asm(
                        "xor %%"REG_a", %%"REG_a"       \n\t"
                        "1:                             \n\t"
                        PREFETCH" 64(%1, %%"REG_a")     \n\t"
                        PREFETCH" 64(%2, %%"REG_a")     \n\t"
                        "movdqa (%1, %%"REG_a"), %%xmm0 \n\t"
                        "movdqa (%1, %%"REG_a"), %%xmm1 \n\t"
                        "movdqa (%2, %%"REG_a"), %%xmm2 \n\t"
                        "punpcklbw %%xmm2, %%xmm0       \n\t"
                        "punpckhbw %%xmm2, %%xmm1       \n\t"
                        "movntdq %%xmm0, (%0, %%"REG_a", 2)\n\t"
                        "movntdq %%xmm1, 16(%0, %%"REG_a", 2)\n\t"
                        "add $16, %%"REG_a"             \n\t"
                        "cmp %3, %%"REG_a"              \n\t"
                        " jb 1b                         \n\t"
                        ::"r"(dest), "r"(src1), "r"(src2), "r" (width-15)
                        : "memory", "%"REG_a""
                );
#else
                asm(
                        "xor %%"REG_a", %%"REG_a"       \n\t"
                        "1:                             \n\t"
                        PREFETCH" 64(%1, %%"REG_a")     \n\t"
                        PREFETCH" 64(%2, %%"REG_a")     \n\t"
                        "movq (%1, %%"REG_a"), %%mm0    \n\t"
                        "movq 8(%1, %%"REG_a"), %%mm2   \n\t"
                        "movq %%mm0, %%mm1              \n\t"
                        "movq %%mm2, %%mm3              \n\t"
                        "movq (%2, %%"REG_a"), %%mm4    \n\t"
                        "movq 8(%2, %%"REG_a"), %%mm5   \n\t"
                        "punpcklbw %%mm4, %%mm0         \n\t"
                        "punpckhbw %%mm4, %%mm1         \n\t"
                        "punpcklbw %%mm5, %%mm2         \n\t"
                        "punpckhbw %%mm5, %%mm3         \n\t"
                        MOVNTQ" %%mm0, (%0, %%"REG_a", 2)\n\t"
                        MOVNTQ" %%mm1, 8(%0, %%"REG_a", 2)\n\t"
                        MOVNTQ" %%mm2, 16(%0, %%"REG_a", 2)\n\t"
                        MOVNTQ" %%mm3, 24(%0, %%"REG_a", 2)\n\t"
                        "add $16, %%"REG_a"             \n\t"
                        "cmp %3, %%"REG_a"              \n\t"
                        " jb 1b                         \n\t"
                        ::"r"(dest), "r"(src1), "r"(src2), "r" (width-15)
                        : "memory", "%"REG_a
                );
#endif
                for(w= (width&(~15)); w < width; w++)
                {
                        dest[2*w+0] = src1[w];
                        dest[2*w+1] = src2[w];
                }
#else
                for(w=0; w < width; w++)
                {
                        dest[2*w+0] = src1[w];
                        dest[2*w+1] = src2[w];
                }
#endif
                dest += dstStride;
                src1 += src1Stride;
                src2 += src2Stride;
        }
#ifdef HAVE_MMX
        asm(
                EMMS" \n\t"
                SFENCE" \n\t"
                ::: "memory"
                );
#endif
}


