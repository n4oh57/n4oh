#ifndef _I2C_BITBANG_H_
#define _I2C_BITBANG_H_

#include "i2c.h"

#define BITBANG_V1 0
#define BITBANG_V2 1

#if BITBANG_V1
struct i2c_bitbang_ops {
    void (*set_scl)(void *ctx, uint8_t state);
    void (*set_sda)(void *ctx, uint8_t state);
    uint8_t (*get_scl)(void *ctx);
    uint8_t (*get_sda)(void *ctx);
};
#endif /* BITBANG_V1 */

struct i2c_bitbang {
#if BITBANG_V1
    const struct i2c_bitbang_ops *ops;
#elif BITBANG_V2
    void (*set_scl)(void *ctx, uint8_t state);
    void (*set_sda)(void *ctx, uint8_t state);
    uint8_t (*get_scl)(void *ctx);
    uint8_t (*get_sda)(void *ctx);
#endif /* BITBANG_V1 */
    void *io_ctx;
    uint16_t delay;
};

int i2c_bitbang_xfer(struct i2c_bitbang *self, struct i2c_msg *msgs, 
                uint16_t num_msgs, uint16_t slave_address);

int i2c_bitbang_recover_bus(struct i2c_bitbang *self);

#if BITBANG_V1
void i2c_bitbang_init(struct i2c_bitbang *bitbang,
                const struct i2c_bitbang_ops *ops, void *io_context);
#elif BITBANG_V2
int i2c_bitbang_init(struct i2c_bitbang *bitbang, void *io_context);
#endif /* BITBANG_V1 */

#endif /* _I2C_BITBANG_H_ */
