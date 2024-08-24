#include "i2c_gpio.h"

#include "stm32f4xx.h"
//#include "stm32f10x.h"

static void i2c_gpio_set_scl(void *ctx, uint8_t state)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_WritePin((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->scl_gpio_pin, state);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_WriteBit((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->scl_gpio_pin, state);
#endif /* USE_HAL_DRIVER || USE_STDPERIPH_DRIVER */
    
}

static void i2c_gpio_set_sda(void *ctx, uint8_t state)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_WritePin((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->sda_gpio_pin, state);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_WriteBit((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->sda_gpio_pin, state);
#endif /* USE_HAL_DRIVER || USE_STDPERIPH_DRIVER */
}

static uint8_t i2c_gpio_get_scl(void *ctx)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_ReadPin((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->scl_gpio_pin);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_ReadInputDataBit((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->scl_gpio_pin);
#endif /* USE_HAL_DRIVER || USE_STDPERIPH_DRIVER */
}

static uint8_t i2c_gpio_get_sda(void *ctx)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_ReadPin((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->sda_gpio_pin);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_ReadInputDataBit((GPIO_TypeDef *)io_ctx->gpiox, io_ctx->sda_gpio_pin);
#endif /* USE_HAL_DRIVER || USE_STDPERIPH_DRIVER */
}

static const struct i2c_bitbang_ops io_ops = {
    .set_scl = &i2c_gpio_set_scl,
    .set_sda = &i2c_gpio_set_sda,
    .get_scl = &i2c_gpio_get_scl,
    .get_sda = &i2c_gpio_get_sda,
};

int i2c_gpio_transfer(struct i2c_gpio_context *gpio_ctx, struct i2c_msg *msgs,
                uint8_t num_msgs, uint16_t slave_address)
{
	return i2c_bitbang_xfer(&gpio_ctx->bitbang, msgs, num_msgs, slave_address);
}

int i2c_gpio_recover_bus(struct i2c_gpio_context *gpio_ctx)
{
	return i2c_bitbang_recover_bus(&gpio_ctx->bitbang);;
}

int i2c_gpio_init(struct i2c_gpio_context *gpio_ctx)
{
    i2c_bitbang_init(&gpio_ctx->bitbang, &io_ops, gpio_ctx);
}

