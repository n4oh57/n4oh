#ifndef _SYS_H_
#define _SYS_H_

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD) || \
    defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || \
    defined (STM32F10X_XL) || defined (STM32F10X_CL)
#include "stm32f10x.h"
#define IDR_OFFSET 0x08
#define ODR_OFFSET 0x0C
#elif defined (STM32F405xx) || defined (STM32F415xx) || defined (STM32F407xx) || defined (STM32F417xx) || \
    defined (STM32F427xx) || defined (STM32F437xx) || defined (STM32F429xx) || defined (STM32F439xx) || \
    defined (STM32F401xC) || defined (STM32F401xE) || defined (STM32F410Tx) || defined (STM32F410Cx) || \
    defined (STM32F410Rx) || defined (STM32F411xE) || defined (STM32F446xx) || defined (STM32F469xx) || \
    defined (STM32F479xx) || defined (STM32F412Cx) || defined (STM32F412Rx) || defined (STM32F412Vx) || \
    defined (STM32F412Zx) || defined (STM32F413xx) || defined (STM32F423xx)
#include "stm32f4xx.h"
#define IDR_OFFSET 0x10
#define ODR_OFFSET 0x14
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
#endif

/* 把"位带地址＋位序号"转换成别名地址的宏 */
#define BITBAND(addr, bitnum) \
    ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
/* 把该地址转换成一个指针 */
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
/* 位带操作 */
#define PAin(bitnum)  (MEM_ADDR(BITBAND(GPIOA_BASE + IDR_OFFSET, bitnum)))
#define PAout(bitnum) (MEM_ADDR(BITBAND(GPIOA_BASE + ODR_OFFSET, bitnum)))
#define PBin(bitnum)  (MEM_ADDR(BITBAND(GPIOB_BASE + IDR_OFFSET, bitnum)))
#define PBout(bitnum) (MEM_ADDR(BITBAND(GPIOB_BASE + ODR_OFFSET, bitnum)))
#define PCin(bitnum)  (MEM_ADDR(BITBAND(GPIOC_BASE + IDR_OFFSET, bitnum)))
#define PCout(bitnum) (MEM_ADDR(BITBAND(GPIOC_BASE + ODR_OFFSET, bitnum)))
#define PDin(bitnum)  (MEM_ADDR(BITBAND(GPIOD_BASE + IDR_OFFSET, bitnum)))
#define PDout(bitnum) (MEM_ADDR(BITBAND(GPIOD_BASE + ODR_OFFSET, bitnum)))
#define PEin(bitnum)  (MEM_ADDR(BITBAND(GPIOE_BASE + IDR_OFFSET, bitnum)))
#define PEout(bitnum) (MEM_ADDR(BITBAND(GPIOE_BASE + ODR_OFFSET, bitnum)))
#define PFin(bitnum)  (MEM_ADDR(BITBAND(GPIOF_BASE + IDR_OFFSET, bitnum)))
#define PFout(bitnum) (MEM_ADDR(BITBAND(GPIOF_BASE + ODR_OFFSET, bitnum)))
#define PGin(bitnum)  (MEM_ADDR(BITBAND(GPIOG_BASE + IDR_OFFSET, bitnum)))
#define PGout(bitnum) (MEM_ADDR(BITBAND(GPIOG_BASE + ODR_OFFSET, bitnum)))

//以下为汇编函数
void WFI_SET(void);               // 执行WFI指令
void INTX_DISABLE(void);          // 关闭所有中断
void INTX_ENABLE(void);           // 开启所有中断
void MSR_MSP(unsigned long addr); // 设置堆栈地址

#endif /* _SYS_H_ */

