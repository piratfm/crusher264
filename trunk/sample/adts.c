/*
 * adts.c
 *
 *  Created on: 19 марта 2010
 *      Author: tipok
 */

#include "adts.h"
static const int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

static uint8_t adtsHdr[ADTS_HDR_SIZE] = {0xff, 0xf0, 0x00, 0x00, 0x00, 0x1f, 0xfc};

static int FindAdtsSRIndex(int sr)
{
    int i;
    for (i = 0; i < 16; i++) {
        if (sr == adts_sample_rates[i])
            return i;
    }
    return 16 - 1;
}

int adts_init(int profile, int samplerate, int channels)
{
    adtsHdr[1] |= 1;       /* 1bit:  protection absent (1 - means "no protection")*/
    adtsHdr[2] = ((1/*0-main, 1-lc*/ << 6) & 0xC0);          /* 2bits: profile */

    adtsHdr[2] |= ((FindAdtsSRIndex(samplerate) << 2) & 0x3C);       /* 4b: sampling_frequency_index */
    adtsHdr[2] |= ((channels >> 2) & 0x1); /* 1b: channel_configuration */
    adtsHdr[3] = ((channels << 6) & 0xC0); /* 2b: channel_configuration */
    return 1;
}

int adts_up_write(FILE *audio, int size)
{
    size += ADTS_HDR_SIZE;
    adtsHdr[3] &= 0xFC;
    adtsHdr[3] |= ((size >> 11) & 0x03);    /* 2b: aac_frame_length */
    adtsHdr[4] = size >> 3;                 /* 8b: aac_frame_length */
    adtsHdr[5] = (size << 5) & 0xE0;        /* 3b: aac_frame_length */
    return fwrite (&adtsHdr, 1, sizeof(adtsHdr), audio);
}
