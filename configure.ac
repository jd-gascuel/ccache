dnl Process this file with autoconf to produce a configure script.

AC_INIT()
AC_PREREQ(2.52)

AC_MSG_NOTICE([Configuring ccache])

AC_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

AC_SUBST(ccache_os, unknown)
case $host in
    *mingw32* | *mingw64* | *cygwin* | *wince* | *mingwce*)
        ccache_os=mingw
        AC_DEFINE(CCACHE_MINGW, 1, Compiling on a MinGW platform)
        AC_DEFINE(_WIN32_WINNT,0x0600, Windows Vista or newer is required)
        ;;

    *msys*)
        ccache_os=msys
        AC_DEFINE(CCACHE_MSYS, 1, Compiling on a MSYS2 platform)
        AC_DEFINE(_WIN32_WINNT,0x0600, Windows Vista or newer is required)
        ;;

    *apple*)
        ccache_os=mac
        AC_DEFINE(CCACHE_MAC, 1, Compiling on an Apple platform)
        ;;

    *linux-gnu*)
        ccache_os=linux
        AC_DEFINE(CCACHE_LINUX, 1, Compiling on a Linux platform)
        ;;
esac

AC_SUBST(extra_libs)
AC_SUBST(include_dev_mk)
AC_SUBST(test_suites)
AC_SUBST(LIBS)

m4_include(m4/feature_macros.m4)

dnl Checks for programs.
# Note: autoconf crash if AC_PROG_CC_C89 and C99 are both *present*
# in two different if branches... *BUT* not if only one is inside if
AC_PROG_CC([gcc clang cc cl.exe])
case x$CC in
    x*gcc* | x*clang* | xcc)
        AC_PROG_CC_C99
        if test "$ac_cv_prog_cc_c99" = no; then
            AC_MSG_ERROR(cannot find a C99-compatible compiler)
        fi
        ;;
    *)
        ;;
esac

AC_PROG_CPP
AC_PROG_INSTALL
if test x${GCC} != x; then
    AC_PROG_RANLIB
    AC_CHECK_TOOL(AR, ar)
    if test -z "$AR"; then
        AC_MSG_ERROR(cannot find ar)
    fi
fi

AC_HEADER_DIRENT
AC_HEADER_TIME
AC_HEADER_SYS_WAIT

AC_CHECK_TYPES(long long)

AC_CHECK_HEADERS(ctype.h pwd.h stdlib.h string.h strings.h sys/time.h sys/mman.h)
AC_CHECK_HEADERS(termios.h)

AC_CHECK_FUNCS(gethostname)
AC_CHECK_FUNCS(getopt_long)
AC_CHECK_FUNCS(getpwuid)
AC_CHECK_FUNCS(gettimeofday)
AC_CHECK_FUNCS(mkstemp)
AC_CHECK_FUNCS(realpath)
AC_CHECK_FUNCS(strndup)
AC_CHECK_FUNCS(strtok_r)
AC_CHECK_FUNCS(unsetenv)
AC_CHECK_FUNCS(utimes)

AC_CACHE_CHECK([for compar_fn_t in stdlib.h],ccache_cv_COMPAR_FN_T, [
    AC_TRY_COMPILE(
        [#include <stdlib.h>],
        [void test_fn(void) { qsort(NULL, 0, 0, (__compar_fn_t)NULL); }],
        ccache_cv_COMPAR_FN_T=yes,
        ccache_cv_COMPAR_FN_T=no)])
if test x"$ccache_cv_COMPAR_FN_T" = x"yes"; then
   AC_DEFINE(HAVE_COMPAR_FN_T, 1,
             Define to 1 if you have the `__compar_fn_t' typedef.)
fi

dnl Replacements of snprintf and friends.
m4_include(m4/snprintf.m4)
HW_FUNC_VSNPRINTF
HW_FUNC_SNPRINTF
HW_FUNC_VASPRINTF
HW_FUNC_ASPRINTF

dnl Check if -lm is needed.
AC_SEARCH_LIBS(cos, m)


dnl Check for zlib
AC_ARG_WITH(bundled-zlib,
  [AS_HELP_STRING([--with-bundled-zlib],
    [use bundled zlib instead of the system's default zlib])])
if test x${with_bundled_zlib} = x; then
    AC_CACHE_CHECK(
        [for zlib >= 1.2.3],
        [ccache_cv_zlib_1_2_3],
        AC_TRY_COMPILE(
            [#include <zlib.h>],
            [
            #if (ZLIB_VERNUM >= 0x1230)
            #else
            #error "ZLIB_VERNUM < 0x1230"
            #endif
            ],
            [ccache_cv_zlib_1_2_3=yes],
            [ccache_cv_zlib_1_2_3=no]))
    AC_CHECK_LIB(z, gzdopen, true)
    if test $ccache_cv_zlib_1_2_3 = yes && test $ac_cv_lib_z_gzdopen = yes; then
        use_bundled_zlib=no
    else
        AC_MSG_WARN(using bundled zlib)
        use_bundled_zlib=yes
    fi
else
    AC_MSG_NOTICE(using bundled zlib as requested)
    use_bundled_zlib=yes
fi

if test x${use_bundled_zlib} = xyes; then
    CPPFLAGS="$CPPFLAGS -I\$(srcdir)/zlib"
    extra_libs="zlib/libz.\$(LIBEXT)"
    mkdir -p zlib
else
    LIBS="$LIBS -lz"
fi

dnl Compiler specific options
AC_SUBST(ccache_cc, unknown)
case x$CC in
    x*cl | x*cl.exe)
        ccache_cc=cl
        CFLAGS="-nologo -MD -W3 -GS -Ox"
        AC_DEFINE(_CRT_SECURE_NO_WARNINGS,  1, Disable false Microsoft warnings)
        AC_DEFINE(_CRT_NONSTDC_NO_WARNINGS, 1, Disable false Microsoft warnings)
        AC_DEFINE(_CRT_SECURE_NO_DEPRECATE, 1, Disable false Microsoft warnings)
        AC_DEFINE(CCACHE_CL, 1, Visual C compiler)
        AC_SUBST(LIBEXT,lib)
        ;;


    x*gcc*)
        ccache_cc=gcc
        CFLAGS="$CFLAGS -W -Werror"
        AC_DEFINE(CCACHE_GCC, 1, Gnu CC compiler)
        AC_SUBST(LIBEXT,a)
        ;;

    x*clang*)
        ccache_cc=clang
        CFLAGS="$CFLAGS -W -Werror"
        AC_DEFINE(CCACHE_CLANG, 1, clang compiler)
        AC_SUBST(LIBEXT,a)
        ;;

    *)
        CFLAGS="$CFLAGS -O2"
        AC_SUBST(LIBEXT,a)
        ;;
esac

dnl Linking on Windows needs ws2_32, and Psapi
if test ${ccache_os} = mingw -o ${ccache_os} = msys; then
    case x$CC in
        x*cl | x*cl.exe)
            LIBS="$LIBS ws2_32.lib Psapi.lib"
            ;;
        *)
            LIBS="$LIBS -lws2_32 -lPsapi"
            ;;
    esac
fi

AC_C_BIGENDIAN

AC_C_INLINE

dnl Check for "extern inline".
AC_CACHE_CHECK(
    for extern inline,
    ac_cv_c_extern_inline,
    [
    ac_cv_c_extern_inline=no
    AC_TRY_COMPILE(
        [
        extern $ac_cv_c_inline double foo(double x);
        extern $ac_cv_c_inline double foo(double x) { return x+1.0; };
        double foo (double x) { return x + 1.0; };
        ],
        [foo(1.0)],
        [ac_cv_c_extern_inline="yes"])])
if test "$ac_cv_c_extern_inline" != no ; then
    AC_DEFINE(HAVE_EXTERN_INLINE, 1,
              Define to 1 if your compiler supports extern inline)
fi

dnl Enable developer mode if dev.mk.in exists.
if test ! -f $srcdir/dev_mode_disabled && test "$RUN_FROM_BUILD_FARM" != yes; then
    AC_MSG_NOTICE(Developer mode enabled)
    AC_CONFIG_FILES([dev.mk])
    include_dev_mk='include dev.mk'
    version=`(git --git-dir=$srcdir/.git describe --dirty 2>/dev/null || echo vunknown) | sed -e 's/v//' -e 's/-/+/' -e 's/-/_/g'`
    echo "const char CCACHE_VERSION@<:@@:>@ = \"$version\";" >version.c
else
    AC_MSG_NOTICE(Developer mode disabled)
fi

if test ! -f $srcdir/version.c; then
    AC_MSG_WARN(unable to determine ccache version)
    echo "const char CCACHE_VERSION@<:@@:>@ = \"unknown\";" >version.c
fi

dnl Find test suite files.
test_suites=`ls $srcdir/test/test_*.c | egrep -v 'BASE|BACKUP|LOCAL|REMOTE' | xargs echo`

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

cat <<EOF >config.h.tmp
#ifndef CCACHE_CONFIG_H
#define CCACHE_CONFIG_H
EOF
cat config.h >>config.h.tmp
echo '#endif' >>config.h.tmp
mv config.h.tmp config.h

mkdir -p .deps test


AC_MSG_NOTICE(now build ccache by running make)
