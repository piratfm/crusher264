
EXTRA_PROGRAMS = crusherRTMP
bin_PROGRAMS = crusher264 $(CRUSHER_RTMP)

crusher264_SOURCES = crusherenc.c logging.c qbox.c wav.c adts.c imgConverter.c
crusher264_LDADD = $(top_builddir)/src/libcrusher264.la
crusher264_CFLAGS = -I$(top_srcdir)/include


crusherRTMP_SOURCES = crusherRTMP.c flvpkt.c logging.c qbox.c
crusherRTMP_LDADD = $(top_builddir)/src/libcrusher264.la @RTMP_LIBS@
crusherRTMP_CFLAGS = -I$(top_srcdir)/include @RTMP_CFLAGS@


noinst_HEADERS = \
	adts.h wav.h \
	qbox.h \
	logging.h \
	imgConverter.h \
	bswap.h \
	flvpkt.h
