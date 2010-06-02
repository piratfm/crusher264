/*
 * i2c.c
 *
 *  Created on: 13 марта 2010
 *      Author: tipok
 */

#include <stdint.h>
#include <string.h>
#include <usb.h>

#include "crusher.h"
#include "logging.h"
#include "crusher-usb.h"
#include "bswap.h"
#include "crusher-i2c.h"

#define I2C_ADDR_ADV7180 0x40 ///< video adc
#define I2C_ADDR_CS4265  0x9e ///< audio adc
#define I2C_ADDR_CY22394 0xd2 ///< clock generator

#define cy22394_init_size 18
static struct i2c_data cy22394_init[cy22394_init_size] = {
    /* ClkA_Div = 25
     * Fa = Fpll2 / ClkA_Div = 273,06(6)M / 25 = 10,9226(6)M
     * */
    { 0x08, 0x19 },     ///< 0 ClkA_FS[0] ClkA_Div[6:0] 0001 1001
    { 0x09, 0x00 },     ///< 1 ClkA_FS[0] ClkA_Div[6:0]
    /* ClkB_Div = 8
     * Fb = Fpll3 / ClkB_Div = 240M / 8 = 30M
     * */
    { 0x0a, 0x08 },     ///< 0 ClkB_FS[0] ClkB_Div[6:0] 0000 1000
    { 0x0b, 0x00 },     ///< 1 ClkB_FS[0] ClkB_Div[6:0]

    /* ClkD_FS = 000 ref
     * ClkC_FS = 000 ref
     * ClkB_FS = 110 pll3
     * ClkA_FS = 100 pll2
     * */
    { 0x0e, 0x0e },     ///< ClkD_FS[2:1] ClkC_FS[2:1] ClkB_FS[2:1] ClkA_FS[2:1]  00 00 11 10

    /* These are nominally set to 01, with a higher vlue shifting the duty cycle higher.
     * Clk{C,X}_ACAdj[1:0] = 01
     * Clk{A,B,D,E}_ACAdj[1:0] = 01
     * ClkX_DCAdj[1] = 01
     * Clk{D,E}_DCAdj[1] = 01
     * ClkC_DCAdj[1] = 01
     * Clk{A,B}_DCAdj[1] = 01
     * */
    { 0x0f, 0x50 },     ///< Clk{C,X}_ACAdj[1:0] Clk{A,B,D,E}_ACAdj[1:0] PdnEn Xbuf_OE ClkE_Div[1:0] 0101 0000
    { 0x10, 0x55 },     ///< ClkX_DCAdj[1] Clk{D,E}_DCAdj[1] ClkC_DCAdj[1] Clk{A,B}_DCAdj[1] 0101 0101

    /* PLL2:
     * PO = 0
     * LF =010
     * Pt = (2 x (P + 3)) + PO = (2 x (253+3)) + 0 = 512
     * Qt = Q + 2 = 43 + 2 = 45
     * Fpll = Fref * (Pt/Qt) = 24M*(512/45) = 273,06(6)M
     * */
    { 0x11, 0x2b },     ///< PLL2_Q[7:0]
    { 0x12, 0xfd },     ///< PLL2_P[7:0]
    { 0x13, 0x60 },     ///< Reserved PLL2_En PLL2_LF[2:0] PLL2_PO PLL2_P[9:8] 0110 0000

    /* PLL3:
     * PO = 0
     * LF =000
     * Pt = (2 x (P + 3)) + PO = (2 x (0x07 + 3)) + 0 = 20
     * Qt = Q + 2 = 0 + 2 = 2
     * Fpll = Fref * (Pt/Qt) = 24M*(20/2) = 240M
     * */
    { 0x14, 0x00 },     ///< PLL3_Q[7:0]
    { 0x15, 0x07 },     ///< PLL3_P[7:0]
    { 0x16, 0x40 },     ///< Reserved PLL3_En PLL3_LF[2:0] PLL3_PO PLL3_P[9:8] = 0100 0000

    { 0x17, 0x01 },     ///< Osc_Cap[5:0] Osc_Drv[1:0] 0000 0001 (25-50Mhz osc)
    { 0x18, 0x00 },
    { 0x19, 0x00 },
    { 0x1a, 0xe9 },
    { 0x1b, 0x08 },
};

#define adv7180_init_size 9
static struct i2c_data adv7180_init[adv7180_init_size] = {
    { 0x01, 0xc8 },
    { 0x03, 0x8c },
    { 0x31, 0x02 },
    { 0x3d, 0xa2 },
    { 0x3e, 0x6a },
    { 0x3f, 0xa0 },
    { 0x0e, 0x80 },
    { 0x55, 0x81 },
    { 0x0e, 0x00 },
};

#define adv7180_input_size 5
static struct i2c_data adv7180_input[3][adv7180_init_size] = {
        {/* composite */
                { 0x0f, 0x00 },
                { 0x00, 0x80 },
                { 0x58, 0x00 },
                { 0x17, 0x41 },
                { 0x4d, 0xef },
        }, {/* s-video */
                { 0x0f, 0x00 },
                { 0x00, 0x86 },
                { 0x58, 0x04 },
                { 0x17, 0x01 },
                { 0x4d, 0xef },
        }, { /* component */
                { 0x0f, 0x00 },
                { 0x00, 0x89 },
                { 0x58, 0x00 },
                { 0x17, 0xe1 },
                { 0x4d, 0xee },
        },
};







#define cs4265_init_size 2
static struct i2c_data cs4265_init[cs4265_init_size] = {
    { 0x02, 0x0a },
    { 0x04, 0x10 },
};


/*
 * Request 0xbf reset
 * */
static int i2c_write(crusher_t *crusher, uint8_t addr, uint8_t busid, uint16_t reg, uint8_t val)
{
    int ret;
    uint16_t full_addr = me2be_16(addr << 8 | busid);
    XTREME("Write I2C");
    //mg1264_hbrr(crusher, 0x001a, &regiter16);
    ret = usb_control_msg((usb_dev_handle *) crusher->udev, USB_TYPE_VENDOR + USB_RECIP_DEVICE,
                    EP0_I2C, full_addr, me2le_16(reg), (char *) &val, 0x0001, USB_TIMEOUT);
    if (ret != 1)
        ERROR("Write I2C failed.");
    return ret;
}

static int i2c_read(crusher_t *crusher, uint8_t addr, uint8_t busid, uint16_t reg, uint8_t *val)
{
    int ret;
    uint16_t full_addr = me2be_16(addr << 8 | busid);
    XTREME("Read I2C.");
    //mg1264_hbrr(crusher, 0x001a, &regiter16);
    ret = usb_control_msg((usb_dev_handle *) crusher->udev, USB_TYPE_VENDOR + USB_RECIP_DEVICE + USB_ENDPOINT_IN,
                    EP0_I2C, full_addr, me2le_16(reg), (char *) val, 0x0001, USB_TIMEOUT);
    if (ret != 1)
        ERROR("Read I2C failed.");
    return ret;
}


int i2c_init(crusher_t *crusher)
{
    int i;

    for (i=0; i < cy22394_init_size; i++)
        i2c_write(crusher, I2C_ADDR_CY22394, 0xff, cy22394_init[i].reg, cy22394_init[i].val);

    for (i=0; i < adv7180_init_size; i++)
        i2c_write(crusher, I2C_ADDR_ADV7180, 0x00, adv7180_init[i].reg, adv7180_init[i].val);

    for (i=0; i < adv7180_input_size; i++)
        i2c_write(crusher, I2C_ADDR_ADV7180, 0x00,
                adv7180_input[crusher->video_input][i].reg, adv7180_input[crusher->video_input][i].val);

    for (i=0; i < cs4265_init_size; i++)
        i2c_write(crusher, I2C_ADDR_CS4265, 0xff, cs4265_init[i].reg, cs4265_init[i].val);

    return 1;
}

/* set:
 * - brightness/contrast/saturation
 * - volume gain control
 * */
int i2c_init2(crusher_t *crusher)
{
    int8_t gain = crusher->audio_gain/5;
    if(gain < 0) {
        gain = (gain | 0x20) & 0x3F;
    }

    /* brightness/contrast/saturation */
    i2c_write(crusher, I2C_ADDR_ADV7180, 0x00, 0x0a, crusher->video_brightness);
    i2c_write(crusher, I2C_ADDR_ADV7180, 0x00, 0x08, crusher->video_contrast);
    i2c_write(crusher, I2C_ADDR_ADV7180, 0x00, 0xffe3, crusher->video_saturation_Cb);
    i2c_write(crusher, I2C_ADDR_ADV7180, 0x00, 0xffe4, crusher->video_saturation_Cr);

    /* set volume gain */
    i2c_write(crusher, I2C_ADDR_CS4265, 0xff, 0x07, gain);
    i2c_write(crusher, I2C_ADDR_CS4265, 0xff, 0x08, gain);

    if(crusher->audio_samplerate == 24000){
#if 0
        /* mclk/2 */
        i2c_write(crusher, I2C_ADDR_CS4265, 0xff, 0x05, 0x40);
#endif
    }

    return 1;
}

/* read video adc status */
int i2c_read_vadc_status(crusher_t *crusher, uint8_t *val){
    return i2c_read(crusher, I2C_ADDR_ADV7180, 0x00, 0x10, val);
}
