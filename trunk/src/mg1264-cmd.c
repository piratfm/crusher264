/*
 * mg1264-cmd.c
 *
 *  Created on: 14 окт. 2009
 *      Author: tipok
 */

#include <string.h>
#include "crusher.h"
#include "logging.h"
#include "mg1264.h"
#include "mg1264-io.h"

/********************************************************************
 * Activation Stuff
 ********************************************************************/
/* 3, 18 */
int ActivateVideoInCfg(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ACTIVATE_INCFG;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 15 */
int ActivateVideoEncCfg(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ACTIVATE_ENCCFG;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 20 */
int ActivateVideoRCCfg(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ACTIVATE_RCCFG;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 42 */
int ActivateAudioEncCfg(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ACTIVATE_AUDIOCFG;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 19 */
int sysActivateOutputCfg(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_ACTIVATE_OUTCFG;
	return mg1264_cmd(crusher, &cmd);
}

/********************************************************************
 * SystemApi Stuff
 ********************************************************************/
/* 1, 27 */
#if 0
int sysDrawText(crusher_t *crusher, int val)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_DRAWTEXT;
	cmd.arguments[0] = val;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 7 */
int sysOSDAlpha(crusher_t *crusher, int val0, int val1)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_VIDEO_OSD_ALPHA;
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	return mg1264_cmd(crusher, &cmd);
}
#endif

/* 1, 16 */
int sysSetOutputParam(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_SET_OUTPARAM;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 1026 */
int sysConfigure(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_CONFIGURE;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 18 */
int sysVOutConfigure(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_VOUT_GAMMA_LUT_SET;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 25 */
int sysSetVerboseLevel(crusher_t *crusher, int level)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_VERBOSE_LVL;
	cmd.arguments[0] = level;
	return mg1264_cmd(crusher, &cmd);
}
/* 1, 1 */
int sysEcho(crusher_t *crusher, int echo_in)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_ECHO;
	cmd.arguments[0] = echo_in;
	return mg1264_cmd(crusher, &cmd);
}

/* **************************************************
 * Stuff
 * *************************************************/
/* 3, 52 */
int setQboxVersion(crusher_t *crusher, int version)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_QBOX_VERSION;
	cmd.arguments[0] = version;
	return mg1264_cmd(crusher, &cmd);
}

/********************************************************************
 * Move/Mask Region Trash
 ********************************************************************/
/* 3, 44 */
int MaskRegionAdd(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MASKREGION_ADD;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 45 */
int MaskRegionSub(crusher_t *crusher, int val)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MASKREGION_SUB;
	/* TODO: ??? */
	cmd.arguments[0] = val;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 46 */
int MoveRegionAlloc(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4, int val5)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MOVEREGION_ALLOC;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	cmd.arguments[5] = val5;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 47 */
int MoveRegionDel(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MOVEREGION_DEL;
	return mg1264_cmd(crusher, &cmd);
}

/********************************************************************
 * Burn IN Trash
 ********************************************************************/
/* 3, 51 */
int BurnInFontSizeSet(crusher_t *crusher, int val0, int val2)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_BURNIN_FONTSIZE_SET;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val2;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 40 */
int BurnInFontSet(crusher_t *crusher, int val)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_BURNIN_FONT_SET;
	/* TODO: ??? */
	cmd.arguments[0] = val;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 39 */
int BurnInFrameCountSet(crusher_t *crusher, int val0, int val1, int val2)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_BURNIN_FRAMECNT_SET;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 37 */
int BurnInStrEnable(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_BURNIN_STR_ENABLE;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 38 */
int BurnInStringSet(crusher_t *crusher, char *str)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_BURNIN_STR_SET;
	/* FIXME: Not implemented! */
	strncpy((char *)&cmd.arguments[0], str, 20);
	/* 5 arg - is free */
	return mg1264_cmd(crusher, &cmd);
}

/********************************************************************
 * MDRegion/MDGlobal Trash
 ********************************************************************/
/* 3, 36 */
int MDRegionSub(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MDREGION_SUB;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 35 */
int MDRegionAdd(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MDREGION_ADD;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 34 */
int MDRegionSet(crusher_t *crusher, int val0, int val1, int val2, int val3, int val4)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MDREGION_SET;
	/* TODO: ??? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	cmd.arguments[2] = val2;
	cmd.arguments[3] = val3;
	cmd.arguments[4] = val4;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 33 */
int MDGlobalReset(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MDGLOBAL_RESET;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 32 */
int MDGlobalEnable(crusher_t *crusher, int val)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_MDGLOBAL_ENABLE;
	cmd.arguments[0] = val;
	return mg1264_cmd(crusher, &cmd);
}

/********************************************************************
 * ZOOM Trash
 ********************************************************************/
/* 3, 48 */
int ZoomInSet(crusher_t *crusher, int val0, int val1)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ZOOM_IN_SET;
	/* TODO: xy? */
	cmd.arguments[0] = val0;
	cmd.arguments[1] = val1;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 50 */
int ZoomInDisable(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ZOOM_IN_DISABLE;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 49 */
int ZoomInEnable(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ZOOM_IN_ENABLE;
	return mg1264_cmd(crusher, &cmd);
}

/* **************************************************
 * Main Commands
 * **************************************************/
/* 3, 3 */
int encoderPause(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_PAUSE;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 4 */
int encoderResume(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_RESUME;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 1 */
int encoderStop(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_FLUSH;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 6 */
int encoderPreview(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_PREVIEW;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 2 */
int encoderRecord(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_RECORD;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 1 */
int encoderFlush(crusher_t *crusher)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_FLUSH;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 11 */
int encoderPlay(crusher_t *crusher)
{
	/* 0000   00 00 00 03 00 00 00 0b 00 00 00 01 b0 1a de 8b
	 * 0010   00 90 75 38 b0 1a de 48 90 09 05 c8 0e 53 9d 38
	 * */
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_PLAY;
	cmd.arguments[0] = 1;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 12 */
int encoderPlayBlocks(crusher_t *crusher, int bufferId, int numBlocks)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = AVE_CMD_PLAYBLOCK;
	cmd.arguments[0] = bufferId;
	cmd.arguments[1] = numBlocks;
	cmd.arguments[3] = 0x40;
	cmd.arguments[5] = 0x05;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 1026 */
int encoderConfigure(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_CONFIGURE;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
#if 0
/* 3, 41 */
int encoderConfigureAudio(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_AUDIOPARAMS;
	/* TODO: ? */
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 41 */
int encoderConfigureAudioEnc(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_AUDIO_ENC_PARAMS;
	/* TODO: ? */
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
#endif
/* 3, 1026
 * 3, 21, 1, 0 */
int encoderConfigureStreamType(crusher_t *crusher, int type)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	encoderConfigure(crusher, AVE_CFG_BITSTREAM_TYPE, type);

	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_ACTIVATE_BITSTREAMCFG;
	cmd.arguments[0] = 1; /* always 1 */
	return mg1264_cmd(crusher, &cmd);
}

/* 3, 17 */
int encoderConfigureVideo(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_VIDEOPARAMS;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}
/* 3, 14 */
int encoderConfigureVideoEnc(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_VIDEOENC_PARAMS;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}

/* 3, 19 */
int encoderConfigureRCFile(crusher_t *crusher, int param, int value)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_CONFIGURE_RCFILE;
	cmd.arguments[0] = param;
	cmd.arguments[1] = value;
	return mg1264_cmd(crusher, &cmd);
}

/* **************************************************
 * Additionap play/stop Commands
 * **************************************************/
int videoRectDisp(crusher_t *crusher, int width, int height, int offset_x, int offset_y)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_VIDEO_DISPLAY_RECT;
	cmd.arguments[0] = width;
	cmd.arguments[1] = height;
	cmd.arguments[2] = offset_x;
	cmd.arguments[3] = offset_y;
	return mg1264_cmd(crusher, &cmd);
}

int RawCmd(crusher_t *crusher, int controlObjectId, int opcode,
					int arg0, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = controlObjectId;
	cmd.opcode = opcode;
	/* TODO: ??? */
	cmd.arguments[0] = arg0;
	cmd.arguments[1] = arg1;
	cmd.arguments[2] = arg2;
	cmd.arguments[3] = arg3;
	cmd.arguments[4] = arg4;
	cmd.arguments[5] = arg5;
	return mg1264_cmd(crusher, &cmd);
}



/***********************************************************
 * More generic
 ***********************************************************/
int encoderRestart(crusher_t *crusher)
{
	uint32_t status;
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	 /* 3, 1026, 56, 6, 68, 1, 0, 0 */
	RawCmd(crusher, AVENCODER_CTRLOBJ_ID, CMD_OPCODE_CONFIGURE, 0x38, 6, 0x44, 1, 0, 0);

	/* 3, 1024, 0x30003, 0x030013, 0, 1 */
	/* 0000   00 00 00 03 00 00 04 00 00 03 00 03 00 03 00 13
	 * 0010   00 00 00 00 00 00 00 01 00 00 00 00 0b 9b 23 43*/
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_SUBSCRIBE_EVENT;
	cmd.arguments[0] = AVE_EV_BITSTREAM_FLUSHED;
	cmd.arguments[1] = AVE_EV_READY;
	cmd.arguments[3] = 1;
	mg1264_cmd(crusher, &cmd);
	if (crusher->devmode == DEV_TYPE_ENCODER) {
	    /* 1, 1024, 0x010002, 0 */
	    /* 0000   00 00 00 01 00 00 04 00 00 01 00 02 00 00 00 00
	     * 0010   00 00 00 00 00 00 00 01 00 00 00 00 0b 9b 23 43*/
	    cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	    cmd.opcode = CMD_OPCODE_SUBSCRIBE_EVENT;
	    cmd.arguments[0] = SYSTEM_EV_2;
	    cmd.arguments[1] = 0;
	    mg1264_cmd(crusher, &cmd);

	    /* 1, 1024, 0x010004, 0 */
	    /* 0000   00 00 00 01 00 00 04 00 00 01 00 04 00 00 00 00
	     * 0010   00 00 00 00 00 00 00 01 00 00 00 00 0b 9b 23 43*/
	    cmd.arguments[0] = SYSTEM_EV_BAD_IMAGE_DIMENSION;
	    mg1264_cmd(crusher, &cmd);
	}
	/* 3, 1024, 0x030001, 0 */
	/* 0000   00 00 00 03 00 00 04 00 00 03 00 01 00 00 00 00
	 * 0010   00 00 00 00 00 00 00 01 00 00 00 00 0b 9b 23 43*/
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_SUBSCRIBE_EVENT;
	cmd.arguments[0] = AVE_EV_BITSTREAM_BLOCK_READY;
	mg1264_cmd(crusher, &cmd);

	if(!mg1264_csrr(crusher, 0x00C2, &status, 0x04, 0x02))
		return 0;
	if (status != 0x40) {
		ERROR("status=%d (must be 0x40)", status);
		return 0;
	}
	XTREME("status=%d", status);


	return status & 0x40;
};



/***********************************************************
 * prepare to configure codec
 ***********************************************************/
int encoderCaptureStart(crusher_t *crusher)
{
    command_t cmd;
    memset(&cmd, 0, sizeof(command_t));
    /* 0000   00 00 00 01 00 00 00 05 00 00 00 01 00 00 00 00
     * 0010   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 1,5,1,0, 0,0,0,0 */
    cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
    cmd.opcode = SYS_CMD_OPCODE_POWERDOWN;
    cmd.arguments[0] = 0x01;
    mg1264_cmd(crusher, &cmd);


    /* 0000   00 00 00 01 00 00 00 05 00 00 00 00 00 00 00 00
     * 0010   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 1,5,0,0, 0,0,0,0 */
    cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
    cmd.opcode = SYS_CMD_OPCODE_POWERDOWN;
    cmd.arguments[0] = 0x00;
    mg1264_cmd(crusher, &cmd);

    /* 0000   00 00 00 03 00 00 04 02 00 00 00 38 00 00 00 06
     * 0010   00 00 00 44 00 00 00 01 00 00 00 00 00 00 00 00
     * 3,1026,56,6, 68,1,0,0*/

    /* 0000   00 00 00 03 00 00 04 00 00 03 00 03 00 03 00 13
     * 0010   00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00
     * 3,1024,0x00030003,0x00030013,  0,1,0,0 */

    /* 0000   00 00 00 03 00 00 04 00 00 03 00 01 00 00 00 00
     * 0010   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 3,1024,0x00030001,0,  0,0,0,0 */
    return encoderRestart(crusher);
};




/* **************************************************
 * Audio Stuff
 * **************************************************/








int mg1264_send_encoder_textconfig(crusher_t *crusher)
{
    /* SEND CONFIG: */
    /* ------------------- */
    /* encmode %s
     * 3, 1026, 10, 1, 0 */
    if (crusher->bitrate >= 2000000)
        encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_HIGH_BITRATE);
    else if (crusher->bitrate >= 750000)
        encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_MED_BITRATE);
    else
        encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_LOW_BITRATE);

    /* ------------------- */
    /* rawcmd 1 28 1
     * 1, 28, 1, 0 */
    RawCmd(crusher, 1, 28, 1, 0, 0, 0, 0, 0);
    /* ------------------- */
    /* streamtype qbox
     * 3, 1026, 1, 2, 0 */
    if(crusher->out_format == OUT_FORMAT_QBOX) {
        encoderConfigure(crusher, AVE_CFG_BITSTREAM_TYPE, AVE_CFP_BITSTREAM_TYPE_QBOX);
    } else if (crusher->out_format == OUT_FORMAT_VES) {
        encoderConfigure(crusher, AVE_CFG_BITSTREAM_TYPE, AVE_CFP_BITSTREAM_TYPE_ELEM_VIDEO);
    } else {
        ERROR("Unknown format: %d", crusher->out_format);
        return -1;
    }
    /* ------------------- */
    /* avselect v
     * 3, 1026, 17, 2, 0 */
    encoderConfigure(crusher, AVE_CFG_ENC_AV_SELECT, AVE_CFP_ENC_SELECT_VIDEO_ONLY);
    /* ------------------- */
    /* inputselect v
     * 3, 1026, 40, 2, 0 */
    encoderConfigure(crusher, AVE_CFG_ENC_INPUT_SELECT, AVE_CFP_ENC_SELECT_VIDEO_ONLY);
    /* ------------------- */
    /* progsource 1
     * 3, 17, 9, 1, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIN_PROG_SOURCE,AVE_CFP_VIN_PROG_SOURCE_PROGRESSIVE);
    /* 3, 18, 1, 0 */
    ActivateVideoInCfg(crusher);
    /* ------------------- */
    /* vbitrate %d
     * 3, 1026, 21, 0x20f580, 0
     * 2400000 = 640x480 2000kbps
     * 2160000 - 720x576,640x480 1800kbps
     * 1200000 - 320x240 1000kbps
     */
    encoderConfigure(crusher, AVE_CFG_VENC_BITRATE, crusher->bitrate * 1.2);
    /* ------------------- */
    /* vgopsize %d
     * 3, 1026, 24, 25, 0 (25fps)
     * 3, 1026, 24, 30, 0 (30fps) */
    if(crusher->gopsize) {
        encoderConfigure(crusher, AVE_CFG_VENC_GOP_SIZE, crusher->gopsize);
    } else {
        encoderConfigure(crusher, AVE_CFG_VENC_GOP_SIZE, crusher->framerate_num/crusher->framerate_den);
    }
    /* ------------------- */
    /* vrcsize 1 %d
     * 3, 19, 6, 1, 0 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_ENABLER, 1);
    /* 2nd arg. of: vrcsize 1 %d
     * 3, 19, 7, 2000000, 0 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_SIZE, crusher->rc_size);
    /* 3, 20, 2000000, 0 */
    ActivateVideoRCCfg(crusher);
    /* ------------------- */
    /* vfieldcoding fra xxxx
     * 3, 17, 36, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIN_FIELD_CODING, AVE_CFP_VIN_FIELD_CODING_FRAME);
    /* 3, 18, 0, 2, 0 */
    ActivateVideoInCfg(crusher);
    /* ------------------- */
    /* voutscale 0
     * 1, 1026, 14, 0 */
    sysConfigure(crusher, SYS_CFG_VENC_VOUT_SCALE, 0);
    /* vintf 0
     * 3, 17, 10, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VINTF_FILTER, 0);
    /* 3, 17, 11, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VINTF_CHROMA_ATTEN, 0);
    /* 3, 17, 12, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VINTF_LUMA_ATTEN, 0);
    /* 3, 17, 15, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VINTF_FILTER_P3, 0);
    /* 3, 18, 0 */
    ActivateVideoInCfg(crusher);
    /* ------------------- */
    /* vseipt %d
     * 3, 14, 94, 1, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_SEI_PICT_TIMING, 1);
    /* 3, 15, 1, 0 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* 3, 14, 91, 1, 0 */
    encoderConfigureVideoEnc(crusher, 91, 1);
    /* 3, 14, 92, 1000000, 0 */
    encoderConfigureVideoEnc(crusher, 92, 1000000);
    /* 3, 14, 93, 1000000, 0 */
    encoderConfigureVideoEnc(crusher, 93, 1000000);
    /* 3, 15, 1000000, 0 */
    ActivateVideoEncCfg(crusher);

    /* encperf %s
     * 3, 27, 4, 16, 3, 3, 0
     * TODO: do it needs other args? */
    RawCmd(crusher, AVENCODER_CTRLOBJ_ID, CMD_OPCODE_PERFORMANCE_MODE,
                    AVE_CMDP_PERFORMANCE_MODE_ASTRO_HVGA, 16, 3, 3, 0, 0);
    /* ------------------- */
    /* vscnewgop 1
     * 3, 14, 115, 1, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SCENECHANGE_NEWGOP, crusher->scenecut_threshold);
    // 3, 15, 1, 0 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* vdeblock 1 0 0
     * 3, 14, 3, 1, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_ENABLED, crusher->deblock);
    /* 3, 14, 4, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_ALPHA, crusher->deblock_alpha);
    /* 3, 14, 5, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_BETA, crusher->deblock_beta);
    /* 3, 15, 0 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* vrectcap %d %d
     * 3, 17, 1, 720, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_W, crusher->width);
    /* 3, 17, 2, 576, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_H, crusher->height);
    /* 3, 17, 3, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_X_OS, 0);
    /* 3, 17, 4, 0 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_Y_OS, 0);
    /* 3, 18, 0 */
    ActivateVideoInCfg(crusher);
    /* 1, 6, 720, 576, 0 */
    videoRectDisp(crusher, crusher->width, crusher->height, 0, 0);
    /* ------------------- */
    /* vvuitiming 1 %d 1 %d (v53, 2 * v54)
      * 3, 14, 108, 1, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING, 1);
    /* 3, 14, 120, 100, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING_NUM_UNITS_IN_TICK, crusher->framerate_den);
    /* 3, 14, 119, 1, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING_FIXED_FRAMERATE, 1);
    /* 3, 14, 150, 5000, 0
     * 30fps=6000 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING_TICKS, crusher->framerate_num*2);
    /* 3, 15, 5000, 0 */
    ActivateVideoEncCfg(crusher);
    /* -------------------*/
    /* vinticks %d %d %d (v54, v53, v53)
     * 3, 17, 48, 2500,
     * 30fps=3000 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKSNUM, crusher->framerate_num);
    /* 3, 17, 28, 100, */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE, crusher->framerate_den);
    /* 3, 17, 31, 100, */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKS, crusher->framerate_den);
    /* 3, 18, */
    ActivateVideoInCfg(crusher);
    /* 3, 14, 121, 100, */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_FRAMETICKS, crusher->framerate_den);
    /* 3, 19, 11, 37500,
     * 30fps=45000 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_INITIAL_DELAY, 15*crusher->framerate_num);
    /* 3, 15 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* avcalign 0 0
     * 3, 1026, 59, 0 */
    encoderConfigure(crusher, AVE_CFG_VENC_AVCALIGN, 0);
    /* 3, 14, 138, 0 */
    encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_AVCALIGN_LEN, 0);
    /* 3, 15, 0 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* voutar %d %d */
    if (crusher->aspect_ratio_x && crusher->aspect_ratio_y) {
        /* 1, 18, 5, 16, 0 320x240,640x480:(1,18,5,1), 720x576:(1,18,5,16)*/
        sysVOutConfigure(crusher, SYS_CFG_VOUT_ASPECT_X, crusher->aspect_ratio_x);
        /* 1, 18, 6, 15, 0 320x240,640x480:(1,18,6,1) 720x576:(1,18,5,15)*/
        sysVOutConfigure(crusher, SYS_CFG_VOUT_ASPECT_Y, crusher->aspect_ratio_y);
        /* 1, 19, 15, 0 */
        sysActivateOutputCfg(crusher);
    }
    /* ------------------- */
    if(crusher->offset_x)
        encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_OFFSET_X, (crusher->width-crusher->offset_x)/2);
    if(crusher->offset_y)
        encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_OFFSET_Y, (crusher->height-crusher->offset_y)/2);
    if(crusher->offset_x||crusher->offset_y)
        ActivateVideoEncCfg(crusher);
    /* ------------------- */
    /* rawcmd 3 19 1 22
     * 3, 19, 1, 22, 0 */
    encoderConfigureRCFile(crusher, 1, 22);
    /* rawcmd 3 20
     * 3, 20, 0 */
    ActivateVideoRCCfg(crusher);
    /* ------------------- */
    /* rawcmd 3 14 2
     * 3, 14, 2, 0 */
    encoderConfigureVideoEnc(crusher, 2, 0);
    /* rawcmd 3 15
     * 3, 15, 0 */
    ActivateVideoEncCfg(crusher);
    /* ------------------- */
    if (crusher->width == 272 || crusher->width == 528 || crusher->width == 544) {
        /*rawcmd 3 17 47 1*/
        encoderConfigureVideo(crusher, 47, 1);
        /*rawcmd 3 15*/
        ActivateVideoEncCfg(crusher);
    }

    return 1;
}


int mg1264_send_capture_textconfig(crusher_t *crusher)
{
    /* 3, 1026, 10 */
    if(crusher->bitrate_mode == BITRATE_MODE_AUTO) {
        if (crusher->bitrate >= 2000000)
            encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_HIGH_BITRATE);
        else if (crusher->bitrate >= 750000)
            encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_MED_BITRATE);
        else
            encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, AVE_CFG_VENC_OPERATIONAL_MODE_LOW_BITRATE);
    } else {
        encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, crusher->bitrate_mode);
    }
    /* 3, 27, 0 */
    RawCmd(crusher, AVENCODER_CTRLOBJ_ID, CMD_OPCODE_PERFORMANCE_MODE,
                AVE_CMDP_PERFORMANCE_MODE_D1_104, 0, 0, 0, 0, 0);
    /* 1, 28, 1 */
    RawCmd(crusher, 1, 28, 1, 0, 0, 0, 0, 0);

    /* 3, 17, 9, 1 */
    encoderConfigureVideo(crusher, AVE_CFG_VIN_PROG_SOURCE, AVE_CFP_VIN_PROG_SOURCE_PROGRESSIVE);
    /* 3, 18 */
    ActivateVideoInCfg(crusher);
    /* 3, 17, 36, 1 */
    encoderConfigureVideo(crusher, AVE_CFG_VIN_FIELD_CODING, AVE_CFP_VIN_FIELD_CODING_FIELD);
    /* 3, 18 */
    ActivateVideoInCfg(crusher);

    /* 3, 17, 0, 1 */
    if(crusher->framerate_den == 3003) {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_STANDARD, AVE_CFP_VIDEO_INPUT_STANDARD_NTSC);
    } else {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_STANDARD, AVE_CFP_VIDEO_INPUT_STANDARD_PAL);
    }

    /* 3, 17, 28, 3600 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE, crusher->framerate_den);
    /* 3, 17, 31, 3600 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKS, crusher->framerate_den);
    /* 3, 17, 120, 3600 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE2, crusher->framerate_den);
    /* 3, 17, 121, 3600 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE3, crusher->framerate_den);
    /* 3, 18 */
    ActivateVideoInCfg(crusher);
    /* 1, 1026, 13, 2 */
    if(crusher->framerate_den == 3003) {
        sysConfigure(crusher, SYS_CFG_VENC_VOUT_STANDARD, SYS_CFP_VENC_VOUT_STANDARD_NTSC);
    } else {
        sysConfigure(crusher, SYS_CFG_VENC_VOUT_STANDARD, SYS_CFP_VENC_VOUT_STANDARD_PAL);
    }
    /* 1, 18, 9, 3600 */
    sysVOutConfigure(crusher, SYS_CFG_VOUT_FRAMETICKS, crusher->framerate_den);
    /* 1, 19 */
    sysActivateOutputCfg(crusher);

/* 0 |<-1->|<-----------3----------->|        720
 +-->+-----------------------------------------+
 2   |                                         |
 +-->|     +-------------------------+         |
 |   |     |                         |         |
 |   |     |                         |         |
 |   |     |                         |         |
 4   |     |                         |         |
 |   |     |                         |         |
 |   |     |                         |         |
 |   |     |                         |         |
 +-->|     +-------------------------+         |
     |                                         |
 576 +-----------------------------------------+
*/

    /* 3, 17, 1, 720 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_W, 720/* #3 */); ///< capture image width 640 for example ("3")
    /* 3, 17, 2, 576 */
    if(crusher->framerate_den == 3003) {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_H, 480/* #4 */); ///< capture image height 492 for example
    } else {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_H, 576/* #4 */); ///< capture image height 492 for example
    }
    /* 3, 17, 3 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_X_OS, 0/* #1 */); ///< x-offset for start of rectangle 32 for example
    /* 3, 17, 4 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_Y_OS, 0/* #2 */); ///< y-offset for start of rectangle 32 for example
    /* 3, 17, 5, 720, 320 */
    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_X, crusher->width); ///< encoded image width

    /* 3, 17, 6, 2 */
    /* if 1 - then AVE_CFG_VIDEO_CAPTURE_RECT_H
     * if 2 - then AVE_CFG_VIDEO_CAPTURE_RECT_H/2
     * if crusher->height, then crusher->height*/
    //encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_Y, crusher->height);
    if (crusher->height == 288 || crusher->height >= 480) {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_Y, 2);
    } else {
        encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_Y, crusher->height);
    }

    /* 3, 18 */
    ActivateVideoInCfg(crusher);
    /* 3, 1026, 1, 2 */
    if(crusher->out_format == OUT_FORMAT_QBOX) {
        encoderConfigure(crusher, AVE_CFG_BITSTREAM_TYPE, AVE_CFP_BITSTREAM_TYPE_QBOX);
    } else if (crusher->out_format == OUT_FORMAT_VES) {
        encoderConfigure(crusher, AVE_CFG_BITSTREAM_TYPE, AVE_CFP_BITSTREAM_TYPE_ELEM_VIDEO);
    } else {
        ERROR("Unknown format: %d", crusher->out_format);
        return -1;
    }
    /* 3, 1026, 17, 1 */
    encoderConfigure(crusher, AVE_CFG_ENC_AV_SELECT, AVE_CFP_ENC_SELECT_AV);
    /* 3, 1026, 40, 1 */
    encoderConfigure(crusher, AVE_CFG_ENC_INPUT_SELECT, AVE_CFP_ENC_SELECT_AV);
    /* 3, 14, 94, 1 */
    encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_SEI_PICT_TIMING, 1);
    /* 3, 15 */
    ActivateVideoEncCfg(crusher);

    /* 3, 14, 91, 1 */
    encoderConfigureVideoEnc(crusher, 91, 1);
    /* 3, 14, 92, 1000000 */
    encoderConfigureVideoEnc(crusher, 92, 1000000);
    /* 3, 14, 93, 1000000 */
    encoderConfigureVideoEnc(crusher, 93, 1000000);
    /* 3, 15 */
    ActivateVideoEncCfg(crusher);

    /* 3, 1026, 21, 500000 */
    encoderConfigure(crusher, AVE_CFG_VENC_BITRATE, crusher->bitrate);
    /* our edit */
    if(crusher->gopsize) {
        encoderConfigure(crusher, AVE_CFG_VENC_GOP_SIZE, crusher->gopsize);
    } else {
        encoderConfigure(crusher, AVE_CFG_VENC_GOP_SIZE, crusher->framerate_num/crusher->framerate_den);
    }

    /* 3, 19, 8, 1 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_ENABLED, 1);
    /* 3, 19, 9, 35000000 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_BITSIZE, crusher->rc_size);
    /* 3, 19, 10, 3500000 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_TRANSFER_BITRATE, crusher->rc_size/10);
    /* 3, 19, 11, 180000 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_INITIAL_DELAY, 180000 /* ??? */);
    /* 3, 19, 12, 0 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_LOW_DELAY, 0);
    /* 3, 20 */
    ActivateVideoRCCfg(crusher);

    /* 3, 19, 6, 0 */
    encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_ENABLER, 0);
    /* 3, 20 */
    ActivateVideoRCCfg(crusher);

    /* 3, 19, 36, 0 */
    encoderConfigureRCFile(crusher, 36, 0);
    /* 3, 20 */
    ActivateVideoRCCfg(crusher);
    /* this 2 cmds used adaptive framerate? (does not exists for youtube profile) */
    /* 3, 19, 29, 1 */
    //encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR_MIN_QP, 1);
    /* 3, 20 */
    //ActivateVideoRCCfg(crusher);

    /* 3, 1026, 67, 2 */

    if(!crusher->audio_codec) {
        ERROR("audio codec not set");
        return 1;
    }

    encoderConfigure(crusher, AVE_CFG_AENC_CODEC, crusher->audio_codec);

    /* TODO: support other samplerates */
    if(crusher->audio_samplerate != 48000) {
        ERROR("forcing 48000 Hz Audio samplerate");
        crusher->audio_samplerate = 48000;
    }

    /* 1, 1026, 7, 48000 */
    sysConfigure(crusher, SYS_CFG_AUDIO_SAMPLERATE, crusher->audio_samplerate);
    /* 1, 1026, 8, 16 */
    sysConfigure(crusher, SYS_CFG_AUDIO_SAMPLESIZE, 16);
    /* 3, 1026, 19, 1 */
    encoderConfigure(crusher, AVE_CFG_AI_CHANNELS, crusher->audio_channels == 2 ? AVE_CFP_AI_CHANNELS_STEREO : AVE_CFP_AI_CHANNELS_STEREO_MONO_LEFT);
    /* 3, 1026, 35, 128000 */
    if(crusher->audio_codec != AUDIO_CODEC_PCM) {
        encoderConfigure(crusher, AVE_CFG_AENC_BITRATE, crusher->audio_bitrate);
    }
    /* 3, 1026, 18, 1 */
    encoderConfigure(crusher, AVE_CFG_PREV_AV_SELECT, AVE_CFP_ENC_SELECT_AV);

    return 1;
}




