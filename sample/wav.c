/*
 * wav.c
 *
 *  Created on: 27 марта 2010
 *      Author: tipok
 */


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "bswap.h"
#include "wav.h"

static WAVEHDR waveHdr;
static unsigned long nBytesDone = 0;

int wav_init(FILE *audio, int channels, unsigned long rate, long nBitsPerSample, unsigned long expected_bytes)
{
  uint64_t nBlockAlign = channels * ((nBitsPerSample + 7) / 8);
  uint64_t nAvgBytesPerSec = nBlockAlign * rate;
  uint64_t temp = expected_bytes + sizeof(WAVEHDR) - sizeof(CHUNKHDR);

  waveHdr.chkRiff.ckid    = me2le_32(FOURCC_RIFF);
  waveHdr.fccWave         = me2le_32(FOURCC_WAVE);
  waveHdr.chkFmt.ckid     = me2le_32(FOURCC_FMT);
  waveHdr.chkFmt.dwSize   = me2le_32(sizeof (PCMWAVEFORMAT));
  waveHdr.wFormatTag      = me2le_16(WAVE_FORMAT_PCM);
  waveHdr.nChannels       = me2le_16(channels);
  waveHdr.nSamplesPerSec  = me2le_32(rate);
  waveHdr.nBlockAlign     = me2le_16(nBlockAlign);
  waveHdr.nAvgBytesPerSec = me2le_32(nAvgBytesPerSec);
  waveHdr.wBitsPerSample  = me2le_16(nBitsPerSample);
  waveHdr.chkData.ckid    = me2le_32(FOURCC_DATA);
  waveHdr.chkRiff.dwSize  = me2le_32(temp);
  waveHdr.chkData.dwSize  = me2le_32(expected_bytes);

  return fwrite (&waveHdr, 1, sizeof(waveHdr), audio);
}

int wav_exit ( FILE *audio)
{
  uint64_t temp = nBytesDone + sizeof(WAVEHDR) - sizeof(CHUNKHDR);

  waveHdr.chkRiff.dwSize = me2le_32(temp);
  waveHdr.chkData.dwSize = me2le_32(nBytesDone);

  /* goto beginning */
  if (fseek(audio, 0L, SEEK_SET) == -1) {
    return 0;
  }
  return fwrite (&waveHdr, 1, sizeof(waveHdr), audio);
}

int wav_write_bytes(FILE *audio, uint8_t *data, int bytes)
{
    int16_t *samples = (int16_t *) data;
    int i, numsamples = bytes/2;
    /* we got pcm_s16be, convert it to pcm_s16le */
    for (i=0; i<numsamples; i++)
        samples[i] = bswap_16(samples[i]);

    nBytesDone+=numsamples*2;
    return fwrite (samples, numsamples, sizeof(int16_t), audio);
}
