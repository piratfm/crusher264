/*
 * io.h
 *
 *  Created on: 10 окт. 2009
 *      Author: tipok
 */

#ifndef IO_H_
#define IO_H_

#include <usb.h>
#include "mg1264.h"
#include "crusher.h"

/* Request 0xbb reset */
int mg1264_adc_reset(crusher_t *crusher);

/* Request 0xbc reset */
int mg1264_reset(crusher_t *crusher);

/* host-bus register read 0xb9 implementation */
int mg1264_hbrr(crusher_t *crusher, uint8_t address, uint16_t *data);

/* host-bus register write 0xba implementation */
int mg1264_hbrw(crusher_t *crusher, uint8_t address, uint16_t value);

/* codec register write 0xb6 implementation */
int mg1264_csrw(crusher_t *crusher, uint16_t address, uint32_t value, uint8_t blockid, uint8_t regWidth);

/* codec CSR register read 0xb5 implementation */
int mg1264_csrr(crusher_t *crusher, uint16_t address, uint32_t *value, uint8_t blockid, uint8_t regWidth);

/* codec Memory Block write 0xB8->EP2 implementation */
int mg1264_mbw (crusher_t *crusher, uint32_t addr, uint32_t dataSize, uint8_t *data, uint8_t partition);

/* codec Memory Block read 0xBD->EP6 implementation */
int mg1264_mbr (crusher_t *crusher, uint32_t addr, uint32_t dataSize, uint8_t *data, uint8_t partition);

/* codec command transfer EP1 implementation */
int mg1264_cmd (crusher_t *crusher, command_t *cmd_in);

/* codec event query EP8 implementation */
int mg1264_event (crusher_t *crusher, event_t *event_out);


#endif /* IO_H_ */
