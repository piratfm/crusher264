/*
 * rtppkt.c
 *
 *  Created on: 6 сент. 2010
 *      Author: tipok
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <netdb.h>

#include "rtppkt.h"
#include "logging.h"
#include "bswap.h"


/***********************************************
 * Static functions
 ***********************************************/
static int socket_open(char *laddr, int port) {
    struct sockaddr_in  lsin;
    int         sock;
    int         reuse=1;

    memset(&lsin, 0, sizeof(struct sockaddr_in));

    sock=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock<0)
        return sock;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&reuse, sizeof(reuse)) < 0) {
        close(sock);
        return -1;
    }

    lsin.sin_family=AF_INET;
    lsin.sin_addr.s_addr=INADDR_ANY;
    if (laddr)
        inet_aton(laddr, &lsin.sin_addr);
    lsin.sin_port=htons(port);

    if (bind(sock, (struct sockaddr *) &lsin,
            sizeof(struct sockaddr_in)) != 0) {
        close(sock);
        return -1;
    }

    return sock;
}


static int socket_set_ttl(int sock, int ttl) {
    if (ttl)
        return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    return 0;
}


/* Join the socket on a multicats group e.g. tell the kernel
 * to send out IGMP join messages ...
 *
 * Returns 0 on success and != 0 in failure
 *
 */
static int socket_join_multicast(int sock, char *addr) {
    struct ip_mreq      mreq;

    memset(&mreq, 0, sizeof(struct ip_mreq));

    /* Its not an ip address ? */
    if (!inet_aton(addr, &mreq.imr_multiaddr))
        return -1;

    if (!IN_MULTICAST(ntohl(mreq.imr_multiaddr.s_addr)))
        return 0;

    mreq.imr_interface.s_addr=INADDR_ANY;

    return setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}


static int socket_set_nonblock(int sock) {
    unsigned int    flags;

    flags=fcntl(sock, F_GETFL);
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}


static int socket_connect(int sock, char *addr, int port)
{
    struct sockaddr_in  rsin;

    memset(&rsin, 0, sizeof(struct sockaddr_in));

    /* Create remote end sockaddr_in */
    rsin.sin_family=AF_INET;
    rsin.sin_port=htons(port);
    rsin.sin_addr.s_addr=INADDR_ANY;

    if (addr)
        inet_aton(addr, &rsin.sin_addr);

    return connect(sock, (struct sockaddr *) &rsin, sizeof(struct sockaddr_in));
}


static int base64enc(const unsigned char *input, int length, char *output, int maxsize)
{

    //TODO: gnutls have SRP-base64 encoder, use it if possible
  static const char b64str[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  while (length && maxsize)
    {
      *output++ = b64str[(input[0] >> 2) & 0x3f];
      if (!--maxsize)
          break;
      *output++ = b64str[((input[0] << 4) + (--length ? input[1] >> 4 : 0)) & 0x3f];
      if (!--maxsize)
          break;
      *output++ = (length ? b64str[((input[1] << 2) + (--length ? input[2] >> 6 : 0)) & 0x3f] : '=');
      if (!--maxsize)
          break;
      *output++ = length ? b64str[input[2] & 0x3f] : '=';
      if (!--maxsize)
          break;
      if (length)
          length--;
      if (length)
          input += 3;
    }
  if (maxsize)
      *output = '\0';

  return 1;
}


#define MAX_PSET_SIZE 1024
static char *extradata2psets(uint8_t *data, int size)
{
    char *psets, *p;
    uint8_t *r;
    uint8_t *next_data;
    int s;
    const char *pset_string = "; sprop-parameter-sets=";

    psets = malloc(MAX_PSET_SIZE);
    memset(psets, 0x00, MAX_PSET_SIZE);
    if (psets == NULL) {
        logwrite(LOG_ERROR, "Cannot allocate memory for the parameter sets.");
        return NULL;
    }
    memcpy(psets, pset_string, strlen(pset_string));
    p = psets + strlen(pset_string);

    next_data=data;

    do {
        uint8_t nal_type;

        s = beptr2me_32(next_data);
        next_data+=4;

        r = next_data;
        next_data+=s;

        nal_type = *r & 0x1f;
        if (nal_type != 7 && nal_type != 8) { /* Only output SPS and PPS */
            continue;
        }

        if (p != (psets + strlen(pset_string))) {
            *p = ',';
            p++;
        }

        if(!base64enc(r, s, p, MAX_PSET_SIZE - (p - psets))){
            logwrite(LOG_ERROR, "Cannot Base64-encode %td %td!\n", MAX_PSET_SIZE - (p - psets), s);
            free(psets);
            return NULL;
        }

        p += strlen(p);
    } while (next_data < data + size);

    return psets;
}


static char *extradata2config(uint8_t *data, int size)
{
    char *config;

    config = malloc(10 + size * 2);
    if (config == NULL) {
        logwrite(LOG_ERROR, "Cannot allocate memory for the config info.\n");
        return NULL;
    }

    memcpy(config, "; config=", 9);
    int i;
    static const char hex_table[16] = { '0', '1', '2', '3',
                                        '4', '5', '6', '7',
                                        '8', '9', 'A', 'B',
                                        'C', 'D', 'E', 'F' };
    for(i = 0; i < size; i++) {
        config[9 + i*2]  = hex_table[data[i] >> 4];
        config[10 + i*2] = hex_table[data[i] & 0xF];
    }

    config[9 + size * 2] = 0;
    return config;
}


static void rtp_send_data(rtp_priv_t *priv, const uint8_t *buff, int size, uint32_t ts, int streamtype, int marker)
{
    /* build the RTP header */
    /* x x xx xxxx xxxx
     * V P SQ  TS  SSRC */
    uint8_t *out = malloc(12 + size);
    out[0] = RTP_VERSION << 6;
    out[1] = ((RTP_PT_PRIVATE+streamtype) & 0x7f) | ((marker & 0x01) << 7);
    *((uint32_t *) &out[4]) = me2be_32(ts);

    *((uint32_t *) &out[8]) = me2be_32(priv->ssrc);
    memcpy(&out[12], buff, size);

    if(!streamtype) {
        *((uint16_t *) &out[2]) = me2be_16(priv->seq_video);
        send(priv->sockfd_video, out, size+12, 0);
        priv->seq_video++;
    } else {
        *((uint16_t *) &out[2]) = me2be_16(priv->seq_audio);
        send(priv->sockfd_audio, out, size+12, 0);
        priv->seq_audio++;
    }
    free(out);
}


static void nal_send(rtp_priv_t *priv, const uint8_t *buf, int size, int last, uint32_t ts, int streamtype)
{
    if (size <= priv->max_payload_size) {
        rtp_send_data(priv, buf, size, ts, streamtype, last);
    } else {
        uint8_t type = buf[0] & 0x1F;
        uint8_t nri = buf[0] & 0x60;
        uint8_t *tempbuf = malloc(priv->max_payload_size);

        tempbuf[0] = 28;        /* FU Indicator; Type = 28 ---> FU-A */
        tempbuf[0] |= nri;
        tempbuf[1] = type;
        tempbuf[1] |= 1 << 7;
        buf += 1;
        size -= 1;
        while (size + 2 > priv->max_payload_size) {
            memcpy(&tempbuf[2], buf, priv->max_payload_size - 2);
            rtp_send_data(priv, tempbuf, priv->max_payload_size, ts, streamtype, 0);
            buf += priv->max_payload_size - 2;
            size -= priv->max_payload_size - 2;
            tempbuf[1] &= ~(1 << 7);
        }
        tempbuf[1] |= 1 << 6;
        memcpy(&tempbuf[2], buf, size);
        rtp_send_data(priv, tempbuf, size + 2, ts, streamtype, last);
    }
}


static void rtp_send_h264(rtp_priv_t *priv, uint8_t *buff, int size, uint32_t ts, int streamtype)
{
    uint8_t *next_data=buff;
    int s;
    uint8_t *r;

    do {
        s = beptr2me_32(next_data);
        next_data+=4;

        r = next_data;
        next_data+=s;

        nal_send(priv, r, s, (next_data >= buff + size), ts, streamtype);
    } while (next_data < buff + size);
}


static void rtp_send_aac(rtp_priv_t *priv, const uint8_t *buff, int size, uint32_t ts, int streamtype)
{
    uint8_t *p = malloc(size+4);

    /* test if the packet must be sent */
    p[0] = 0;
    p[1] = 16;
    p[2] = size >> 5;
    p[3] = (size & 0x1F) << 3;
    memcpy(p + 4, buff, size);
    rtp_send_data(priv, p, size+4, ts, streamtype, 1);
    free(p);
}


/* rfc3016 */
static void rtp_send_aac_latm(rtp_priv_t *priv, const uint8_t *buff, int size, uint32_t ts, int streamtype)
{
    int     latmhdrsize = size / 0xff + 1;
    int     pkts_count = ( size + priv->max_payload_size - 1 ) / priv->max_payload_size;

    uint8_t *p_header = NULL;
    int     i;
    int     len;

    for( i = 0; i < pkts_count; i++ )
    {
        if(size > priv->max_payload_size) {
            len = priv->max_payload_size;
        } else {
            len = size;
        }

        if( i != 0 )
            latmhdrsize = 0;

        uint8_t *out = malloc(latmhdrsize + len);


        /* rtp common header */
        if( i == 0 ) {
            int tmp = size;
            p_header=out;
            while( tmp > 0xfe ) {
                *p_header = 0xff;
                p_header++;
                tmp -= 0xff;
            }
            *p_header = tmp;
        }

        memcpy( &out[latmhdrsize], buff, len );
        rtp_send_data(priv, out, latmhdrsize + len, ts, streamtype, ((i == pkts_count - 1) ? 1 : 0));
        free(out);

        buff += len;
        size -= len;
    }
}





/***********************************************
 * Shared functions
 ***********************************************/
int rtp_open(rtp_priv_t *private_data, char *host, int port, int ttl)
{
    private_data->dst_addr = host;
    private_data->dst_port = port;
    private_data->dst_ttl = ttl;
    /* video stream */
    private_data->sockfd_video=socket_open(NULL, 0);
    if (private_data->sockfd_video < 0)
        return 0;

    private_data->sockfd_audio=socket_open(NULL, 0);
    if (private_data->sockfd_audio < 0) {
        close(private_data->sockfd_video);
        return 0;
    }

    socket_set_nonblock(private_data->sockfd_video);
    socket_set_nonblock(private_data->sockfd_audio);

    if (socket_connect(private_data->sockfd_video, host, port)) {
        close(private_data->sockfd_video);
        return 0;
    }

    if (socket_connect(private_data->sockfd_audio, host, port+2)) {
        close(private_data->sockfd_video);
        close(private_data->sockfd_audio);
        return 0;
    }


    /* Join Multicast group if its a multicast destination */
    socket_join_multicast(private_data->sockfd_video, host);
    socket_join_multicast(private_data->sockfd_audio, host);

    if (ttl) {
        socket_set_ttl(private_data->sockfd_video, ttl);
        socket_set_ttl(private_data->sockfd_audio, ttl);
    }

    /* some constants */
    private_data->max_payload_size = RTP_MAX_PAYLOAD;
    private_data->ssrc = rand();
    return 1;
}


int rtp_close(rtp_priv_t *private_data)
{
    close(private_data->sockfd_video);
    close(private_data->sockfd_audio);
    return 1;
}


int rtp_add_sdp(rtp_priv_t *private_data, uint8_t *data, int size, int streamtype)
{
    if(!private_data->av_flag) {
        fprintf(private_data->sdp_file,
                "v=%d\r\n"
                "o=- %d %d IN %s %s\r\n"
                "s=%s\r\n",
                0, rand(), 2, "IP4", "0.0.0.0", "LIVE");
        fprintf(private_data->sdp_file,
                "t=%d %d\r\n"
                "a=tool:crusherRTP\r\n", 0, 0);

    }

    char *config = NULL;
    if(!streamtype && ((private_data->av_flag & 0x01) == 0)) {
        fprintf(private_data->sdp_file, "m=%s %d RTP/AVP %d\r\n", "video", private_data->dst_port, RTP_PT_PRIVATE + streamtype);

        if (private_data->dst_ttl > 0) {
            fprintf(private_data->sdp_file, "c=IN %s %s/%d\r\n", "IP4", private_data->dst_addr, private_data->dst_ttl);
        } else {
            fprintf(private_data->sdp_file, "c=IN %s %s\r\n", "IP4", private_data->dst_addr);
        }

        if (private_data->bit_rate_video) {
            fprintf(private_data->sdp_file, "b=AS:%d\r\n", private_data->bit_rate_video / 1000);
        }

        config = extradata2psets(data,size);
        fprintf(private_data->sdp_file,
                "a=rtpmap:%d H264/90000\r\n"
                "a=fmtp:%d packetization-mode=1%s\r\n",
                RTP_PT_PRIVATE + streamtype,
                RTP_PT_PRIVATE + streamtype, config ? config : "");
        if(config) free(config);

        private_data->av_flag |= 0x01;
    } else if((private_data->av_flag & 0x02) == 0) {
        fprintf(private_data->sdp_file, "m=%s %d RTP/AVP %d\r\n", "audio", private_data->dst_port+2, RTP_PT_PRIVATE + streamtype);

        if (private_data->dst_ttl > 0) {
            fprintf(private_data->sdp_file, "c=IN %s %s/%d\r\n", "IP4", private_data->dst_addr, private_data->dst_ttl);
        } else {
            fprintf(private_data->sdp_file, "c=IN %s %s\r\n", "IP4", private_data->dst_addr);
        }

        if (private_data->bit_rate_audio) {
            fprintf(private_data->sdp_file, "b=AS:%d\r\n", private_data->bit_rate_audio / 1000);
        }


        if(private_data->audio_rtp_latm) {
            int i;
            unsigned char config_bin[6];
            unsigned int aacsrates[15] = {
                96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
                16000, 12000, 11025, 8000, 7350, 0, 0 };

            for( i = 0; i < 15; i++ )
                if( private_data->audio_sample_rate == aacsrates[i] )
                    break;

            config_bin[0]=0x40;
            config_bin[1]=0;
            config_bin[2]=0x20|i;
            config_bin[3]=private_data->audio_channels<<4;
            config_bin[4]=0x3f;
            config_bin[5]=0xc0;
            config = extradata2config(config_bin, 6);

            fprintf(private_data->sdp_file,
                    "a=rtpmap:%d MP4A-LATM/%d\r\n"
                    "a=fmtp:%d profile-level-id=15;object=2; cpresent=0;%s\r\n",
                    RTP_PT_PRIVATE + streamtype, 90000,
                    RTP_PT_PRIVATE + streamtype, config);
        } else {
            config = extradata2config(data,size);
            fprintf(private_data->sdp_file,
                    "a=rtpmap:%d MPEG4-GENERIC/%d\r\n"
                    "a=fmtp:%d profile-level-id=1;"
                    "mode=AAC-hbr;sizelength=13;indexlength=3;"
                    "indexdeltalength=3%s\r\n",
                    RTP_PT_PRIVATE + streamtype, 90000,
                    RTP_PT_PRIVATE + streamtype, config ? config : "");
        }
        if(config) free(config);

        /* have audio */
        private_data->av_flag |= 0x02;
    } else {
        logwrite(LOG_ERROR, "Audio & video configs already presented.\n");
    }

    if((private_data->av_flag & 0x02) && (private_data->av_flag & 0x01)) {
        fclose(private_data->sdp_file);
    }
    return 1;
}


int rtp_write(rtp_priv_t *private_data, uint8_t *data, int size, uint32_t cts, int streamtype)
{
    if(!streamtype) {
        rtp_send_h264(private_data, data, size, cts, streamtype);
    } else if(private_data->audio_rtp_latm) {
        rtp_send_aac_latm(private_data, data, size, cts, streamtype);
    } else {
        rtp_send_aac(private_data, data, size, cts, streamtype);
    }
    return 1;
}
