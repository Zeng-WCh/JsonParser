#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifndef DEFAULT_LOG_STREAM
#define DEFAULT_LOG_STREAM stderr
#endif

#define LOG(STREAM, LOG_LEVEL, fmt, ...)                                       \
  do {                                                                         \
    fprintf(STREAM, "[%s] " fmt "\n", LOG_LEVEL, ##__VA_ARGS__);               \
  } while (0)

#define BEGIN_COLOR(STREAM, COLOR_CODE)                                        \
  do {                                                                         \
    fprintf(STREAM, "\033[%sm", COLOR_CODE);                                   \
  } while (0)

#define END_COLOR(STREAM)                                                      \
  do {                                                                         \
    fprintf(STREAM, "\033[0m");                                                \
  } while (0)

// Use default console colors
#define INFO(fmt, ...) LOG(DEFAULT_LOG_STREAM, "INFO", fmt, ##__VA_ARGS__)

// Warning log level, use yellow color
// Modified the fmt so that the color is applied
#define WARN(fmt, ...)                                                         \
  do {                                                                         \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "33");                                     \
    LOG(DEFAULT_LOG_STREAM, "WARN", fmt, ##__VA_ARGS__);                       \
    END_COLOR(DEFAULT_LOG_STREAM);                                             \
  } while (0)

// Error log level, use red color
#define ERROR(fmt, ...)                                                        \
  do {                                                                         \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "31");                                     \
    LOG(DEFAULT_LOG_STREAM, "ERROR", fmt, ##__VA_ARGS__);                      \
    END_COLOR(DEFAULT_LOG_STREAM);                                             \
  } while (0)

// Debug log level, use blue color
// Also attach the file name and line number
// First check if NDEBUG is defined, if it is, do nothing
#ifndef NDEBUG
#define DEBUG(fmt, ...)                                                        \
  do {                                                                         \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "34");                                     \
    LOG(DEFAULT_LOG_STREAM, "DEBUG", "[%s:%d] " fmt, __FILE__, __LINE__,       \
        ##__VA_ARGS__);                                                        \
    END_COLOR(DEFAULT_LOG_STREAM);                                             \
  } while (0)
#else
#define DEBUG(fmt, ...)
#endif

#endif
