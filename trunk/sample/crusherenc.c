
#include <usb.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crusher.h"
#include "imgConverter.h"
#include "logging.h"
#include "qbox.h"
#include "adts.h"
#include "wav.h"

int Interrupted=0;

/***********************************************
 * Usage
 ***********************************************/
void usage(char *name)
{
  fprintf (stderr, "%s is a program to control usb h.264 encoder.\n"
        "This is \"Crusher\" reference design platform control application (%s).\n"
        "Usage: %s [options] [output-file]\n"
        "-m, --mode       : Input mode: encoder, encoderhd, capture (default=based on usb id)\n"
        "\n"
        "-s, --size       : Image size of input file (default: 640x480)\n"
        "-b, --bitrate    : Specify output bitrate (default 1500Kbps)\n"
        "-o, --outfile    : Specify output file location (alternative of [output-file])\n"
        "-f, --format     : Otput file format: qbox, es (default: es for encoder, qbox for capture)\n"
        "-n, --num        : Specify device number in USB bus [default=auto]\n"
        "\n"
        "Encoder specific options:\n"
        "-i, --input      : Specify input YUV420p file location [default=STDIN]\n"
        "\n"
        "HD Encoder specific options:\n"
        "-i, --input      : Specify input mpeg file location [default=STDIN]\n"
        "\n"
        "Capture-specific options:\n"
        "-i, --input      : Number (or name) of input (0-composite, 1-s-video, 2-component) (default: 0-composite)\n"
        "-C, --acodec     : Specify audio codec (pcm,aac,mp2) (default: aac)\n"
        "-a, --abitrate   : Specify audio output bitrate (no matter for pcm) (default: 128Kbps)\n"
        "-c, --channels   : Specify audio channels (default: 2)\n"
        "-g, --again      : Specify audio gain (default: 0), -120...120 (step: 5)\n"
        "\n"
        "-d, --debug      : More verbose\n"
        "-q, --qiet       : Less verbose\n"
        "-h, --help       : Show this help\n"
        "\n", name, "$Rev: 126 $", name );
}

static void signal_handler(int signum)
{
    if (signum != SIGPIPE) {
        Interrupted=signum;
        logwrite(LOG_INFO, "Exit clearly");
    }
    signal(signum,signal_handler);
}



int main (int argc, char **argv)
{
	char *infile = NULL;
	char *outfile = NULL;
    int  ret2 = CODEC_FAIL;
	char *size = NULL;
	int  format = OUT_FORMAT_VES;
    int  acodec = AUDIO_CODEC_AAC;
	int  width, height;
    FILE *outfh = NULL;
    qboxContext qbox;
	/* used in capture */
    FILE *outfh_audio = NULL;
	/* used in encoder*/
	FILE *infh = NULL;
	int ret = CONVERTER_FAIL;
	uint8_t *inbuff = NULL;
	int framesize = 0;
    imgConverter_t converter;


	crusher_t			crusher;
	crusher_defaults(&crusher);

	/* own defaults */
	width = 640;
	height = 480;
	crusher.bitrate = 1500000;

	/******************************************************
	 * Getopt
	 ******************************************************/
    const char short_options[] = "m:s:b:o:f:r:n:i:a:g:C:c:dqh";
    const struct option long_options[] = {
       {"mode", optional_argument, NULL, 'm'},

       {"size", optional_argument, NULL, 's'},
       {"bitrate", optional_argument, NULL, 'b'},
       {"outfile", optional_argument, NULL, 'o'},

       {"format", optional_argument, NULL, 'f'},
       {"num", optional_argument, NULL, 'n'},
       {"debug", no_argument, NULL, 'd'},
       {"quet", no_argument, NULL, 'q'},
       {"help", no_argument, NULL, 'h'},

       {"channels", optional_argument, NULL, 'c'},
       {"acodec", optional_argument, NULL, 'C'},
       {"samplerate", optional_argument, NULL, 'r'},
       {"abitrate", optional_argument, NULL, 'a'},
       {"again", optional_argument, NULL, 'g'},
       {"input", optional_argument, NULL, 'i'},
       {0, 0, 0, 0}
    };
	int c, option_index = 0;
	 
	if (argc == 1) {
		usage (argv[0]);
		exit(-1);
	}
	
	while (1) {
		c = getopt_long (argc, argv, short_options,
	       long_options, &option_index);
		if (c == -1)
			break;
	
        switch (c) {
            case 'm':
                if(strcmp(optarg,"encoder")==0){
                    crusher.devmode = DEV_TYPE_ENCODER;
                } else if(strcmp(optarg,"capture")==0) {
                    crusher.devmode = DEV_TYPE_CAPTURE;
                } else if(strcmp(optarg,"encoderhd")==0) {
                    crusher.devmode = DEV_TYPE_ENCODER_HD;
                } else {
                    usage (argv[0]);
                    exit(-1);
                };
                break;
            case 's':
                size = optarg;
                break;
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'f':
                if(strcmp(optarg,"qbox")==0) {
                    format = OUT_FORMAT_QBOX;
                } else if(strcmp(optarg,"es")==0) {
                    format = OUT_FORMAT_VES;
                } else {
                    usage (argv[0]);
                    exit(-1);
                };
                break;
            case 'n':
                crusher.devnum = atoi(optarg);
                break;
            case 'C':
                if(strcmp(optarg,"aac")==0) {
                    acodec = AUDIO_CODEC_AAC; ///< default
                } else if(strcmp(optarg,"pcm")==0) {
                    acodec = AUDIO_CODEC_PCM;
                } else if(strcmp(optarg,"mp2")==0) {
                    acodec = AUDIO_CODEC_MP2;
                } else {
                    usage (argv[0]);
                    exit(-1);
                };
                break;
            case 'c':
                crusher.audio_channels = atoi(optarg);
                break;
            case 'a':
                crusher.audio_bitrate = atoi(optarg);
                break;
            case 'g':
                crusher.audio_gain = atoi(optarg);
                break;
            case 'r':
                crusher.audio_samplerate = atoi(optarg);
                break;
            case 'b':
                crusher.bitrate = atoi(optarg);
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

    if (optind+1 <= argc)
        outfile = argv[optind];

    if(outfile==NULL)
        fail(LOG_ERROR, "Specify output file!");

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

	crusher.width = width;
	crusher.height = height;
	logwrite(LOG_DEBUG, "Picture size: %dx%d", crusher.width, crusher.height);

	// Setup signal handlers
	if (signal(SIGHUP, signal_handler) == SIG_IGN) signal(SIGHUP, SIG_IGN);
	if (signal(SIGINT, signal_handler) == SIG_IGN) signal(SIGINT, SIG_IGN);
	if (signal(SIGTERM, signal_handler) == SIG_IGN) signal(SIGTERM, SIG_IGN);

	/* not currently implemented */
    crusher.loglevel = loglevel;

	logwrite(LOG_DEBUG, "IN: %s, OUT: %s dev=%d fmt=%s",
					infile, outfile, crusher.devnum,
					format==OUT_FORMAT_VES?"es":"qbox");
	/* Find device,
	 * if second arg ==1, then fx2 firmware will be uploaded to usbcpu.
	 * */
	if( !crusher_find_device(&crusher, 1) ) {
		logwrite(LOG_DEBUG, "device not found");
		crusher_close(&crusher);
		return -1;
	}



    if (strcmp(outfile, "-") != 0){
        outfh = fopen(outfile, "w");
    } else {
        outfh = stdout;
    }
    if(!outfh){
        logwrite(LOG_ERROR, "OPENING OUTPUT FILE FAILED");
        exit(0);
    }


    if (crusher.devmode == DEV_TYPE_ENCODER) {
        crusher.out_format = format;
        logwrite(LOG_INFO, "You have encoder device");
        if (strcmp(infile, "-") != 0){
            infh = fopen(infile, "r");
        } else {
            infh = stdin;
        }
        if(!infh){
            logwrite(LOG_ERROR, "OPENING INPUT FILE FAILED");
            exit(0);
        }

        /* initialize converter */
        framesize = width*height + 2*(width/2*height/2);
        inbuff = malloc(framesize);

        converter.y_width   = width;
        converter.y_height  = height;
        converter.y_stride  = width;
        converter.uv_width  = width / 2;
        converter.uv_height = height / 2;
        converter.uv_stride = width / 2;

        converter.y = inbuff;
        converter.u = inbuff + width*height;
        converter.v = inbuff + width*height + (width/2*height/2);

        imgConverter_init(&converter);

        /* need to know frame length to allocate inner buffer */
        crusher.inputFrameLen = converter.iyuv_frame_size_d;
    } else if (crusher.devmode == DEV_TYPE_CAPTURE) {
        /* capture-based tunes */
        crusher.out_format = OUT_FORMAT_QBOX;
        //crusher.gopsize = 25;
        crusher.rc_size = 35000000; /* 2000000 for !cap */
        crusher.audio_codec = acodec;
        crusher.scenecut_threshold = 0;
        crusher.deblock = 0;

        logwrite(LOG_INFO, "You have capture device");
        if(strcmp(infile,"composite")==0){
            crusher.video_input = CAPTURE_INPUT_COMPOSITE;
        } else if(strcmp(infile,"s-video")==0) {
            crusher.video_input = CAPTURE_INPUT_SVIDEO;
        } else if(strcmp(infile,"component")==0) {
            crusher.video_input = CAPTURE_INPUT_COMPONENT;
        } else {
            crusher.video_input = infile ? atoi(infile) : 0;
            if(crusher.video_input < 0 || crusher.video_input > 2) {
                usage (argv[0]);
                exit(-1);
            }
        }
        logwrite(LOG_INFO, "input: %d", crusher.video_input);

        /* save audio in different file */
        if(format != OUT_FORMAT_QBOX) {
            char *audiofile = malloc(strlen(outfile) + 4);
            strcpy(audiofile, outfile);
            switch (acodec) {
                case AUDIO_CODEC_AAC:
                    strcpy(audiofile + strlen(outfile), ".aac");
                    outfh_audio = fopen(audiofile, "w");
                    break;
                case AUDIO_CODEC_PCM:
                    strcpy(audiofile + strlen(outfile), ".wav");
                    outfh_audio = fopen(audiofile, "w");
                    wav_init(outfh_audio, crusher.audio_channels, crusher.audio_samplerate, 16, 0);
                    break;
                case AUDIO_CODEC_MP2:
                    logwrite(LOG_ERROR, "MPEG2 audio format selected, but it still not working propertly!");
                    strcpy(audiofile + strlen(outfile), ".mp2");
                    outfh_audio = fopen(audiofile, "w");
                    break;

            }
            logwrite(LOG_INFO, "audio saved to: %s", audiofile);
            free(audiofile);

            if(!outfh_audio){
                logwrite(LOG_ERROR, "OPENING AUDIO OUTPUT FILE FAILED");
                exit(0);
            }
        }

    } else if (crusher.devmode == DEV_TYPE_ENCODER) {
        crusher.out_format = format;
        framesize = 128*1024;
        logwrite(LOG_INFO, "You have HD-encoder device");
        if (strcmp(infile, "-") != 0){
            infh = fopen(infile, "r");
        } else {
            infh = stdin;
        }
        if(!infh){
            logwrite(LOG_ERROR, "OPENING INPUT FILE FAILED");
            exit(0);
        }

        /* initialize converter */
        inbuff = malloc(framesize);

        /* need to know frame length to allocate inner buffer */
        crusher.inputFrameLen = framesize;
    } else {
        logwrite(LOG_INFO, "You have unknown device");
    }

	/*  textconfig or vgaconfig  may be here */
	logwrite(LOG_XTREME, "usbdevice=%d", crusher.udev);


	//crusher->framesToLoad = 1;

	/* before init device, we have to know vgaconfig file.
	 * if it does not set, then try to create it, depends on video size (WxH)
	 * */
	if( !crusher_init_device(&crusher) ) {
		logwrite(LOG_DEBUG, "init failed");
		crusher_close(&crusher);
		return -1;
	}

	/* before sending textconfig, initialize
	 * path crusher->textconfig to textconfig file.
	 * */
	if( !crusher_send_textconfig(&crusher) ) {
		logwrite(LOG_DEBUG, "sending textconfig failed");
		crusher_close(&crusher);

	}

	crusher_start(&crusher);

	int finished = 0;
	int i;
	do {

		if(crusher.devmode == DEV_TYPE_ENCODER && !finished){
			if(!fread(inbuff, framesize, 1, infh))
				finished=1;

			ret = imgConverter_convert(&converter);
			if (ret != CONVERTER_FINISHED)
				break;

			ret2 = crusher_encode(&crusher, converter.iyuv_blocked, converter.iyuv_frame_size_d);
		} else {
			/* if we have no data anymore, just give some loops to request encoded data. */
			ret2 = crusher_encode(&crusher, NULL, 0);
		}

		if (ret2 == CODEC_FLUSHED && crusher.out_blocks) {
            for(i=0;i<crusher.out_blocks;i++) {
                if(crusher.out_format == OUT_FORMAT_QBOX && format != OUT_FORMAT_QBOX) {
                        if(qbox_parse(&qbox, (void *) crusher.out_data[i].data, crusher.out_data[i].len, QBOX_MAKE_STARTCODES)) {
                            switch(qbox.sample_stream_type) {
                                case SAMPLE_TYPE_H264:
                                    fwrite(qbox.data, 1, qbox.qbox_size, outfh);
                                    break;
                                case SAMPLE_TYPE_AAC:
                                    if(qbox.sample_flags & SAMPLE_FLAGS_CONFIGURATION_INFO) {
                                        adts_init(1, crusher.audio_samplerate, crusher.audio_channels);
                                    } else {
                                        adts_up_write(outfh_audio, qbox.qbox_size);
                                        fwrite(qbox.data, 1, qbox.qbox_size, outfh_audio);
                                    }
                                    break;
                                case SAMPLE_TYPE_PCM:
                                    wav_write_bytes(outfh_audio, qbox.data, qbox.qbox_size);
                                    break;
                                case SAMPLE_TYPE_QMA:
                                    fwrite(qbox.data, 1, qbox.qbox_size, outfh_audio);
                                    break;
                                default:
                                    logwrite(LOG_ERROR, "unknown qbox payload: 0x%08x", qbox.sample_stream_type);
                                    break;
                            }
                        } else {
                            logwrite(LOG_ERROR, "bad qbox");
                            ret2 = CODEC_FAIL;
                        }
                } else {
                    /* write raw qbox or es */
                    fwrite((void *) crusher.out_data[i].data, 1, crusher.out_data[i].len, outfh);
                }
            }
		}
		if(ret2 == CODEC_FINISHED)
			break;

	} while ((ret2 == CODEC_FLUSHED ||ret2 == CODEC_FRAME_SENT) && ret2 != CODEC_FAIL && !Interrupted);

	if (crusher.devtype == DEV_TYPE_ENCODER) {
	    imgConverter_free(&converter);
	    free(inbuff);
	    fclose(infh);
	} else if(crusher.out_format == OUT_FORMAT_QBOX && format != OUT_FORMAT_QBOX) {
	    if(acodec == AUDIO_CODEC_PCM)
	        wav_exit(outfh_audio);
	    fclose(outfh_audio);
	}

	crusher_close(&crusher);

	fclose(outfh);
    return 1;
}
