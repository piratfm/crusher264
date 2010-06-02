
#include <usb.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crusher-usb.h"
#include "crusher.h"
#include "logging.h"

/* Request 0xa0: Cypress A2 EZ-USB Loader */
#define A2_REQ				0xA0

#define A2_RESET 			0xE600
static char a2_reset_on[1] = { 0x00 };
static char a2_reset_off[1] = { 0x01 };

static const uint16_t A2_RESET_DOWN		= 0x01;


static int fx2hex2usb(usb_dev_handle	*udev, FILE *fx2Fw);

/************************************************************
 * Uploading FX2 FIRMWARE
 ************************************************************/
int crusher_fx2loader(void	*handler, FILE* fx2Fw){
	usb_dev_handle	*udev = (usb_dev_handle	*) handler;
	int				ret;
	
	ret = usb_control_msg(udev, USB_TYPE_VENDOR + USB_RECIP_DEVICE, 
				A2_REQ, A2_RESET, 0x00, a2_reset_off, 0x01, USB_TIMEOUT);
    if (ret < 0){
    	ERROR("sending FX2 Reset failed, code: %d", ret);
    	return 0;
    }
    
    if (!fx2hex2usb(udev, fx2Fw)){
    	ERROR("FX2 firmware sending failed!");
    	return 0;
    }
    
	ret = usb_control_msg(udev,	USB_TYPE_VENDOR + USB_RECIP_DEVICE, 
				A2_REQ, A2_RESET, 0x00, a2_reset_on, 0x01, USB_TIMEOUT);
    if (ret < 0){
    	ERROR("sending FX2 Reset failed, code: %d", ret);
    	return 0;
    }
    
	return 1;
}

/* Read hex data and send to usb device */
static int	fx2hex2usb(usb_dev_handle	*udev, FILE* fx2Fw)
{
	char 		buf [512], *cp;
	char		tmp, type;
	size_t		len;
	uint32_t	idx, off;
    int			first_line = 1;
	uint16_t		data_addr = 0;
	size_t		data_len = 0;
	uint8_t	data [1023];
    
    DEBUG("Sending HEX file");

	for (;;) {
		cp = fgets(buf, sizeof(buf), fx2Fw);
		if (cp == 0) {
			ERROR("EOF without EOF record!");
			//break;
			return 0;
		}
		
		/* EXTENSION: "# comment-till-end-of-line", for copyrights etc */
		if (buf[0] == '#')
			continue;
		
		if (buf[0] != ':') {
			ERROR("Not an ihex record: %s", buf);
			return 0;
		}
		
		cp = strchr (buf, '\n');
		if (cp)
			*cp = 0;
		XTREME("LINE: %s", buf);
		
		/* Read the length field (up to 16 bytes) */
		tmp = buf[3];
		buf[3] = 0;
		len = strtoul(buf+1, 0, 16);
		buf[3] = tmp;
		
		/* Read the target offset (address up to 64KB) */
		tmp = buf[7];
		buf[7] = 0;
		off = strtoul(buf+3, 0, 16);
		buf[7] = tmp;
		
		/* Initialize data_addr */
		if (first_line) {
			data_addr = off;
			first_line = 0;
		}
		
		/* Read the record type */
		tmp = buf[9];
		buf[9] = 0;
		type = strtoul(buf+7, 0, 16);
		buf[9] = tmp;

		/* If this is an EOF record, then make it so. */
		if (type == 1) {
	    	XTREME("EOF on hexfile.");
			break;
		}
		
		if (type != 0) {
			ERROR("Unsupported record type: %u.", type);
			return 0;
		}
		
		if ((len * 2) + 11 > strlen(buf)) {
			ERROR("Record too short: propozed length: %d bytes, real length: %d bytes.",
							len, strlen(buf));
			return 0;
		}


		/* flush the saved data if it's not contiguous,
		 * or when we've buffered as much as we can. */
		 if (data_len != 0 && (off != (data_addr + data_len) || (data_len + len) > sizeof(data))) {
			if (usb_control_msg(udev, USB_TYPE_VENDOR + USB_RECIP_DEVICE, A2_REQ,
								 data_addr, 0x00, (char *) data, data_len, USB_TIMEOUT) < 0)
			{
				ERROR("Can't send %d bytes to 0x%04x address into device",
							data_len, data_addr);
				return 0;
			}
			data_addr = off;
			data_len = 0;
		}

		/* append to saved data, flush later */
		for (idx = 0, cp = buf+9 ;  idx < len ;  idx += 1, cp += 2) {
			tmp = cp[2];
			cp[2] = 0;
			data [data_len + idx] = strtoul(cp, 0, 16);
			cp[2] = tmp;
		}
		data_len += len;
	}
    return 1;
}
