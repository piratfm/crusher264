/*
 * i2c.h
 *
 *  Created on: 13 марта 2010
 *      Author: tipok
 */

#ifndef CRUSHER_I2C_H_
#define CRUSHER_I2C_H_

#include <usb.h>
#include "mg1264.h"
#include "crusher.h"

struct i2c_data {
        uint16_t reg;
        uint8_t val;
} GCC_PACK;

/* sent after startup */
int i2c_init(crusher_t *crusher);
/* sent in the middle of cms sending process */
int i2c_init2(crusher_t *crusher);
int i2c_read_vadc_status(crusher_t *crusher, uint8_t *val);



#endif /* CRUSHER_I2C_H_ */
