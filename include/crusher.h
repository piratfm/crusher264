#ifndef CRUSHER_H_
#define CRUSHER_H_

#include <stdio.h>
#include <stdint.h>

#define FX2_Firmware "firmware/FX2Firmware.hex"
#define FX2_CaptureFirmware "firmware/VideoRecorder.hex"
#define MG1264_Firmware "firmware/qmmapp.seve1-eb-16mb.bin"
#define MG1264_CaptureFirmware "firmware/cap.qmmapp.seve1-eb-16mb.bin"

#define MAX_H264_FRAMESIZE 131072
#define DEFAULT_LOAD_FRAMES 2

enum {
	CODEC_FAIL,
	CODEC_FRAME_SENT,
	CODEC_FLUSHED,
	CODEC_FINISHED,
};

enum {
    CAPTURE_INPUT_COMPOSITE,
    CAPTURE_INPUT_SVIDEO,
    CAPTURE_INPUT_COMPONENT,
};

enum {
	OUT_FORMAT_VES = 1,
	OUT_FORMAT_QBOX,
};

enum {
    DEV_TYPE_AUTO,
    DEV_TYPE_ENCODER,
    DEV_TYPE_CAPTURE,
};

enum {
    AUDIO_CODEC_PCM = 1,
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_MP2,
};

enum {
    BITRATE_MODE_LOW,
    BITRATE_MODE_MEDIUM,
    BITRATE_MODE_HIGH,
    BITRATE_MODE_AUTO,
};

typedef struct {
	uint32_t len;
	uint8_t *data;
} StreamBlock_t;

/* main device handler */
typedef struct {
	uint8_t loglevel;
	uint8_t devnum;
	uint8_t devtype; ///< physical device type (capture or encoder)
	uint8_t devmode; ///< used mode: capture or encoder NOTE: devtype and devmode - are different!
	/* usb device handler,
	 * HACK: void used instead usb_dev_handle because usb.h header don't needed here */
	void	*udev;
	/* firmwares */
	char *fx2fw;
	char *mg1264fw;
	/* global pointer block */
	uint32_t gpb;
	/* default vgaconfig */
	uint32_t *mware_data;

	/* configs */
	char *mwarecfg;
	char *textcfg;

	int framesToLoad;
	uint8_t *input_buffer;
	uint32_t inputFrameLen;
	int inputFramesFilled;

	int width;
	int height;

	/* used only in CrusherCapture */
    int audio_codec;           ///< pcm, aac, qma default: aac
	int audio_channels;        ///< 1, 2 default: 2 (if 1 channel selected, only left-channel will be activated)
    int audio_samplerate;      ///< 22050, 32000, 44100, 48000 default: 48000
	int audio_bitrate;         ///< 64000, 128000 default: 128000
	int8_t audio_gain;            ///< -120..+120, step=5 default: 0 (-12dB ... +12dB )

	/* used only in CrusherCapture */
    uint8_t video_adc_status;
	int video_input;      ///< 0 - s-video, 1 - composit, 2 - component default: 0
	int video_brightness;      ///< 0..256 default: 0
	int video_contrast;        ///< 0..256 default: 128
	int video_saturation_Cb;   ///< 0..256 default: 128
	int video_saturation_Cr;   ///< 0..256 default: 128

	/* TODO: undone! */
	int offset_x;
	int offset_y;

	/* TODO: undone! */
	int aspect_ratio_x;
	int aspect_ratio_y;

	int bitrate;
	int bitrate_mode;
	int framerate_num;
    int framerate_den;
	int gopsize;
	int rc_size;
	int scenecut_threshold;
	int deblock;
	int deblock_alpha;
	int deblock_beta;

	int out_format;

	/* device specific */
	void *lastEvent;
	void *blockConfig;
	uint32_t blockConfigSize;
	uint32_t blockConfigId;
	int event_complete;
	uint32_t lastBlockPtr;

	StreamBlock_t out_data[6];
	int out_blocks;
    int finished;
} crusher_t;

/**
 * \brief initialize crusher_t structure, and add defaults values
 * \param crusher struct
 */
void crusher_defaults(crusher_t *crusher);


/**
 * \brief Searching USB device with devnum, if presended,
 *        open it and return 1 if ok.
 * \param crusher struct
 * \param upload firmware if device w/o fw found
 * \return 1 of device opened.
 */
int crusher_find_device(crusher_t *crusher, int upload_fw);


/**
 * \brief Init crusher device.
 *        This function uploads mg1264 firmware
 *        returns 1 on success.
 * \param crusher struct
 * \return status of init device.
 */
int crusher_init_device(crusher_t *crusher);


/**
 * \brief Send textconfig to device.
 *        This function translate text config params to mg1264 commands
 *        and sends them to device
 *        returns 1 on success.
 * \param crusher struct
 * \return status of sending textconfig.
 */
int crusher_send_textconfig(crusher_t *crusher);


/**
 * \brief Close crusher device.
 * \param crusher struct
 */
void crusher_close(crusher_t *crusher);


/**
 * \brief Uploading FX2 LOADER & FIRMWARE.
 *        return succcesss when firmware uploaded,
 *        application have to search "crusher" device again and
 *        check USB-ID to confirm that fx2 firmware uploaded.
 * \param usb device
 * \param firmware file
 * \return status of uploading firmware.
 */
int	crusher_fx2loader(void	*udev, FILE* fx2Fw);


/**
 * \brief sends START command to device.
 * \param crusher struct
 * \return device answer.
 */
int	crusher_start(crusher_t *crusher);


/**
 * \brief give new frame to encoder
 * \param crusher struct
 * \param pre-converter and blocked nv12 frame ptr (NULL, if capture-mode used)
 * \param frame size
 * \return codec status.
 */
int	crusher_encode(crusher_t *crusher, uint8_t *converted_data, int data_size);

#endif /*CRUSHER_H_*/
