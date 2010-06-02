#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include <ctype.h>

#include "logging.h"

int     loglevel=LOG_INFO;


const char *dbg[] = {
	"[NULL]",
	"[ERROR]",
	"[WARNING]",
	"[INFO]",
	"[DEBUG]",
	"[XTREME]",
};

void fail(int level, const char *format, ...) {
	va_list		pvar;
	char		logbuffer[MAXPATHLEN];

	if (level > loglevel)
		return;

	va_start (pvar, format);
	vsnprintf(logbuffer, sizeof(logbuffer), format, pvar);
	va_end (pvar);


	fprintf(stderr, "%s: %s\n",
			level < 6 ? dbg[level] : "",
			logbuffer);
	exit(-1);
}

void logwrite(int level, const char *format, ...) {
	va_list		pvar;
	char		logbuffer[MAXPATHLEN];

	if (level > loglevel)
		return;

	va_start (pvar, format);
	vsnprintf(logbuffer, sizeof(logbuffer), format, pvar);
	va_end (pvar);

	fprintf(stderr, "%s app: %s\n",
			level < 6 ? dbg[level] : "",
			logbuffer);
}

void logwrite2(int level, const char *format, va_list pvar) {
	char		logbuffer[MAXPATHLEN];

	if (level > loglevel)
		return;

	vsnprintf(logbuffer, sizeof(logbuffer), format, pvar);

	fprintf(stderr, "%s app: %s\n",
			level < 6 ? dbg[level] : "",
			logbuffer);
}

void print_bytes(int level, char *bytes, int len) {
    int i;
    int count;
    int done = 0;
    
    if (level > loglevel)
		return;

    while (len > done) {
		if (len > 32){
			count = 32; 
		} else {
			count = len;
		}
		fprintf(stderr, "\t    ");
		
		for (i=0; i<count; i++) {	
	    	fprintf(stderr, "%02x ", (int)((unsigned char)bytes[done+i]));
		}
		
		for (i=count; i<32; i++) {	
	    	fprintf(stderr, "   ");
		}
		
		
		fprintf(stderr, "\t\"");
		
        for (i=0; i<count; i++) {
	    	fprintf(stderr, "%c", isprint(bytes[done+i]) ? bytes[done+i] : '.');
        }
        fprintf(stderr, "\"\n");
    	done += count;
    }
}
