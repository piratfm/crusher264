/*
 * flvpkt.c
 *
 *  Created on: 7 мая 2010
 *      Author: tipok
 */

#include <string.h>
#include <assert.h>
#include <time.h>

#include <librtmp/amf.h>

#include "crusher.h"
#include "logging.h"
#include "qbox.h"
#include "bswap.h"
#include "flvpkt.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


//#undef EXTRA_DEBUG
#define EXTRA_DEBUG


/************************************************
 * Buffer writing funcs
 ************************************************/
static inline void put_byte(uint8_t **data, uint8_t val)
{
    *data[0] = val;
    *data += 1;
}

static inline void put_be16(uint8_t **data, uint16_t val)
{
#if (defined ARCH_ARM) && !(defined HAVE_ARMV6)
    put_byte(data, val >> 8);
    put_byte(data, val & 0x0ff);
#else
    *(uint16_t *) (*data) = me2be_16(val);
    *data += 2;
#endif
}

static inline void put_be24(uint8_t **data, uint32_t val)
{
    put_be16(data, val >> 8);
    put_byte(data, val & 0x0ff);
}

static inline void put_be32(uint8_t **data, uint32_t val)
{
#if (defined ARCH_ARM) && !(defined HAVE_ARMV6)
    put_be16(data, val >> 16);
    put_be16(data, val & 0x0ffff);
#else
    *(uint32_t *) (*data) = me2be_32(val);
    *data += 4;
#endif
}

static inline void put_buff(uint8_t **data, const uint8_t *src, int32_t srcsize)
{
    memcpy(*data, src, srcsize);
    *data += srcsize;
}

void put_tag(uint8_t **data, const char *tag)
{
    while (*tag) {
        put_byte(data, *tag++);
    }
}


/************************************************
 * FLV
 ************************************************/
void flv_open(flvContext *flv, const char *filename)
{
    memset(flv, 0x00, sizeof(flvContext));

    if(!filename)
        return;

    if (strcmp(filename, "-") != 0){
        flv->fh = fopen(filename, "w");
    } else {
        flv->fh = stdout;
    }
}

#define STR2AVAL(av,str)        av.av_val = str; av.av_len = strlen(av.av_val)
int flv_hdr(crusher_t *crusher, flvContext *flv, uint8_t *buffer)
{
    char pbuf[4096], *pend = pbuf + sizeof(pbuf);
    uint8_t *ptr = (uint8_t *) pbuf;
    char    *enc = NULL;
    uint8_t *metadata_size_pos;
    int data_size;
    AVal av;

    put_tag(&ptr, "FLV");         // tag type META
    put_byte(&ptr, 1);
    put_byte(&ptr, FLV_HEADER_FLAG_HASAUDIO | FLV_HEADER_FLAG_HASVIDEO);
    put_be32(&ptr, 9);
    put_be32(&ptr, 0);

    /* write meta_tag */
    put_byte(&ptr, 18);         // tag type META
    metadata_size_pos= ptr;
    put_be24(&ptr, 0);          // size of data part (sum of all parts below)
    put_be24(&ptr, 0);          // time stamp
    put_byte(&ptr, 0);          // time stamp upper8
    put_be24(&ptr, 0);          // streamID

    enc = (char *) ptr;
    STR2AVAL(av, "onMetaData");
    enc = AMF_EncodeString(enc, pend, &av);
    *enc++ = AMF_ECMA_ARRAY;

    enc = AMF_EncodeInt32(enc, pend, 5+5+2);

    flv->duration_offset = enc - pbuf;
    STR2AVAL(av, "duration");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, 0.0);

    STR2AVAL(av, "width");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->width);

    STR2AVAL(av, "height");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->height);

    STR2AVAL(av, "videodatarate");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->bitrate / 1024.0);

    STR2AVAL(av, "framerate");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->framerate_num / crusher->framerate_den);

    STR2AVAL(av, "videocodecid");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, 7.0);

    STR2AVAL(av, "audiodatarate");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->audio_bitrate / 1024.0);

    STR2AVAL(av, "audiosamplerate");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, crusher->audio_samplerate);

    STR2AVAL(av, "audiosamplesize");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, 16);

    STR2AVAL(av, "stereo");
    enc = AMF_EncodeNamedBoolean(enc, pend,  &av, crusher->audio_channels == 2);

    STR2AVAL(av, "audiocodecid");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, 10.0);

    flv->filesize_offset = enc - pbuf;
    STR2AVAL(av, "filesize");
    enc = AMF_EncodeNamedNumber(enc, pend,  &av, 0.0);

    enc = AMF_EncodeInt16(enc, pend, 0);
    *enc++ = AMF_OBJECT_END;


    /* write total size of tag */
    data_size = enc - (char *) metadata_size_pos - 10;
    put_be24(&metadata_size_pos, data_size);
    ptr = (uint8_t *) enc;
    put_be32(&ptr, data_size + 11);

    data_size = ptr - (uint8_t *) pbuf;
    memcpy(buffer, pbuf, data_size);
    return data_size;
}

inline int flv_write(flvContext *flv, uint8_t *buffer, int size)
{
    if(flv->fh)
        return fwrite(buffer, size, 1, flv->fh);
    else
        return 1;
}

void flv_close(flvContext *flv)
{
    int64_t file_size;
    double duration;
    char pbuf[128], *pend = pbuf + sizeof(pbuf);
    char *enc = pbuf;
    AVal av;

    if(!flv->fh)
        return;

    file_size = ftell(flv->fh);
    duration = (flv->ts + flv->ts_last) / (double)1000;

    /* update informations */
    fseek(flv->fh, flv->duration_offset, SEEK_SET);
    STR2AVAL(av, "duration");
    enc = AMF_EncodeNamedNumber(pbuf, pend,  &av, duration);
    fwrite(pbuf, enc - pbuf, 1, flv->fh);

    enc = pbuf;
    fseek(flv->fh, flv->filesize_offset, SEEK_SET);
    STR2AVAL(av, "filesize");
    enc = AMF_EncodeNamedNumber(pbuf, pend,  &av, file_size);
    fwrite(pbuf, enc - pbuf, 1, flv->fh);

    fseek(flv->fh, file_size, SEEK_SET);
#ifdef EXTRA_DEBUG
    logwrite(LOG_DEBUG, "FLV: duration: %f (o: %ld), size %ld (o:%ld)",
            duration, flv->duration_offset, file_size, flv->filesize_offset);
#endif
    fclose(flv->fh);
}

static int modify_sps_pps(qboxContext *qbox, uint8_t *data)
{
    uint8_t *ptr = data;
    uint8_t *buf;
    uint32_t sps_size = 0;
    uint32_t pps_size = 0;
    const uint8_t *sps = NULL;
    const uint8_t *pps = NULL;
    uint32_t size;
    uint8_t nal_type;

    buf = qbox->data;
    while(buf - qbox->data < qbox->qbox_size) {
        size = beptr2me_32(buf);
        nal_type = buf[4] & 0x1f;
        if (nal_type == 7) { /* SPS */
            sps = buf + 4;
            sps_size = size;
        } else if (nal_type == 8) { /* PPS */
            pps = buf + 4;
            pps_size = size;
        }
        buf += size + 4;
    }

    assert(sps);
    assert(pps);
#ifdef EXTRA_DEBUG
    logwrite(LOG_DEBUG, "sps_size: %u, pps_size: %u", sps_size, pps_size);
#endif
    put_byte(&ptr, 1); /* version */
    put_byte(&ptr, sps[1]); /* profile */
    put_byte(&ptr, sps[2]); /* profile compat */
    put_byte(&ptr, sps[3]); /* level */
    put_byte(&ptr, 0xff); /* 6 bits reserved (111111) + 2 bits nal size length - 1 (11) */
    put_byte(&ptr, 0xe1); /* 3 bits reserved (111) + 5 bits number of sps (00001) */

    put_be16(&ptr, sps_size);
    put_buff(&ptr, sps, sps_size);
    put_byte(&ptr, 1); /* number of pps */
    put_be16(&ptr, pps_size);
    put_buff(&ptr, pps, pps_size);

    return ptr - data;
}

int qbox2flv(qboxContext *qbox, flvContext *flv, uint8_t *buffer)
{
    int64_t newts = qbox->sample_cts/90; ///< 1000/90000
    int32_t flags_size = 0;
    int size = qbox->qbox_size;
    int flags = 0;
    uint8_t *ptr = buffer;

    if(!(qbox->sample_flags & (SAMPLE_FLAGS_CTS_PRESENT|SAMPLE_FLAGS_CONFIGURATION_INFO)))
        newts++;

#ifdef EXTRA_DEBUG
    logwrite(LOG_DEBUG, "FLV: pts_in: %u", qbox->sample_cts);
    logwrite(LOG_DEBUG, "FLV: pts_in/90: %ld", newts);
#endif

    if(newts < flv->ts_last) {
        flv->ts += flv->ts_last;
#ifdef EXTRA_DEBUG
        logwrite(LOG_DEBUG, "FLV: pts_out: %ld, flv->ts: %ld", newts, flv->ts);
#endif
    }

    flv->ts_last = newts;
    newts += flv->ts;
#ifdef EXTRA_DEBUG
    logwrite(LOG_DEBUG, "FLV: pts_out: %ld", newts);
#endif

    if (qbox->sample_stream_type == SAMPLE_TYPE_H264) {
        put_byte(&ptr, FLV_TAG_TYPE_VIDEO);

        flags = FLV_CODECID_H264;
        flags_size= 5;

        /* create sps/pps for flv */
        if(qbox->sample_flags & SAMPLE_FLAGS_CONFIGURATION_INFO) {
            size = modify_sps_pps(qbox, &buffer[11+flags_size]);
            if(!size)
                return -1;
            qbox->qbox_size=0;
            flags |= FLV_FRAME_KEY;
        } else if (qbox->sample_flags & SAMPLE_FLAGS_SYNC_POINT){
            flags |= FLV_FRAME_KEY;
        } else {
            flags |= FLV_FRAME_INTER;
        }
    } else if(qbox->sample_stream_type == SAMPLE_TYPE_AAC) {
        put_byte(&ptr, FLV_TAG_TYPE_AUDIO);

        flags = FLV_CODECID_AAC | FLV_SAMPLERATE_44100HZ | FLV_SAMPLESSIZE_16BIT | FLV_STEREO;
        flags_size= 2;
    } else {
        logwrite(LOG_ERROR, "unknown qbox payload: 0x%08x", qbox->sample_stream_type);
        return -1;
    }

    put_be24(&ptr, size + flags_size);
    put_be24(&ptr, newts);
    put_byte(&ptr, (newts >> 24) & 0x7F); // timestamps are 32bits _signed_
    put_be24(&ptr, 0);

    put_byte(&ptr, flags);
    /* for aac/avc only */
    put_byte(&ptr, qbox->sample_flags & SAMPLE_FLAGS_CONFIGURATION_INFO ? 0 : 1);
    if (qbox->sample_stream_type == SAMPLE_TYPE_H264) {
        put_be24(&ptr, 0);
        if(qbox->sample_flags & SAMPLE_FLAGS_CONFIGURATION_INFO)
            ptr+=size;
    }

    put_buff(&ptr, qbox->data, qbox->qbox_size);
    put_be32(&ptr,size+flags_size+11); // previous tag size

#ifdef EXTRA_DEBUG
    //1920 * 1000/90000
    logwrite(LOG_DEBUG, "FLV: sz_or: %u, sz_flv: %u, %c",
            qbox->qbox_size, ptr - buffer,
            (qbox->sample_flags & SAMPLE_FLAGS_SYNC_POINT) &&
            (qbox->sample_stream_type == SAMPLE_TYPE_H264) ? 'S' : ' ');
#endif

    return ptr - buffer;
}
