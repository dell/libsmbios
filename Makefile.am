# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

LIBTOOL_CURRENT=@LIBTOOL_CURRENT@
LIBTOOL_REVISION=@LIBTOOL_REVISION@
LIBTOOL_AGE=@LIBTOOL_AGE@

bin_PROGRAMS=
sbin_PROGRAMS=
dist_sbin_SCRIPTS=
dist_bin_SCRIPTS=
lib_LTLIBRARIES=
include_HEADERS=
noinst_PROGRAMS=

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

$(bin_PROGRAMS): $(lib_LTLIBRARIES)
$(sbin_PROGRAMS): $(lib_LTLIBRARIES)
$(EXTRA_PROGRAMS): $(lib_LTLIBRARIES)
$(check_PROGRAMS): $(lib_LTLIBRARIES)
$(noinst_PROGRAMS): $(lib_LTLIBRARIES)
CLEANFILES += $(EXTRA_PROGRAMS)

########## DOCS ################
clean-generic:
	-test -z "$(CLEANFILES)" || rm -rf $(CLEANFILES)

if HAS_DOXYGEN
all: out/full/html/index.html out/interface/html/index.html
endif

doxygen: out/full/html/index.html out/interface/html/index.html
out/full/html/index.html: out/reports/code-coverage.txt out/reports/valgrind_output.txt $(shell find doc -name \*.txt)
	cd out/; doxygen full-documentation.dox > doxygen-full.log

out/interface/html/index.html: out/interface-only.dox out/reports/code-coverage.txt out/reports/valgrind_output.txt $(shell find doc -name \*.txt)
	cd out/; doxygen interface-only.dox > doxygen-int.log

out/reports/code-coverage.txt:
	[ -e out/reports ] || mkdir -p out/reports
	@echo '/** \page code_coverage Automatically generated code coverage report' > out/reports/code-coverage.txt
	@echo '<pre>' >> out/reports/code-coverage.txt
	@echo 'CODE COVERAGE REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> out/reports/code-coverage.txt
	@echo "</pre>*/" >> out/reports/code-coverage.txt

out/reports/valgrind_output.txt:
	[ -e out/reports ] || mkdir -p out/reports
	@echo '/** \page leak_detect Automatically generated leak detection report' > out/reports/valgrind_output.txt
	@echo '\section leaks Leak detection report' >> out/reports/valgrind_output.txt
	@echo "Generated for $$(whoami) on $$(date)" >> out/reports/valgrind_output.txt
	@echo '<pre>' >> out/reports/valgrind_output.txt
	@echo 'LEAK DETECTION REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> out/reports/valgrind_output.txt
	@echo "</pre>*/" >> out/reports/valgrind_output.txt
	@echo '/** \section leak_time Time to run leak detection report.' >> out/reports/valgrind_output.txt
	@echo '<pre>' >> out/reports/valgrind_output.txt
	@echo 'LEAK DETECTION REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> out/reports/valgrind_output.txt
	@echo "</pre>*/" >> out/reports/valgrind_output.txt
########## END DOCS ################

EXTRA_DIST += \
 pkg/libsmbios.spec 	\
 pkg/scripts 		\
 pkg/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL		\
 doc

