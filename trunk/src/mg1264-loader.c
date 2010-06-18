/*
 * mg1264-loader.c
 *
 *  Created on: 12 окт. 2009
 *      Author: tipok
 */

#include <usb.h>
#include <stdio.h>
#include <string.h>


#include "crusher.h"
#include "crusher-usb.h"
#include "mg1264.h"
#include "mg1264-io.h"
#include "mg1264-mware.h"
#include "bswap.h"
#include "logging.h"

enum {
	MGFW_LOAD_SECTION = 1,
	MGFW_CSR_SECTION = 6,
	MGFW_BSS_SECTION = 3, /* FIXME: check it!*/
	MGFW_GPB_SECTION = 4,
	MGFW_MWCONFIG_SECTION = 5,
	MGFW_END_SECTION = 0x0a
};

static char magicword[] = "MBY0";

/*
 * Checking firmware file for magicword existance
 */
static int check_mg1264_magicword(FILE *fwFile) {
    uint8_t	buffer[4];

    if(fread (buffer,1,4,fwFile) != 4 || memcmp(buffer, magicword, 4) != 0) {
		ERROR("file looks like not firmware for MG1264.");
		return 0;
    }
	return 1;
}

int mg1264_load_firmware(crusher_t *crusher, FILE *mg1264Fw)
{
	int i;
	uint32_t version;
	uint32_t numRegisters;
	uint32_t sectionId;
	uint32_t status;

	/* dram struct */
	uint32_t dramSize;
	uint32_t dramAddr;
	uint32_t dramPartition;

	/* csr struct */
	uint8_t csrSize;
	uint8_t csrBlock;
	uint32_t csrAddr;
	uint32_t csrData;

	uint8_t  *temp = NULL;
	uint8_t  *temp1 = NULL;
	uint32_t  tempregs[4];
	uint32_t  *int_data = NULL;

	// read the first 4 bytes and check against the magic number and
	// fail if they do not match
	if(!check_mg1264_magicword(mg1264Fw))
		return 0;

	if(fread (tempregs, sizeof(uint32_t),1,mg1264Fw) != 1)
		    	return 0;
	version = be2me_32(tempregs[0]);
	DEBUG("firmware version: %u.%u.%u.%u",
	        (version >> 24) & 0x0ff, (version >> 16) & 0x0ff, (version >> 8) & 0x0ff, version & 0x0ff);
	// Continue in a loop processing each section as it is found.
	// In order to handle corrupted images, the loop exits as
	// soon as the current firmware image pointer goes past the
	// size of the firmware image.
	while ( fread (tempregs, sizeof(uint32_t), 1, mg1264Fw) == 1 )
	{
		sectionId = be2me_32(tempregs[0]);

		XTREME("section %u:", sectionId);
	    switch (sectionId)
	    {
	        case MGFW_LOAD_SECTION:
	            // read the size, address, and partition of the firmware
	            // data to be downloaded.
	        	if(fread (tempregs, sizeof(uint32_t), 3, mg1264Fw) != 3)
	        		return 0;

	        	dramSize = be2me_32(tempregs[0]);
	        	dramAddr = be2me_32(tempregs[1]);
	        	dramPartition = be2me_32(tempregs[2]);

	        	XTREME("LOAD_SECTION addr: 0x%08x, size: %d, partition: 0x%x",
	        			dramAddr, dramSize, dramPartition);
	        	temp = malloc(dramSize);
	        	if(!temp)
	        		return 0;
	        	if(fread (temp, sizeof(uint8_t), dramSize, mg1264Fw) != dramSize) {
	        		free(temp);
	        		return 0;
	        	}

	        	if(!mg1264_mbw(crusher, dramAddr, dramSize, temp, dramPartition)){
	        		free(temp);
	        		return 0;
	        	}

	        	free(temp);
	            break;
	        case MGFW_CSR_SECTION:
	            // get number of registers to write
	        	if(fread (tempregs, sizeof(uint32_t), 1, mg1264Fw) != 1)
	        		return 0;
	        	numRegisters = be2me_32(tempregs[0]);
	        	XTREME("CSR_SECTION numRegisters: %d", numRegisters);

	            // iterate across the set of registers, writing each one as they
	            // are read.
	            for (i = 0; i < numRegisters; i++)
	            {
	            	if(fread (tempregs, sizeof(uint32_t), 4, mg1264Fw) != 4)
	            		 return 0;

	            	csrAddr = be2me_32(tempregs[1]);
	            	csrData =  be2me_32(tempregs[2]);
	            	tempregs[0] = be2me_32(tempregs[0]);
	            	int to_read = be2me_32(tempregs[3]);
	            	csrSize = tempregs[0] >> 16;
	            	csrBlock = tempregs[0] >> 8;


	            	if (!to_read) {
	            	    // write the register
	                    XTREME("CSR write addr: 0x%08x, size: %d, block: 0x%x, value: 0x%08x",
	                            csrAddr, csrSize, csrBlock, csrData);
	            	    if(!mg1264_csrw(crusher, csrAddr, csrData, csrBlock, csrSize))
	            	        return 0;
	            	} else {
                        XTREME("CSR read addr: 0x%08x, size: %d, block: 0x%x, query:  0x%08x",
                                                    csrAddr, csrSize, csrBlock, to_read);
                        if(!mg1264_csrr(crusher, csrAddr, &csrData, csrBlock, csrSize))
                            return 0;
                        XTREME("CSR read addr: value: 0x%08x", csrData);
	            	}


	            	/* !!! recheck csr:
	            	 *  addr: 0x0014, value: 0x01dd6164 blockid:0x08, size:0x04
	            	 *  addr: 0x0060, value: 0x00000005 blockid:0x11, size:0x01
	            	 *   */
	            }
	            break;
	        case MGFW_BSS_SECTION:
	            // read the size, address and partition of the bss section
	        	if(fread (tempregs, sizeof(uint32_t), 3, mg1264Fw) != 3)
	        		return 0;

	        	dramSize = be2me_32(tempregs[0]);
	        	dramAddr = be2me_32(tempregs[1]);
	        	dramPartition = be2me_32(tempregs[2]);

	        	XTREME("BSS_SECTION addr: 0x%08x, size: %d, partition: 0x%x",
	        			dramAddr, dramSize, dramPartition);

	        	temp = malloc(dramSize);
	        	if(!temp)
	        		return 0;
	        	memset(temp, 0x00, dramSize);
	        	if(!mg1264_mbw(crusher, dramAddr, dramSize, temp, dramPartition)){
	        		free(temp);
	        		return 0;
	        	}
	        	free(temp);
	            break;
	        case MGFW_GPB_SECTION:
	            // retrieve the GPB address for this image
	        	if(fread (tempregs, sizeof(uint32_t), 1, mg1264Fw) != 1)
	        		return 0;
	        	crusher->gpb = be2me_32(tempregs[0]);
	        	XTREME("GPB_SECTION gpb: 0x%08x", crusher->gpb);
	            break;
	        case MGFW_END_SECTION:
	            // Flag that the end section has been found
	        	XTREME("END_SECTION eof");
	        	return 1; /* maybe don't needed */
	            break;
	        case MGFW_MWCONFIG_SECTION:
	        	// read the size, address and partition of the vgaconfig section
	        	if(fread (tempregs, sizeof(uint32_t), 3, mg1264Fw) != 3)
	        		return 0;
	        	dramSize = be2me_32(tempregs[0]);
	        	dramAddr = be2me_32(tempregs[1]);
	        	dramPartition = be2me_32(tempregs[2]);

	        	XTREME("MGFW_MWCONFIG_SECTION preparing addr: 0x%08x, size: %d, partition: 0x%x",
	        		dramAddr, dramSize, dramPartition);

	        	if (dramSize < 16 * sizeof(uint32_t)) {
	        		XTREME("too small or corrupted partition: size: %d, must be at least %d",
	        	   		dramSize, 16 * sizeof(uint32_t));
	        	}

	        	temp = malloc(dramSize);
	        	if(!temp)
	        		return 0;
	        	if(!mg1264_mbr(crusher, dramAddr, dramSize, temp, dramPartition)){
	        		free(temp);
	        		return 0;
	        	}
	        	dump_bytes((char *) temp, dramSize);

	        	numRegisters = dramSize/sizeof(uint32_t);
	        	int_data = (uint32_t *) temp;

	        	for (i=0; i < numRegisters; i+=4){
	        		XTREME("%u, %u, %u, %u,",
	        				int_data[i],
	        				(i+1)<numRegisters?int_data[i+1]:-1,
	        				(i+2)<numRegisters?int_data[i+2]:-1,
	        				(i+3)<numRegisters?int_data[i+3]:-1);
	        	}

	        	uint8_t reg_partition = 0x40;
	        	switch (crusher->devtype) {
	        	    case DEV_TYPE_CAPTURE:
	                    int_data[0] = me2be_32(0x041e);      ///< sysclk1/100000;
	                    int_data[11] = me2be_32(0x0649534e); ///< sysclk1
	                    int_data[16] = me2be_32(0xffffffff); ///< reserved for app.
	                    break;
	        	    case DEV_TYPE_ENCODER:
	                    int_data[0] = me2be_32(0x0444);      ///< sysclk1/100000;
	                    int_data[8] = me2be_32(3);           ///< debug
	                    int_data[11] = me2be_32(0xd0058306); ///< sysclk1
	                    int_data[16] = me2be_32(0xffffffff); ///< reserved for app.
                        break;
                    case DEV_TYPE_ENCODER_HD:
                        int_data[0] = me2be_32(0x0000032a);      ///< sysclk1/100000;
                        int_data[4] = me2be_32(0x00000000);
                        int_data[11] = me2be_32(0x202fbf00);
                        int_data[12] = me2be_32(0x0aba9500);
                        int_data[13] = me2be_32(0x02dc6c00);
                        int_data[14] = me2be_32(0x00000000);
                        reg_partition = 0x7d;
                        break;
                    default:
                        ERROR("unknown device type in MGFW_MWCONFIG_SECTION %d",crusher->devtype);
	        	}

	        	if(!mg1264_mbw(crusher, dramAddr, dramSize, temp, dramPartition)){
	        		free(temp);
	        		return 0;
	        	}
	        	dump_bytes((char *) temp, dramSize);
	        	for (i=0; i < numRegisters; i+=4){
	        		XTREME("%u, %u, %u, %u,",
	        				int_data[i],
	        				(i+1)<numRegisters?int_data[i+1]:-1,
	        				(i+2)<numRegisters?int_data[i+2]:-1,
	        				(i+3)<numRegisters?int_data[i+3]:-1);
	        	}

	        	temp1 = malloc(dramSize);
	        	if(!temp1)
	        		return 0;
	        	if(!mg1264_mbr(crusher, dramAddr, dramSize, temp1, dramPartition)){
	        		free(temp);
	        		free(temp1);
	        		return 0;
	        	}
	        	dump_bytes((char *) temp1, dramSize);

	        	if( memcmp(temp, temp1, dramSize) ){
	        		ERROR("can't prepare vgaconfig, memory blocks mismatch!");
	        		free(temp);
	        		free(temp1);
	        		return 0;
	        	}

	        	free(temp);
	        	free(temp1);

	        	tempregs[0] = 0;
	        	tempregs[1] = 0;
	        	tempregs[2] = 0xffffffff;

	        	if( !mg1264_mbw(crusher, 0x00000ffc, 0x04, (uint8_t *) tempregs, reg_partition) )
	        		return 0;
	        	if( !mg1264_mbw(crusher, 0x00000ff8, 0x04, (uint8_t *) tempregs+1, reg_partition) )
	        		return 0;

	        	if (crusher->devtype != DEV_TYPE_ENCODER_HD) {
                    if( !mg1264_mbw(crusher, crusher->gpb + 0x18 /*0xc5198*/,
                                                         0x04, (uint8_t *) tempregs+2, reg_partition) )
                        return 0;

                    int_data = calloc(MWARE_SIZE, sizeof(uint32_t));
                    if(!int_data)
                        return 0;

                    for (i=0; i < MWARE_SIZE; i+=4){
                        XTREME("%u, %u, %u, %u,",
                                crusher->mware_data[i],
                                (i+1)<MWARE_SIZE?crusher->mware_data[i+1]:-1,
                                (i+2)<MWARE_SIZE?crusher->mware_data[i+2]:-1,
                                (i+3)<MWARE_SIZE?crusher->mware_data[i+3]:-1);
                    }


                    /* convert vgaconfig to big-endian */
                    for (i=0; i<MWARE_SIZE; i++)
                        int_data[i] = me2be_32(crusher->mware_data[i]);

                    if( !mg1264_mbw(crusher, 0x00000000, MWARE_SIZE*sizeof(uint32_t),
                                                                        (uint8_t *) int_data, 0x4F) ){
                        free(int_data);
                        return 0;
                    }

                    free(int_data);
                }

	        	break;

	        default :
	        	ERROR("Unknown section %d", sectionId);
	        	break;
	      }

	      if (crusher->devtype != DEV_TYPE_ENCODER_HD) {
            mg1264_csrr(crusher, 0x00C2, &status, 0x04, 0x02);
            if (sectionId != MGFW_GPB_SECTION && status != 0x40) {
                XTREME("status=%u (must be 0x40)", status);
                return 0;
            }
	      }
	  }

	  return 1;
}
