/*
 * mg1264-mware.c
 *
 *  Created on: 19 марта 2010
 *      Author: tipok
 */

#include <stdlib.h>

#include "crusher.h"
#include "mg1264-mware.h"
#include "logging.h"
#include "bswap.h"


/* contents of qvgaconfig_16mb.bin
    2, 0, 0, 84,
    1, 0, 6144, 512,
    1, 257, 64, 64,
    6, 1, 2, 32,
    1, 1, 257, 256,
    64, 23 *23=400x300,320x240,11=640x480,7=720x576*, 1, 320,
    240, 320, 240, 8192,
    8192, 0, 0, 0,
    0 *0=320x240,400x300,640x480,1=720x576*, 320, 240, 1,
    1, 44, 0, 0,
    1, 0, 320, 240,
    8, 9, 0, 0,
    1, 1, 1, 521,
    0, 45, 228, 17,
    320, 260 *h+20 (300x400=324??)*, 0, 0,
    0, 0, 45, 144,
    8, 0, 16, 16,
    8, 16, 16, 2,
    0, 7, 7, 16,
    0, 0, 0, 1,
    0, 7, 3, 16,
    0, 1, 0, 1
*/

/* prodcfg17.bin:
     2, 0, 0, 84,
     0, 0, 6144, 1024,
     64, 0x00232801, 64, 64,
     6, 8, 2, 32,
     1024, 64, 0x00232801, 256,
     64, 8, 8, 720,
     576, 720, 576, 8192,
     8192, 0, 0, 0,
     0, 720, 480, 1,
     1, 21, 0, 0,
     0, 0, 720, 480,
     8, 9, 0, 0,
     0, 0, 0, 521,
     0, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 45, 144,
     8, 2, 0, 0,
     8, 160, 120, 8,
     0, 7, 3, 16,
     0, 0, 0, 1,
     0, 7, 3, 16,
     0, 1, 0, 0
*/


/* MG1264 Capture Codec initialization for half duplex operation in 16MB */
static const uint32_t capture_init_mware[MWARE_SIZE] =
{
    2, 0, 0, MWARE_NUM_PARAMS,            // Header info (version,0,0,size)
    0,                      // MWARE_FULL_DUPLEX
    0,                      // MWARE_MEM_POOL_START
    6144,                   // MWARE_MEM_POOL_SIZE
    1024,                   // MWARE_MUXER_VIDEO_BUFFER_SIZE

    64,                     // MWARE_MUXER_AUDIO_BUFFER_SIZE
    /*(0<<8)|1*/0x00232801, // MWARE_MUXER_DATA_BUFFER_INFO
    64,                     // MWARE_MUXER_MAX_VIDEO_AU
    64,                     // MWARE_MUXER_MAX_AUDIO_AU

    6,                      // MWARE_INPUT_VIDEO_FRAMES
    8,                      // MWARE_INPUT_AUDIO_FRAMES
    2,                      // MWARE_ENCODE_VIDEO_FRAMES
    32,                     // MWARE_VENC_INSTRUCTION_BUFFER

    1024,                   // MWARE_DEMUX_VIDEO_BUFFER_SIZE
    64,                     // MWARE_DEMUX_AUDIO_BUFFER_SIZE
    /*(0<<8)|1*/0x00232801, // MWARE_DEMUX_DATA_BUFFER_INFO
    256,                    // MWARE_DEMUX_MAX_VIDEO_AU

    64,                     // MWARE_DEMUX_MAX_AUDIO_AU
    8,                      // MWARE_DECODE_VIDEO_FRAMES
    8,                      // MWARE_DECODE_AUDIO_FRAMES
    720,                    // MWARE_MAX_VIDEO_ENCODE_WIDTH

    576,                    // MWARE_MAX_VIDEO_ENCODE_HEIGHT
    720,                    // MWARE_MAX_VIDEO_DECODE_WIDTH
    576,                    // MWARE_MAX_VIDEO_DECODE_HEIGHT
    8192,                   // MWARE_MAX_AUDIO_FRAME_SIZE

    8192,                   // MWARE_MAX_AUDIO_AU_SIZE
    0,                      // MWARE_VIDEO_VPP_EXTERNAL_SYNC
    0,                      // MWARE_VIDEO_VPU_EXTERNAL_SYNC
    0,                      // MWARE_VIDEO_BIDIRECTIONAL_PORT

    0,                      // MWARE_VIDEO_PAL
    720,                    // MWARE_VIDEO_VPP_INPUT_WIDTH
    480,                    // MWARE_VIDEO_VPP_INPUT_HEIGHT
    1,                      // MWARE_VIDEO_VPP_PIXEL_AR_X

    1,                      // MWARE_VIDEO_VPP_PIXEL_AR_Y
    21,                     // MWARE_VIDEO_VPP_START_VLINE
    0,                      // MWARE_VIDEO_VPP_START_HPIXEL
    0,                      // MWARE_VIDEO_VPP_CHROMA_OFFSETYC

    0,                      // MWARE_VIDEO_VPP_FREE_RUN
    0,                      // MWARE_VIDEO_VPU_SYNC
    720,                    // MWARE_VIDEO_VPU_OUTPUT_WIDTH
    480,                    // MWARE_VIDEO_VPU_OUTPUT_HEIGHT

    8,                      // MWARE_VIDEO_VPU_PIXEL_AR_X
    9,                      // MWARE_VIDEO_VPU_PIXEL_AR_Y
    0,                      // MWARE_VIDEO_VPU_OUTPUT_OFFSET_X
    0,                      // MWARE_VIDEO_VPU_OUTPUT_OFFSET_Y

    0,                      // MWARE_VIDEO_VPP_PROGRESSIVE_IF
    0,                      // MWARE_VIDEO_VPP_PROGRESSIVE_SOURCE
    0,                      // MWARE_VIDEO_VPU_PROGRESSIVE_IF
    0x209,                  // MWARE_VIDEO_VPU_PHYSICAL_OFFSET_VERT

    0,                      // MWARE_VIDEO_VPU_PHYSICAL_OFFSET_HORZ
    0,                      // MWARE_VIDEO_VPU_VBLANK_LINES,
    0,                      // MWARE_VIDEO_VPU_HBLANK_CLOCKS,
    0,                      // MWARE_VIDEO_VPU_VACT_START,

    0,                      // MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_WIDTH,
    0,                      // MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_HEIGHT,
    0,                      // MWARE_VIDEO_VPP_START_VLINE_2NDFIELD
    0,                      // RESERVED,

    0,                      // RESERVED,
    0,                      // RESERVED,
    45,                     // MWARE_VIS_OSD_MAX_WIDTH
    144,                    // MWARE_VIS_OSD_MAX_HEIGHT,

    8,                      // MWARE_VIS_OSD_BIT_DEPTH
    2,                      // MWARE_APP_OSD_SCREENS
    0,                      // MWARE_APP_OSD_MAX_WIDTH
    0,                      // MWARE_APP_OSD_MAX_HEIGHT

    8,                      // MWARE_APP_OSD_BIT_DEPTH
    160,                    // MWARE_APP_OSD_SCRN_WIDTH
    120,                    // MWARE_APP_OSD_SCRN_HEIGHT
    8,                      // MWARE_APP_RES_N_OSD_FRAMES,

    AUD_SERIAL_MODE_I2S,    // MWARE_AUDIO_AII_SERIALMODE
    7,                      // MWARE_AUDIO_AII_DRIVELRCK_SCKPHASE
    3/*7*/,                 // MWARE_AUDIO_AII_SAMPLE_SCKPHASE
    AUD_SAMPLE_SIZE_16,     // MWARE_AUDIO_AII_SAMPLESIZE

    0,                      // MWARE_AUDIO_AII_SCK_SLAVE
    0,                      // MWARE_AUDIO_AII_LRCK_SLAVE
    AUD_MASTER_CLOCK_256FS, // MWARE_AUDIO_AII_MCLKTOFS_RATIO
    AUD_BUS_CLOCK_64FS,     // MWARE_AUDIO_AII_SCKTOLRCK_RATIO

    AUD_SERIAL_MODE_I2S,    // MWARE_AUDIO_AOI_SERIALMODE
    7,                      // MWARE_AUDIO_AOI_DRIVE_SCKPHASE
    3,                      // MWARE_AUDIO_AOI_SAMPLELRCK_SCKPHASE
    AUD_SAMPLE_SIZE_16,     // MWARE_AUDIO_AOI_SAMPLESIZE

    AUD_MASTER_CLOCK_256FS, // MWARE_AUDIO_AOI_MCLKTOFS_RATIO
    AUD_BUS_CLOCK_64FS,     // MWARE_AUDIO_AOI_SCKTOLRCK_RATIO
    0,                      // RESERVED
    0,                      // RESERVED
};


/* MG1264 Encoder Codec initialization for half duplex operation in 16MB */
static const uint32_t encoder_init_mware[MWARE_SIZE] =
{
/*    2, 0, 0, 84,
    1, 0, 6144, 512,
    1, 257, 64, 64,
    6, 1, 2, 32,
    1, 1, 257, 256,
    64, 23 *23=400x300,320x240,11=640x480,7=720x576*, 1, 320,
    240, 320, 240, 8192,
    8192, 0, 0, 0,
*/
    2, 0, 0, MWARE_NUM_PARAMS,            // Header info (version,0,0,size)
    1,                      // MWARE_FULL_DUPLEX
    0,                      // MWARE_MEM_POOL_START
    6144,                   // MWARE_MEM_POOL_SIZE
    512,                    // MWARE_MUXER_VIDEO_BUFFER_SIZE

    1,                      // MWARE_MUXER_AUDIO_BUFFER_SIZE
    /*(0<<8)|1*/257, // MWARE_MUXER_DATA_BUFFER_INFO
    64,                     // MWARE_MUXER_MAX_VIDEO_AU
    64,                     // MWARE_MUXER_MAX_AUDIO_AU

    6,                      // MWARE_INPUT_VIDEO_FRAMES
    1,                      // MWARE_INPUT_AUDIO_FRAMES
    2,                      // MWARE_ENCODE_VIDEO_FRAMES
    32,                     // MWARE_VENC_INSTRUCTION_BUFFER

    1,                      // MWARE_DEMUX_VIDEO_BUFFER_SIZE
    1,                      // MWARE_DEMUX_AUDIO_BUFFER_SIZE
    /*(0<<8)|1*/257,        // MWARE_DEMUX_DATA_BUFFER_INFO
    256,                    // MWARE_DEMUX_MAX_VIDEO_AU

    64,                     // MWARE_DEMUX_MAX_AUDIO_AU
    23/*23=400x300,320x240,11=640x480,7=720x576*/, // MWARE_DECODE_VIDEO_FRAMES
    1,                      // MWARE_DECODE_AUDIO_FRAMES
    320,                    // MWARE_MAX_VIDEO_ENCODE_WIDTH

    240,                    // MWARE_MAX_VIDEO_ENCODE_HEIGHT
    320,                    // MWARE_MAX_VIDEO_DECODE_WIDTH
    240,                    // MWARE_MAX_VIDEO_DECODE_HEIGHT
    8192,                   // MWARE_MAX_AUDIO_FRAME_SIZE

    8192,                   // MWARE_MAX_AUDIO_AU_SIZE
    0,                      // MWARE_VIDEO_VPP_EXTERNAL_SYNC
    0,                      // MWARE_VIDEO_VPU_EXTERNAL_SYNC
    0,                      // MWARE_VIDEO_BIDIRECTIONAL_PORT
/*    0 *0=320x240,400x300,640x480,1=720x576*, 320, 240, 1,
    1, 44, 0, 0,
    1, 0, 320, 240,
    8, 9, 0, 0,
    1, 1, 1, 521,
 * */
    0, /*0=320x240,400x300,640x480,1=720x576*/  // MWARE_VIDEO_PAL
    320,                    // MWARE_VIDEO_VPP_INPUT_WIDTH
    240,                    // MWARE_VIDEO_VPP_INPUT_HEIGHT
    1,                      // MWARE_VIDEO_VPP_PIXEL_AR_X

    1,                      // MWARE_VIDEO_VPP_PIXEL_AR_Y
    44,                     // MWARE_VIDEO_VPP_START_VLINE
    0,                      // MWARE_VIDEO_VPP_START_HPIXEL
    0,                      // MWARE_VIDEO_VPP_CHROMA_OFFSETYC

    1,                      // MWARE_VIDEO_VPP_FREE_RUN
    0,                      // MWARE_VIDEO_VPU_SYNC
    320,                    // MWARE_VIDEO_VPU_OUTPUT_WIDTH
    240,                    // MWARE_VIDEO_VPU_OUTPUT_HEIGHT

    8,                      // MWARE_VIDEO_VPU_PIXEL_AR_X
    9,                      // MWARE_VIDEO_VPU_PIXEL_AR_Y
    0,                      // MWARE_VIDEO_VPU_OUTPUT_OFFSET_X
    0,                      // MWARE_VIDEO_VPU_OUTPUT_OFFSET_Y

    1,                      // MWARE_VIDEO_VPP_PROGRESSIVE_IF
    1,                      // MWARE_VIDEO_VPP_PROGRESSIVE_SOURCE
    1,                      // MWARE_VIDEO_VPU_PROGRESSIVE_IF
    0x209,                  // MWARE_VIDEO_VPU_PHYSICAL_OFFSET_VERT
/*
    0, 45, 228, 17,
    320, 260 *h+20 (300x400=324??)*, 0, 0,
    0, 0, 45, 144,
    8, 0, 16, 16,
    8, 16, 16, 2,
    0, 7, 7, 16,
    0, 0, 0, 1,
    0, 7, 3, 16,
    0, 1, 0, 1
*/
    0,                      // MWARE_VIDEO_VPU_PHYSICAL_OFFSET_HORZ
    45,                      // MWARE_VIDEO_VPU_VBLANK_LINES,
    228,                      // MWARE_VIDEO_VPU_HBLANK_CLOCKS,
    17,                      // MWARE_VIDEO_VPU_VACT_START,

    320,                      // MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_WIDTH,
    260, /*h+20 (300x400=324??)*/                     // MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_HEIGHT,
    0,                      // MWARE_VIDEO_VPP_START_VLINE_2NDFIELD
    0,                      // RESERVED,

    0,                      // RESERVED,
    0,                      // RESERVED,
    45,                     // MWARE_VIS_OSD_MAX_WIDTH
    144,                    // MWARE_VIS_OSD_MAX_HEIGHT,

    8,                      // MWARE_VIS_OSD_BIT_DEPTH
    0,                      // MWARE_APP_OSD_SCREENS
    16,                      // MWARE_APP_OSD_MAX_WIDTH
    16,                      // MWARE_APP_OSD_MAX_HEIGHT

    8,                      // MWARE_APP_OSD_BIT_DEPTH
    16,                    // MWARE_APP_OSD_SCRN_WIDTH
    16,                    // MWARE_APP_OSD_SCRN_HEIGHT
    2,                      // MWARE_APP_RES_N_OSD_FRAMES,

    AUD_SERIAL_MODE_I2S,    // MWARE_AUDIO_AII_SERIALMODE
    7,                      // MWARE_AUDIO_AII_DRIVELRCK_SCKPHASE
    7,                      // MWARE_AUDIO_AII_SAMPLE_SCKPHASE
    AUD_SAMPLE_SIZE_16,     // MWARE_AUDIO_AII_SAMPLESIZE

    0,                      // MWARE_AUDIO_AII_SCK_SLAVE
    0,                      // MWARE_AUDIO_AII_LRCK_SLAVE
    AUD_MASTER_CLOCK_256FS, // MWARE_AUDIO_AII_MCLKTOFS_RATIO
    AUD_BUS_CLOCK_64FS,     // MWARE_AUDIO_AII_SCKTOLRCK_RATIO

    AUD_SERIAL_MODE_I2S,    // MWARE_AUDIO_AOI_SERIALMODE
    7,                      // MWARE_AUDIO_AOI_DRIVE_SCKPHASE
    3,                      // MWARE_AUDIO_AOI_SAMPLELRCK_SCKPHASE
    AUD_SAMPLE_SIZE_16,     // MWARE_AUDIO_AOI_SAMPLESIZE

    AUD_MASTER_CLOCK_256FS, // MWARE_AUDIO_AOI_MCLKTOFS_RATIO
    AUD_BUS_CLOCK_64FS,     // MWARE_AUDIO_AOI_SCKTOLRCK_RATIO
    0,                      // RESERVED
    1,                      // RESERVED
};





int mware_tune_config(crusher_t *crusher){
    FILE    *mware_h = NULL;
    uint32_t *temp32;
    long    fsize;
    int i;



    if(crusher->mwarecfg) {
        mware_h = fopen(crusher->mwarecfg, "r");
        if(!mware_h) {
            ERROR("Open file %s failed.", crusher->mwarecfg);
            return 0;
        }

        // obtain file size:
        fseek (mware_h, 0, SEEK_END);
        fsize = ftell (mware_h);
        rewind (mware_h);

        if(fsize != MWARE_SIZE*sizeof(uint32_t)) {
            ERROR("Strange size of %s file, must be %d bytes.",
                    crusher->mwarecfg, MWARE_SIZE*sizeof(uint32_t));
            fclose(mware_h);
            return 0;
        }

        temp32 = calloc(MWARE_SIZE, sizeof(uint32_t));
        if(!temp32){
            fclose(mware_h);
            return 0;
        }

        if(fread (temp32, sizeof(uint32_t), MWARE_SIZE, mware_h) != MWARE_SIZE) {
            ERROR("Can't read %d byts from file %s.", fsize, crusher->mwarecfg);

            fclose(mware_h);
            return 0;
        }
        fclose(mware_h);

        for (i=0; i<MWARE_SIZE; i++)
                crusher->mware_data[i] = le2me_32(temp32[i]);
    } else {

        if (crusher->devmode == DEV_TYPE_ENCODER) {
            /* use default mware */
            if (crusher->width != 320 || crusher->height != 240) {
/* mware values:
qvgaconfig_16mb.bin         vgaconfig_16mb.bin      captured_d1.bin     arbiconfig_16mb.bin     qvgaconfig.bin
0:  2, 0, 0, 84,
4:  1, 0, 6144, 2048                                *x, x, x, 512*      *x, x, x, 512*          *x, x, x, 512*
8:  1, 76801, 64, 64,       *x, 257, x, x*          *x, 257, x, x*      *x, 257, x, x*
12: 6, 1, 2, 32,
16: 1, 1, 257, 256,
20: 64, 23, 1, 320,         *x, 9, x, 640*          *x, 7, x, 720*      *x, 7, x, 512*
24: 240, 320, 240, 8192,    *480, 640, 480, x*      *576, 720, 576, x*  *240, 512, 240, x*
28: 8192, 0, 0, 0,
32: 0, 320, 240, 1,         *x, 640, 480, x*        *1, 720, 576, x,*   *x, 512, 240, x,*
36: 1, 44, 0, 0,            *x, 21, x, x*
40: 1, 0, 320, 240,         *0, x, 640, 480*        *x, x, 720, 576*    *x, x, 512, 240*
44: 8, 9, 0, 0,
48: 1, 1, 1, 521,           *0, 0, 0, x*
52: 0, 45, 144, 17,         *x, 0, 0, 0*            *x, x, 228, x*      *x, x, 228, x*
56: 320, 260, 0, 0,         *0, 0, x, x*            *720, 596, x, x*    *512, 260, x, x*
60: 0, 0, 45, 144,
64: 8, 0, 16, 16,           *x, 2, x, x*
68: 8, 16, 16, 2,
72: 0, 7, 7, 16,
76: 0, 0, 0, 1,
80: 0, 7, 3, 16,
84: 0, 1, 0, 1
*/
                /* tune mware
                 * TODO: what this values means?
                 * */
                /* 7 for 720x576, 11 for 640x480, 23 for lower */
                if((crusher->width > 400) && (crusher->width <= 640))
                    crusher->mware_data[MWARE_DECODE_VIDEO_FRAMES+4] = 11;
                else if((crusher->width > 640))
                    crusher->mware_data[MWARE_DECODE_VIDEO_FRAMES+4] = 7;

                crusher->mware_data[MWARE_MAX_VIDEO_ENCODE_WIDTH+4] = crusher->width;
                crusher->mware_data[MWARE_MAX_VIDEO_ENCODE_HEIGHT+4] = crusher->height;
                crusher->mware_data[MWARE_MAX_VIDEO_DECODE_WIDTH+4] = crusher->width;
                crusher->mware_data[MWARE_MAX_VIDEO_DECODE_HEIGHT+4] = crusher->height;

                crusher->mware_data[MWARE_VIDEO_PAL+4] = (crusher->width > 640) ? 1 : 0; /* ! */
                crusher->mware_data[MWARE_VIDEO_VPP_INPUT_WIDTH+4] = crusher->width;
                crusher->mware_data[MWARE_VIDEO_VPP_INPUT_HEIGHT+4] = crusher->height;

                crusher->mware_data[MWARE_VIDEO_VPU_OUTPUT_WIDTH+4] = crusher->width;
                crusher->mware_data[MWARE_VIDEO_VPU_OUTPUT_HEIGHT+4] = crusher->height;

                crusher->mware_data[MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_WIDTH+4] = crusher->width;
                if(crusher->height % 16)
                    crusher->mware_data[MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_HEIGHT+4] = crusher->height + 20 + 16 - (crusher->height % 16);
                else
                    crusher->mware_data[MWARE_VIDEO_VPU_PHYSICAL_OUTPUT_HEIGHT+4] = crusher->height + 20;
            }
        } else {
            //TODO: configure mware for CrusherCapture if needed.
            if(crusher->audio_samplerate != 48000) {
#if 0
                crusher->mware_data[MWARE_AUDIO_AII_DRIVELRCK_SCKPHASE+4] = 7;
                crusher->mware_data[MWARE_AUDIO_AOI_DRIVE_SCKPHASE+4] = 7;

                crusher->mware_data[MWARE_AUDIO_AII_SAMPLE_SCKPHASE+4] = 3;
                crusher->mware_data[MWARE_AUDIO_AOI_SAMPLELRCK_SCKPHASE+4] = 3;

                crusher->mware_data[MWARE_AUDIO_AII_MCLKTOFS_RATIO+4] = AUD_MASTER_CLOCK_512FS;
                crusher->mware_data[MWARE_AUDIO_AOI_MCLKTOFS_RATIO+4] = AUD_MASTER_CLOCK_512FS;

                crusher->mware_data[MWARE_AUDIO_AII_SCKTOLRCK_RATIO+4] = AUD_BUS_CLOCK_32FS;
                crusher->mware_data[MWARE_AUDIO_AOI_SCKTOLRCK_RATIO+4] = AUD_BUS_CLOCK_32FS;
#endif
                ERROR("Samplerate %d is not partially supported", crusher->audio_samplerate);
            }
        }
    }
    return 1;
}


void mware_defaults(crusher_t *crusher){
    int i;

    if(crusher->devtype == DEV_TYPE_CAPTURE && crusher->devmode == DEV_TYPE_CAPTURE) {
        for (i=0; i<MWARE_SIZE; i++)
            crusher->mware_data[i] = capture_init_mware[i];
    } else {
        for (i=0; i<MWARE_SIZE; i++)
            crusher->mware_data[i] = encoder_init_mware[i];
    }
}

