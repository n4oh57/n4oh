#include <stdio.h>
#include "usart.h"
/******************************************************************************/
/************************** ARMCC compiler ************************************/

#if !defined(__MICROLIB)
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm (".global __use_no_semihosting\n\t");

FILE __stdout;

/* __use_no_semihosting was requested, but _sys_exit was */
void _sys_exit(int x)
{
    x = x;
}
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch)
{
    ch = ch;
}
#elif defined(__CC_ARM)
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};
FILE __stdout;

/* __use_no_semihosting was requested, but _sys_exit was */
void _sys_exit(int x)
{
    x = x;
}
#endif /* __ARMCC_VERSION */

#endif /* __MICROLIB */
/******************************************************************************/

#if defined ( __GNUC__ ) && !defined (__clang__) 
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x1000);
	return ch;
}
#endif /* __GNUC__ */
