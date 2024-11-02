#ifndef _DRIVERS_I2C_H_
#define _DRIVERS_I2C_H_

#include <n4oh/logging/log.h>

struct i2c_device {
    const struct log_instance *log_inst;
    void *priv_data;
};

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

struct i2c_adapter {
    const char *name;
    int (*config)(struct i2c_device *device);
    int (*xfer)(struct i2c_device *device, struct i2c_msg *msgs, 
                uint16_t num_msgs, uint16_t slave_address);
    int (*recover)(struct i2c_device *device);
};

#endif /* _DRIVERS_I2C_H_ */
