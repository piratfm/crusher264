/*
 * converter.h
 *
 *  Created on: 20 окт. 2009
 *      Author: tipok
 */

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include <stdint.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

enum {
	CONVERTER_FAIL = 0,
	CONVERTER_FRAME_SENT = 1,
	CONVERTER_FINISHED = 2,
};


/* image conversion handler
 * buffers initialized only once */
typedef struct {
	/* Image size */
	uint16_t y_width;
	uint16_t y_height;
	uint16_t y_stride;

	uint16_t uv_width;
	uint16_t uv_height;
	uint16_t uv_stride;

	/* pointers */
    uint8_t	*y;
    uint8_t	*u;
    uint8_t	*v;
	uint8_t	*iyuv;

	uint8_t	*iyuv_blocked;


    uint16_t y_width_d;
   	uint16_t y_height_d;
    uint16_t uv_width_d;
	uint16_t uv_height_d;

	/* number of rows/columns */
	uint8_t y_rows;
	uint8_t y_cols;
	uint8_t iuv_rows;

	/* data sizes */
	uint32_t y_frame_size_d;
	uint32_t uv_frame_size_d;
	uint32_t iyuv_frame_size_d;
} imgConverter_t;


/**************************************************************
 * IMAGE CONVERTER Functions
 **************************************************************/
int imgConverter_init(imgConverter_t *imgConv);
int imgConverter_convert(imgConverter_t *imgConv);
void imgConverter_free(imgConverter_t *imgConv);


#endif /* CONVERTER_H_ */
