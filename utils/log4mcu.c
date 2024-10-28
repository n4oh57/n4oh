#include "log4mcu.h"

#include <stdio.h>
#include <string.h>

#include "usart.h"
#if defined(OS_USING_FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif /* OS_USING_FREERTOS */

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

#if defined ( __GNUC__ ) && !defined (__clang__) 
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/******************************************************************************/

#if defined(OS_USING_FREERTOS)
SemaphoreHandle_t g_putc_mutex;
SemaphoreHandle_t g_log_mutex; 
#endif /* OS_USING_FREERTOS */

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
#if defined(OS_USING_FREERTOS)
    xSemaphoreTake(g_putc_mutex, portMAX_DELAY);
#endif /* OS_USING_FREERTOS */

	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x1000);

#if defined(OS_USING_FREERTOS)
    xSemaphoreGive(g_putc_mutex);
#endif /* OS_USING_FREERTOS */
	return ch;
}

static int s_log_level = LOG_LEVEL_INFO;
static log_callback_t s_log_callback;
static char s_fmt_content[512];

static const char *get_level_str(int level)
{
    switch (level) {
    case LOG_LEVEL_DEBUG:   return "DEBUG";
    case LOG_LEVEL_INFO:    return "INFO";
    case LOG_LEVEL_WARN:    return "WARNING";
    case LOG_LEVEL_ERROR:   return "ERROR";
    case LOG_LEVEL_RAW:     return "RAW";
    default:                return "";
    }
}

static uint32_t get_time(void)
{
    uint32_t time = 0;
    
    time = HAL_GetTick();
    
    return time;
}
static uint32_t (*log_time)(void) = get_time;

static inline void log_muetx_lock(void)
{
#if defined(OS_USING_FREERTOS)
    xSemaphoreTake(g_log_mutex, portMAX_DELAY);
#endif /* OS_USING_FREERTOS */
}

static inline void log_muetx_unlock(void)
{
#if defined(OS_USING_FREERTOS)
    xSemaphoreGive(g_log_mutex);
#endif /* OS_USING_FREERTOS */
}

static void mcu_fputs(const char *str)
{
    HAL_UART_Transmit(&huart1, (const uint8_t *)str, strlen(str), 0x1000);
}

#define MAKE_RUNTIME_MSG(buf, fmt, ...)                                 \
    do {                                                                \
        if (buf) {                                                      \
            buf[snprintf(buf, sizeof(buf), fmt, __VA_ARGS__)] = '\0';   \
        }                                                               \
    } while (0)
static void log_default_callback_func(void *inst, int level, const char *fmt, va_list vl)
{
    if (level > s_log_level)
        return;

    log_instance_t *instance = inst ? (log_instance_t *)inst : NULL;
    char part[3][32] = {0};
    
    
    // time
    uint32_t time = log_time();
    if (time) {
        MAKE_RUNTIME_MSG(part[0], "[%9.3f] ", time / 1000.0);
    }
    // log level
    MAKE_RUNTIME_MSG(part[1], "[%s] ", get_level_str(level));

    // class info
    if (instance) {
        MAKE_RUNTIME_MSG(part[2], "[%s @ %p] ", instance->name, instance);
    }
    // format
    memset(s_fmt_content, 0, sizeof(s_fmt_content));
    s_fmt_content[vsnprintf(s_fmt_content, sizeof(s_fmt_content), fmt, vl)] = '\0';
    
    for (uint8_t i = 0; i < 3; ++i) {
        mcu_fputs(part[i]);
    }
    mcu_fputs(s_fmt_content);
}

static void log_invoke_callback(void *inst, int level, const char *fmt, va_list vl)
{
    log_callback_t log_cb = s_log_callback;
    
    log_muetx_lock();
    if (log_cb)
        s_log_callback(inst, level, fmt, vl);
    log_muetx_unlock();
}

void log_make_msg(void *inst, int level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    log_invoke_callback(inst, level, fmt, vl);
    va_end(vl);
}

void log_set_callback(log_callback_t callback)
{
    s_log_callback = callback;
}

void log_set_level(int level)
{
    s_log_level = level;
}

int log_get_level(void)
{
    return s_log_level;
}

int log_init(void)
{
#if defined(OS_USING_FREERTOS)
    g_putc_mutex = xSemaphoreCreateMutex();
    if (!g_putc_mutex)
        return -1;
    g_log_mutex = xSemaphoreCreateMutex();
    if (!g_log_mutex) {
        vSemaphoreDelete(g_putc_mutex);
        return -2;
    }
#endif /* OS_USING_FREERTOS */
    
    s_log_callback = log_default_callback_func;
    
    return 0;
}


