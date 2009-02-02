#                                               -*- Autoconf -*-
# vim:tw=0:et:ts=4:sw=4
# Process this file with autoconf to produce a configure script.

##############################################################################
#  RELEASE VARIABLES
##############################################################################
#
# The following variables define the libsmbios release version.
#   This is the "marketing" version, or overall version of the project.
#   This doesnt have anything in relation to the ABI versions of individual
#   libraries, which are defined further below.
m4_define([libsmbios_major_version], [2])
m4_define([libsmbios_minor_version], [2])
m4_define([libsmbios_micro_version], [12])
# if you define any "extra" version info, include a leading dot (".")
m4_define([libsmbios_extra_version], [])

##########################################################
# ABI Version Variables
##########################################################
#NOTE: libtool version, below, is different and distinct from 'marketing' version, above.
# non-interface changes/bugfixes: current++; revision++; age++;
# interface add:                  current++; revision=0; age++;
# interface remove/change:        current++; revision=0; age=0;
m4_define([libsmbios_c_lt_current], [4])
m4_define([libsmbios_c_lt_revision], [1])
m4_define([libsmbios_c_lt_age], [2])

m4_define([libsmbios_cplusplus_lt_current], [3])
m4_define([libsmbios_cplusplus_lt_revision], [0])
m4_define([libsmbios_cplusplus_lt_age], [1])
##########################################################
# END ABI Version Variables
##########################################################
##############################################################################
# END RELEASE VARIABLES
##############################################################################

# put the versioning stuff together now.
m4_define([libsmbios_version],
          [libsmbios_major_version().libsmbios_minor_version().libsmbios_micro_version()libsmbios_extra_version()])

AC_INIT([libsmbios],[libsmbios_version()])
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

RELEASE_MAJOR=libsmbios_major_version()
AC_SUBST(RELEASE_MAJOR)
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MAJOR,$RELEASE_MAJOR,"Major release version")

RELEASE_MINOR=libsmbios_minor_version()
AC_SUBST(RELEASE_MINOR)
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MINOR,$RELEASE_MINOR,"Minor release version")

RELEASE_MICRO=libsmbios_micro_version()
AC_SUBST(RELEASE_MICRO)

RELEASE_EXTRA=libsmbios_extra_version()
AC_SUBST(RELEASE_EXTRA)

if test -z "$RELEASE_EXTRA"; then
    RELEASE_RPM_EXTRA=%{nil}
else
    RELEASE_RPM_EXTRA=$RELEASE_EXTRA
fi
AC_SUBST(RELEASE_RPM_EXTRA)

GETTEXT_PACKAGE=libsmbios-$RELEASE_MAJOR.$RELEASE_MINOR
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Name of the gettext message domain])

LIBSMBIOS_C_LIBTOOL_CURRENT=libsmbios_c_lt_current
LIBSMBIOS_C_LIBTOOL_REVISION=libsmbios_c_lt_revision
LIBSMBIOS_C_LIBTOOL_AGE=libsmbios_c_lt_age
LIBSMBIOS_C_SO_MAJOR=$(($LIBSMBIOS_C_LIBTOOL_CURRENT - $LIBSMBIOS_C_LIBTOOL_AGE))
LIBSMBIOS_C_SONAME=libsmbios_c.so.$LIBSMBIOS_C_SO_MAJOR
AC_SUBST(LIBSMBIOS_C_LIBTOOL_CURRENT)
AC_SUBST(LIBSMBIOS_C_LIBTOOL_REVISION)
AC_SUBST(LIBSMBIOS_C_LIBTOOL_AGE)
AC_SUBST(LIBSMBIOS_C_SO_MAJOR)
AC_SUBST(LIBSMBIOS_C_SONAME)

LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT=libsmbios_cplusplus_lt_current
LIBSMBIOS_CPLUSPLUS_LIBTOOL_REVISION=libsmbios_cplusplus_lt_revision
LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE=libsmbios_cplusplus_lt_age
LIBSMBIOS_CPLUSPLUS_SO_MAJOR=$(($LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT - $LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE))
LIBSMBIOS_CPLUSPLUS_SONAME=libsmbios.so.$LIBSMBIOS_CPLUSPLUS_SO_MAJOR
AC_SUBST(LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT)
AC_SUBST(LIBSMBIOS_CPLUSPLUS_LIBTOOL_REVISION)
AC_SUBST(LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE)
AC_SUBST(LIBSMBIOS_CPLUSPLUS_SO_MAJOR)
AC_SUBST(LIBSMBIOS_CPLUSPLUS_SONAME)

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

PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])

# generate files and exit
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([pkg/libsmbios.spec])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([libsmbios_c-uninstalled.pc:pkg/libsmbios_c-uninstalled.pc.in])
AC_CONFIG_FILES([pkg/libsmbios_c.pc])
AC_CONFIG_FILES([out/public-include/smbios/dlopen.h:pkg/dlopen.h.in])
AC_CONFIG_FILES([out/public-include/smbios_c/dlopen.h:pkg/dlopen.h.in])
AC_OUTPUT
