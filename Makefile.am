# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = po

# dont move these 5 lines, as this is how we override the autoconf version and package name
RELEASE_NAME=@RELEASE_NAME@
RELEASE_MAJOR=@RELEASE_MAJOR@
RELEASE_MINOR=@RELEASE_MINOR@
RELEASE_SUBLEVEL=@RELEASE_SUBLEVEL@
RELEASE_EXTRALEVEL=@RELEASE_EXTRALEVEL@

RELEASE_VERSION = $(RELEASE_MAJOR).$(RELEASE_MINOR).$(RELEASE_SUBLEVEL)$(RELEASE_EXTRALEVEL)
RELEASE_STRING = $(RELEASE_NAME)-$(RELEASE_VERSION)
VERSION = $(RELEASE_VERSION)
PACKAGE = $(RELEASE_NAME)

LIBTOOL_CURRENT=@LIBTOOL_CURRENT@
LIBTOOL_REVISION=@LIBTOOL_REVISION@
LIBTOOL_AGE=@LIBTOOL_AGE@

CLEANFILES=libsmbios-*.tar.gz libsmbios-*.tar.bz2 libsmbios-*.rpm _buildtemp version smbios-utils*.rpm out
DISTCLEANFILES=*~

EXTRA_DIST =
bin_PROGRAMS=
sbin_PROGRAMS=
dist_sbin_SCRIPTS=
dist_bin_SCRIPTS=
EXTRA_PROGRAMS=
lib_LTLIBRARIES=
include_HEADERS=

AM_LDFLAGS = -L$(top_builddir)/out/

#-Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4
AM_CFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall 

#-Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4 -fexceptions
AM_CXXFLAGS = -I$(top_builddir)/out/include -I$(top_srcdir)/src/include -Werror -Wall 

include src/libsmbios/Makefile.am
include src/libsmbios_c/Makefile.am
include src/bin-supported/Makefile.am
include src/bin-unsupported/Makefile.am
include src/cppunit/Makefile.am

$(bin_PROGRAMS): $(lib_LTLIBRARIES)
$(sbin_PROGRAMS): $(lib_LTLIBRARIES)
$(EXTRA_PROGRAMS): $(lib_LTLIBRARIES)
$(check_PROGRAMS): $(lib_LTLIBRARIES)
static: $(EXTRA_PROGRAMS)
CLEANFILES += $(EXTRA_PROGRAMS)

rpm: dist
	BLD_DIR=$$(mktemp -d /tmp/rpmbuild-$$$$-XXXXXX); \
	mkdir -p $$BLD_DIR/SOURCES $$BLD_DIR/BUILD $$BLD_DIR/RPMS $$BLD_DIR/RPMS/i386 $$BLD_DIR/RPMS/noarch $$BLD_DIR/SRPMS $$BLD_DIR/SPECS ;\
	rpmbuild --define "_topdir $$BLD_DIR" -ta --nodeps $(RELEASE_STRING).tar.gz  ;\
	cp $$BLD_DIR/SRPMS/*rpm .       ;\
	cp $$BLD_DIR/RPMS/*/*rpm .       ;\
	rm -rf $$BLD_DIR


srpm: dist
	BLD_DIR=$$(mktemp -d /tmp/rpmbuild-$$$$-XXXXXX); \
	mkdir -p $$BLD_DIR/SOURCES $$BLD_DIR/BUILD $$BLD_DIR/RPMS $$BLD_DIR/RPMS/i386 $$BLD_DIR/RPMS/noarch $$BLD_DIR/SRPMS $$BLD_DIR/SPECS ;\
	rpmbuild --define "_topdir $$BLD_DIR" -ts --nodeps $(RELEASE_STRING).tar.gz  ;\
	cp $$BLD_DIR/SRPMS/*rpm .       ;\
	rm -rf $$BLD_DIR

static:
	make -C bin-supported static
	make -C bin-unsupported static

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

dist: ChangeLog AUTHORS
ChangeLog:
	(GIT_DIR=.git git-log > .changelog.tmp && mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) || (touch ChangeLog; echo 'git directory not found: installing possibly empty changelog.' >&2)

AUTHORS:
	(GIT_DIR=.git git-log | grep ^Author | sort |uniq > .authors.tmp && mv .authors.tmp AUTHORS; rm -f .authors.tmp) || (touch AUTHORS; echo 'git directory not found: installing possibly empty AUTHORS.' >&2)

EXTRA_DIST += \
 build/libsmbios.spec 	\
 build/scripts 		\
 build/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL		\
 doc

