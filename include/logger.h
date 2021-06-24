#ifndef LOGGER
#define LOGGER

typedef enum LoggingLevel {
    OFF = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
} LoggingLevel;

void error(const char *msg, ...);

void warn(const char *format, ...);

void info(const char *format, ...);

void debug(const char *format, ...);

void trace(const char *format, ...);

#endif
