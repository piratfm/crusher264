
SUBDIRS = src include sample firmware

if HAVE_PKGCONFIG
    pkgconfigdir = $(libdir)/pkgconfig
    pkgconfig_DATA = crusher264.pc
else
    bin_SCRIPTS = crusher264-config
endif

EXTRA_DIST = autogen.sh crusher264.pc crusher264.spec
