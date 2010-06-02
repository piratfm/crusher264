/*
 * qbox.h
 *
 *  Created on: 18 марта 2010
 *      Author: tipok
 */

#ifndef QBOX_H_
#define QBOX_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crusher.h"


#define QBOX_HDR_SIZE (6*4)

enum {
    SAMPLE_TYPE_AAC = 0x1,
    SAMPLE_TYPE_H264,
    SAMPLE_TYPE_PCM,
    SAMPLE_TYPE_DEBUG,
    SAMPLE_TYPE_H264_SLICE,
    SAMPLE_TYPE_QMA,
    SAMPLE_TYPE_VIN_STATS_GLOBAL,
    SAMPLE_TYPE_VIN_STATS_MB,
    SAMPLE_TYPE_Q711,
    SAMPLE_TYPE_Q728,
    SAMPLE_TYPE_MAX,
};




#define SAMPLE_FLAGS_CONFIGURATION_INFO 0x01
#define SAMPLE_FLAGS_CTS_PRESENT 0x02
#define SAMPLE_FLAGS_SYNC_POINT 0x04
#define SAMPLE_FLAGS_DISPOSABLE 0x08
#define SAMPLE_FLAGS_MUTE 0x10
#define SAMPLE_FLAGS_BASE_CTS_INCREMENT 0x20
#define SAMPLE_FLAGS_META_INFO 0x40
#define SAMPLE_FLAGS_END_OF_SEQUENCE 0x80
#define SAMPLE_FLAGS_END_OF_STREAM 0x100
#define SAMPLE_FLAGS_PADDING_MASK 0xFF000000

typedef struct qboxContext {
    uint32_t qbox_size;     ///< will be decreased
    uint32_t boxflags;
    uint8_t  version;
    uint16_t sample_stream_type;
    uint16_t sample_stream_id;
    uint32_t sample_flags;
    uint32_t sample_cts;
    uint8_t  *data;
} qboxContext;


#define QBOX_MAKE_STARTCODES 0x01
#define QBOX_STRICT_NALS 0x02
int qbox_parse(qboxContext *qbox, uint8_t *input_data, int data_size, int flags);

#endif /* QBOX_H_ */
