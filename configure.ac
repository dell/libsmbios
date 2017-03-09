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
#
m4_define([release_major_version], [2])
m4_define([release_minor_version], [3])
m4_define([release_micro_version], [3])
# if you define any "extra" version info, include a leading dot (".")
m4_define([release_extra_version], [])

AC_INIT([libsmbios],
          [release_major_version().release_minor_version().release_micro_version()release_extra_version()])

##########################################################
# ABI Version Variables
##########################################################
#NOTE: libtool version, below, is different and distinct from 'marketing' version, above.
# non-interface changes/bugfixes: current++; revision++; age++;
# interface add:                  current++; revision=0; age++;
# interface remove/change:        current++; revision=0; age=0;
AC_SUBST([LIBSMBIOS_C_LIBTOOL_CURRENT], [4])
AC_SUBST([LIBSMBIOS_C_LIBTOOL_REVISION], [1])
AC_SUBST([LIBSMBIOS_C_LIBTOOL_AGE], [2])

AC_SUBST([LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT], [3])
AC_SUBST([LIBSMBIOS_CPLUSPLUS_LIBTOOL_REVISION], [0])
AC_SUBST([LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE], [1])
##########################################################
# END ABI Version Variables
##########################################################
##############################################################################
# END RELEASE VARIABLES
##############################################################################

AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR([pkg])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([subdir-objects dist-xz no-define foreign])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14])
AC_GNU_SOURCE
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([out/include/config.h:pkg/config.h.in])

# Version variables
AC_SUBST([RELEASE_MAJOR], [release_major_version()])
AC_SUBST([RELEASE_MINOR], [release_minor_version()])
AC_SUBST([RELEASE_MICRO], [release_micro_version()])
AC_SUBST([RELEASE_EXTRA], [release_extra_version()])
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MAJOR,$RELEASE_MAJOR,"Major release version")
AC_DEFINE_UNQUOTED(LIBSMBIOS_RELEASE_MINOR,$RELEASE_MINOR,"Minor release version")

# Ensure we use c++98 instead of c++14
CXXFLAGS="$CXXFLAGS -std=c++98"

if test "$enable_shared" != "no"; then
    AC_DEFINE([LIBSMBIOS_C_ALL_DYN_LINK],[1],[Enable dynamic libraries to build])
    CFLAGS="$CFLAGS -DLIBSMBIOS_C_ALL_DYN_LINK"
fi

AC_SUBST([RELEASE_RPM_EXTRA], [$RELEASE_EXTRA])
if test -z "$RELEASE_EXTRA"; then
    RELEASE_RPM_EXTRA=%{nil}
fi

# gettext domain name
AC_SUBST([GETTEXT_PACKAGE], [libsmbios-$RELEASE_MAJOR.$RELEASE_MINOR-$host_cpu])
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Name of the gettext message domain])
AC_SUBST([GETTEXT_POT_FILE_BASENAME], [libsmbios])

# Library ABI versions
AC_SUBST([LIBSMBIOS_C_SO_MAJOR], [$(($LIBSMBIOS_C_LIBTOOL_CURRENT - $LIBSMBIOS_C_LIBTOOL_AGE))])
AC_SUBST([LIBSMBIOS_C_SONAME], [libsmbios_c.so.$LIBSMBIOS_C_SO_MAJOR])

# Library ABI versions
AC_SUBST([LIBSMBIOS_CPLUSPLUS_SO_MAJOR], [$(($LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT - $LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE))])
AC_SUBST([LIBSMBIOS_CPLUSPLUS_SONAME], [libsmbios.so.$LIBSMBIOS_CPLUSPLUS_SO_MAJOR])

dnl Help line for doxygen
AC_ARG_ENABLE([libsmbios_cxx],
    AS_HELP_STRING([--enable-libsmbios_cxx],[Disable build of the c++ libsmbios.so library. This library is deprecated, most things should migrate to libsmbios_c.so. default: disabled]),
    [want_libsmbios_cxx=$enableval], [want_libsmbios_cxx=no])
AM_CONDITIONAL(BUILD_LIBSMBIOS_CXX, [test x$want_libsmbios_cxx == xyes])

dnl Help line for doxygen
AC_ARG_ENABLE(doxygen,
    AS_HELP_STRING([--disable-doxygen],[Disable API docs build via Doxygen. default: enabled if doxygen present]),
    [wantdoxygen=$enableval], [wantdoxygen=yes])

dnl Help line for graphviz
AC_ARG_ENABLE(graphviz,
    AS_HELP_STRING([--disable-graphviz],[Enhance API docs with pretty graphs and pictures. default: enabled if graphviz present]),
    [wantgraphviz=$enableval], [wantgraphviz=yes])

dnl Help line for graphviz
AC_ARG_ENABLE(python,
    AS_HELP_STRING([--disable-python],[Build and install python utilities. default: enabled if python present]),
    [wantpython=$enableval], [wantpython=yes])

AC_ARG_ENABLE(as-needed,
    AS_HELP_STRING([--disable-as-needed],[Pass in -Wl,-as-needed as LDFLAG default: enabled]),
    [wantasneeded=$enableval], [wantasneeded=yes])
AC_SUBST([LDFLAG_AS_NEEDED],[])
AC_SUBST([LDFLAG_NO_AS_NEEDED],[])
if test "$wantasneeded" = "yes"; then
    LDFLAG_AS_NEEDED="-Wl,-as-needed"
    LDFLAG_NO_AS_NEEDED="-Wl,-no-as-needed"
fi

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL

dnl Check for doxygen support
AM_CONDITIONAL([HAVE_PYTHON], [false])
if test x$wantpython == xyes ; then
    AM_PATH_PYTHON([2.3],,[:])
    AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != :])
fi

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

case $host_os in
	linux*)
		linux_os=1
		;;
	solaris*)
		solaris_os=1
		AC_SEARCH_LIBS([nanosleep],[rt])
		;;
	mingw32*)
		windows_os=1
		;;
esac

AM_CONDITIONAL(BUILD_LINUX,   [test -n "$linux_os"])
AM_CONDITIONAL(BUILD_SOLARIS, [test -n "$solaris_os"])
AM_CONDITIONAL(BUILD_WINDOWS, [test -n "$windows_os"])

PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])
PKG_CHECK_MODULES([CPPUNIT],[cppunit],[],[echo "CPPUNIT not found, disabling CPPUNIT dependent modules"])
AM_CONDITIONAL(CPPUNIT_PRESENT, [test -n "$CPPUNIT_LIBS"])

# generate files and exit
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([pkg/libsmbios.spec])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([libsmbios_c-uninstalled.pc:pkg/libsmbios_c-uninstalled.pc.in])
AC_CONFIG_FILES([pkg/libsmbios_c.pc])
AC_CONFIG_FILES([libsmbios_c++-uninstalled.pc:pkg/libsmbios_c++-uninstalled.pc.in])
AC_CONFIG_FILES([pkg/libsmbios_c++.pc])
AC_CONFIG_FILES([out/public-include/smbios/dlopen.h:pkg/dlopen.h.in])
AC_CONFIG_FILES([out/public-include/smbios_c/dlopen.h:pkg/dlopen.h.in])
AC_CONFIG_FILES([pkg/pkginfo])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
    c++ library:            ${want_libsmbios_cxx}
    doxygen:                ${wantdoxygen}
    graphviz:               ${wantgraphviz}
    python:                 ${wantpython}
    as-needed:              ${wantasneeded}
==============================================================================
])
