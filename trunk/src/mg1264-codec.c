/*
 * mg1264-codec.c
 *
 *  Created on: 20 окт. 2009
 *      Author: tipok
 */
#include <assert.h>
#include <string.h>
#include "bswap.h"
#include "crusher.h"
#include "mg1264.h"
#include "mg1264-io.h"
#include "logging.h"

/* firstly send only 1 frame,
 * after first event completition can be sended more (5, etc.)...
 * */
int mg1264_sendframes(crusher_t *crusher, uint8_t *converted_data, int data_size)
{
	block_config_chunk_t *blockConfig;
	event_t *event = (event_t *) crusher->lastEvent;
	event_ready_t *eventReady = (event_ready_t *) &event->payload[0];
	uint8_t memblock;
	//uint8_t justStarted = 0;
	uint8_t wantblocks = crusher->framesToLoad;

	XTREME("sendframes: event_complete:%d, blockConfigId:%d, data_size:%d",
			crusher->event_complete, crusher->blockConfigId, data_size);

	assert(eventReady->blocksNum > 0);

	/* first time here */
	if (!crusher->blockConfig) {
		crusher->blockConfig = calloc(eventReady->blocksNum, sizeof(block_config_chunk_t));
		XTREME("allocate %d blocks for blockConfig", eventReady->blocksNum);
		crusher->blockConfigSize = eventReady->blocksNum;
		crusher->blockConfigId = 0;
		/* this fo extract sps/pps at startup */
		wantblocks = 1;
	} else
		assert(eventReady->blocksNum <= crusher->blockConfigSize);

	blockConfig = (block_config_chunk_t *) crusher->blockConfig;

	if (!converted_data || !data_size)
		wantblocks=0;
	else if(eventReady->blocksNum - crusher->blockConfigId < 2)
		wantblocks=1;


	/* first time here */
	if(wantblocks && crusher->event_complete) {
		XTREME("sendframes: start event");
		/* read config for new event */
		mg1264_mbr(crusher, eventReady->startAddr,
					eventReady->blocksNum * sizeof(block_config_chunk_t),
					(uint8_t *) blockConfig, 0x40);
		//XTREME("blockConfig:");
		//dump_bytes((char *)blockConfig, eventReady->blocksNum * sizeof(block_config_chunk_t));
		crusher->event_complete = 0;
	}

	memblock = be2me_32(blockConfig[crusher->blockConfigId].Id);
	XTREME("memblock:%02x",memblock);


	if(wantblocks == 1) {
		XTREME("BUFF: Sending block.");
		if  ( !mg1264_mbw(crusher, 0, data_size, converted_data, memblock) ) {
			ERROR("Can't send frames.");
			return 0;
		}
		crusher->blockConfigId+=wantblocks;
	} else if(wantblocks) {
		XTREME("BUFF: Buffering block.");
		assert(crusher->inputFramesFilled < crusher->framesToLoad);
		assert(crusher->inputFramesFilled*crusher->inputFrameLen + data_size <=
							crusher->framesToLoad*crusher->inputFrameLen);
		memcpy(crusher->input_buffer + crusher->inputFramesFilled*crusher->inputFrameLen, converted_data, data_size);
		crusher->inputFramesFilled++;
		if(wantblocks == crusher->inputFramesFilled) {
			XTREME("BUFF: Sending block.");
			if  ( !mg1264_mbw(crusher, 0, crusher->inputFramesFilled*crusher->inputFrameLen,
																crusher->input_buffer, memblock) ) {
				ERROR("Can't send frames.");
				return 0;
			}
			crusher->inputFramesFilled = 0;
			crusher->blockConfigId+=wantblocks;
		}
	}




	/* blocks filled, last time here with this event */
	if (crusher->blockConfigId >= eventReady->blocksNum || !wantblocks ) {
		XTREME("sendframes: finish event");

		if(crusher->blockConfigId) {
			mg1264_mbw(crusher, eventReady->startAddr,
					crusher->blockConfigId*sizeof(block_config_chunk_t),
					(uint8_t *) blockConfig, 0x40);
			XTREME("playing %d frames", crusher->blockConfigId);
			encoderPlayBlocks(crusher, eventReady->bufferId, crusher->blockConfigId);
		}

		crusher->event_complete = 1;
		crusher->blockConfigId = 0;
	}

	return 1;
}


int mg1264_readframes(crusher_t *crusher)
{
	event_t *event = (event_t *) crusher->lastEvent;
	event_bs_block_ready_t *eventReadyBlock = (event_bs_block_ready_t *) &event->payload[0];
	int blockId, readLen;
//	uint8_t mpegbuff[MAX_H264_FRAMESIZE];

	/* ev:
	 * 6 blocks:
	 * #0 addr: 0xcb7000 len: 0x40
	 * #1 addr: 0xcb7040 len: 0x3ee5
	 * #2 addr: 0xcbaf28 len: 0x472c
	 * #3 addr: 0xcbf654 len: 0x4c86
	 * #4 addr: 0xcc42dc len: 0x540a
	 * #5 addr: 0xcc96e8 len: 0x2744
	 *
	 * #0 -> #1 -> |0x03| -> #2 -> #3 -> |0x02| -> #4 -> |0x02| -> #5 -> 0xCCBE2C
	 *
	 * data:
	 * (#0,#1 + 0x03) addr: 0xcb7000 len: 0x3f28
	 * (#2) addr: 0xcbaf28 len: 0x472c
	 * (#3 + 0x02) addr: 0xcbf654 len: 0x4c88
	 * (#4 + 0x02) addr: 0xcc42dc len: 0x540c
	 * (#5) addr: 0xcc96e8 len: 0x2744
	 * */

	XTREME("readframes: starting");
	XTREME("event: blocksStreamType:%d, blocksNum:%d",
				eventReadyBlock->blocksStreamType, eventReadyBlock->blocksNum);
	assert(eventReadyBlock->blocksNum > 0 && eventReadyBlock->blocksNum <= 6);

	crusher->out_blocks = eventReadyBlock->blocksNum;
	for(blockId=0; blockId < eventReadyBlock->blocksNum; blockId++) {
		XTREME("readframes: block:%d, addr:%08x, len:%d", blockId,
				eventReadyBlock->block[blockId].addr,
				eventReadyBlock->block[blockId].len);
		crusher->out_data[blockId].len = eventReadyBlock->block[blockId].len;
		readLen = eventReadyBlock->block[blockId].len;
		/* readed data must divide by 4 w/o reminder */
		if (readLen % 4)
			readLen += 4-(readLen % 4);

		if(eventReadyBlock->block[blockId].len > 0) {
			assert(readLen <= MAX_H264_FRAMESIZE);
			if(!mg1264_mbr(crusher, eventReadyBlock->block[blockId].addr,
							readLen, crusher->out_data[blockId].data, 0x40)) {
				ERROR("Can't read %d bytes of H.264 data", eventReadyBlock->block[blockId].len);
				return -1;
			}
		}
	}
	XTREME("readframes: ended");
	crusher->event_complete = 1;

	return 1;
}

#if 0
int mg1264_finish(crusher_t *crusher)
{
//	event_t *event = (event_t *) crusher->lastEvent;
//	event_bs_block_ready_t *eventReadyBlock = (event_bs_block_ready_t *) &event->payload[0];
//	uint32_t readLen = 0;
	uint32_t buff_be[11];
	uint32_t buf[11];
	uint32_t ptr = 0x00;
	int i;

	DEBUG("FINISH!");
	/*read status0:
	 * addr:0x0ffc len: 0x04 (40)
	 * */
	mg1264_mbr (crusher, 0x0ffc, 1*4, (uint8_t *) buff_be, 0x40);
	dump_bytes((char *) buff, 1*4);
	ptr = be2me_32(buff[0]);

	/*read status1:
	 * addr: 0x400c5180 len: 0x1c (40)
	 * TODO: what's this? */
	mg1264_mbr (crusher, ptr, 7*4, (uint8_t *) buff_be, 0x40);
	dump_bytes((char *) buff, 7*4);
	ptr = be2me_32(buff[4]);


	/*read status2:
	 * addr: 0x400c5300 len: 0x2c (40) */
	mg1264_mbr (crusher, ptr, 11*4, (uint8_t *) buff_be, 0x40);
	dump_bytes((char *) buff, 11*4);

	/*event:
	 * 0000   00 00 00 03 00 03 00 01 00 00 00 00 00 02 00 06
	 * 0010   00 ce 5d 04 00 00 33 0c 00 ce 90 10 00 00 30 fa
	 * 0020   00 ce c1 0c 00 00 01 1a 00 ce c2 28 00 00 00 36
	 * 0030   00 ce c2 60 00 00 05 52 00 ce c7 b4 00 00 00 1d */

	//read:   addr: 0xce5d04 len: 0x330c

	/*event:
	 * 0000   00 00 00 03 00 03 00 01 00 00 00 00 00 02 00 01
	 * 0010   00 ce c7 d4 00 00 00 1d 00 ce 90 10 00 00 30 fa
	 * 0020   00 ce c1 0c 00 00 01 1a 00 ce c2 28 00 00 00 36
	 * 0030   00 ce c2 60 00 00 05 52 00 ce c7 b4 00 00 00 1d*/

	/*event:
	 * 0000   00 00 00 03 00 03 00 01 00 00 00 00 00 02 00 01
	 * 0010   00 ce c7 f4 00 00 00 18 00 ce 90 10 00 00 30 fa
	 * 0020   00 ce c1 0c 00 00 01 1a 00 ce c2 28 00 00 00 36
	 * 0030   00 ce c2 60 00 00 05 52 00 ce c7 b4 00 00 00 1d*/

	/* read:
	 * addr: 0xcec10c len: 0x0700 (40) */
	//mg1264_mbr (crusher, addr, readLen, crusher->out_payload, 0x40);
	return 1;
}
#else
int mg1264_finish(crusher_t *crusher)
{
	uint32_t buff_be[46];
	uint32_t b[46];
	uint32_t ptr = 0x00;
	int i;

	XTREME("FINISH!");
	/*read status0:
	 * addr:0x0ffc len: 0x04 (40)
	 * */
	if(!mg1264_mbr (crusher, 0x0ffc, 1*4, (uint8_t *) buff_be, 0x40))
		return 0;
	ptr = be2me_32(buff_be[0]);

	/*read status1:
	 * addr: 0x400c5180 len: 0x1c (40)
	 * TODO: what's this? */
	if(!mg1264_mbr (crusher, ptr, 7*4, (uint8_t *) buff_be, 0x40))
		return 0;
	dump_bytes((char *) buff_be, 7*4);
	ptr = be2me_32(buff_be[4]);


	/*read status2:
	 * addr: 0x400c5300 len: 0x2c (40) */
	if(!mg1264_mbr (crusher, ptr, 46*4, (uint8_t *) buff_be, 0x40))
		return 0;

	for(i=0; i<46; i++)
		b[i] = be2me_32(buff_be[i]);


	DEBUG("ME Status");
	DEBUG("Aud: In %d Out %d Dec %d Enc %d", b[0], b[1], b[2], b[3]);
	DEBUG("Vid: In %d Out %d Dec %d Enc %d", b[4], b[5], b[6], b[7]);
	DEBUG("ISR: vin %d ain %d vout %d aout %d", b[8], b[9], b[10], b[11]);
	DEBUG("Enc PTS: vin %ld ain %ld venc %ld aenc %ld vmux %ld amux %ld",
		(long ) b[12], (long ) b[14], (long ) b[16],
		(long ) b[18], (long ) b[20], (long ) b[22]);
	DEBUG("Dec PTS: vdmx %ld admx %ld vdec %ld adec %ld vout %ld aout %ld",
		(long ) b[24], (long ) b[26], (long ) b[28],
		(long ) b[30], (long ) b[32], (long ) b[34]);
	DEBUG("AV Sync: vin drops %d vin repeats %d vout drops %d vout repeats %d",
		b[36], b[37], b[38], b[39]);
	DEBUG("VFR: frames sent %d/%d", b[40], b[41]);
	return 1;
}
#endif
