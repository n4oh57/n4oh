#if 0
#include "i2c_gpio.h"

#include "stm32f4xx.h"
//#include "stm32f10x.h"

static void i2c_gpio_set_scl(void *ctx, uint8_t state)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
    GPIO_TypeDef *gpiox = io_ctx->gpiox;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_WritePin(gpiox, io_ctx->scl_gpio_pin, (GPIO_PinState)state);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_WriteBit(gpiox, io_ctx->scl_gpio_pin, (BitAction)state);
#else
    gpiox->BSRR = io_ctx->scl_gpio_pin;
#endif /* USE_HAL_DRIVER */
    
}

static void i2c_gpio_set_sda(void *ctx, uint8_t state)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
    GPIO_TypeDef *gpiox = io_ctx->gpiox;
    
#if defined(USE_HAL_DRIVER)
    HAL_GPIO_WritePin(gpiox, io_ctx->sda_gpio_pin, (GPIO_PinState)state);
#elif defined(USE_STDPERIPH_DRIVER)
    GPIO_WriteBit(gpiox, io_ctx->sda_gpio_pin, (BitAction)state);
#else
    gpiox->BSRR = io_ctx->sda_gpio_pin;
#endif /* USE_HAL_DRIVER */
}

static uint8_t i2c_gpio_get_scl(void *ctx)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
    GPIO_TypeDef *gpiox = io_ctx->gpiox;
    
#if defined(USE_HAL_DRIVER)
    return (uint8_t)HAL_GPIO_ReadPin(gpiox, io_ctx->scl_gpio_pin);
#elif defined(USE_STDPERIPH_DRIVER)
    return (uint8_t)GPIO_ReadInputDataBit(gpiox, io_ctx->scl_gpio_pin);
#else
    return gpiox->IDR & io_ctx->scl_gpio_pin ? 1 : 0;
#endif /* USE_HAL_DRIVER */
}

static uint8_t i2c_gpio_get_sda(void *ctx)
{
    struct i2c_gpio_context *io_ctx = (struct i2c_gpio_context *)ctx;
    
    GPIO_TypeDef *gpiox = io_ctx->gpiox;
    
#if defined(USE_HAL_DRIVER)
    return (uint8_t)HAL_GPIO_ReadPin(gpiox, io_ctx->sda_gpio_pin);
#elif defined(USE_STDPERIPH_DRIVER)
    return (uint8_t)GPIO_ReadInputDataBit(gpiox, io_ctx->sda_gpio_pin);
#else
    return gpiox->IDR & io_ctx->sda_gpio_pin ? 1 : 0;
#endif /* USE_HAL_DRIVER */
}

#if BITBANG_V1
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
#endif /* BITBANG_V1 */

int i2c_gpio_init(struct i2c_gpio_context *gpio_ctx)
{
#if BITBANG_V1
    i2c_bitbang_init(&gpio_ctx->bitbang, &io_ops, gpio_ctx);
    return 0;
#endif /* BITBANG_V1 */
    if (!gpio_ctx)
        return -1;
    
    gpio_ctx->bitbang.set_scl = &i2c_gpio_set_scl;
    gpio_ctx->bitbang.set_sda = &i2c_gpio_set_sda;
    gpio_ctx->bitbang.get_scl = &i2c_gpio_get_scl;
    gpio_ctx->bitbang.get_sda = &i2c_gpio_get_sda;
    gpio_ctx->bitbang.delay = 5;
    
    return i2c_bitbang_init(&gpio_ctx->bitbang, gpio_ctx);
}
#endif
#include <n4oh/drivers/i2c.h>

static inline int i2c_gpio_xfer(struct i2c_device *device, struct i2c_msg *msgs, 
                  uint16_t num_msgs, uint16_t slave_address)
{
    return 0;
}

const struct i2c_adapter adap_gpio_cortex_m3_m4 = {
    .name               = "gpio_cortex_m3_m4",
    .config             = 0,
    .xfer               = i2c_gpio_xfer,
    .recover            = 0
};
