#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "configuration.h"

void logformat(const char* level, const char* format, va_list arg) {
    char cur_time_str[21];
    struct tm cur_tm;

    pid_t pid = getpid();
    time_t cur_time_t = time(NULL);
    
    flockfile(stdout);

    strftime(cur_time_str, 21, "%FT%TZ", gmtime_r(&cur_time_t, &cur_tm));
    printf("%s %s [%7d] : ", cur_time_str, level, pid);
    vprintf(format, arg);
    printf("\n");
    fflush(stdout);

    funlockfile(stdout);
}

void error(const char *format, ...) {
    if(config.LOGGING_LEVEL - ERROR < 0) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    logformat("ERROR", format, arg);
    va_end(arg);
}

void warn(const char *format, ...) {
    if(config.LOGGING_LEVEL - WARN < 0) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    logformat(" WARN", format, arg);
    va_end(arg);
}

void info(const char *format, ...) {
    if(config.LOGGING_LEVEL - INFO < 0) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    logformat(" INFO", format, arg);
    va_end(arg);
}

void debug(const char *format, ...) {
    if(config.LOGGING_LEVEL - DEBUG < 0) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    logformat("DEBUG", format, arg);
    va_end(arg);
}

void trace(const char *format, ...) {
    if(config.LOGGING_LEVEL - TRACE < 0) {
        return;
    }
    va_list arg;
    va_start(arg, format);
    logformat("ERROR", format, arg);
    va_end(arg);
}

