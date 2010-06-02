/*
 * qbox.c
 *
 *  Created on: 18 марта 2010
 *      Author: tipok
 */
#include "crusher.h"
#include "bswap.h"
#include "qbox.h"
#include "logging.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef EXTRA_DEBUG
//#define EXTRA_DEBUG
/*
 * The following functions constitute our interface to the world
 */
int qbox_parse(qboxContext *qbox, uint8_t *input_data, int data_size, int flags)
{
    uint8_t *buf;
    uint32_t nal_size;

    if(data_size < QBOX_HDR_SIZE)
        return 0;

    qbox->qbox_size = beptr2me_32(input_data);
    if(qbox->qbox_size < QBOX_HDR_SIZE)
        return 0;
    qbox->qbox_size -= QBOX_HDR_SIZE;
    //qbox
    //all input data aligned by 4
    qbox->version = input_data[8];
    qbox->boxflags = beptr2me_32(input_data+8) & 0x00FFFFFF;
    qbox->sample_stream_type = beptr2me_16(input_data+12);
    qbox->sample_stream_id = beptr2me_16(input_data+14);
    qbox->sample_flags = beptr2me_32(input_data+16);
    qbox->sample_cts = beptr2me_32(input_data+20);
    qbox->data = input_data + QBOX_HDR_SIZE;

#ifdef EXTRA_DEBUG
    logwrite(LOG_XTREME, "qbox_size0: %d, version: 0x%02x, boxflags: 0x%08x, sample_stream_type: %d, sample_stream_id: %d, sample_flags: %08x, sample_cts: %u",
                qbox->qbox_size,
                qbox->version,
                qbox->boxflags,
                qbox->sample_stream_type,
                qbox->sample_stream_id,
                qbox->sample_flags,
                qbox->sample_cts );
#endif

    if (qbox->sample_stream_type == SAMPLE_TYPE_H264 && (flags & (QBOX_STRICT_NALS | QBOX_MAKE_STARTCODES))) {
        buf = qbox->data;
        while(buf - qbox->data + 4 < qbox->qbox_size) {
                nal_size = beptr2me_32(buf);
#ifdef EXTRA_DEBUG
                logwrite(LOG_XTREME, "nal: %d", nal_size);
#endif

                if(flags & QBOX_MAKE_STARTCODES) {
                    buf[0] = 0x00;
                    buf[1] = 0x00;
                    buf[2] = 0x00;
                    buf[3] = 0x01;
                }

                buf += nal_size + 4;
                if(buf + 4 >= qbox->data + qbox->qbox_size) {
                    if(flags & QBOX_STRICT_NALS) {
                        qbox->qbox_size = buf - qbox->data;
#ifdef EXTRA_DEBUG
                        logwrite(LOG_DEBUG, "cutted %d bytes", qbox->qbox_size - (buf - qbox->data));
#endif
                    }
                    break;
                }
            }
    }

    return qbox->qbox_size;
}
