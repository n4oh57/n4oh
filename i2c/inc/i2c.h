#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

struct i2c_msg {
    uint8_t *buf;

    uint16_t len;

#define I2C_MSG_WRITE       0x0000
#define I2C_MSG_READ        0x0001
#define I2C_MSG_RESTART     0x0002
#define I2C_MSG_STOP        0x0004
#define I2C_MSG_ADDR_10BITS 0x0008
    uint16_t flags;
};

#endif /* _I2C_H_ */
