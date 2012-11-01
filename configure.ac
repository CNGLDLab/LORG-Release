dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT([lorg_tools], [SVN], [jleroux@computing.dcu.ie])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

LT_INIT

CXXFLAGS=""


AC_PROG_CXX

AC_LANG_CPLUSPLUS

AC_CONFIG_HEADERS([config.h])


AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

AC_PROG_SED

##not sure why we need that
#AC_PROG_LEX

AC_C_INLINE
AC_HEADER_STDBOOL
AC_CHECK_HEADER([string],,AC_MSG_ERROR(STL classes missing ?))
AC_CHECK_HEADER([list],,AC_MSG_ERROR(STL classes missing ?))
AC_CHECK_HEADER([vector],,AC_MSG_ERROR(STL classes missing ?))

##AC_CHECK_HEADER([google/malloc_extension.h],[use_tcmalloc=yes],AC_MSG_WARN(google malloc missing ?))

AC_CHECK_FUNCS([memset],,,AC_MSG_ERROR(memset is missing))
AC_CHECK_FUNCS([strdup],,,AC_MSG_ERROR(strdup is missing))


##AM_CONDITIONAL([WANT_TEST],[test "$build_test" = yes])
##AM_CONDITIONAL([WANT_TCMALLOC],[test "$use_tcmalloc" = yes])

#Fix a bug in Linux with tbb tick_count (need -ltr)
case "$host_os" in
    linux*)
        AM_CONDITIONAL(WANT_LTR, true);;
    *)
        AM_CONDITIONAL(WANT_LTR, false);;
esac


AC_ARG_WITH(tcmalloc, [message],
                      [
                      if test "$withval" = "no"; then
                        want_tcmalloc="no"
                      elif test "$withval" = "yes"; then
                        want_tcmalloc="yes"
                      else
                        want_tcmalloc="yes"
                      fi
                   ], [ want_tcmalloc="no" ])

AM_CONDITIONAL([WANT_TCMALLOC], [test "x$want_tcmalloc" != "xno"])


AX_BOOST_BASE([1.46])
AX_BOOST_REGEX
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_THREAD

AX_TBB([], AC_MSG_ERROR([cannot find the Intel TBB library consider to give --with-tbb to link it]))

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/training/Makefile
])
AC_OUTPUT
