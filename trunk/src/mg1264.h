#ifndef MG1264_H_
#define MG1264_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crusher.h"


enum {
	//SYSTEM_CTRLOBJ_ID = 0,
	SYSTEM_CTRLOBJ_ID = 1,
	AVDECODER_CTRLOBJ_ID = 2,
	AVENCODER_CTRLOBJ_ID = 3
};
typedef uint32_t controlobjectId_t;
/* unique for system ctrlobjId */




/* shared for system & encoder ctrlobjId */
#define CMD_OPCODE_CONFIGURE			1026 /* 3 params, TODO: check if 1st always 0? */
#define CMD_OPCODE_SUBSCRIBE_EVENT		1024
//#define CMD_OPCODE_UNSUBSCRIBE_EVENT	/*TODO*/



enum {
    SYS_CMD_OPCODE_POWERDOWN            = 5,
    SYS_CMD_OPCODE_VIDEO_DISPLAY_RECT	= 6,
//    SYS_CMD_OPCODE_VIDEO_OSD_ALPHA		= 7,  /* 2 params */
    SYS_CMD_OPCODE_ZOOM					= 7,  /* 3 params */

    SYS_CMD_OPCODE_SET_OUTPARAM			= 16, /* 2 params */
    //SYS_CMD_OPCODE_ACTIVATE_OUTCFG		= 17, /* 0 params */
 	SYS_CMD_OPCODE_ACTIVATE_OUTCFG		= 19, /* 0 params */
    SYS_CMD_OPCODE_VOUT_GAMMA_LUT_SET   = 18,
#if 0
    SYS_CMD_OPCODE_DRAWTEXT				= 27, /* 0 params */
#endif
    //    SYS_CMD_OPCODE_VERBOSE_LVL			= 25, /* 1 params */
    SYS_CMD_OPCODE_VERBOSE_LVL			= 27, /* 1 params */
    SYS_CMD_OPCODE_ECHO					= 1,  /* 1 params */
    /* unused */
    SYS_CMD_OPCODE_BMP_BLIT        = 9,
    SYS_CMD_OPCODE_BMPDATA_BLIT    = 10,
    SYS_CMD_OPCODE_BMPDATA_FILL    = 11,
    SYS_CMD_OPCODE_BMP_SHOW        = 12,
    SYS_CMD_OPCODE_RES_DOWNLOAD    = 13,
    SYS_CMD_OPCODE_RES_RELEASE     = 14,
    SYS_CMD_OPCODE_SOC_GET_SENSOR_TYPE = 21,
    SYS_CMD_OPCODE_SOC_LCD_SETDIR  = 22,
    SYS_CMD_OPCODE_WAVEPLAY        = 24,
    SYS_CMD_OPCODE_GRP_INVALIDATERECT = 28,
    SYS_CMD_OPCODE_AVIO_DISABLE    = 30,
    SYS_CMD_OPCODE_AVIO_ENABLE     = 31,
    SYS_CMD_OPCODE_BATCH           = 32,

};


enum {
/* unique for encoder ctrlobjId */
    CMD_OPCODE_VIDEOPARAMS			= 17, /* 2 params */
    CMD_OPCODE_VIDEOENC_PARAMS		= 14, /* 2 params */
    CMD_OPCODE_CONFIGURE_RCFILE		= 19, /* 2 params */
    CMD_OPCODE_PERFORMANCE_MODE		= 27, /* 1 param */

    CMD_OPCODE_ACTIVATE_INCFG		= 18, /* 0 params */
    CMD_OPCODE_ACTIVATE_ENCCFG		= 15, /* 0 params */
    CMD_OPCODE_ACTIVATE_RCCFG		= 20, /* 0 params */
    CMD_OPCODE_ACTIVATE_BITSTREAMCFG		= 21, /* 1 param (==1) */
    CMD_OPCODE_ACTIVATE_AUDIOCFG	= 42, /* 0 params */

/* other */
    CMD_OPCODE_VIDEO_GAMMA_LUT		= 23, /* 4 params (2 + (4in1) + (4in1))*/
    CMD_OPCODE_VIDEO_FORCE_NEW_GOP	= 25, /* 0 params */

    CMD_OPCODE_AUDIO_ENC_PARAMS		= 41, /* 2 params */

    CMD_OPCODE_FMT_ENABLE_QBOXMETA	= 31, /* 1 param */

    CMD_OPCODE_MDREGION_SUB			= 36, /* 5 params */
    CMD_OPCODE_MDREGION_ADD			= 35, /* 5 params */
    CMD_OPCODE_MDREGION_SET 		= 34, /* 5 params */
    CMD_OPCODE_MDGLOBAL_RESET		= 33, /* 0 params */
    CMD_OPCODE_MDGLOBAL_ENABLE		= 32, /* 1 param */

    CMD_OPCODE_BURNIN_FONTSIZE_SET	= 51, /* 2 params */
    CMD_OPCODE_BURNIN_FONT_SET		= 40, /* 1 param */
    CMD_OPCODE_BURNIN_FRAMECNT_SET  = 39, /* 3 params */
    CMD_OPCODE_BURNIN_STR_ENABLE    = 37, /* 5 params */
    CMD_OPCODE_BURNIN_STR_SET       = 38, /* 6 params (string 14 chars) */

    CMD_OPCODE_MASKREGION_ADD		= 44, /* 5 params */
    CMD_OPCODE_MASKREGION_SUB		= 45, /* 1 param */
    CMD_OPCODE_MOVEREGION_ALLOC		= 46, /* 6 params */
    CMD_OPCODE_MOVEREGION_DEL		= 47, /* 0 params */

    CMD_OPCODE_ZOOM_IN_SET			= 48, /* 2 params */
    CMD_OPCODE_ZOOM_IN_ENABLE		= 49, /* 0 params */
    CMD_OPCODE_ZOOM_IN_DISABLE		= 50, /* 0 params */

    CMD_OPCODE_QBOX_VERSION			= 52, /* 1 param */

    AVE_CMD_FLUSH 					= 1,
    AVE_CMD_RECORD 					= 2,
    AVE_CMD_PAUSE 					= 3, /* 0 params */
    AVE_CMD_RESUME 					= 4, /* 0 params */
    AVE_CMD_PREVIEW 				= 6, /* 0 params */
    AVE_CMD_PLAY					= 11, /* 0 params */
    AVE_CMD_PLAYBLOCK				= 12, /* 0 params */
};


/*
 *
 *
AVE_CMD_VIDEO_CAPTURE_RECT 20
AVE_CMD_BITSTREAM_BLOCK_DONE 5
AVE_CMD_RC_BUFFER_MODEL 23
AVE_CMD_RC_FILE_SIZE 22
AVE_CMD_RC_SNR_LIMIT 24
*/

//SYS_CMD_OPCODE_VOUT_PARAMS
//sysVOutConfigure:
enum {
	SYS_CFG_VOUT_ASPECT_X = 5,
	SYS_CFG_VOUT_ASPECT_Y = 6,
	SYS_CFG_VOUT_FRAMETICKS = 9,
};


//AVE: CMD_OPCODE_CONFIGURE
//encoderConfigure:
enum {
//AVE_CFG_VIDEO_STC_OFFSET = 39
//AVE_CFG_VIDEO_MUTE = 53
//AVE_CFG_AUDIO_MUTE = 54,
	AVE_CFG_PVS_ENCODE = 9,
    AVE_CFG_VENC_AVCALIGN = 59,
	AVE_CFG_AENC_BITRATE = 35,
	AVE_CFG_AENC_CODEC = 67,
	AVE_CFG_PREV_AV_SELECT = 18,
	AVE_CFG_AI_CHANNELS = 19,
	AVE_CFG_BITSTREAM_TYPE = 1,
	AVE_CFG_ENC_AV_SELECT = 17,
	AVE_CFG_ENC_PREV_AV_SELECT = 18,
	AVE_CFG_ENC_INPUT_SELECT = 40,
	AVE_CFG_VENC_OPERATIONAL_MODE = 10,
	AVE_CFG_VENC_BITRATE = 21,
	AVE_CFG_VENC_GOP_SIZE = 24,

};

//SYS: CMD_OPCODE_CONFIGURE
//sysConfigure:
enum {
	SYS_CFG_LLATENCY = 11,
    SYS_CFG_AUDIO_SAMPLERATE = 7,
    SYS_CFG_AUDIO_SAMPLESIZE = 8,
	SYS_CFG_VENC_VOUT_SCALE = 14,
	SYS_CFG_VENC_VOUT_STANDARD = 13,
};

//SYS_CFG_VENC_VOUT_STANDARD:
enum {
	SYS_CFP_VENC_VOUT_STANDARD_NTSC = 1,
	SYS_CFP_VENC_VOUT_STANDARD_PAL = 2
};

//AVE_CFG_AENC_CODEC:
/* same as enum in crusher.h */
enum {
	AVE_CFP_AENC_CODEC_PCM = 1,
	AVE_CFP_AENC_CODEC_AAC = 2,
	AVE_CFP_AENC_CODEC_QMA = 3
};

enum {
	AVE_CMDP_PERFORMANCE_MODE_D1_104 = 0,
	AVE_CMDP_PERFORMANCE_MODE_D1_81_NOPV = 1,
	AVE_CMDP_PERFORMANCE_MODE_QVGA_81 = 2,
	AVE_CMDP_PERFORMANCE_MODE_FD_104 = 3,
	AVE_CMDP_PERFORMANCE_MODE_ASTRO_HVGA = 4
};


enum {
	AVE_CFP_BITSTREAM_TYPE_ELEM_VIDEO = 1,
	AVE_CFP_BITSTREAM_TYPE_QBOX = 2,
	AVE_CFP_BITSTREAM_TYPE_ELEM_AUDIO = 3,
	AVE_CFP_BITSTREAM_TYPE_ELEM_MP4 = 4
};

#define AVE_CFG_NUMBLOCKSPEREVENT 56

//AVE_CFG_ENC_AV_SELECT:
//AVE_CFG_PREV_AV_SELECT:
enum {
	AVE_CFP_ENC_SELECT_AV = 1,
	AVE_CFP_ENC_SELECT_VIDEO_ONLY = 2,
	AVE_CFP_ENC_SELECT_AUDIO_ONLY = 3
};

//#define AVE_CFG_VIN_PROG_SOURCE 17

//CMD_OPCODE_VIDEOPARAMS:
//encoderConfigureVideo:
enum {
	AVE_CFG_VIN_PROG_SOURCE = 9,
	AVE_CFG_VIN_FIELD_CODING = 36,

	AVE_CFG_VINTF_FILTER = 10,
	AVE_CFG_VINTF_CHROMA_ATTEN = 11,
	AVE_CFG_VINTF_LUMA_ATTEN = 12,
	AVE_CFG_VINTF_FILTER_P3 = 15,

	AVE_CFG_VIDEO_CAPTURE_RECT_W = 1,
	AVE_CFG_VIDEO_CAPTURE_RECT_H = 2,
	AVE_CFG_VIDEO_CAPTURE_RECT_X_OS = 3,
	AVE_CFG_VIDEO_CAPTURE_RECT_Y_OS = 4,

	AVE_CFG_VIDEO_IN_DECIMATION_X = 5,
	AVE_CFG_VIDEO_IN_DECIMATION_Y = 6,

	AVE_CFG_VIDEO_IN_FIELD_ORDER = 19,

	AVE_CFG_VIDEO_IN_ASPECT_RATIO_X = 25,
	AVE_CFG_VIDEO_IN_ASPECT_RATIO_Y = 26,
	AVE_CFG_VIDEO_IN_ASPECT_RATIO_FORCE = 27,

	AVE_CFG_VIDEO_IN_FRAMERATE = 28,
	AVE_CFG_VIDEO_IN_INTERLACED_PROGSCALE = 29,

	AVE_CFG_VIDEO_IN_HUESAT_CBKA = 32,
	AVE_CFG_VIDEO_IN_HUESAT_CBKB = 33,
	AVE_CFG_VIDEO_IN_HUESAT_CRKC = 34,
	AVE_CFG_VIDEO_IN_HUESAT_CRKD = 35,

	AVE_CFG_VIDEO_INOUT_FRAME_TICKS = 31,
	AVE_CFG_VIDEO_INOUT_FRAME_TICKSNUM = 48,

	AVE_CFG_VIDEO_IN_STANDARD = 0,
	AVE_CFG_VIDEO_IN_FRAMERATE2 = 120,
	AVE_CFG_VIDEO_IN_FRAMERATE3 = 121,

};
enum {
	AVE_CFP_VIN_FIELD_ORDER_TFF = 1,
	AVE_CFP_VIN_FIELD_ORDER_BFF = 2
};

enum {
	AVE_CFP_VIN_PROG_SOURCE_INTERLACED = 0,
	AVE_CFP_VIN_PROG_SOURCE_PROGRESSIVE = 1,
};

enum {
	AVE_CFP_VIN_FIELD_CODING_FIELD = 1,
	//AVE_CFP_VIN_FIELD_CODING_FRAME = 2,
	AVE_CFP_VIN_FIELD_CODING_FRAME = 0,
	//AVE_CFP_VIN_FIELD_CODING_ADAPTIVE = 3
	AVE_CFP_VIN_FIELD_CODING_ADAPTIVE = 2
};


enum {
	AVE_CFP_VIDEO_INPUT_STANDARD_NTSC = 0,
	AVE_CFP_VIDEO_INPUT_STANDARD_PAL = 1
};
enum {
	AVE_CFP_VIDEO_STANDARD_NTSC_RATE = 3003,
	AVE_CFP_VIDEO_STANDARD_PAL_RATE = 3600
};


//encoderConfigureVideoEnc:
enum {
	AVE_CFG_VENC_AVCALIGN_LEN = 138,
	AVE_CFG_VENC_DEBLOCK_ENABLED = 3,
	AVE_CFG_VENC_DEBLOCK_ALPHA = 4,
	AVE_CFG_VENC_DEBLOCK_BETA = 5,

	AVE_CFG_VENC_FRAMETICKS = 121,

	AVE_CFG_VENC_SLICES_NUM = 135,
	AVE_CFG_VENC_SLICES_NUM1 = 136,

	AVE_CFG_VENC_SCENECHANGE = 113,
	AVE_CFG_VENC_SCENECHANGE_ISLICE = 114,
	AVE_CFG_VENC_SCENECHANGE_NEWGOP = 115,

	AVE_CMD_VENC_SEI_PICT_TIMING = 94,

	AVE_CMD_VENC_SPS_LEVEL = 118,

	AVE_CMD_VENC_TELECINE = 117,
	AVE_CMD_VENC_TELECINE_DROPFRAMES = 141,

	AVE_CMD_VENC_VUTIMING = 108,
	AVE_CMD_VENC_VUTIMING_NUM_UNITS_IN_TICK = 120,
	AVE_CMD_VENC_VUTIMING_FIXED_FRAMERATE =  119,
	AVE_CMD_VENC_VUTIMING_TICKS = 150,

	AVE_CFG_VENC_OFFSET_X = 146,
	AVE_CFG_VENC_OFFSET_Y = 148,


};


#define AVE_CFG_VIN_FRAMERATE_DECIMATION 32
#define AVE_CFG_VIN_DECIMATION_H 13
#define AVE_CFG_VIN_DECIMATION_V 14

//CMD_OPCODE_CONFIGURE_RCFILE:
//encoderConfigureRCFile:
enum {
	AVE_CFG_RCFILE_ENABLER = 6,
	AVE_CFG_RCFILE_SIZE = 7,
	AVE_CFG_RCFILE_VIOLATIONS = 8,

	AVE_CFG_RCBUFFER_ENABLED = 8,
	AVE_CFG_RCBUFFER_BITSIZE = 9,
	AVE_CFG_RCBUFFER_TRANSFER_BITRATE = 10,
	AVE_CFG_RCBUFFER_INITIAL_DELAY = 11,
	AVE_CFG_RCBUFFER_LOW_DELAY = 12,

	AVE_CFG_RC_ADAPTIVE_FR = 28,
	AVE_CFG_RC_ADAPTIVE_FR_MIN_QP = 29,
	AVE_CFG_RC_ADAPTIVE_FR_MAX_QP = 30,
	AVE_CFG_RC_ADAPTIVE_FR_SCAL_DENOM = 31,
	AVE_CFG_RC_ADAPTIVE_FR_SCAL_MIN_NUMERATOR = 32,
};





enum {
	AVE_CFG_VENC_OPERATIONAL_MODE_LOW_BITRATE = 0,
	AVE_CFG_VENC_OPERATIONAL_MODE_MED_BITRATE = 1,
	AVE_CFG_VENC_OPERATIONAL_MODE_HIGH_BITRATE = 2
};

//AVE_CFG_AUDIO_INPUT_CHANNELS:
//AVE_CFG_AUDIO_INPUT:
enum {
	AVE_CFP_AI_CHANNELS_STEREO = 1,
	AVE_CFP_AI_CHANNELS_STEREO_SWAP = 2,
	AVE_CFP_AI_CHANNELS_STEREO_MONO_LEFT = 3,
	AVE_CFP_AI_CHANNELS_STEREO_MONO_RIGHT = 4
};

enum {
	AVE_EV_BITSTREAM_BLOCK_READY = 0x030001, /* Encoder Have Data in Buffer */
	AVE_EV_BITSTREAM_FLUSHED = 0x030003, /* Encoder Finished */
	AVE_EV_VIDEO_FRAME_ENCODED = 0x030005,
	AVE_EV_AUDIO_FRAME_ENCODED = 0x030004,
	AVE_EV_VIDEO_FRAME_DROP = 0x030009,
	AVE_EV_VIDEO_FRAME_REPEAT = 0x03000A,
	AVE_EV_READY = 0x030013, /* Encoder Ready */
	AVE_EV_FLUSH_COMPLETE = 0x030014,
	SYSTEM_EV_2 = 0x010002,
	SYSTEM_EV_BAD_IMAGE_DIMENSION = 0x010004,
};
typedef uint32_t eventId_t;

typedef struct command_s {
    controlobjectId_t controlObjectId;
    uint32_t     opcode;
    uint32_t     arguments[6];
    uint32_t     returnCode;
    uint32_t     returnValues[7];
} GCC_PACK command_t;

typedef struct event_s {
	controlobjectId_t  controlObjectId;
	eventId_t          eventId;
	uint32_t           timestamp;
	uint32_t           payload[13];
} GCC_PACK event_t;

/* event payloads */
typedef struct {
	uint32_t bufferId;
	uint32_t blocksNum;
	uint32_t startAddr;
} GCC_PACK event_ready_t;


typedef struct {
	uint32_t addr;
	uint32_t len;
} GCC_PACK coded_block_t;

typedef struct {
	uint16_t blocksNum;
	uint16_t blocksStreamType; /* ves, aes, qbox or mp4 */
	coded_block_t block[6];
} GCC_PACK event_bs_block_ready_t;

typedef struct {
	uint32_t Id;
	uint32_t offset0;
	uint32_t width;
	uint32_t height;
	uint32_t offset1;
	uint32_t offset2;
} GCC_PACK block_config_chunk_t;


int mg1264_load_firmware(crusher_t *crusher, FILE *mg1264Fw);

/* config activation */
int ActivateVideoInCfg(crusher_t *crusher);
int ActivateVideoEncCfg(crusher_t *crusher);
int ActivateVideoRCCfg(crusher_t *crusher);
int ActivateAudioEncCfg(crusher_t *crusher);
int sysActivateOutputCfg(crusher_t *crusher);
/* osd */
int sysDrawText(crusher_t *crusher, int val);
int sysOSDAlpha(crusher_t *crusher, int val0, int val1);
/* system */
int sysSetOutputParam(crusher_t *crusher, int param, int value);
int sysConfigure(crusher_t *crusher, int param, int value);
int sysVOutConfigure(crusher_t *crusher, int param, int value);
int sysSetVerboseLevel(crusher_t *crusher, int level);
int sysEcho(crusher_t *crusher, int echo_in);
int sysRestart(crusher_t *crusher);
/* stuff */
int setQboxVersion(crusher_t *crusher, int version);
/* move/mask regions */
int MaskRegionAdd(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4);
int MaskRegionSub(crusher_t *crusher, int val);
int MoveRegionAlloc(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4, int val5);
int MoveRegionDel(crusher_t *crusher);
/* Burn IN */
int BurnInFontSizeSet(crusher_t *crusher, int val0, int val2);
int BurnInFontSet(crusher_t *crusher, int val);
int BurnInFrameCountSet(crusher_t *crusher, int val0, int val1, int val2);
int BurnInStrEnable(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4);
int BurnInStringSet(crusher_t *crusher, char *str);
/* MDRegion/MDGlobal */
int MDRegionSub(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4);
int MDRegionAdd(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4);
int MDRegionSet(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4);
int MDGlobalReset(crusher_t *crusher);
int MDGlobalEnable(crusher_t *crusher, int val);
/* Zoom */
int ZoomInSet(crusher_t *crusher, int val0, int val1);
int ZoomInDisable(crusher_t *crusher);
int ZoomInEnable(crusher_t *crusher);
/* encoder */
int encoderPause(crusher_t *crusher);
int encoderResume(crusher_t *crusher);
int encoderStop(crusher_t *crusher);
int encoderPreview(crusher_t *crusher);
int encoderRecord(crusher_t *crusher);
int encoderFlush(crusher_t *crusher);
int encoderPlay(crusher_t *crusher);
int encoderPlayBlocks(crusher_t *crusher, int bufferId, int numBlocks);
int encoderConfigure(crusher_t *crusher, int param, int value);
#if 0
int encoderConfigureAudio(crusher_t *crusher, int param, int value);
int encoderConfigureAudioEnc(crusher_t *crusher, int param, int value);
#endif
int encoderConfigureStreamType(crusher_t *crusher, int type);
int encoderConfigureVideo(crusher_t *crusher, int param, int value);
int encoderConfigureVideoEnc(crusher_t *crusher, int param, int value);
int encoderConfigureRCFile(crusher_t *crusher, int param, int value);

int videoRectDisp(crusher_t *crusher, int width, int height, int offset_x, int offset_y);
int encoderRestart(crusher_t *crusher);
int RawCmd(crusher_t *crusher, int controlObjectId, int opcode,
					int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

/************************************************************************
 * mg1264-cmd called by crusher
 ************************************************************************/
int mg1264_send_encoder_textconfig(crusher_t *crusher);
int mg1264_send_capture_textconfig(crusher_t *crusher);


/************************************************************************
 * mg1264-codec
 ************************************************************************/

int mg1264_sendframes(crusher_t *crusher, uint8_t *converted_data, int data_size);
int mg1264_readframes(crusher_t *crusher);
int mg1264_finish(crusher_t *crusher);

/************************************************************************
 * mg1264-cli entry point
 ************************************************************************/
int mg1264_cli_parse_textconfig(crusher_t *crusher);

#endif /*MG1264_H_*/
