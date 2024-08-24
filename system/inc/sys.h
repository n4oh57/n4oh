#ifndef _SYS_H_
#define _SYS_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/* 把"位带地址＋位序号"转换成别名地址的宏 */
#define BITBAND(addr, bitnum) \
    ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
/* 把该地址转换成一个指针 */
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
/* 位带操作 */
#define PAin(bitnum)  (MEM_ADDR(BITBAND(GPIOA_BASE + 0x10, bitnum))) // GPIOA_IDR
#define PAout(bitnum) (MEM_ADDR(BITBAND(GPIOA_BASE + 0x14, bitnum))) // GPIOA_ODR
#define PBin(bitnum)  (MEM_ADDR(BITBAND(GPIOB_BASE + 0x10, bitnum))) // GPIOB_IDR
#define PBout(bitnum) (MEM_ADDR(BITBAND(GPIOB_BASE + 0x14, bitnum))) // GPIOB_ODR
#define PCin(bitnum)  (MEM_ADDR(BITBAND(GPIOC_BASE + 0x10, bitnum))) // GPIOC_IDR
#define PCout(bitnum) (MEM_ADDR(BITBAND(GPIOC_BASE + 0x14, bitnum))) // GPIOC_ODR
#define PDin(bitnum)  (MEM_ADDR(BITBAND(GPIOD_BASE + 0x10, bitnum))) // GPIOD_IDR
#define PDout(bitnum) (MEM_ADDR(BITBAND(GPIOD_BASE + 0x14, bitnum))) // GPIOD_ODR
#define PEin(bitnum)  (MEM_ADDR(BITBAND(GPIOE_BASE + 0x10, bitnum))) // GPIOD_IDR
#define PEout(bitnum) (MEM_ADDR(BITBAND(GPIOE_BASE + 0x14, bitnum))) // GPIOD_ODR
#define PFin(bitnum)  (MEM_ADDR(BITBAND(GPIOF_BASE + 0x10, bitnum))) // GPIOF_IDR
#define PFout(bitnum) (MEM_ADDR(BITBAND(GPIOF_BASE + 0x14, bitnum))) // GPIOF_ODR
#define PGin(bitnum)  (MEM_ADDR(BITBAND(GPIOG_BASE + 0x10, bitnum))) // GPIOG_IDR
#define PGout(bitnum) (MEM_ADDR(BITBAND(GPIOG_BASE + 0x14, bitnum))) // GPIOG_ODR

//以下为汇编函数
void WFI_SET(void);               // 执行WFI指令
void INTX_DISABLE(void);          // 关闭所有中断
void INTX_ENABLE(void);           // 开启所有中断
void MSR_MSP(unsigned long addr); // 设置堆栈地址

#endif /* _SYS_H_ */

