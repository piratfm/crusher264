/*
 * crusherRTMP.c
 *
 *  Created on: 29 апр. 2010
 *      Author: tipok
 */

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <assert.h>

#include <librtmp/rtmp.h>
#include <librtmp/log.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "crusher.h"
#include "logging.h"
#include "qbox.h"
#include "flvpkt.h"


#define BUFFER_MAXSIZE (MAX_H264_FRAMESIZE+32)

int Interrupted=0;

/***********************************************
 * Usage
 ***********************************************/
void usage(char *name)
{
  fprintf (stderr, "%s is a program to stream h.264+aac, produced by CrusherCapture device, to flash server.\n"
       "Revision (%s)"
       ".\n"
       "Usage: %s [options] [output-stream]\n"
       "Device-specific options:\n"
       "-n, --num        : Specify device number in USB bus [default=auto]\n"
       "-i, --input      : Number (or name) of input (0-composite, 1-s-video, 2-component) (default: 0-composite)\n"
       "\n"
       "Encoder-specific options:\n"
       "-s, --size       : Image size of input file (default: 640x480)\n"
       "-b, --bitrate    : Specify video output bitrate (default: 1500Kbps)\n"
       "-a, --abitrate   : Specify audio output bitrate (default: 128Kbps)\n"
       "-c, --channels   : Specify audio channels (default: 2)\n"
       "\n"
       "Application-specific options:\n"
       "-f, --file       : Write stream to file (default: don't write)\n"
       "-d, --debug      : More verbose\n"
       "-q, --qiet       : Less verbose\n"
       "-h, --help       : Show this help\n"
       "\n", name, "$Rev: 128 $", name );
}

static void signal_handler(int signum)
{
    if (signum != SIGPIPE) {
        Interrupted=signum;
        logwrite(LOG_INFO, "Exit clearly");
    }
    signal(signum,signal_handler);
}




/***********************************************
 * RTMP STUFF
 ***********************************************/
static int rtmp_flv_write(qboxContext *qbox, RTMP *rtmp, flvContext *flv, uint8_t *buffer)
{
    int pktsize = qbox2flv(qbox, flv, buffer);
    if(pktsize < 0)
        return -1;

    if (flv && !flv_write(flv, buffer, pktsize))
            return -1;
    if (rtmp && !RTMP_Write(rtmp, (char *) buffer, pktsize))
            return -1;
    return pktsize;
}

static void rtmp_log(int level, const char *fmt, va_list args)
{
    switch(level){
    default:
    case RTMP_LOGCRIT:    level = LOG_NULL;     break;
    case RTMP_LOGERROR:   level = LOG_ERROR;    break;
    case RTMP_LOGWARNING: level = LOG_WARNING;  break;
//    case RTMP_LOGINFO:    level = LOG_INFO;     break;
//    case RTMP_LOGDEBUG:   level = LOG_DEBUG;    break;
//    case RTMP_LOGDEBUG2:  level = LOG_XTREME;   break;
    }

    logwrite2(level, fmt, args);
}

/***********************************************
 * entry point
 ***********************************************/
int main (int argc, char **argv)
{
    RTMP *rtmp = NULL;
    int rc;
    flvContext flv;

    char *size = NULL;
    const char *input = NULL;
    char *rtmp_output = NULL;
    char *flv_output = NULL;
    uint8_t buffer[256000];

    int  width, height;
    qboxContext qbox;

    crusher_t           crusher;
    crusher_defaults(&crusher);
    memset(&flv, 0x00, sizeof(flvContext));

    /* own defaults */
    width = 640;
    height = 480;
    crusher.devmode = DEV_TYPE_CAPTURE;
    crusher.out_format = OUT_FORMAT_QBOX;
    crusher.video_input = CAPTURE_INPUT_COMPOSITE;
    //crusher.gopsize = 25;
    crusher.rc_size = 35000000; /* 2000000 for !cap */
    crusher.scenecut_threshold = 0;
    crusher.deblock = 0;
    crusher.bitrate = 1500000;
    crusher.audio_codec = AUDIO_CODEC_AAC;
    crusher.audio_bitrate = 128000;
    crusher.audio_channels = 2;



    /******************************************************
     * Getopt
     ******************************************************/
    const char short_options[] = "n:i:s:b:a:c:f:dqh";
    const struct option long_options[] = {
       {"num", optional_argument, NULL, 'n'},
       {"input", optional_argument, NULL, 'i'},

       {"size", optional_argument, NULL, 's'},
       {"bitrate", optional_argument, NULL, 'b'},
       {"abitrate", optional_argument, NULL, 'a'},
       {"channels", optional_argument, NULL, 'c'},

       {"file", optional_argument, NULL, 'f'},
       {"debug", no_argument, NULL, 'd'},
       {"quet", no_argument, NULL, 'q'},
       {"help", no_argument, NULL, 'h'},
       {0, 0, 0, 0}
    };
    int c, option_index = 0;

    if (argc < 1) {
        usage (argv[0]);
        exit(-1);
    }

    while (1) {
        c = getopt_long (argc, argv, short_options,
           long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'n':
                crusher.devnum = atoi(optarg);
                break;
            case 'i':
                input = optarg;
                break;

            case 's':
                size = optarg;
                break;
            case 'b':
                crusher.bitrate = atoi(optarg);
                break;
            case 'a':
                crusher.audio_bitrate = atoi(optarg);
                break;
            case 'c':
                crusher.audio_channels = atoi(optarg);
                break;

            case 'f':
                flv_output = optarg;
            case 'd':
                loglevel++;
                break;
            case 'q':
                loglevel--;
                break;
            case 'h':
                usage (argv[0]);
                exit(-1);
                break;
        }
    }

    if(loglevel > LOG_XTREME)
        loglevel = LOG_XTREME;
    if(loglevel < LOG_NULL)
        loglevel = LOG_NULL;
    

    if (optind+1 <= argc) {
        rtmp_output = strdup(argv[optind]);
        logwrite(LOG_DEBUG, "RTMP output to: %s", rtmp_output);
        rtmp = calloc(1, sizeof(RTMP));
        memset(rtmp, 0x00, sizeof(RTMP));
    }

    if(flv_output) {
        flv_open(&flv, flv_output);
        logwrite(LOG_DEBUG, "FLV output to: %s", flv_output);
    }

    if(!flv_output && !rtmp_output) {
        usage (argv[0]);
        goto cleanup;
    }


    char *size_h;
    char *pch;
    
    if(size) {
        size_h = strdup(size);
        pch = strtok (size_h,"x");
        if (pch != NULL) {
            width = atoi(pch);
            pch = strtok (NULL, "");
            if (pch != NULL)
                height = atoi(pch);
        }
    }

    if(input) {
        if(strcmp(input,"composite")==0){
            crusher.video_input = CAPTURE_INPUT_COMPOSITE;
        } else if(strcmp(input,"s-video")==0) {
            crusher.video_input = CAPTURE_INPUT_SVIDEO;
        } else if(strcmp(input,"component")==0) {
            crusher.video_input = CAPTURE_INPUT_COMPONENT;
        } else {
            crusher.video_input = input ? atoi(input) : 0;
            if(crusher.video_input < 0 || crusher.video_input > 2) {
                usage (argv[0]);
                goto cleanup;
            }
        }
    }
    logwrite(LOG_INFO, "input: %d", crusher.video_input);



    crusher.width = width;
    crusher.height = height;
    logwrite(LOG_DEBUG, "Picture size: %dx%d", crusher.width, crusher.height);

    // Setup signal handlers
    if (signal(SIGHUP, signal_handler) == SIG_IGN) signal(SIGHUP, SIG_IGN);
    if (signal(SIGINT, signal_handler) == SIG_IGN) signal(SIGINT, SIG_IGN);
    if (signal(SIGTERM, signal_handler) == SIG_IGN) signal(SIGTERM, SIG_IGN);

    crusher.loglevel = loglevel;

    if( !crusher_find_device(&crusher, 1) ) {
        logwrite(LOG_ERROR, "device not found");
        goto cleanup;
    }

    if(crusher.devtype != DEV_TYPE_CAPTURE) {
        logwrite(LOG_ERROR, "This is not capture device");
        goto cleanup;
    }


    switch (loglevel) {
    default:
    case LOG_NULL:    rc = RTMP_LOGCRIT;    break;
    case LOG_ERROR:   rc = RTMP_LOGERROR;   break;
    case LOG_WARNING: rc = RTMP_LOGWARNING; break;
//    case LOG_INFO:    rc = RTMP_LOGINFO;    break;
//    case LOG_DEBUG:   rc = RTMP_LOGDEBUG;   break;
//    case LOG_XTREME:  rc = RTMP_LOGDEBUG2;  break;
    }

    if (rtmp) {
        RTMP_LogSetLevel(rc);
        RTMP_LogSetCallback(rtmp_log);

        RTMP_Init(rtmp);
        if (!RTMP_SetupURL(rtmp, rtmp_output))
            goto cleanup;

        rtmp->Link.protocol |= RTMP_FEATURE_WRITE;
        if (!RTMP_Connect(rtmp, NULL) || !RTMP_ConnectStream(rtmp, 0))
            goto cleanup;
    }


    if( !crusher_init_device(&crusher) ) {
        logwrite(LOG_ERROR, "init failed");
        goto cleanup;
    }

    /* before sending textconfig, initialize
     * path crusher->textconfig to textconfig file.
     * */
    if( !crusher_send_textconfig(&crusher) ) {
        logwrite(LOG_ERROR, "sending textconfig failed");
        goto cleanup;

    }

    int hdrsize = flv_hdr(&crusher, &flv, buffer);
    logwrite(LOG_DEBUG, "FLV: hdr=%d", hdrsize);
    flv_write(&flv, buffer, hdrsize);
    if (rtmp)
        RTMP_Write(rtmp, (char *) buffer, hdrsize);

    crusher_start(&crusher);

    int i;
    int ret2 = CODEC_FAIL;
    
    do {
        /* if we have no data anymore, just give some loops to request encoded data. */
        ret2 = crusher_encode(&crusher, NULL, 0);
        if (ret2 == CODEC_FLUSHED && crusher.out_blocks) {
            for(i=0;i<crusher.out_blocks;i++) {
                if(!qbox_parse(&qbox, crusher.out_data[i].data, crusher.out_data[i].len, QBOX_STRICT_NALS) ||
                   !rtmp_flv_write(&qbox, rtmp, &flv, buffer))
                    goto cleanup;
            }
        }
        if(ret2 == CODEC_FINISHED)
            break;

    } while ((ret2 == CODEC_FLUSHED ||ret2 == CODEC_FRAME_SENT) && ret2 != CODEC_FAIL && !Interrupted);

cleanup:
    flv_close(&flv);
    if (rtmp) {
        RTMP_Close(rtmp);
        free(rtmp);
        free(rtmp_output);
    }
    crusher_close(&crusher);
    exit(0);
}
