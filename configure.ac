dnl Require autoconf version >= 2.57
AC_PREREQ(2.57)


dnl ############# Initialization

AC_INIT([crusher264], [1.1.1], [piratfm@gmail.com])

dnl Version 1.7 of automake is recommended
AM_INIT_AUTOMAKE(crusher264, 1.1.1)
AM_CONFIG_HEADER(config.h)


dnl ############# Compiler and tools Checks

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_C_INLINE
AC_STDC_HEADERS
AM_PROG_LIBTOOL


AC_DEFUN([APR_HELP_STRING],[ifelse(regexp(AC_ACVERSION, 2\.1), -1, 
AC_HELP_STRING([$1],[$2]),[  ][$1] substr([                       
],len($1))[$2])])


dnl check byte ordering



dnl  ------ define IS_LITTLE_ENDIAN ------
# We try to use, where possible the headers <endian.h>, <mach/endian.h>,
# and <machine/endian.h>, in that order.  They must define the macros
# __LITTLE_ENDIAN and __BYTE_ORDER.  We structure the test so that if they 
# do NOT define __LITTLE_ENDIAN or __BYTE_ORDER then we will disable
# the little-endian optimizations, so the resulting code should be correct,
# but not as fast, if there's a broken endian.h somewhere.
#
# If none of those headers exist, we fallback on a runtime test.
# TODO: support --with-endianness=[little|big]

AH_TEMPLATE(HAVE_BIGENDIAN, [], [use big endian byte ordering])
AH_TEMPLATE(HAVE_LITTLEENDIAN, [], [use little endian byte ordering])

knows_endianness=0
is_little_endian=1

AC_MSG_CHECKING([Checking target endianess])
AC_ARG_WITH([endianness], [APR_HELP_STRING([--with-endianness], [force target CPU endianess [[little|big|auto]] [[default=auto]]])],
[
    if test "$with_endianness" = "little"; then
        is_little_endian=1
        knows_endianness=1
    fi

    if test "$with_endianness" = "big"; then
        is_little_endian=0
        knows_endianness=1
    fi
], [AC_MSG_RESULT([auto])])


if test $knows_endianness != 1 ; then
 AC_CHECK_HEADERS([endian.h], [has_endian_h=1; knows_endianness=1], [has_endian_h=0])
 if test $knows_endianness = 1 ; then
  AC_TRY_COMPILE([#include <endian.h>], [
   switch (1) { case __LITTLE_ENDIAN: break;
		case __BYTE_ORDER: break; } ],
    [is_little_endian=0], [is_little_endian=1])
 else

  # ------------------ try <mach/endian.h>  ------------------
  AC_CHECK_HEADERS([mach/endian.h], [has_mach_endian_h=1; knows_endianness=1], [has_mach_endian_h=0])
  if test $knows_endianness = 1 ; then
    AC_TRY_COMPILE([#include <mach/endian.h>],[
      switch (1) { case __LITTLE_ENDIAN: break;
		   case __BYTE_ORDER: break; }
			      ],
      [is_little_endian=0], [is_little_endian=1])
  fi
  # ------------------ try <machine/endian.h>  ------------------
  if test $knows_endianness = 0; then
    AC_CHECK_HEADERS([machine/endian.h], [has_machine_endian_h=1; knows_endianness=1], [has_machine_endian_h=0])
    if test $knows_endianness = 1 ; then
      AC_TRY_COMPILE([#include <machine/endian.h>],[
	switch (1) { case __LITTLE_ENDIAN: break;
		     case __BYTE_ORDER: break; }
				],
	[is_little_endian=0], [is_little_endian=1])
    fi
  fi
  if test $knows_endianness = 0; then
    AC_MSG_CHECKING([for little-endianness via runtime check])
    AC_RUN_IFELSE([#include <inttypes.h>
    int main() {
      uint32_t v = 0x01020304;
      return memcmp (&v, "\4\3\2\1", 4) == 0 ? 0 : 1;
    }
    ], [is_little_endian=1; result=yes], [is_little_endian=0; result=no])
    AC_MSG_RESULT($result)
  fi
 fi
fi

AC_MSG_CHECKING([Detect target endianess])
if test $is_little_endian = 1; then
  AC_MSG_RESULT([little-endian])
  AC_DEFINE(HAVE_LITTLEENDIAN)
else
  AC_MSG_RESULT([big-endian])
  AC_DEFINE(HAVE_BIGENDIAN)
fi


dnl ############## Arguments Checks

AC_MSG_CHECKING([enabled assembler optimisations])
AC_ARG_WITH([asm], [APR_HELP_STRING([--with-asm], [enable assembler optimisations [[default=yes]]])],
[
    if test "$with_asm" != "no"; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi
], [AC_MSG_RESULT([yes])])

if test "$with_asm" != "no"; then
    AC_DEFINE([ASM_OPTIMISATIONS], 1, [assembler optimisations])
fi



dnl ############## Arguments Checks

AC_MSG_CHECKING([enabled command-line interface with codec])
AC_ARG_WITH([codeccli], [APR_HELP_STRING([--with-codeccli], [enabled command-line interface with codec [[default=no]]])],
[
    if test "$with_codeccli" != "yes"; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
    fi
], [AC_MSG_RESULT([no])])

if test "$with_codeccli" == "yes"; then
    AC_DEFINE([CODEC_CLI], 1, [command-line interface with codec])
fi


dnl ############## Arguments Checks

AC_MSG_CHECKING([enabled RTMP source])
AC_ARG_WITH([rtmpsource], [APR_HELP_STRING([--with-rtmpsource], [enabled RTMP source, that able streaming to server [[default=no]]])],
[
    if test "$with_rtmpsource" != "yes"; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
    fi
], [AC_MSG_RESULT([no])])

PKG_CHECK_MODULES(RTMP, [librtmp >= v2.2c], [
	if test "$with_rtmpsource" == "yes"; then
    	AC_DEFINE([RTMP_SOURCE], 1, [RTMP streaming source])
    	CRUSHER_RTMP="crusherRTMP"
	fi
], [
	if test "$with_rtmpsource" == "yes"; then
		AC_MSG_ERROR([librtmp not found!])
	fi
])


dnl ############## Arguments Checks

AC_MSG_CHECKING([enabled RTP streamer])
AC_ARG_WITH([rtpstreamer], [APR_HELP_STRING([--with-rtpstreamer], [enabled RTP streamer [[default=no]]])],
[
    if test "$with_rtpstreamer" != "yes"; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
    fi
], [AC_MSG_RESULT([no])])

if test "$with_rtpstreamer" == "yes"; then
    AC_DEFINE([RTP_STREAMER], 1, [RTP streamer])
    CRUSHER_RTP="crusherRTP"
fi




dnl ############## Arguments Checks

AH_TEMPLATE([DBG], [Define as 1 if debugging needed])
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_WITH([debug], [APR_HELP_STRING([--with-debug[[=level]]], [Enable debugging level=0..3 (default: 1)])
APR_HELP_STRING([--without-debug], [Disable debugging.])],
[
    case "$with_debug" in
	no)
	    AC_MSG_RESULT(no)
	    ;;
	yes)
	    AC_MSG_RESULT(yes)
	    AC_DEFINE(DBG)
	    CFLAGS="$CFLAGS -g"
	    ;;
	*)
	    AC_MSG_RESULT(DBG=$with_debug)
	    AC_DEFINE_UNQUOTED(DBG,$with_debug)
	    CFLAGS="$CFLAGS -g"
	    ;;
    esac
], [
    AC_MSG_RESULT(default=yes)
    AC_DEFINE(DBG)
    CFLAGS="$CFLAGS -g" 
]
)


AC_DEFINE(TEST_MEMBLOCK_SIZE, [1], [test memblocks to be divisible by 4 w/o remainder])

AH_TEMPLATE([ARCH_X86], [Define as 1 if host is an I386])
AH_TEMPLATE([ARCH_X86_64], [Define as 1 if host is an AMD64])
AH_TEMPLATE([ARCH_ARM], [Define as 1 if host is an ARM])
AH_TEMPLATE([HAVE_ARMV6], [Define if ARM is higher or equal 6])
AH_TEMPLATE([HAVE_BSWAP], [have byteswap])

case "$host_cpu" in
    i?86)
        AC_DEFINE(ARCH_X86)
        ;;
    x86_64)
        AC_DEFINE(ARCH_X86)
        AC_DEFINE(ARCH_X86_64)
        AC_DEFINE(HAVE_BSWAP)
        ;;
    armv5*e*|arm[79]*e*|arm9[24]6*|arm96*|arm102[26])
        #CRUSHER264_CFLAGS+="-mv=5e"
	#For some reason this breaks ffmpeg 0.6.1 on the Dockstar
	CRUSHER264_CFLAGS+=""        
	AC_DEFINE(ARCH_ARM)
        ;;
    armv4*|arm7*|arm9[24]*)
        CRUSHER264_CFLAGS+="-mv=4"
        AC_DEFINE(ARCH_ARM)
        ;;
    armv7-a|cortex-a*|armv7-r|cortex-r*|armv7-m|cortex-m*|armv6*|arm11*)
        AC_DEFINE(HAVE_ARMV6)
        AC_DEFINE(ARCH_ARM)
        ;;
    arm*|cortex*)
        AC_DEFINE(ARCH_ARM)
        ;;
    *)
        ;;
esac

PKG_CHECK_MODULES(USB, [libusb >= 0.1.12])

dnl pkgconfig/shout-config.
dnl If pkgconfig is found, use it and disable shout-config, otherwise do the
dnl opposite, unless the user overrides.

AC_ARG_ENABLE([pkgconfig], 
    AC_HELP_STRING([--disable-pkgconfig],[disable pkgconfig data files (auto)]),
    [dopkgconfig="$enableval"], [dopkgconfig="maybe"])
if test "$dopkgconfig" = "maybe"
then
    AC_CHECK_PROG([PKGCONFIG], [pkg-config], [yes], [no])
else
    AC_MSG_CHECKING([whether pkgconfig should be used])
    PKGCONFIG="$dopkgconfig"
    AC_MSG_RESULT([$PKGCONFIG])
fi
    AM_CONDITIONAL([HAVE_PKGCONFIG], [test "$PKGCONFIG" != "no"])

CRUSHER264_VERSION="$VERSION"
CRUSHER264_CPPFLAGS="-I$crusher264_includedir $USB_CPPFLAGS"
CRUSHER264_CFLAGS+="$USB_CFLAGS"
CRUSHER264_LIBS="-lcrusher264 -lm ${USB_LIBS}"


dnl ############## Header Checks
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h usb.h])


AC_DEFINE(GCC_PACK, [__attribute__((packed))], [Using GCC packed structs])
AC_SUBST(CRUSHER264_LIBS)
AC_SUBST(CRUSHER264_REQUIRES)
AC_SUBST(CRUSHER264_CPPFLAGS)
AC_SUBST(CRUSHER264_CFLAGS)
AC_SUBST(CRUSHER264_OPTIONAL)

dnl Make substitutions

AC_SUBST(LIBTOOL_DEPS)
AC_SUBST(OPT)
AC_SUBST(LIBS)
AC_SUBST(CFLAGS)
AC_SUBST(PROFILE)

AC_SUBST(USB_CFLAGS)
AC_SUBST(USB_LIBS)

AC_SUBST(CRUSHER_RTMP)
AC_SUBST(RTMP_CFLAGS)
AC_SUBST(RTMP_LIBS)

AC_SUBST(CRUSHER_RTP)

dnl ############## Final Output
CFLAGS="$CFLAGS -Wall"

AC_CONFIG_FILES([crusher264.spec crusher264-config crusher264.pc])
AC_OUTPUT([ Makefile src/Makefile include/Makefile sample/Makefile firmware/Makefile ])
