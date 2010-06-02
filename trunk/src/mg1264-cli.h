/*
 * mg1264-cli-help.h
 *
 *  Created on: 16 окт. 2009
 *      Author: tipok
 */

#ifndef MG1264CLI_H_
#define MG1264CLI_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef CODEC_CLI

/* mobiapp: */
#define RAWCMD_DESCR "Send a raw command to the codec specifying opcode and arguments directly."
#define STREAMTYPE_DESCR "Set mux/demux stream type : streamtype [\"ves\" | \"qbox\" | \"aes\" | \"mp4\"]"
#define AVSELECT_DESCR "Set audio or video only encoding: avselect [\"av\" | \"a\" | \"v\"]"
#define PREVAVSELECT_DESCR "Set audio or video only preview: prevavselect [\"av\" | \"a\" | \"v\"]"
#define INPUTSELECT_DESCR "Set audio or video input only: inputselect [av | a | v]"
#define ENCOMODE_DESCR "set encoder modestr <cfgfilename>"
#define PROGSOURCE_DESCR "set VPP source material to interlaced/progressive: progsource [0/1]"

#define VBITRATE_DESCR "Set video bitrate"
#define VGOPSIZE_DESCR "Set GOP size"
#define VSCNEWGOP_DESCR "Enable new GOP on scene change: vscnewgop [0|1]"

#define VRCSIZE_DESCR "Set rate control file size params: vrcsize <enable> <bit tolerance>"
#define VRCBUFFER_DESCR "Set rate control buffer model params: vrcbuffer <enable> <bit size> <transfer bitrate> <initial delay> <low delay>"

#define VFIELDCODING_DESCR "Set interlaced coding mode: vfieldcoding [field | frame | aff]"
#define VFIELDORDER_DESCR "Set video field order [tff | bff]"
#define VOUTSCALE_DESCR "enable/disable vout scaling: voutscale <enable>"
#define VINTF_DESCR "Configure video input temporal filter : vintf <level 0-3> <luma attenuation 0-7> <chroma attenuation <0-7>"
#define VRECTCAP_DESCR "Set VPP capture rectangle: vrectcap <width> <height> <x offset> <y offset>"
#define VRECTDISP_DESCR "Set VPU display rectangle: vrectdisp <width> <height> <x offset> <y offset>"
#define VSEIPT_DESCR "Set SEI picture timing: vseipt <enable>"
#define ENCPERF_DESCR "set encoder performance mode"

/* crusher.dll: */
#define AUDIOBITRATE_DESCR "Set audio bitrate"
#define AUDIOENC_DESCR "Set audio encoder: audioenc [\"pcm\" | \"aac\" | \"qma\"]"
#define AUDIOINPUT_DESCR "Set audio input: [s | ss | ml | mr]"
#define AUDIOOUTPUT_DESCR "Set audio output: <numchannels> <samplerate> <samplesize>"
#define AVCALIGN_DESCR "AV Codec output sample alignment (default 4: word-aligned; 0: no align)"
#define ECHOCMD_DESCR "Create an echo event which has first payload same as first arg : echocmd <num>"
#define HB_DESCR "Enable/disable heartbeat"
#define LLATENCY_DESCR "set low latency operation mode 0/1"
#define PVSENC_DESCR "Switch reconstructed frame preview on/off: pvsenc [0/1]"
#define VDEBLOCK_DESCR "Set encoder deblocking filter stength: vdeblock <enable> <alpha> <beta>"
#define VENCFRAMETICKS_DESCR "Set minimum frame length using 90 KHz clock for video encoder: vencframeticks <ticks>"
#define VFORCENEWGOP_DESCR "Force a new GOP immediately"
#define VINAR_DESCR "Set video input pixel aspect ratio: vinar <x> <y> <force>"
#define VINDEC_DESCR "set video input decimation, zeroes to disable: vindec <horizontal> <vertical>"
#define VINFRAMERATE_DESCR "Set video input framerate in ticks per frame: vinframerate <ticks> <override>"
#define VINITPROGSCALE_DESCR "Set interlaced to progressive vertical decimation: vinintprogscale <enable> <use hardware>"
#define VINHUESAT_DESCR "set hue/saturation as N/256 (256 == 1x): vinhuesat <Cb Ka> <Cb Kb> <Cr Kc> <Cr Kd>"
#define VINOUTTRAMETICKS_DESCR "Set length of frames sent by VPP, for < 30 FPS encode, in 90 KHz PTS: vinoutframeticks <ticks>"
#define VINSTANDART_DESCR "Set video input to NTSC/PAL: vinstandard [ntsc | pal]"
#define VNUMSLICES_DESCR "Set number of slices per frame : vnumslices <slices>"
#define VOUTAR_DESCR "Set video output pixel aspect ratio: voutar <x> <y>"
#define VOUTFRAMETICKS_DESCR "Set frame display ticks: voutframeticks <ticks>"
#define VOUTSTANDART_DESCR "Set video output standard to NTSC/PAL: voutstandard [ntsc | pal]"
#define VRCAFR_DESCR "Set adaptive framerate: vrcafr <enable> <min QP> <max QP> <scaling denominator> <scaling min numerator>"
#define VSC_DESCR "'Enable/disable scene change and I slice on scene change: vsc <enable> <I slice>"
#define VSPSLEVEL_DESCR "Set SPS H.264 Level e.g. 21, 30: vspslevel <level>"
#define VTELECINE_DESCR "Set 24fps telecine detection for interlaced to progressive scaling mode: vtelecine <enable> <drop frames>"
#define VVUTIMING_DESCR "Enable VUI timing information: vvuitiming <enable> <num_units_in_tick> <fixed_frame_rate_flag>"
#define ZOOM_DESCR "Zoom : zoom <N/256> <x offset> <y offset>"

#define _DESCR ""












/* mobiapp: */
static int cliRawCmd(crusher_t *crusher, int args, char **argstr);

static int cliSetVEncStreamType(crusher_t *crusher, int args, char **argstr);
static int cliEncAVSelect(crusher_t *crusher, int args, char **argstr);
static int cliEncPreVAVSelect(crusher_t *crusher, int args, char **argstr);
static int cliEncInputSelect(crusher_t *crusher, int args, char **argstr);
static int cliEncMode(crusher_t *crusher, int args, char **argstr);
static int cliProgSource(crusher_t *crusher, int args, char **argstr);

static int cliVEncBitrate(crusher_t *crusher, int args, char **argstr);
static int cliVEncGOPSize(crusher_t *crusher, int args, char **argstr);
static int cliVEncSceneChangeNewGOP(crusher_t *crusher, int args, char **argstr);

static int cliVRCFileSize(crusher_t *crusher, int args, char **argstr);
static int cliVRCBuffer(crusher_t *crusher, int args, char **argstr);

static int cliVFieldCoding(crusher_t *crusher, int args, char **argstr);
static int cliVFieldOrder(crusher_t *crusher, int args, char **argstr);
static int cliVOutscale(crusher_t *crusher, int args, char **argstr);
static int cliSetVideoInParams(crusher_t *crusher, int args, char **argstr);
static int cliSetVideoRectCap(crusher_t *crusher, int args, char **argstr);
static int cliSetVideoRectDisp(crusher_t *crusher, int args, char **argstr);
static int cliVEncSEIPictureTiming(crusher_t *crusher, int args, char **argstr);
static int cliEncPerformanceMode(crusher_t *crusher, int args, char **argstr);

/* crusher.dll: */
static int cliAudioBitrate(crusher_t *crusher, int args, char **argstr);
static int cliAudioEnc(crusher_t *crusher, int args, char **argstr);
static int cliAudioInput(crusher_t *crusher, int args, char **argstr);
static int cliAudioOutput(crusher_t *crusher, int args, char **argstr);
static int cliAVCAlign(crusher_t *crusher, int args, char **argstr);
static int cliEchoCmd(crusher_t *crusher, int args, char **argstr);
static int cliHB(crusher_t *crusher, int args, char **argstr);
static int cliLlatency(crusher_t *crusher, int args, char **argstr);
static int cliPvsEnc(crusher_t *crusher, int args, char **argstr);
static int cliVDeblock(crusher_t *crusher, int args, char **argstr);
static int cliVEncFrameTicks(crusher_t *crusher, int args, char **argstr);
static int cliVForceNewGOP(crusher_t *crusher, int args, char **argstr);
static int cliVInAR(crusher_t *crusher, int args, char **argstr);
static int cliVInDec(crusher_t *crusher, int args, char **argstr);
static int cliVInFrameRate(crusher_t *crusher, int args, char **argstr);
static int cliVInInitProgScale(crusher_t *crusher, int args, char **argstr);
static int cliVInHueSat(crusher_t *crusher, int args, char **argstr);
static int cliVinOutFrameTicks(crusher_t *crusher, int args, char **argstr);
static int cliVinStandard(crusher_t *crusher, int args, char **argstr);
static int cliVNumSlices(crusher_t *crusher, int args, char **argstr);
static int cliVOutAR(crusher_t *crusher, int args, char **argstr);
static int cliVOutFrameTicks(crusher_t *crusher, int args, char **argstr);
static int cliVOutStandard(crusher_t *crusher, int args, char **argstr);
static int cliVRCaFr(crusher_t *crusher, int args, char **argstr);
static int cliVSC(crusher_t *crusher, int args, char **argstr);
static int cliVSPSLevel(crusher_t *crusher, int args, char **argstr);
static int cliVTelecine(crusher_t *crusher, int args, char **argstr);
static int cliVVuTiming(crusher_t *crusher, int args, char **argstr);
static int cliZoom(crusher_t *crusher, int args, char **argstr);


int mg1264_exec_cmd(crusher_t *crusher, char *cmd, int args_num, char **conf_args);


#endif

#endif /* MG1264CLI_H_ */
