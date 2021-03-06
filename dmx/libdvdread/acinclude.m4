dnl AC_C_ATTRIBUTE_PACKED
dnl set ATTRIBUTE_PACKED if this is supported
AC_DEFUN([AC_C_ATTRIBUTE_PACKED],
    [AC_CACHE_CHECK([__attribute__ ((packed)) support],
	[ac_cv_c_attribute_packed],
	[AC_TRY_COMPILE([],
		[static struct { int a; } __attribute__ ((packed)) c = {0};],
		[ac_cv_c_attribute_packed=yes],
		[ac_cv_c_attribute_packed=no])
	])
     if test x"$ac_cv_c_attribute_packed" = x"yes"; then
	ATTRIBUTE_PACKED="__attribute__ ((packed))"
     fi])


dnl The following AC macros are from mpeg2dec.
dnl Written by Michel Lespinasse <walken@zoy.org>

dnl AC_C_ATTRIBUTE_ALIGNED
dnl define ATTRIBUTE_ALIGNED_MAX to the maximum alignment if this is supported
AC_DEFUN([AC_C_ATTRIBUTE_ALIGNED],
    [AC_CACHE_CHECK([__attribute__ ((aligned ())) support],
	[ac_cv_c_attribute_aligned],
	[ac_cv_c_attribute_aligned=0
	for ac_cv_c_attr_align_try in 2 4 8 16 32 64; do
	    AC_TRY_COMPILE([],
		[static char c __attribute__ ((aligned($ac_cv_c_attr_align_try))) = 0; return c;],
		[ac_cv_c_attribute_aligned=$ac_cv_c_attr_align_try])
	done])
    if test x"$ac_cv_c_attribute_aligned" != x"0"; then
	AC_DEFINE_UNQUOTED([ATTRIBUTE_ALIGNED_MAX],
	    [$ac_cv_c_attribute_aligned],[maximum supported data alignment])
    fi])

dnl AC_TRY_CFLAGS (CFLAGS, [ACTION-IF-WORKS], [ACTION-IF-FAILS])
dnl check if $CC supports a given set of cflags
AC_DEFUN([AC_TRY_CFLAGS],
    [AC_MSG_CHECKING([if $CC supports $1 flags])
    SAVE_CFLAGS="$CFLAGS"
    CFLAGS="$1"
    AC_TRY_COMPILE([],[],[ac_cv_try_cflags_ok=yes],[ac_cv_try_cflags_ok=no])
    CFLAGS="$SAVE_CFLAGS"
    AC_MSG_RESULT([$ac_cv_try_cflags_ok])
    if test x"$ac_cv_try_cflags_ok" = x"yes"; then
	ifelse([$2],[],[:],[$2])
    else
	ifelse([$3],[],[:],[$3])
    fi])

dnl AC_CHECK_GENERATE_INTTYPES_H (INCLUDE-DIRECTORY)
dnl generate a default inttypes.h if the header file does not exist already
dnl Written by: Michel Lespinasse <walken@zoy.org>
AC_DEFUN([AC_CHECK_GENERATE_INTTYPES],
    [rm -f $1/inttypes.h
    AC_CHECK_HEADER([inttypes.h],[],
        [AC_CHECK_SIZEOF([char])
        AC_CHECK_SIZEOF([short])
        AC_CHECK_SIZEOF([int])
        AC_CHECK_SIZEOF([long long])
        if test x"$ac_cv_sizeof_char" != x"1" -o \
            x"$ac_cv_sizeof_short" != x"2" -o \
            x"$ac_cv_sizeof_int" != x"4" -o \
            x"$ac_cv_sizeof_long_long" != x"8"; then
            AC_MSG_ERROR([can not build a default inttypes.h])
        fi
        cat >$1/inttypes.h << EOF
/* default inttypes.h for people who do not have it on their system */

#ifndef _INTTYPES_H
#define _INTTYPES_H
#if (!defined __int8_t_defined) && (!defined __BIT_TYPES_DEFINED__)
#define __int8_t_defined
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
#endif
#if (!defined _LINUX_TYPES_H)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#endif
#endif
EOF
        ])])

dnl AC_COMPILE_CHECK_SIZEOF (TYPE SUPPOSED-SIZE)
dnl abort if the given type does not have the supposed size
AC_DEFUN([AC_COMPILE_CHECK_SIZEOF],
    [AC_MSG_CHECKING(that size of $1 is $2)
    AC_TRY_COMPILE([],[switch (0) case 0: case (sizeof ($1) == $2):;],[],
	[AC_MSG_ERROR([can not build a default inttypes.h])])
    AC_MSG_RESULT([yes])])
