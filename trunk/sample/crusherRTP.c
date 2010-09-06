/*
 * crusherRTP.c
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "crusher.h"
#include "logging.h"
#include "qbox.h"
#include "rtppkt.h"


#define BUFFER_MAXSIZE (MAX_H264_FRAMESIZE+32)

int Interrupted=0;

/***********************************************
 * Usage
 ***********************************************/
void usage(char *name)
{
  fprintf (stderr, "%s is a program to stream h.264+aac, produced by CrusherCapture device, to specified RTP address.\n"
       "Revision (%s)"
       ".\n"
       "Usage: %s [options] [address:port]\n"
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
       "-d, --debug      : More verbose\n"
       "-q, --qiet       : Less verbose\n"
       "-h, --help       : Show this help\n"
       "\n"
       "Network-specific options:\n"
       "-S, --sdp        : SDP-file destination (default: stdout)\n"
       "-l, --latm       : send aac-audio in rfc3016 (latm) format (default: no)\n"
       "-t, --ttl        : time-to-live (default: OS-specified)\n"
       "address:port     : Specify video output address and port, audio stream will be port+2 (default: 239.100.100.1:1234)"
       "\n", name, "$Rev$", name );
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
 * entry point
 ***********************************************/
int main (int argc, char **argv)
{
    rtp_priv_t *rtp = NULL;

    char *size = NULL;
    const char *input = NULL;
    char *rtp_output = "239.100.100.1";
    char *sdp_output = NULL;
    int rtp_port = 1234;
    int rtp_ttl = 0;
    int latm = 0;

    int  width, height;
    qboxContext qbox;

    crusher_t           crusher;
    crusher_defaults(&crusher);

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
    const char short_options[] = "n:i:s:S:b:a:c:f:ldqh";
    const struct option long_options[] = {
       {"num", optional_argument, NULL, 'n'},
       {"input", optional_argument, NULL, 'i'},

       {"size", optional_argument, NULL, 's'},
       {"bitrate", optional_argument, NULL, 'b'},
       {"abitrate", optional_argument, NULL, 'a'},
       {"channels", optional_argument, NULL, 'c'},

       {"sdp", no_argument, NULL, 'S'},
       {"latm", no_argument, NULL, 'l'},
       {"ttl", no_argument, NULL, 't'},
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

            case 'S':
                sdp_output = optarg;
                break;
            case 'l':
                latm = 1;
                break;
            case 't':
                rtp_ttl = atoi(optarg);
                break;
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
        rtp_output = strdup(argv[optind]);
        logwrite(LOG_DEBUG, "RTP output to: %s", rtp_output);
        char *doublpx = strchr(rtp_output, ':');
        if(doublpx) {
            rtp_port = atoi(doublpx+1);
            doublpx[0] = '\0';
        }
    }

    rtp = calloc(1, sizeof(rtp_priv_t));
    memset(rtp, 0x00, sizeof(rtp_priv_t));
    if(!rtp_open(rtp, rtp_output, rtp_port, rtp_ttl)){
        goto cleanup;
    }

    if(sdp_output){
        rtp->sdp_file = fopen(sdp_output, "w");
    } else {
        rtp->sdp_file = stdout;
    }

    rtp->bit_rate_audio = crusher.audio_bitrate;
    rtp->audio_sample_rate = crusher.audio_samplerate;
    rtp->audio_channels = crusher.audio_channels;
    rtp->bit_rate_video = crusher.bitrate;
    rtp->audio_rtp_latm = latm;

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

    crusher_start(&crusher);

    int i;
    int ret2 = CODEC_FAIL;
    
    do {
        /* if we have no data anymore, just give some loops to request encoded data. */
        ret2 = crusher_encode(&crusher, NULL, 0);
        if (ret2 == CODEC_FLUSHED && crusher.out_blocks) {
            for(i=0;i<crusher.out_blocks;i++) {
                if(qbox_parse(&qbox, crusher.out_data[i].data, crusher.out_data[i].len, QBOX_STRICT_NALS)){

                    if(qbox.sample_stream_type != SAMPLE_TYPE_H264 &&
                       qbox.sample_stream_type != SAMPLE_TYPE_AAC) {
                        logwrite(LOG_ERROR, "unknown qbox payload: 0x%08x", qbox.sample_stream_type);
                        continue;
                    }

                    if(qbox.sample_flags & SAMPLE_FLAGS_CONFIGURATION_INFO) {
                        rtp_add_sdp(rtp, qbox.data, qbox.qbox_size,
                                (qbox.sample_stream_type == SAMPLE_TYPE_H264) ? 0 : 1);
                    } else {
                        rtp_write(rtp, qbox.data, qbox.qbox_size, qbox.sample_cts,
                                (qbox.sample_stream_type == SAMPLE_TYPE_H264) ? 0 : 1);
                    }
                } else {
                    goto cleanup;
                }
            }
        }
        if(ret2 == CODEC_FINISHED)
            break;

    } while ((ret2 == CODEC_FLUSHED ||ret2 == CODEC_FRAME_SENT) && ret2 != CODEC_FAIL && !Interrupted);

cleanup:
    rtp_close(rtp);
    crusher_close(&crusher);
    exit(0);
}
