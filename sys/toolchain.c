#include <stdio.h>
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
