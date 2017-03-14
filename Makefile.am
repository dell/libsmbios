# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

ACLOCAL_AMFLAGS = -I m4
SUBDIRS = po

bin_PROGRAMS=
sbin_PROGRAMS=
dist_sbin_SCRIPTS=
dist_bin_SCRIPTS=
include_HEADERS=
noinst_PROGRAMS=
check_PROGRAMS =
check_SCRIPTS =

noinst_LTLIBRARIES =
lib_LTLIBRARIES=

TESTS=

AM_CPPFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -DLIBSMBIOS_LOCALEDIR=\"$(localedir)\"
AM_CFLAGS = -Wall -fPIC
AM_CXXFLAGS = -Wall -fPIC
AM_LDADD = $(LIBINTL)

AM_LDFLAGS = -L$(top_builddir)/out/

if BUILD_LINUX
AM_LDFLAGS += -Wl,--no-undefined @LDFLAG_AS_NEEDED@
endif

if BUILD_WINDOWS
AM_LDFLAGS += --Wl,--no-undefined Wl,--avoid-version -Wl,--enable-auto-import
endif

include Makefile-std

if BUILD_LIBSMBIOS_CXX
include src/libsmbios_c++/Makefile.am
endif

include src/libsmbios_c/Makefile.am
include src/python/Makefile.am
include src/py-cli/Makefile.am
include src/yum-plugin/Makefile.am
include src/bin/Makefile.am
include src/pyunit/Makefile.am

if CPPUNIT_PRESENT
include src/cppunit/Makefile.am
endif

pkgconfdir=$(sysconfdir)/$(PACKAGE)
if HAVE_PYTHON
dist_pkgconf_DATA=etc/logging.conf
endif

pcdir = ${libdir}/pkgconfig
pc_DATA = pkg/libsmbios_c.pc pkg/libsmbios_c++.pc
DISTCLEANFILES += pkg/*.pc *-uninstalled.pc

CLEANFILES+=_buildtemp smbios-utils*.rpm out/libsmbios_c out/libsmbios_c++
DISTCLEANFILES += out

CLEANFILES += $(EXTRA_PROGRAMS)

# bug in po/Makefile or something...
DISTCLEANFILES += ./po/stamp-po

.PHONY: valgrind
valgrind: $(check_PROGRAMS)
	VALGRIND="valgrind -v --leak-check=full --error-exitcode=1" make check-TESTS

CLEANFILES+=gmon.out *.gcov coverage
.PHONY: coverage
coverage:
	make $(AM_MAKEFLAGS) $(check_PROGRAMS) CFLAGS="-pg -fprofile-arcs -ftest-coverage" CXXFLAGS="-pg -fprofile-arcs -ftest-coverage"
	make check-TESTS
	find $(top_builddir)/src/libsmbios* -name out_libsmbios*.gcda | while read fn; do	\
		bas=$$(basename $$fn .gcda)	;\
		bas=$${fn##*-}	;\
		d=$$(dirname $$fn)	;\
		d=$${d##top_builddir/}	;\
		gcov -p -o $$fn  $(top_srcdir)/$$d/$$bas.* ;\
	done
	rm \#usr*gcov
	mkdir $(top_builddir)/coverage
	mv ^\#src*gcov $(top_builddir)/coverage
	$(top_srcdir)/pkg/scripts/tr-report.py $(top_builddir)/coverage


########## DOCS ################
all: doxygen
if HAVE_DOXYGEN
DOXYGEN_DEPS=out/libsmbios_c/html/index.html
if BUILD_LIBSMBIOS_CXX
DOXYGEN_DEPS+=out/libsmbios_c++/html/index.html
endif
doxygen: $(DOXYGEN_DEPS)
endif

DOT=@DOT@
if HAVE_DOT
HAVE_DOT=YES
else
HAVE_DOT=NO
endif
export DOT HAVE_DOT top_builddir top_srcdir

out/libsmbios_c/html/index.html: $(top_srcdir)/pkg/libsmbios_c.dox $(wildcard $(top_srcdir)/src/include/smbios_c/*.h)
	doxygen $< > out/$$(basename $< .dox).out

out/libsmbios_c++/html/index.html: $(top_srcdir)/pkg/libsmbios_c++.dox $(wildcard $(top_srcdir)/src/include/smbios/*.h)
	doxygen $< > out/$$(basename $< .dox).out

########## END DOCS ################

EXTRA_DIST += \
 pkg/libsmbios_c++.dox  \
 pkg/libsmbios_c.dox    \
 pkg/libsmbios.spec 	\
 pkg/scripts 		\
 pkg/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL		\
 doc

