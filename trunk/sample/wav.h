/*
 * wav.h - for creating wav header (borrowed from cdda2wav)
 *
 *  Created on: 27 марта 2010
 *      Author: tipok
 */

#ifndef WAV_H_
#define WAV_H_

#include <stdint.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

typedef unsigned int FOURCC;    /* a four character code */

typedef struct CHUNKHDR {
  FOURCC ckid;          /* chunk ID */
  unsigned int dwSize;  /* chunk size */
} GCC_PACK CHUNKHDR;

/* flags for 'wFormatTag' field of WAVEFORMAT */
#define WAVE_FORMAT_PCM 1

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
  unsigned short wFormatTag;    /* format type */
  unsigned short nChannels;     /* number of channels (i.e. mono, stereo, etc.) */
  unsigned int nSamplesPerSec; /* sample rate */
  unsigned int nAvgBytesPerSec;/* for buffer size estimate */
  unsigned short nBlockAlign;   /* block size of data */
  unsigned short wBitsPerSample;
} GCC_PACK PCMWAVEFORMAT;
typedef PCMWAVEFORMAT *PPCMWAVEFORMAT;


/* MMIO macros */
#define mmioFOURCC(ch0, ch1, ch2, ch3) \
  ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | \
  ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24))

#define FOURCC_RIFF     mmioFOURCC ('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC ('L', 'I', 'S', 'T')
#define FOURCC_WAVE     mmioFOURCC ('W', 'A', 'V', 'E')
#define FOURCC_FMT      mmioFOURCC ('f', 'm', 't', ' ')
#define FOURCC_DATA     mmioFOURCC ('d', 'a', 't', 'a')


/* simplified Header for standard WAV files */
typedef struct WAVEHDR {
  CHUNKHDR chkRiff;
  FOURCC fccWave;
  CHUNKHDR chkFmt;
  unsigned short wFormatTag;    /* format type */
  unsigned short nChannels;     /* number of channels (i.e. mono, stereo, etc.) */
  unsigned int nSamplesPerSec; /* sample rate */
  unsigned int nAvgBytesPerSec;/* for buffer estimation */
  unsigned short nBlockAlign;   /* block size of data */
  unsigned short wBitsPerSample;
  CHUNKHDR chkData;
} GCC_PACK WAVEHDR;

#define IS_STD_WAV_HEADER(waveHdr) ( \
  waveHdr.chkRiff.ckid == FOURCC_RIFF && \
  waveHdr.fccWave == FOURCC_WAVE && \
  waveHdr.chkFmt.ckid == FOURCC_FMT && \
  waveHdr.chkData.ckid == FOURCC_DATA && \
  waveHdr.wFormatTag == WAVE_FORMAT_PCM)


/* Prototypes */
int wav_init(FILE *audio, int channels, unsigned long rate,
        long nBitsPerSample, unsigned long expected_bytes);
int wav_exit ( FILE *audio);
int wav_write_bytes(FILE *audio, uint8_t *data, int bytes);

#endif /* WAV_H_ */
