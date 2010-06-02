/*
 * adts.h
 *
 *  Created on: 19 марта 2010
 *      Author: tipok
 */

#ifndef ADTS_H_
#define ADTS_H_

#include <stdio.h>
#include <stdint.h>

#define ADTS_HDR_SIZE     7
int adts_init(int profile, int samplerate, int channels);
int adts_up_write(FILE *audio, int size);

#endif /* ADTS_H_ */
