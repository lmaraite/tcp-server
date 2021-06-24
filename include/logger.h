#ifndef LOGGER
#define LOGGER

enum LOGGING_LEVEL {
    OFF = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
} LOGGING_LEVEL;

int error(const char *msg);
int error(const char *format, ...);

int warn(const char *msg);
int warn(const char *format, ...);

int info(const char *msg);
int info(const char *format, ...);

int debug(const char *msg);
int debug(const char *format, ...);

int trace(const char *msg);
int trace(const char *format, ...);

#endif
