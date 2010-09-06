/*
 * rtppkt.h
 *
 *  Created on: 6 сент. 2010
 *      Author: tipok
 */

#ifndef RTPPKT_H_
#define RTPPKT_H_

#include <stdint.h>
#include <stdio.h>

#define RTP_VERSION 2
#define RTP_PT_PRIVATE 96
#define RTP_MAX_PAYLOAD 1400

typedef struct {
        int     sockfd_video;
        int     bit_rate_video;
        uint16_t seq_video;

        int     sockfd_audio;
        int     bit_rate_audio;
        int     audio_rtp_latm;
        int     audio_channels;
        int     audio_sample_rate;
        uint16_t seq_audio;

        char    *dst_addr;
        int     dst_port;
        int     dst_ttl;

        uint32_t ssrc;
        int     av_flag;
        FILE    *sdp_file;

        int     max_frames_per_packet;
        int     max_payload_size;
} rtp_priv_t;


int rtp_open(rtp_priv_t *private_data, char *host, int port, int ttl);

int rtp_add_sdp(rtp_priv_t *private_data, uint8_t *data, int size, int streamtype);
int rtp_write(rtp_priv_t *private_data, uint8_t *data, int size, uint32_t cts, int streamtype);

int rtp_close(rtp_priv_t *private_data);


#endif /* RTPPKT_H_ */
