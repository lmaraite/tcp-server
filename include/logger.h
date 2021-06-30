#ifndef LOGGER
#define LOGGER

#define STDIO_SEM_KEY "stdio_sem"

typedef short LoggingLevel;
#define OFF 0
#define ERROR 1
#define WARN  2
#define INFO 3
#define DEBUG 4
#define TRACE 5

void error(const char *msg, ...);

void warn(const char *format, ...);

void info(const char *format, ...);

void debug(const char *format, ...);

void trace(const char *format, ...);

#endif
