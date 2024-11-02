#include <n4oh/logging/log.h>

#include <stdio.h>
#include <string.h>

#if defined(OS_USING_FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif /* OS_USING_FREERTOS */

#if defined(OS_USING_FREERTOS)
SemaphoreHandle_t g_log_mutex; 
#endif /* OS_USING_FREERTOS */

static int s_log_level = LOG_LEVEL_INFO;
static log_callback_t s_log_callback = NULL;
static log_gettime_t s_log_gettime = NULL;
static char s_format_buf[512];

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

#define MAKE_RUNTIME_MSG(buf, fmt, ...)                                 \
    do {                                                                \
        if (buf) {                                                      \
            buf[snprintf(buf, sizeof(buf), fmt, __VA_ARGS__)] = '\0';   \
        }                                                               \
    } while (0)
static void log_default_callback_func(void *inst, int level, const char *fmt, va_list vl)
{
    uint8_t i;
    uint32_t time;

    if (level > s_log_level)
        return;

    log_instance_t *instance = inst ? (log_instance_t *)inst : NULL;
    char part[3][32] = {0};
    
    time = s_log_gettime ? s_log_gettime() : 0;
    if (time) {
        MAKE_RUNTIME_MSG(part[0], "[%9.3f] ", time / 1000.0);
    }
    
    MAKE_RUNTIME_MSG(part[1], "[%s] ", get_level_str(level));
    
    if (instance) {
        MAKE_RUNTIME_MSG(part[2], "[%s @ %p] ", instance->name, instance);
    }
    
    memset(s_format_buf, 0, sizeof(s_format_buf));
    s_format_buf[vsnprintf(s_format_buf, sizeof(s_format_buf), fmt, vl)] = '\0';
    
    for (i = 0; i < 3; ++i) {
        fputs(part[i], stdout);
    }
    fputs(s_format_buf, stdout);
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

void log_set_gettime(log_gettime_t gettime)
{
    s_log_gettime = gettime;
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
    g_log_mutex = xSemaphoreCreateMutex();
    if (!g_log_mutex) {
        return -1;
    }
#endif /* OS_USING_FREERTOS */
    
    s_log_callback = log_default_callback_func;
    
    return 0;
}


