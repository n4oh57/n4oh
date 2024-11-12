#ifndef _SYS_CM3_CM4_BITBAND_H_
#define _SYS_CM3_CM4_BITBAND_H_

/**
 * @book
 * @brief Cortex-M3/M4 Bit-band simple implement
 *
 * The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors
*                                              Author: Joseph Yiu
 */

/** 
 * convert bit-band region @p addr to bit-band alisa region address
 */
#define CM_ALIAS_ADDR(addr) ((addr & 0xF0000000) + (0x2000000))

/**
 * convert origin bit-band region address's 1MB offset @p addr to
 * bit-band alias region offset (32 bytes strides)
 */
#define CM_ALIAS_OFFSET(addr) ((addr & 0xFFFFF) << 5)

/**
 * 4 bytes offset in bit-band alias region
 * @p bit[0~7]
 */
#define CM_ALIAS_BIT_OFFSET(bit) ((bit) << 2)

/**
 * Make Cortex-M3/M4 predefined memory map 
 * e.g. SRAM[0x20000000-0x23FFFFFF] or Peripheral[0x40000000-0x43FFFFFF]
 * "bit-band" @p addr byte's @p bitnum to "bit-band alias" filed byte
 */
#define CM_BITBAND(addr, bitnum) \
    (CM_ALIAS_ADDR(addr) + CM_ALIAS_OFFSET(addr) + CM_ALIAS_BIT_OFFSET(bitnum))

#endif /* _SYS_CM3_CM4_BITBAND_H_ */
