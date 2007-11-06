#                                               -*- Autoconf -*-
# vim:tw=0:et:ts=4:sw=4
# Process this file with autoconf to produce a configure script.

####################################
# change version here.
AC_INIT([libsmbios],[0.13.13])

temp_RELEASE_NAME=libsmbios
temp_RELEASE_MAJOR=0
temp_RELEASE_MINOR=13
temp_RELEASE_SUBLEVEL=13
temp_RELEASE_EXTRALEVEL=
####################################

AC_PREREQ(2.61)
AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AM_PATH_CPPUNIT(1.9.6)

#AC_CONFIG_SRCDIR([.])
#AC_CONFIG_HEADER([config.h])

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
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL

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
AC_CHECK_FUNCS([getpagesize memmove memset munmap strerror strndup strtol strtoul])

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

# update 'real' variables from the temp variable names.
# do this at the end of the file so that they A) are not overwitten by other
# autoconf stuff, and B) so that user can override on cmdline
for i in RELEASE_NAME RELEASE_MAJOR RELEASE_MINOR RELEASE_SUBLEVEL RELEASE_EXTRALEVEL
do
    varname=temp_$i
    if test -z "${!i}"; then
        eval $i=${!varname}; export $i
    fi
done

# Update the version information only immediately before a public release of your software. More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# If the library source code has changed at all since the last update, then increment revision (c:r:a becomes c:r+1:a).
# If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# If any interfaces have been added since the last public release, then increment age.
# If any interfaces have been removed since the last public release, then set age to 0. 
LIBSMBIOS_LIBTOOL_CURRENT=$(( $RELEASE_MAJOR + 1 ))
LIBSMBIOS_LIBTOOL_REVISION=$RELEASE_MINOR
LIBSMBIOS_LIBTOOL_AGE=0

LIBSMBIOSXML_LIBTOOL_CURRENT=$(( $RELEASE_MAJOR + 1 ))
LIBSMBIOSXML_LIBTOOL_REVISION=$RELEASE_MINOR
LIBSMBIOSXML_LIBTOOL_AGE=0

if test -z "$RELEASE_RPM_EXTRALEVEL"; then
    if test -z "$RELEASE_EXTRALEVEL"; then
        RELEASE_RPM_EXTRALEVEL=%{nil}
    else
        RELEASE_RPM_EXTRALEVEL=$RELEASE_EXTRALEVEL
    fi
fi

PACKAGE_VERSION=[`echo ${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_SUBLEVEL}${RELEASE_EXTRALEVEL}`]
PACKAGE_STRING=[`echo ${RELEASE_NAME} ${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_SUBLEVEL}${RELEASE_EXTRALEVEL}`]
VERSION=$PACKAGE_VERSION

cat confdefs.h |
    grep -v 'define VERSION ' |
    grep -v 'define PACKAGE_VERSION ' |
    grep -v 'define PACKAGE_STRING '  > confdefs.new
mv confdefs.new confdefs.h
cat >>confdefs.h <<_ACEOF 
#define VERSION "$VERSION"
#define PACKAGE_VERSION "$PACKAGE_VERSION"
#define PACKAGE_STRING "$PACKAGE_STRING"
_ACEOF

AC_SUBST([RELEASE_NAME RELEASE_MAJOR RELEASE_MINOR RELEASE_SUBLEVEL RELEASE_EXTRALEVEL RELEASE_RPM_EXTRALEVEL])
AC_SUBST([ LIBSMBIOS_LIBTOOL_CURRENT  LIBSMBIOS_LIBTOOL_REVISION LIBSMBIOS_LIBTOOL_AGE  LIBSMBIOSXML_LIBTOOL_CURRENT LIBSMBIOSXML_LIBTOOL_REVISION  LIBSMBIOSXML_LIBTOOL_AGE ])

# this is used by the autobuilder hook to determine the version we built
echo "PACKAGE_NAME='$PACKAGE_NAME'" > version
echo "PACKAGE_VERSION='$PACKAGE_VERSION'" >> version
echo "PACKAGE_STRING='$PACKAGE_STRING'" >> version

# generate files and exit
AC_CONFIG_FILES([
    Makefile 
    include/smbios/version.h 
    doc/full-documentation.dox 
    doc/interface-only.dox 
    libsmbios.spec:libsmbios.spec.in:ChangeLog
    ])
AC_OUTPUT
