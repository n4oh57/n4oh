#ifndef _DELAY_H_
#define _DELAY_H_

#include <stdint.h>

void delay_init(void);

void delay_us(volatile uint32_t n_us);

void delay_ms(volatile uint32_t n_ms);

#endif /* _DELAY_H_ */
