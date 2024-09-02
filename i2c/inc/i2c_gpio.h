#ifndef _I2C_GPIO_H_
#define _I2C_GPIO_H_

#include "i2c_bitbang.h"

/* Driver instance data */
struct i2c_gpio_context {
	struct i2c_bitbang bitbang;	/* Bit-bang library data */
	void *gpiox;
    uint16_t scl_gpio_pin;
    uint16_t sda_gpio_pin;
};

#if BITBANG_V1
int i2c_gpio_transfer(struct i2c_gpio_context *gpio_ctx, struct i2c_msg *msgs,
				uint8_t num_msgs, uint16_t slave_address);

int i2c_gpio_recover_bus(struct i2c_gpio_context *gpio_ctx);
#endif /* BITBANG_V1 */

int i2c_gpio_init(struct i2c_gpio_context *gpio_ctx);

#endif /* _I2C_GPIO_H_ */
