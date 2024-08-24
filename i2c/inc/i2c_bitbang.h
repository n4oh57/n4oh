#ifndef _I2C_BITBANG_H_
#define _I2C_BITBANG_H_

#include "i2c.h"

struct i2c_bitbang_ops {
    void (*set_scl)(void *ctx, uint8_t state);
    void (*set_sda)(void *ctx, uint8_t state);
    uint8_t (*get_scl)(void *ctx);
    uint8_t (*get_sda)(void *ctx);
};

struct i2c_bitbang {
    const struct i2c_bitbang_ops *ops;
    void *io_ctx;
    uint16_t delay;
};

int i2c_bitbang_xfer(struct i2c_bitbang *self, struct i2c_msg *msgs, 
                uint16_t num_msgs, uint16_t slave_address);

int i2c_bitbang_recover_bus(struct i2c_bitbang *self);

void i2c_bitbang_init(struct i2c_bitbang *bitbang,
                const struct i2c_bitbang_ops *ops, void *io_context);

#endif /* _I2C_BITBANG_H_ */
