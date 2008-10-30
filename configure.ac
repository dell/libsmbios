#                                               -*- Autoconf -*-
# vim:tw=0:et:ts=4:sw=4
# Process this file with autoconf to produce a configure script.

####################################
####################################
# change version here.
AC_INIT([libsmbios],[2.2.0])

RELEASE_NAME=libsmbios
RELEASE_MAJOR=2
RELEASE_MINOR=2
RELEASE_SUBLEVEL=0
RELEASE_EXTRALEVEL=

# Update the version information only immediately before a public release of your software. More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# If the library source code has changed at all since the last update, then increment revision (c:r:a becomes c:r+1:a).
# If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# If any interfaces have been added since the last public release, then increment age.
# If any interfaces have been removed since the last public release, then set age to 0.
LIBTOOL_CURRENT=3
LIBTOOL_REVISION=0
LIBTOOL_AGE=1
####################################
####################################

AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR([pkg])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AM_PATH_CPPUNIT(1.9.6)
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14])
AC_GNU_SOURCE
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([out/include/config.h:pkg/config.h.in])

dnl Help line for doxygen
AC_ARG_ENABLE(doxygen,
    AS_HELP_STRING(--disable-doxygen,Disable API docs build via Doxygen. default: enabled if doxygen present),
    [wantdoxygen=$enableval], [wantdoxygen=yes])

dnl Help line for graphviz
AC_ARG_ENABLE(graphviz,
    AS_HELP_STRING(--disable-graphviz,Enhance API docs with pretty graphs and pictures. default: enabled if graphviz present),
    [wantgraphviz=$enableval], [wantgraphviz=yes])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PATH_PYTHON

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h limits.h stdlib.h string.h sys/file.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_FSEEKO
AC_CHECK_FUNCS([strlcpy strlcat getpagesize memmove memset munmap strerror strndup strtol strtoul])

dnl Check for doxygen support
AC_PATH_PROG([DOXYGEN], [doxygen])
AM_CONDITIONAL(HAS_DOXYGEN, test $DOXYGEN)
if test x$wantdoxygen != xyes ; then
    DOXYGEN=
    AM_CONDITIONAL(HAS_DOXYGEN, false)
fi

dnl Check for graphviz support
AC_PATH_PROG([DOT], [dot])
AM_CONDITIONAL(HAS_DOT, test $DOT)
if test x$wantgraphviz != xyes ; then
    DOT=
    AM_CONDITIONAL(HAS_DOT, false)
fi

# the manual assembly in the media direct sources fails to build on ia64 as it
# relies on more registers than available on ia64; there's no ia64 system with
# media direct, so it's disabled on ia64
AM_CONDITIONAL(BUILD_MEDIA_DIRECT, [test "$host_cpu" != ia64])

GETTEXT_PACKAGE=libsmbios
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Name of the gettext message domain])

if test -z "$RELEASE_EXTRALEVEL"; then
    RELEASE_RPM_EXTRALEVEL=%{nil}
else
    RELEASE_RPM_EXTRALEVEL=$RELEASE_EXTRALEVEL
fi

AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_VERSION,"$VERSION","Full package version string")
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MAJOR,$RELEASE_MAJOR,"Major release version")
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MINOR,$RELEASE_MINOR,"Minor release version")
AC_SUBST([RELEASE_NAME])
AC_SUBST([RELEASE_MAJOR])
AC_SUBST([RELEASE_MINOR])
AC_SUBST([RELEASE_SUBLEVEL])
AC_SUBST([RELEASE_EXTRALEVEL])
AC_SUBST([RELEASE_RPM_EXTRALEVEL])
AC_SUBST([LIBTOOL_CURRENT])
AC_SUBST([LIBTOOL_REVISION])
AC_SUBST([LIBTOOL_AGE])

LIBXML2_LIBS=$(xml2-config --libs)
LIBXML2_CFLAGS=$(xml2-config --cflags)
AC_SUBST([ LIBXML2_LIBS LIBXML2_CFLAGS ])

# generate files and exit
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([out/libsmbios_c++.dox:pkg/libsmbios_c++.dox.in])
AC_CONFIG_FILES([out/libsmbios_c.dox:pkg/libsmbios_c.dox.in])
AC_CONFIG_FILES([pkg/libsmbios.spec])
AC_CONFIG_FILES([pkg/mk-rel-rpm.sh])
AC_CONFIG_FILES([po/Makefile.in])
AC_OUTPUT
