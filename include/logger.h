#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifndef DEFAULT_LOG_STREAM
#define DEFAULT_LOG_STREAM stderr
#endif

#define LOG(STREAM, LOG_LEVEL, fmt, ...)                         \
  do {                                                           \
    fprintf(STREAM, "[%s] " fmt "\n", LOG_LEVEL, ##__VA_ARGS__); \
    fflush(STREAM);                                              \
  } while (0)

#define BEGIN_COLOR(STREAM, COLOR_CODE) fprintf(STREAM, "\033[%sm", COLOR_CODE);

#define END_COLOR(STREAM) fprintf(STREAM, "\033[39m");

// Use default console colors
#define INFO(fmt, ...)                                   \
  do {                                                   \
    LOG(DEFAULT_LOG_STREAM, "INFO", fmt, ##__VA_ARGS__); \
    fflush(DEFAULT_LOG_STREAM);                          \
  } while (0)

// Warning log level, use yellow color
// Modified the fmt so that the color is applied
#define WARN(fmt, ...)                                   \
  do {                                                   \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "33");               \
    LOG(DEFAULT_LOG_STREAM, "WARN", fmt, ##__VA_ARGS__); \
    END_COLOR(DEFAULT_LOG_STREAM);                       \
    fflush(DEFAULT_LOG_STREAM);                          \
  } while (0)

// Error log level, use red color
#define ERROR(fmt, ...)                                   \
  do {                                                    \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "31");                \
    LOG(DEFAULT_LOG_STREAM, "ERROR", fmt, ##__VA_ARGS__); \
    END_COLOR(DEFAULT_LOG_STREAM);                        \
    fflush(DEFAULT_LOG_STREAM);                           \
  } while (0)

// Debug log level, use blue color
// Also attach the file name and line number
// First check if NDEBUG is defined, if it is, do nothing
#ifndef NDEBUG
#define DEBUG(fmt, ...)                                                  \
  do {                                                                   \
    BEGIN_COLOR(DEFAULT_LOG_STREAM, "34");                               \
    LOG(DEFAULT_LOG_STREAM, "DEBUG", "[%s:%d] " fmt, __FILE__, __LINE__, \
        ##__VA_ARGS__);                                                  \
    END_COLOR(DEFAULT_LOG_STREAM);                                       \
    fflush(DEFAULT_LOG_STREAM);                                          \
  } while (0)
#else
#define DEBUG(fmt, ...)
#endif

#ifndef NDEBUG
#define TRACE() TraceGuard trace_guard(DEFAULT_LOG_STREAM, __PRETTY_FUNCTION__);

// Here we use some RAII magic to make sure the TRACE macro does
class TraceGuard {
 public:
  explicit TraceGuard(FILE *stream, const char *func_name)
      : stream(stream), func_name(func_name) {
    BEGIN_COLOR(stream, "36");
    LOG(stream, "TRACE", "[%s] %s", func_name, "Enter");
    END_COLOR(stream);
    fflush(stream);
  }

  ~TraceGuard() {
    BEGIN_COLOR(stream, "36");
    LOG(stream, "TRACE", "[%s] %s", func_name, "Exit");
    END_COLOR(stream);
    fflush(stream);
  }

 private:
  FILE *stream = nullptr;
  const char *func_name = nullptr;
};
#else
#define TRACE()
#endif

#endif
