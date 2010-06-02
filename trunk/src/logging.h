#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

void write_log(const char *startstr, const char *format, ...);


/* ISO-style:
 * #define error(...) fprintf(stderr, __VA_ARGS__);
 * GNU-style:
 * #define error(format, args...) fprintf(stderr, format , ## args);
 * */

/* always print error messages */
#define ERROR(a, args...) write_log("[ERROR]", a, ## args)
#ifdef DBG

#if DBG > 0
#define DEBUG(a, args...) write_log("[DEBUG]", a, ## args)
#else
#define DEBUG(a, args...)
#endif

#if DBG > 1
#define XTREME(a, args...) write_log("[XTREME]", a, ## args)
#else
#define XTREME(a, args...)
#endif

#if DBG > 2
#define DUMP(a, args...) write_log("[DUMP]", a, ## args)
void dump_bytes(char *bytes, int len);
#else
#define DUMP(a, args...)
#define dump_bytes(a,b)
#endif

#else
#define dump_bytes(a,b)
#define DEBUG(a, args...)
#define XTREME(a, args...)
#define DUMP(a, args...)
#endif

#endif /*LOGGING_H_*/
