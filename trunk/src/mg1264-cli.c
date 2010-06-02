/*
 * mg1264-cli.c
 *
 *  Created on: 15 окт. 2009
 *      Author: tipok
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef CODEC_CLI
#include <string.h>
#include "crusher.h"
#include "logging.h"
#include "mg1264.h"
#include "mg1264-io.h"
#include "mg1264-cli.h"


/* TODO: implemented only minimal needed commands,
 *  if needed more, disassemble "mobiapp" and crusher.dll */


static const struct {
	int  min_args;
	int  max_args;
	int (*function)(crusher_t *crusher, int args, char **argstr);
	char *command;
	char *descr;
	char *help;
} commands[] = {
	/* mobiapp: */
	{ 1, 8, cliRawCmd, "rawcmd", RAWCMD_DESCR },

	{ 1, 1, cliSetVEncStreamType, "streamtype", STREAMTYPE_DESCR },
	{ 1, 1, cliEncAVSelect, "avselect", AVSELECT_DESCR },
	{ 1, 1, cliEncPreVAVSelect, "prevavselect", PREVAVSELECT_DESCR },
    { 1, 1, cliEncInputSelect, "inputselect",  INPUTSELECT_DESCR },
    { 1, 1, cliEncMode, "encmode", ENCOMODE_DESCR },
    { 1, 1, cliProgSource, "progsource", PROGSOURCE_DESCR },

    { 1, 1, cliVEncBitrate, "vbitrate", VBITRATE_DESCR },
    { 1, 1, cliVEncGOPSize, "vgopsize", VGOPSIZE_DESCR },
    { 1, 1, cliVEncSceneChangeNewGOP, "vscnewgop", VSCNEWGOP_DESCR },

    { 1, 3, cliVRCFileSize, "vrcsize", VRCSIZE_DESCR },
    { 1, 3, cliVRCBuffer, "vrcbuffer", VRCBUFFER_DESCR },

    { 1, 1, cliVFieldCoding, "vfieldcoding", VFIELDCODING_DESCR },
    { 1, 1, cliVFieldOrder, "vfieldorder", VFIELDORDER_DESCR },
    { 1, 1, cliVOutscale, "voutscale", VOUTSCALE_DESCR },
    { 1, 4, cliSetVideoInParams, "vintf", VINTF_DESCR },
    { 1, 4, cliSetVideoRectCap, "vrectcap", VRECTCAP_DESCR },
    { /* not 3! */2, 4, cliSetVideoRectDisp, "vrectdisp", VRECTDISP_DESCR },
    { 1, 1, cliVEncSEIPictureTiming, "vseipt", VSEIPT_DESCR },
    { 1, 1, cliEncPerformanceMode, "encperf", ENCPERF_DESCR },

    /* crusher.dll: */
    { 1, 1, cliAudioBitrate, "abitrate", AUDIOBITRATE_DESCR },
    { 1, 1, cliAudioEnc, "audioenc", AUDIOENC_DESCR },
    { 1, 1, cliAudioInput, "audioinput", AUDIOINPUT_DESCR },
    { 1, 1, cliAudioOutput, "audiooutput",AUDIOOUTPUT_DESCR },
    { 1, 1, cliAVCAlign, "avcalign", AVCALIGN_DESCR },
    { 1, 1, cliEchoCmd, "echocmd", ECHOCMD_DESCR },
    { 1, 1, cliHB, "hb", HB_DESCR },
    { 1, 1, cliLlatency, "llatency", LLATENCY_DESCR },
    { 1, 1, cliPvsEnc, "pvsenc", PVSENC_DESCR },
    { 1, 1, cliVDeblock, "vdeblock", VDEBLOCK_DESCR },
    { 1, 1, cliVEncFrameTicks, "vencframeticks", VENCFRAMETICKS_DESCR },
    { 1, 1, cliVForceNewGOP, "vforcegop", VFORCENEWGOP_DESCR },
    { 1, 1, cliVInAR, "vinar", VINAR_DESCR },
    { 1, 1, cliVInDec, "vindec", VINDEC_DESCR },
    { 1, 1, cliVInFrameRate, "vinframerate", VINFRAMERATE_DESCR },
    { 1, 1, cliVInInitProgScale, "vinintprogscale", VINITPROGSCALE_DESCR },
    { 1, 1, cliVInHueSat, "vinhuesat", VINHUESAT_DESCR },
    { 1, 1, cliVinOutFrameTicks, "vinoutframeticks", VINOUTTRAMETICKS_DESCR },
    { 1, 1, cliVinStandard, "vinstandard", VINSTANDART_DESCR },
    { 1, 1, cliVNumSlices, "vnumslices", VNUMSLICES_DESCR },
    { 1, 1, cliVOutAR, "voutar", VOUTAR_DESCR },
    { 1, 1, cliVOutFrameTicks, "voutframeticks", VOUTFRAMETICKS_DESCR },
    { 1, 1, cliVOutStandard, "voutstandard", VOUTSTANDART_DESCR },
    { 1, 1, cliVRCaFr, "vrcafr", VRCAFR_DESCR },
    { 1, 1, cliVSC, "vsc", VSC_DESCR },
    { 1, 1, cliVSPSLevel, "vspslevel", VSPSLEVEL_DESCR },
    { 1, 1, cliVTelecine, "vtelecine", VTELECINE_DESCR },
    { 1, 1, cliVVuTiming, "vvuitiming", VVUTIMING_DESCR },
    { 1, 1, cliZoom, "zoom", ZOOM_DESCR },

    { 0, 0, NULL, NULL, NULL }

};

/*
 * do stuff functions
 * */
static int cliRawCmd(crusher_t *crusher, int args, char **argstr)
{	/* rawcmd 1 28 1
	 * { 1, 28, 1, 0, 0, 0, 0, 0 }; */
	command_t cmd;
	uint32_t *raw = (uint32_t *) &cmd;
	int i;
	memset(&cmd, 0, sizeof(command_t));

	for (i=0; i<args; i++){
		*(raw + i) = atoi(argstr[i]);
	}
	return mg1264_cmd(crusher, &cmd);
}

static int cliSetVEncStreamType(crusher_t *crusher, int args, char **argstr)
{
	/* streamtype ves/qbox (cliSetVEncStreamType)
	 * { 3, 1026, 1, 2, 0, 1, 28, 1 };
	 * a=EnableQboxMeta
	 * 3, 31, a, 0
	 *
	 * 3, 14, 138, x
	 * 3, 41, 2, x
	 * */
	/*
	 * if mp4 {
	 * 1 2 1
	 *
	 * */
	int val = 0;

	if (strcmp("ves", argstr[0]) == 0) {
		val = AVE_CFP_BITSTREAM_TYPE_ELEM_VIDEO;
	} else if (strcmp("qbox", argstr[0]) == 0) {
		val = AVE_CFP_BITSTREAM_TYPE_QBOX;
	} else if (strcmp("aes", argstr[0]) == 0) {
		val = AVE_CFP_BITSTREAM_TYPE_ELEM_AUDIO;
	} else if (strcmp("mp4", argstr[0]) == 0) {
		val = AVE_CFP_BITSTREAM_TYPE_ELEM_MP4;
	} else {
		ERROR("Invalid stream type: %s", argstr[1]);
		return 0;
	}
	return encoderConfigureStreamType(crusher, val);
}
//ok
static int cliEncAVSelect(crusher_t *crusher, int args, char **argstr)
{
	/* avselect v (cliEncAVSelect)
	 * { 3, 1026, 17, 2, 0, 1, 28, 1 }; */
	/*uint32_t cmd[8] = { 3, 1026, 17, 0, 0, 0, 0, 0 };*/
	int val = 0;

	if (strcmp("av", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AV;
	} else if (strcmp("v", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_VIDEO_ONLY;
	} else if (strcmp("a", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AUDIO_ONLY;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return encoderConfigure(crusher, AVE_CFG_ENC_AV_SELECT, val);
}
//ok
static int cliEncPreVAVSelect(crusher_t *crusher, int args, char **argstr)
{
	/* avselect v (cliEncAVSelect)
	 * { 3, 1026, 17, 2, 0, 1, 28, 1 }; */
	/*uint32_t cmd[8] = { 3, 1026, 17, 0, 0, 0, 0, 0 };*/
	int val = 0;

	if (strcmp("av", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AV;
	} else if (strcmp("v", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_VIDEO_ONLY;
	} else if (strcmp("a", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AUDIO_ONLY;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return encoderConfigure(crusher, AVE_CFG_ENC_PREV_AV_SELECT, val);
}
//ok
static int cliEncInputSelect(crusher_t *crusher, int args, char **argstr)
{
	/* inputselect v (cliEncInputSelect)
	 * { 3, 1026, 40, 2, 0, 1, 28, 1 }; */
	/*uint32_t cmd[8] = { 3, 1026, 40, 0, 0, 0, 0, 0 };*/
	int val = 0;
	if (strcmp("av", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AV;
	} else if (strcmp("v", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_VIDEO_ONLY;
	} else if (strcmp("a", argstr[0]) == 0) {
		val = AVE_CFP_ENC_SELECT_AUDIO_ONLY;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return encoderConfigure(crusher, AVE_CFG_ENC_INPUT_SELECT, val);
}
//ok
static int cliEncMode(crusher_t *crusher, int args, char **argstr)
{
	/* encmode low
	 * { 3, 1026, 10, 0, 0, 1026, 40, 2 }; */
	/*uint32_t cmd[8] = { 3, 1026, 10, 0, 0, 0, 0, 0 };*/
	int val = 0;
	if (strcmp("low", argstr[0]) == 0) {
		val = AVE_CFG_VENC_OPERATIONAL_MODE_LOW_BITRATE;
	} else if (strcmp("medium", argstr[0]) == 0) {
		val = AVE_CFG_VENC_OPERATIONAL_MODE_MED_BITRATE;
	} else if (strcmp("high", argstr[0]) == 0) {
		val = AVE_CFG_VENC_OPERATIONAL_MODE_HIGH_BITRATE;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return encoderConfigure(crusher, AVE_CFG_VENC_OPERATIONAL_MODE, val);
}
//ok
static int cliProgSource(crusher_t *crusher, int args, char **argstr)
{
	/* progsource 1 (cliProgSource)
	 * { 3, 17, 9, 1, 0, 0, 0, 0 };*/
	/*uint32_t cmd[8] = { 3, 17, 9, 0, 0, 0, 0, 0 };*/
	int arg = atoi(argstr[0]);
	int val = 0;
	if (arg == 0) {
		val = AVE_CFP_VIN_PROG_SOURCE_INTERLACED;
	} else if (arg == 1) {
		val = AVE_CFP_VIN_PROG_SOURCE_PROGRESSIVE;
	} else {
		ERROR("Invalid mode: %d", arg);
		return 0;
	}

	encoderConfigureVideo(crusher, AVE_CFG_VIN_PROG_SOURCE, val);
	return ActivateVideoInCfg(crusher);
}

static int cliVEncBitrate(crusher_t *crusher, int args, char **argstr)
{
	/* vbitrate 2000000 (cliVEncBitrate)
	 * { 3, 1026, 21, 2000000, 0, 0, 0, 0 }; */
	/*uint32_t cmd[8] = { 3, 1026, 21, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	return encoderConfigure(crusher, AVE_CFG_VENC_BITRATE, val);
}
//ok
static int cliVEncGOPSize(crusher_t *crusher, int args, char **argstr)
{
	/* vgopsize 15 (cliVEncGOPSize)
	 * { 3, 1026, 24, 15, 0, 0, 0, 0 }; */
	/*uint32_t cmd[8] = { 3, 1026, 24, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	return encoderConfigure(crusher, AVE_CFG_VENC_GOP_SIZE, val);
}
//ok
static int cliVEncSceneChangeNewGOP(crusher_t *crusher, int args, char **argstr)
{
	/* experimental ???? */
	/*uint32_t cmd[8] = { 3, 14, 115, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SCENECHANGE_NEWGOP, val);
	return ActivateVideoEncCfg(crusher);
}
//ok
static int cliVRCFileSize(crusher_t *crusher, int args, char **argstr)
{
	/* vrcsize 1 2000000 (cliRCFileSize)
	 * <enable> <deviation> <allowviolation>
	 * { 3, 19, 6, 1, 0, 0, 0, 0 };
	 * { 3, 19, 7, 2000000, 6, 1, 0, 0 };
	 * { 3, 20, 0, 0, 1, 0, 0, 0 };*/
	/*uint32_t cmd[8] = { 3, 19, 0, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_ENABLER, val);

	if (args > 1){
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_SIZE, val);
	}

	if (args > 2){
		val = atoi(argstr[2]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCFILE_VIOLATIONS, val);
	}

	return ActivateVideoRCCfg(crusher);
}

static int cliVRCBuffer(crusher_t *crusher, int args, char **argstr)
{
	/* vrcbuffer <enable> <bit size> <transfer bitrate> <initial delay> <low delay> */
	int val = atoi(argstr[0]);
	encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_ENABLED, val);
	if (args > 1){
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_BITSIZE, val);
	}

	if (args > 2){
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_TRANSFER_BITRATE, val);
	}

	if (args > 3){
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_INITIAL_DELAY, val);
	}

	if (args > 4){
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RCBUFFER_LOW_DELAY, val);
	}

	return ActivateVideoRCCfg(crusher);
}
//ok
static int cliVFieldCoding(crusher_t *crusher, int args, char **argstr)
{
	/* vfieldcoding frame
	 * { 3, 17, 36, 0, 3, 20, 0, 0 };
	 * { 3, 18, 3, 20, 0, 0, 1, 0 }; */
	/*uint32_t cmd[8] = { 3, 17, 36, 0, 0, 0, 0, 0 };*/
	int val = 0;
	if (strcmp("frame", argstr[0]) == 0) {
		val = AVE_CFP_VIN_FIELD_CODING_FRAME;
	} else if (strcmp("field", argstr[0]) == 0) {
		val = AVE_CFP_VIN_FIELD_CODING_FIELD;
	} else if (strcmp("aff", argstr[0]) == 0) {
		val = AVE_CFP_VIN_FIELD_CODING_ADAPTIVE;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	encoderConfigureVideo(crusher, AVE_CFG_VIN_FIELD_CODING, val);
	return ActivateVideoInCfg(crusher);
}

static int cliVFieldOrder(crusher_t *crusher, int args, char **argstr)
{
	/* vfieldcoding frame
	 * { 3, 17, 36, 0, 3, 20, 0, 0 };
	 * { 3, 18, 3, 20, 0, 0, 1, 0 }; */
	/*uint32_t cmd[8] = { 3, 17, 36, 0, 0, 0, 0, 0 };*/
	int val = 0;
	if (strcmp("tff", argstr[0]) == 0) {
		val = AVE_CFP_VIN_FIELD_ORDER_TFF;
	} else if (strcmp("bff", argstr[0]) == 0) {
		val = AVE_CFP_VIN_FIELD_ORDER_BFF;
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FIELD_ORDER, val);
	return ActivateVideoInCfg(crusher); /* two times? */
}


//ok
static int cliVOutscale(crusher_t *crusher, int args, char **argstr)
{
	/* voutscale 0
	 * { 1, 1026, 14, 0, 0, 0, 1, 0 }; */
	/*uint32_t cmd[8] = { 1, 1026, 14, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	return sysConfigure(crusher, SYS_CFG_VENC_VOUT_SCALE, val);
}
//TODO: check it again!
static int cliSetVideoInParams(crusher_t *crusher, int args, char **argstr)
{
	/* vintf <level 0-3> <luma attenuation=> <chroma attenuation=>
	 * (SetVideoInPara)
	 * { 3, 17, 10, 0, 14, 0, 0, 0 };
	 * { 3, 17, 11, 0, 10, 0, 14, 0 };
	 * { 3, 17, 12, 0, 11, 0, 10, 0 };
	 * { 3, 17, 15, 0, 12, 0, 11, 0 };
	 * { 3, 18, 0, 0, 0, 107296664, 15, 0 };*/
	/*uint32_t cmd[8] = { 3, 17, 0, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VINTF_FILTER, val);

	if (args > 1)
		val = atoi(argstr[1]);
	else
		val = 0;
	encoderConfigureVideo(crusher, AVE_CFG_VINTF_CHROMA_ATTEN, val);

	if (args > 2)
		val = atoi(argstr[2]);
	else
		val = 0;
	encoderConfigureVideo(crusher, AVE_CFG_VINTF_LUMA_ATTEN, val);

	if (args > 3)
		val = atoi(argstr[3]);
	else
		val = 0;
	encoderConfigureVideo(crusher, AVE_CFG_VINTF_FILTER_P3, val);
	return ActivateVideoInCfg(crusher);
}


static int cliSetVideoRectCap(crusher_t *crusher, int args, char **argstr)
{
	/* vrectcap 640 480
	 * { 3, 17, 1, 640, 18, 0, 0, 0 };
	 * { 3, 17, 2, 480, 1, 640, 18, 0 };
	 * { 3, 17, 3, 0, 2, 480, 1, 640 };
	 * { 3, 17, 4, 0, 3, 0, 2, 480 };
	 * ActivateVideoInCfg
	 * { 3, 18, 0, 0, 0, 0, 0, 0 }; */
	/*uint32_t cmd[8] = { 3, 17, 0, 0, 0, 0, 0, 0 };*/
	int val = 0;

	if (args == 3) {
		ERROR("vrectcap parameters must be 2 or 4!");
		return 0;
	}

	val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_W, val);

	val = atoi(argstr[1]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_H, val);

	if (args == 4)
		val = atoi(argstr[2]);
	else
		val = 0;
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_X_OS, val);

	if (args == 4)
		val = atoi(argstr[3]);
	else
		val = 0;
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_CAPTURE_RECT_Y_OS, val);
	return ActivateVideoInCfg(crusher);;
}

//ok
static int cliSetVideoRectDisp(crusher_t *crusher, int args, char **argstr)
{
	/* vrectdisp 640 480
	 * { 1, 6, 640, 480, 0, 0, 0, 0 }; */
	/*uint32_t cmd[8] = { 1, 6, 0, 0, 0, 0, 0, 0 };*/
	int width = atoi(argstr[0]);
	int height = atoi(argstr[1]);
	int offset_x = 0;
	int offset_y = 0;

	if (args != 2 && args != 4 ) {
		ERROR("vrectdisp parameters must be 2 or 4!");
		return 0;
	}

	if (args == 4) {
		offset_x = atoi(argstr[2]);
		offset_y = atoi(argstr[3]);
	}

	return videoRectDisp(crusher, width, height, offset_x, offset_y);
}
//ok
static int cliVEncSEIPictureTiming(crusher_t *crusher, int args, char **argstr)
{
	/* Set SEI picture timing: vseipt <enable>
	 * vseipt 0
	 * { 3, 14, 94, 0, 0, 0, 1, 0 };
	 * ActivateVideoEncCfg
	 * { 3, 15, 0, 0, 1, 0, 1, 0 };*/
	/*uint32_t cmd[8] = { 3, 14, 94, 0, 0, 0, 0, 0 };*/
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_SEI_PICT_TIMING, val);
	return ActivateVideoEncCfg(crusher);
}
//ok
static int cliEncPerformanceMode(crusher_t *crusher, int args, char **argstr)
{
	/* (cliEncPerformanceMode)
	 * { 3, 27, 4, 107303069, 0, 3, 15, 0 };*/
	/*uint32_t cmd[8] = { 3, 27, 0, 0, 0, 0, 0, 0 };*/
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_PERFORMANCE_MODE;

	if (strcmp("d1_104", argstr[0]) == 0) {
		cmd.arguments[0] = AVE_CMDP_PERFORMANCE_MODE_D1_104;
	} else if (strcmp("d1_81_nopv", argstr[0]) == 0) {
		cmd.arguments[0] = AVE_CMDP_PERFORMANCE_MODE_D1_81_NOPV;
	} else if (strcmp("qvga_81", argstr[0]) == 0) {
		cmd.arguments[0] = AVE_CMDP_PERFORMANCE_MODE_QVGA_81;
	} else if (strcmp("full_duplex_104", argstr[0]) == 0) {
		cmd.arguments[0] = AVE_CMDP_PERFORMANCE_MODE_FD_104;
	} else if (strcmp("astro_hvga", argstr[0]) == 0) {
		cmd.arguments[0] = AVE_CMDP_PERFORMANCE_MODE_ASTRO_HVGA;
	} else {
		ERROR("Invalid performance mode: %s", argstr[0]);
		return 0;
	}

	return mg1264_cmd(crusher, &cmd);
}

/******************************************
 * CrusherDLL.dll
 ******************************************/
static int cliAudioBitrate(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	return encoderConfigure(crusher, AVE_CFG_AENC_BITRATE, val);
};

static int cliAudioEnc(crusher_t *crusher, int args, char **argstr)
{
	int val = 0;
	if (strcmp("pcm", argstr[0]) == 0) {
		val = AVE_CFP_AENC_CODEC_PCM;
	} else if (strcmp("aac", argstr[0]) == 0) {
		val = AVE_CFP_AENC_CODEC_AAC;
	} else if (strcmp("qma", argstr[0]) == 0) {
		val = AVE_CFP_AENC_CODEC_QMA;
	} else {
		ERROR("Invalid audio codec: %s", argstr[0]);
		return 0;
	}
	return encoderConfigure(crusher, AVE_CFG_AENC_CODEC, val);
};

static int cliAudioInput(crusher_t *crusher, int args, char **argstr)
{
	int val = 0;
	if (strcmp("s", argstr[0]) == 0) {
		val = AVE_CFP_AI_CHANNELS_STEREO;
	} else if (strcmp("ss", argstr[0]) == 0) {
		val = AVE_CFP_AI_CHANNELS_STEREO_SWAP;
	} else if (strcmp("ml", argstr[0]) == 0) {
		val = AVE_CFP_AI_CHANNELS_STEREO_MONO_LEFT;
	} else if (strcmp("mr", argstr[0]) == 0) {
		val = AVE_CFP_AI_CHANNELS_STEREO_MONO_RIGHT;
	} else {
		ERROR("Invalid audio codec input: %s", argstr[0]);
		return 0;
	}
	return encoderConfigure(crusher, AVE_CFG_AI_CHANNELS, val);
};

static int cliAudioOutput(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	sysConfigure(crusher, SYS_CFG_AUDIO_SAMPLERATE, val);
	val = atoi(argstr[1]);
	sysConfigure(crusher, SYS_CFG_AUDIO_SAMPLESIZE, val);
	return 1;
};

static int cliAVCAlign(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigure(crusher, AVE_CFG_VENC_AVCALIGN, val);
	val = atoi(argstr[1]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SCENECHANGE_NEWGOP, val);
	return ActivateVideoEncCfg(crusher);
};

//cli_ddirection
//cli_difpause
//cli_diplay
//cli_dpause
//cli_dresume
//cli_dsscan
//cli_dumpframe - if needed
//

static int cliEchoCmd(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	return sysEcho(crusher, val);
};

//cli_hb
static int cliHB(crusher_t *crusher, int args, char **argstr)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_SUBSCRIBE_EVENT;
	if(!atoi(argstr[0]))
		cmd.opcode = 1025;
	return mg1264_cmd(crusher, &cmd);
};

static int cliLlatency(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	return sysConfigure(crusher, SYS_CFG_LLATENCY, val);
};

//TODO: cli_memquery
//cli_play
static int cliPvsEnc(crusher_t *crusher, int args, char **argstr)
{
	int val = 1;
	if(atoi(argstr[0]) == 0)
		val = 2;
	return sysConfigure(crusher, AVE_CFG_PVS_ENCODE, val);
};

//cli_qccrd = csrr
//cli_qccwr = csrw

//TODO: cli_query
//cli_record

static int cliVDeblock(crusher_t *crusher, int args, char **argstr)
{
	/* Set encoder deblocking filter stength: vdeblock <enable> <alpha> <beta> */

	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_ENABLED, val);
	if (args > 1) {
		val = atoi(argstr[1]);
		encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_ALPHA, val);
	}
	if (args > 2) {
		val = atoi(argstr[2]);
		encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_DEBLOCK_BETA, val);
	}

	return ActivateVideoEncCfg(crusher);
};


static int cliVEncFrameTicks(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_FRAMETICKS, val);
	return ActivateVideoEncCfg(crusher);
};

static int cliVForceNewGOP(crusher_t *crusher, int args, char **argstr)
{
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = AVENCODER_CTRLOBJ_ID;
	cmd.opcode = CMD_OPCODE_VIDEO_FORCE_NEW_GOP;
	return mg1264_cmd(crusher, &cmd);
}

//cli_vgammalut - direct cmd
//cli_vfilerec

static int cliVInAR(crusher_t *crusher, int args, char **argstr)
{
	/* Set video input pixel aspect ratio: vinar <x> <y> <force> */
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_ASPECT_RATIO_X, val);

	val = atoi(argstr[1]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_ASPECT_RATIO_Y, val);

	val = atoi(argstr[2]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_ASPECT_RATIO_FORCE, val);
	return ActivateVideoInCfg(crusher);
};

static int cliVInDec(crusher_t *crusher, int args, char **argstr)
{
	/* set video input decimation, zeroes to disable: vindec <horizontal> <vertical> */
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_X, val);

	val = atoi(argstr[1]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_DECIMATION_Y, val);
	return ActivateVideoInCfg(crusher);
};

static int cliVInFrameRate(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE, val);
	return ActivateVideoInCfg(crusher);
};

/* TODO: undone */
static int cliVInInitProgScale(crusher_t *crusher, int args, char **argstr)
{	/* Set interlaced to progressive vertical decimation: vinintprogscale <enable> <use hardware>*/
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_INTERLACED_PROGSCALE, val);
	return ActivateVideoInCfg(crusher);
};

static int cliVInHueSat(crusher_t *crusher, int args, char **argstr)
{	/* set hue/saturation as N/256 (256 == 1x): vinhuesat <Cb Ka> <Cb Kb> <Cr Kc> <Cr Kd>' */
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_HUESAT_CBKA, val);
	val = atoi(argstr[1]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_HUESAT_CBKB, val);
	val = atoi(argstr[2]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_HUESAT_CRKC, val);
	val = atoi(argstr[3]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_HUESAT_CRKD, val);
	return ActivateVideoInCfg(crusher);
}

static int cliVinOutFrameTicks(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKS, val);
	return ActivateVideoInCfg(crusher);
}

static int cliVinStandard(crusher_t *crusher, int args, char **argstr)
{
	if (strcmp("ntsc", argstr[0]) == 0) {
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_STANDARD, AVE_CFP_VIDEO_INPUT_STANDARD_NTSC);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE, AVE_CFP_VIDEO_STANDARD_NTSC_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKS, AVE_CFP_VIDEO_STANDARD_NTSC_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE2, AVE_CFP_VIDEO_STANDARD_NTSC_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE3, AVE_CFP_VIDEO_STANDARD_NTSC_RATE);
	} else if (strcmp("pal", argstr[0]) == 0) {
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_STANDARD, AVE_CFP_VIDEO_INPUT_STANDARD_PAL);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE, AVE_CFP_VIDEO_STANDARD_PAL_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_INOUT_FRAME_TICKS, AVE_CFP_VIDEO_STANDARD_PAL_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE2, AVE_CFP_VIDEO_STANDARD_PAL_RATE);
	    encoderConfigureVideo(crusher, AVE_CFG_VIDEO_IN_FRAMERATE3, AVE_CFP_VIDEO_STANDARD_PAL_RATE);
	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return ActivateVideoInCfg(crusher);
}

static int cliVNumSlices(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SLICES_NUM, val);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SLICES_NUM1, val);
	return ActivateVideoEncCfg(crusher);
}

//TODO: activate something...
static int cliVOutAR(crusher_t *crusher, int args, char **argstr)
{
	/* voutar <x> <y> */
	int val = atoi(argstr[0]);
	sysVOutConfigure(crusher, SYS_CFG_VOUT_ASPECT_X, val);
	val = atoi(argstr[1]);
	sysVOutConfigure(crusher, SYS_CFG_VOUT_ASPECT_Y, val);
	return sysActivateOutputCfg(crusher);
};

//TODO: activate something...
static int cliVOutFrameTicks(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	sysVOutConfigure(crusher, SYS_CFG_VOUT_FRAMETICKS, val);
	return sysActivateOutputCfg(crusher);
};

static int cliVOutStandard(crusher_t *crusher, int args, char **argstr)
{
	if (strcmp("ntsc", argstr[0]) == 0) {
	    sysConfigure(crusher, SYS_CFG_VENC_VOUT_STANDARD, SYS_CFP_VENC_VOUT_STANDARD_NTSC);
	    sysVOutConfigure(crusher, SYS_CFG_VOUT_FRAMETICKS, AVE_CFP_VIDEO_STANDARD_NTSC_RATE);

	} else if (strcmp("pal", argstr[0]) == 0) {
	    sysConfigure(crusher, SYS_CFG_VENC_VOUT_STANDARD, SYS_CFP_VENC_VOUT_STANDARD_PAL);
	    sysVOutConfigure(crusher, SYS_CFG_VOUT_FRAMETICKS, AVE_CFP_VIDEO_STANDARD_PAL_RATE);

	} else {
		ERROR("Invalid mode: %s", argstr[0]);
		return 0;
	}

	return sysActivateOutputCfg(crusher);
}

static int cliVRCaFr(crusher_t *crusher, int args, char **argstr)
{
	/* Set adaptive framerate:
	 * vrcafr <enable> <min QP> <max QP> <scaling denominator> <scaling min numerator> */
	int val = atoi(argstr[0]);
	encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR, val);

	if (args > 2) {
		val = atoi(argstr[1]);
		encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR_MIN_QP, val);
		val = atoi(argstr[2]);
		encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR_MAX_QP, val);
	}

	if (args > 3){
		val = atoi(argstr[3]);
		encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR_SCAL_DENOM, val);
	}

	if (args > 4){
		val = atoi(argstr[4]);
		encoderConfigureRCFile(crusher, AVE_CFG_RC_ADAPTIVE_FR_SCAL_MIN_NUMERATOR, val);
	}

	return ActivateVideoRCCfg(crusher);
}


static int cliVSC(crusher_t *crusher, int args, char **argstr)
{
	/* Enable/disable scene change and I slice on scene change: vsc <enable> <I slice> */
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SCENECHANGE, val);
	val = atoi(argstr[1]);
	encoderConfigureVideoEnc(crusher, AVE_CFG_VENC_SCENECHANGE_ISLICE, val);
	return ActivateVideoEncCfg(crusher);
}

static int cliVSPSLevel(crusher_t *crusher, int args, char **argstr)
{
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_SPS_LEVEL, val);
	return ActivateVideoEncCfg(crusher);
}

static int cliVTelecine(crusher_t *crusher, int args, char **argstr)
{
	/* Set 24fps telecine detection for interlaced to progressive
	 * scaling mode: vtelecine <enable> <drop frames> */
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_TELECINE, val);
	val = atoi(argstr[1]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_TELECINE_DROPFRAMES, val);
	return ActivateVideoEncCfg(crusher);
}

static int cliVVuTiming(crusher_t *crusher, int args, char **argstr)
{
	/* timing information: vvuitiming <enable> <num_units_in_tick> <fixed_frame_rate_flag> */
	int val = atoi(argstr[0]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING, val);
	val = atoi(argstr[1]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING_NUM_UNITS_IN_TICK, val);
	val = atoi(argstr[2]);
	encoderConfigureVideoEnc(crusher, AVE_CMD_VENC_VUTIMING_FIXED_FRAMERATE, val);
	return ActivateVideoEncCfg(crusher);
}

static int cliZoom(crusher_t *crusher, int args, char **argstr)
{
	/* Zoom : zoom <N/256> <x offset> <y offset> */
	command_t cmd;
	memset(&cmd, 0, sizeof(command_t));
	cmd.controlObjectId = SYSTEM_CTRLOBJ_ID;
	cmd.opcode = SYS_CMD_OPCODE_ZOOM;
	cmd.arguments[0] = atoi(argstr[0]);
	cmd.arguments[1] = atoi(argstr[1]);
	cmd.arguments[2] = atoi(argstr[2]);
	return mg1264_cmd(crusher, &cmd);
}




/*
 * command execution function
 * it calls all stuff functions, depends on cmd.
 * */
int mg1264_exec_cmd(crusher_t *crusher, char *cmd, int args_num, char **conf_args)
{
	int i = 0;
	do {
		if(!strcmp(commands[i].command, cmd)) {
			//DEBUG( "got commands[%i] = %s", i, cmd);
			if (args_num > commands[i].max_args) {
				ERROR( "too much arguments for command %s, must be no more than %d",
						cmd, commands[i].max_args);
				return 0;
			}

			if (args_num < commands[i].min_args) {
				ERROR( "too low arguments for command %s, must be no less than %d",
						cmd, commands[i].min_args);
				return 0;
			}
			//DEBUG( "command %s have ptr=%u", cmd, commands[i].function);
			commands[i].function(crusher, args_num, conf_args);
			//DEBUG("Done");
			return 1;
		}

		i++;
	} while (commands[i].command);

	return 1;
}


int mg1264_cli_parse_textconfig(crusher_t *crusher)
{
	FILE	*textconfig_h = NULL;
	char conf_line[128];
	char *conf_args[16];
	char *pch;
	char *cmd;
	int args_num;

	textconfig_h = fopen(crusher->textcfg, "r");
	if(!textconfig_h) {
		ERROR("Open file %s failed.", crusher->textcfg);
		return 0;
	}

	/*while( read_str(conf_line, textconfig_h) ){*/
	while( fgets(conf_line, 127, textconfig_h) ){

		if(!strlen(conf_line) || conf_line[0] == '#')
			continue;

		args_num = 0;
		pch = strtok (conf_line," \t\r\n");
		cmd = pch;
		if(!cmd)
			break;

		while(pch && (args_num < 16)){
			pch = strtok (NULL, " \t\r\n");
			conf_args[args_num] = pch;
			if(conf_args[args_num])
				args_num++;
		};

		XTREME("textconfig: cmd:\"%s\", args:%d "
		  "\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %s", cmd, args_num,
			args_num > 0 ? conf_args[0] : "",
			args_num > 1 ? conf_args[1] : "",
			args_num > 2 ? conf_args[2] : "",
			args_num > 3 ? conf_args[3] : "",
			args_num > 4 ? conf_args[4] : "",
			args_num > 5 ? "...":"" );

		if(!mg1264_exec_cmd(crusher, cmd, args_num, conf_args)) {
			ERROR("textconfig: cmd:\"%s\", args:%d FAILED!");
			return 0;
		}
	}
	return 1;
}

#endif
