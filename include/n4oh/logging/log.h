#ifndef _LOGGING_LOG_H_
#define _LOGGING_LOG_H_

#include <stdarg.h>
#include <stdint.h>

/**
 * @addtogroup log_constants Logging Constants
 * @{
 */

/**
 * Print no output.
 */
#define LOG_LEVEL_QUIET     0U

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 */
#define LOG_LEVEL_ERROR       10U

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems.
 */
#define LOG_LEVEL_WARN       20U

/**
 * Standard information.
 */
#define LOG_LEVEL_INFO       30U

/**
 * Stuff which is only useful for developers.
 */
#define LOG_LEVEL_DEBUG       40U

/** 
 * Provided string is printed as is without appending any characters 
 * (e.g., color or newline).
 */
#define LOG_LEVEL_RAW       LOG_LEVEL_QUIET

/**
 * @}
 */

#define LOG_ERROR(inst, fmt, ...) log_make_msg(inst, LOG_LEVEL_ERROR,            fmt,                     ##__VA_ARGS__)
#define LOG_WARN(inst, fmt, ...)  log_make_msg(inst,  LOG_LEVEL_WARN,            fmt,                     ##__VA_ARGS__)
#define LOG_INFO(inst, fmt, ...)  log_make_msg(inst,  LOG_LEVEL_INFO,            fmt,                     ##__VA_ARGS__)
#define LOG_DEBUG(inst, fmt, ...) log_make_msg(inst, LOG_LEVEL_DEBUG, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

typedef struct log_instance {
    const char *name;
    int version;
    void *context;
    void *parent;
} log_instance_t;

typedef void (*log_callback_t)(void *inst, int level, const char *fmt, va_list vl);
typedef uint32_t (*log_gettime_t)(void);

void log_make_msg(void *inst, int level, const char *fmt, ...);
void log_set_callback(log_callback_t callback);
void log_set_gettime(log_gettime_t gettime);
void log_set_level(int level);
int log_get_level(void);
int log_init(void);

#endif /* _LOGGING_LOG_H_ */
