/**
 * @file
 * @brief Software driven 'bit-banging' library for I2C
 *
 * Timings and protocol are based Rev. 6 of the I2C specification:
 * http://www.nxp.com/documents/user_manual/UM10204.pdf
 */

#include "i2c_bitbang.h"

#include "delay.h"

#if BITBANG_V1
static inline void i2c_set_scl(struct i2c_bitbang *self, uint8_t state)
{
    self->ops->set_scl(self->io_ctx, state);
}

static inline void i2c_set_sda(struct i2c_bitbang *self, uint8_t state)
{
    self->ops->set_sda(self->io_ctx, state);
}

static inline uint8_t i2c_get_scl(struct i2c_bitbang *self)
{
    return self->ops->get_scl(self->io_ctx);
}

static inline uint8_t i2c_get_sda(struct i2c_bitbang *self)
{
    return self->ops->get_sda(self->io_ctx);
}
#elif BITBANG_V2
static inline void i2c_set_scl(struct i2c_bitbang *self, uint8_t state)
{
    self->set_scl(self->io_ctx, state);
}

static inline void i2c_set_sda(struct i2c_bitbang *self, uint8_t state)
{
    self->set_sda(self->io_ctx, state);
}

static inline uint8_t i2c_get_scl(struct i2c_bitbang *self)
{
    return self->get_scl(self->io_ctx);
}

static inline uint8_t i2c_get_sda(struct i2c_bitbang *self)
{
    return self->get_sda(self->io_ctx);
}
#endif /* BITBANG_V1 */

static inline void i2c_delay(uint16_t nus)
{
    delay_us(nus);
}

static inline void i2c_delay_2(uint16_t nus)
{
    delay_us((nus + 1) >> 1);
}

/**
 *     ___________
 * SDA            |_____________
 *     _________________
 * SCL                  |_______
 */
static void i2c_start(struct i2c_bitbang *self)
{
    /* assert: scl=1 sda=1 */
    i2c_set_sda(self, 0);
    i2c_delay(self->delay);

    i2c_set_scl(self, 0);
    i2c_delay(self->delay >> 1);
}

/**
 *     _________________
 * SDA                  |________________
 *              ________________
 * SCL ________|                |________
 */
static void i2c_repeat_start(struct i2c_bitbang *self)
{
    /* assert: scl=0 */
    i2c_set_sda(self, 1);
    i2c_delay_2(self->delay);
    i2c_set_scl(self, 1);
    i2c_delay(self->delay);

    i2c_set_sda(self, 0);
    i2c_delay(self->delay);
    i2c_set_scl(self, 0);
    i2c_delay(self->delay >> 1);
}

/**
 *                    ___________
 * SDA ______________|
 *              _________________
 * SCL ________|
 */
static void i2c_stop(struct i2c_bitbang *self)
{
    /* assert: scl=0 */
    i2c_set_sda(self, 0);
    i2c_delay_2(self->delay);

    i2c_set_scl(self, 1);
    i2c_delay(self->delay);

    i2c_delay_2(self->delay);
    i2c_set_sda(self, 1);
    i2c_delay(self->delay); /* In case we start again too soon */
}

/**
 *      ACK SCL高电平时SDA低电平
 *     ____________              ____________
 * SDA             |____________|(从机释放总线恢复高阻态(开漏模式))
 *                  ____________
 * SCL ____________|            |____________
 *
 *      NACK SCL高电平时SDA高电平
 *     ______________________________________
 * SDA
 *                  ____________
 * SCL ____________|            |____________
 */
static uint8_t i2c_wait_ack(struct i2c_bitbang *self)
{
    uint8_t ack = 1;
    uint16_t timeout = 0;

    i2c_set_sda(self, 1); /* Stop driving low, so slave has control */
    i2c_delay_2(self->delay);

    i2c_set_scl(self, 1);
    i2c_delay(self->delay);

    while (!i2c_get_scl(self)) {
        if (timeout > 255) {
            if (i2c_get_scl(self))
                break;
            return 1;
        }
        timeout++;
    }
    /* data valid now */
    ack = i2c_get_sda(self);

    i2c_set_scl(self, 0);
    i2c_delay(self->delay >> 1);

    return ack;
}

static void i2c_write_bit(struct i2c_bitbang *self, uint8_t bit)
{
    /* assert: scl=0 */
    i2c_set_sda(self, bit);
    i2c_delay_2(self->delay);
    i2c_set_scl(self, 1);
    i2c_delay(self->delay);
    i2c_set_scl(self, 0);
    i2c_delay(self->delay >> 1);
}

/**
 *     |          b0         | b1~b6 |          b7         |       NACK/ACK      |
 *                 __________                    __________            __________
 *  SCL __________|          |---x---|__________|          |__________|          |__
 */
static uint8_t i2c_write_byte(struct i2c_bitbang *self, uint8_t byte)
{
    /* MSB format */
    uint8_t i;
    for (i = 0; i < 8; ++i) {
        /* assert: scl=0 */
        i2c_write_bit(self, ((byte << i) & 0x80) ? 1 : 0);
    }

    /* Return inverted ACK bit, i.e. '0' for ACK, '1' for NACK */
    return i2c_wait_ack(self);
    /* assert: scl is low (sda undef) */
}

static uint8_t i2c_read_byte(struct i2c_bitbang *self)
{
    uint8_t i;
    uint8_t indata = 0x00;
    /* assert: scl=0 */
    i2c_set_sda(self, 1); /* Stop driving low, so slave has control */
    i2c_delay_2(self->delay);

    /* MSB format */
    for (i = 0; i < 8; ++i) {
        i2c_set_scl(self, 1);
        i2c_delay(self->delay);

        if (i2c_get_sda(self))
            indata |= 1 << (7 - i);

        i2c_set_scl(self, 0);
        i2c_delay(self->delay >> 1);
    }
    /* assert: scl=0 */
    return indata;
}

int i2c_bitbang_xfer(struct i2c_bitbang *self, struct i2c_msg *msgs, 
                uint16_t num_msgs, uint16_t slave_address)
{
    int i;
    int result = -1;
    struct i2c_msg *pmsg;
    uint8_t *buf, *buf_end;
    uint16_t addr;

    if (!num_msgs || !msgs)
        return result;

    i2c_start(self);
    for (i = 0; i < num_msgs; ++i) {
        pmsg = &msgs[i];
        
        if (i && (pmsg->flags & I2C_MSG_RESTART)) {
            i2c_repeat_start(self);
        } else if (pmsg->flags & I2C_MSG_STOP) {
            // enforced stop condition
            i2c_stop(self);
            if (i != num_msgs - 1)
                i2c_start(self);
        }
        

        /* send slave address firstly */
        addr = slave_address << 1;
        addr |= pmsg->flags & I2C_MSG_READ ? 0x01 : 0x00;
        if (i2c_write_byte(self, addr)) {
            /* no ack */
            goto __end;
        }

        buf = pmsg->buf;
        buf_end = buf + pmsg->len;
        if (pmsg->flags & I2C_MSG_READ) {
            /* read */
            while (buf < buf_end) {
                *buf = i2c_read_byte(self);
                buf++;
                /* not sent ack until last msg */
                i2c_write_bit(self, buf == buf_end);
            }
        } else { 
            /* write */
            while (buf < buf_end) {
                if (i2c_write_byte(self, *buf++)) {
                    /* no ack */
                    goto __end;
                }
            }
        }
    }
    
    result = 0;
__end:
    i2c_stop(self);

    return result;
}

int i2c_bitbang_recover_bus(struct i2c_bitbang *self)
{
    int i;

    /*
     * The I2C-bus specification and user manual (NXP UM10204
     * rev. 6, section 3.1.16) suggests the master emit 9 SCL
     * clock pulses to recover the bus.
     *
     * The Linux kernel I2C bitbang recovery functionality issues
     * a START condition followed by 9 STOP conditions.
     *
     * Other I2C slave devices (e.g. Microchip ATSHA204a) suggest
     * issuing a START condition followed by 9 SCL clock pulses
     * with SDA held high/floating, a REPEATED START condition,
     * and a STOP condition.
     *
     * The latter is what is implemented here.
     */

    /* Start condition */
    i2c_start(self);

    /* 9 cycles of SCL with SDA held high */
    for (i = 0; i < 9; i++) {
        i2c_write_bit(self, 1);
    }

    /* Another start condition followed by a stop condition */
    i2c_repeat_start(self);
    i2c_stop(self);

    /* Check if bus is clear */
    if (i2c_get_sda(self)) {
        return 0;
    } else {
        return -1;
    }
}

#if BITBANG_V1
void i2c_bitbang_init(struct i2c_bitbang *bitbang, 
                const struct i2c_bitbang_ops *ops, void *io_context)
{
    bitbang->ops = ops;
    bitbang->io_ctx = io_context;
}
#elif BITBANG_V2
int i2c_bitbang_init(struct i2c_bitbang *bitbang, void *io_context)
{
    if (!bitbang)
        return -1;
    if (!io_context)
        return -1;
    
    bitbang->io_ctx = io_context;
    return 0;
}
#endif /* BITBANG_V1 */

