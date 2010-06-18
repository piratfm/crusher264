#ifndef CRUSHERUSB_H_
#define CRUSHERUSB_H_

#include <usb.h>

#define EP0				0x00
#define EP0_B1_RESET    0xB1
#define EP0_CSRR        0xB5
#define EP0_CSRW        0xB6
#define EP0_PUSH        0xB7
#define EP0_MBW         0xB8
#define EP0_HBRR		0xB9
#define EP0_HBRW		0xBA
#define EP0_BB_RESET    0xBB
#define EP0_RESET       0xBC
#define EP0_MBR			0xBD
#define EP0_I2C         0xBF

/* 32byte commands ep */
#define EP1				0x01

/* hi-speed data xchange ep's */
#define EP2				0x02
#define EP6				0x06

/* event interrupt ep */
#define EP8				0x08


#define EP_MAX_LEN      0x10000
//#define MAX_BULK_LEN	0x3d000
#define MAX_BULK_LEN    0x10000

/* Default USB Timeouts */
#define	USB_TIMEOUT	1000
#define	BULK_TOUT	3000

#endif /*CRUSHERUSB_H_*/
