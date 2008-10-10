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
AM_CFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall

#-Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4 -fexceptions
AM_CXXFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall

include Makefile-std
include src/libsmbios/Makefile.am
include src/libsmbios_c/Makefile.am
include src/bin-supported/Makefile.am
include src/bin-unsupported/Makefile.am
include src/cppunit/Makefile.am

CLEANFILES+=_buildtemp smbios-utils*.rpm out
SUBDIRS += po
ACLOCAL_AMFLAGS = -I m4

CLEANFILES += $(EXTRA_PROGRAMS)

# bug in po/Makefile or something...
DISTCLEANFILES += ./po/stamp-po

clean-generic:
	-test -z "$(CLEANFILES)" || rm -rf $(CLEANFILES)

.PHONY: obs_upload valgrind
obs-upload: dist
	@[ -n "$(PROJECT)" ] || echo "You must specify the PROJECT variable"
	@[ -n "$(PROJECT)" ] || exit 1
	$(top_srcdir)/pkg/obs-upload.sh $(PROJECT) $(PACKAGE)

valgrind: $(check_PROGRAMS)
	LD_LIBRARY_PATH=out/.libs/ TST=out/.libs/ VALGRIND="valgrind -v --leak-check=full --error-exitcode=1" make check-TESTS

########## DOCS ################
if HAS_DOXYGEN
all: out/full/html/index.html out/interface/html/index.html
endif

doxygen: out/full/html/index.html out/interface/html/index.html
out/full/html/index.html: $(shell find $(top_srcdir)/doc -name \*.txt)
	cd out/; doxygen full-documentation.dox > doxygen-full.log

out/interface/html/index.html: out/interface-only.dox $(shell find $(top_srcdir)/doc -name \*.txt)
	cd out/; doxygen interface-only.dox > doxygen-int.log

########## END DOCS ################

EXTRA_DIST += \
 pkg/libsmbios.spec 	\
 pkg/scripts 		\
 pkg/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL		\
 doc

