#                                               -*- Autoconf -*-
# vim:tw=0:et:ts=4:sw=4
# Process this file with autoconf to produce a configure script.

####################################
####################################
# change "unconfigure version" here.
AC_INIT([libsmbios],[unconfigured version])

# allow configurer to override version #
test -n "$RELEASE_MAJOR"      || RELEASE_MAJOR=2
test -n "$RELEASE_MINOR"      || RELEASE_MINOR=2
test -n "$RELEASE_MICRO"      || RELEASE_MICRO=8
test -n "$RELEASE_EXTRA"      || RELEASE_EXTRA=

#NOTE: libtool version, below, is different and distinct from 'marketing' version, above.
# non-interface changes/bugfixes:            revision++;
# interface add:                  current++; revision=0; age++;
# interface remove/change:        current++; revision=0; age=0;
LIBTOOL_CURRENT=3
LIBTOOL_REVISION=0
LIBTOOL_AGE=1
####################################
####################################

AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR([pkg])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([subdir-objects tar-ustar dist-bzip2 dist-lzma no-define ])
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
AC_PROG_INSTALL
AM_PATH_PYTHON

# windows dll support
# newer libtool uses this:
#LT_INIT([shared static win32-dll dlopen])
# but the version we are using uses this:
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

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
AC_SYS_LARGEFILE  # needed for rhel4 compile
AC_CHECK_FUNCS([strlcpy strlcat getpagesize memmove memset munmap strerror strndup strtol strtoul])

dnl Check for doxygen support
AC_PATH_PROG([DOXYGEN], [doxygen])
AM_CONDITIONAL(HAVE_DOXYGEN, [test $DOXYGEN])
if test x$wantdoxygen != xyes ; then
    DOXYGEN=
    AM_CONDITIONAL(HAVE_DOXYGEN, [false])
fi

dnl Check for graphviz support
AC_PATH_PROG([DOT], [dot])
AM_CONDITIONAL(HAVE_DOT, [test $DOT])
if test x$wantgraphviz != xyes ; then
    DOT=
    AM_CONDITIONAL(HAVE_DOT, [false])
fi

# the manual assembly in the media direct sources fails to build on ia64 as it
# relies on more registers than available on ia64; there's no ia64 system with
# media direct, so it's disabled on ia64
AM_CONDITIONAL(BUILD_MEDIA_DIRECT, [test "$host_cpu" != ia64])
AM_CONDITIONAL(BUILD_LINUX, [test "$host_os" != mingw32])
AM_CONDITIONAL(BUILD_WINDOWS, [test "$host_os" = mingw32])

GETTEXT_PACKAGE=libsmbios-$RELEASE_MAJOR.$RELEASE_MINOR
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Name of the gettext message domain])

if test -z "$RELEASE_EXTRA"; then
    RELEASE_RPM_EXTRA=%{nil}
else
    RELEASE_RPM_EXTRA=$RELEASE_EXTRA
fi

AC_DEFINE_UNQUOTED(PACKAGE_STRING, ["$PACKAGE_NAME $RELEASE_MAJOR.$RELEASE_MINOR.$RELEASE_MICRO$RELEASE_EXTRA"], [package version])
AC_DEFINE_UNQUOTED(PACKAGE_VERSION, ["$RELEASE_MAJOR.$RELEASE_MINOR.$RELEASE_MICRO$RELEASE_EXTRA"], [package version])
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_VERSION,"$RELEASE_MAJOR.$RELEASE_MINOR.$RELEASE_MICRO$RELEASE_EXTRA","Full package version string")
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MAJOR,$RELEASE_MAJOR,"Major release version")
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MINOR,$RELEASE_MINOR,"Minor release version")
AC_SUBST(RELEASE_MAJOR)
AC_SUBST(RELEASE_MINOR)
AC_SUBST(RELEASE_MICRO)
AC_SUBST(RELEASE_EXTRA)
AC_SUBST(RELEASE_RPM_EXTRA)
AC_SUBST(LIBTOOL_CURRENT)
AC_SUBST(LIBTOOL_REVISION)
AC_SUBST(LIBTOOL_AGE)

LIBXML2_LIBS=$(xml2-config --libs)
LIBXML2_CFLAGS=$(xml2-config --cflags)
AC_SUBST(LIBXML2_LIBS)
AC_SUBST(LIBXML2_CFLAGS)

VERSION=[`echo ${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_MICRO}${RELEASE_EXTRA}`]
PACKAGE_VERSION=$VERSION
PACKAGE_STRING=[`echo ${RELEASE_NAME}-${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_MICRO}${RELEASE_EXTRA}`]
sed -i "s|(^#define PACKAGE_VERSION).*|\$1 \"$PACKAGE_VERSION\"|" confdefs.h
sed -i "s|(^#define PACKAGE_STRING).*|\$1 \"$PACKAGE_STRING\"|" confdefs.h

# generate files and exit
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([pkg/libsmbios.spec])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([libsmbios_c-uninstalled.pc:pkg/libsmbios_c-uninstalled.pc.in])
AC_CONFIG_FILES([pkg/libsmbios_c.pc:pkg/libsmbios_c.pc.in])
AC_OUTPUT
