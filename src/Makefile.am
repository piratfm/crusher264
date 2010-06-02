lib_LTLIBRARIES = libcrusher264.la

libcrusher264_la_LDFLAGS = -version-info 1:1:1 $(libtool_flags) @USB_LIBS@

libcrusher264_la_CFLAGS = -DDATADIR="\"${pkgdatadir}\"" -I$(top_srcdir)/include @USB_CFLAGS@

libcrusher264dir = $(includedir)/libcrusher264

libcrusher264_la_SOURCES = logging.c \
	crusher-fx2.c crusher.c crusher-i2c.c \
	mg1264-loader.c mg1264-cmd.c  mg1264-io.c mg1264-codec.c mg1264-mware.c mg1264-cli.c

noinst_HEADERS = \
	bswap.h \
	crusher-usb.h \
	crusher-i2c.h \
	mg1264.h \
	logging.h \
	mg1264-cli.h \
	mg1264-io.h \
	mg1264-mware.h
