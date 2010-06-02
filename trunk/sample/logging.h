#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdarg.h>

/* LOGGING */
enum {
        LOG_NULL,
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO,
        LOG_DEBUG,
        LOG_XTREME,
};

extern int     loglevel;

void fail(int level, const char *format, ...);
void logwrite(int level, const char *format, ...);
void logwrite2(int level, const char *format, va_list pvar);
void print_bytes(int level, char *bytes, int len);

#endif /*LOGGING_H_*/
