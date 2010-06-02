#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crusher.h"

void write_log(const char *startstr, const char *format, ...)
{
	va_list		pvar;
	char		logbuffer[1024];

	va_start (pvar, format);
	vsnprintf(logbuffer, sizeof(logbuffer), format, pvar);
	va_end (pvar);

	fprintf(stderr, "%s: %s\n",
			startstr,
			logbuffer);
}

#ifdef DBG
void dump_bytes(char *bytes, int len) {
    int i;
    int count;
    int done = 0;
    
    while (len > done) {
		if (len > 32){
			count = 32; 
		} else {
			count = len;
		}
		fprintf(stderr, "%04x: ", done);
		
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
#endif
