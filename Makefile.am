# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

LIBTOOL_CURRENT=@LIBTOOL_CURRENT@
LIBTOOL_REVISION=@LIBTOOL_REVISION@
LIBTOOL_AGE=@LIBTOOL_AGE@

LIBXML2_LIBS =  @LIBXML2_LIBS@
LIBXML2_CFLAGS = @LIBXML2_CFLAGS@

bin_PROGRAMS=
sbin_PROGRAMS=
dist_sbin_SCRIPTS=
dist_bin_SCRIPTS=
include_HEADERS=
noinst_PROGRAMS=
check_PROGRAMS =

noinst_LTLIBRARIES =
lib_LTLIBRARIES=

AM_LDFLAGS = -L$(top_builddir)/out/

#-Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4
AM_CFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall -DLIBSMBIOS_LOCALEDIR=\"$(localedir)\"

#-Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4 -fexceptions
AM_CXXFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall -DLIBSMBIOS_LOCALEDIR=\"$(localedir)\"

AM_LDADD = $(LIBINTL)

include Makefile-std
include src/libsmbios_c++/Makefile.am
include src/libsmbios_c/Makefile.am
include src/python/Makefile.am
include src/bin/Makefile.am
include src/cppunit/Makefile.am

CLEANFILES+=_buildtemp smbios-utils*.rpm out
SUBDIRS += po
ACLOCAL_AMFLAGS = -I m4

CLEANFILES += $(EXTRA_PROGRAMS)

# bug in po/Makefile or something...
DISTCLEANFILES += ./po/stamp-po

clean-generic:
	-test -z "$(CLEANFILES)" || rm -rf $(CLEANFILES)

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
if HAS_DOXYGEN
all: doxygen
doxygen: out/libsmbios_c/html/index.html
doxygen: out/libsmbios_c++/html/index.html
endif

out/libsmbios_c/html/index.html: out/libsmbios_c.dox $(wildcard $(top_srcdir)/src/include/smbios_c/*.h)
	cd out; doxygen $$(basename $<) > $$(basename $< .dox).out

out/libsmbios_c++/html/index.html: out/libsmbios_c++.dox $(wildcard $(top_srcdir)/src/include/smbios/*.h)
	cd out; doxygen $$(basename $<) > $$(basename $< .dox).out

########## END DOCS ################

EXTRA_DIST += \
 pkg/libsmbios.spec 	\
 pkg/scripts 		\
 pkg/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL		\
 doc

