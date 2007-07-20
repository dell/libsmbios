# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:

AUTOMAKE_OPTIONS := subdir-objects
SUBDIRS = 

# dont move these 5 lines, as this is how we override the autoconf version and package name
RELEASE_NAME=@RELEASE_NAME@
RELEASE_MAJOR=@RELEASE_MAJOR@
RELEASE_MINOR=@RELEASE_MINOR@
RELEASE_SUBLEVEL=@RELEASE_SUBLEVEL@
RELEASE_EXTRALEVEL=@RELEASE_EXTRALEVEL@

RELEASE_VERSION := $(RELEASE_MAJOR).$(RELEASE_MINOR).$(RELEASE_SUBLEVEL)$(RELEASE_EXTRALEVEL)
RELEASE_STRING := $(RELEASE_NAME)-$(RELEASE_VERSION)
VERSION = $(RELEASE_VERSION)
PACKAGE = $(RELEASE_NAME)

LIBSMBIOS_LIBTOOL_CURRENT=@LIBSMBIOS_LIBTOOL_CURRENT@
LIBSMBIOS_LIBTOOL_REVISION=@LIBSMBIOS_LIBTOOL_REVISION@
LIBSMBIOS_LIBTOOL_AGE=@LIBSMBIOS_LIBTOOL_AGE@
LIBSMBIOSXML_LIBTOOL_CURRENT=@LIBSMBIOSXML_LIBTOOL_CURRENT@
LIBSMBIOSXML_LIBTOOL_REVISION=@LIBSMBIOSXML_LIBTOOL_REVISION@
LIBSMBIOSXML_LIBTOOL_AGE=@LIBSMBIOSXML_LIBTOOL_AGE@

CLEANFILES=libsmbios-*.tar.gz libsmbios-*.tar.bz2 libsmbios-*.rpm _buildtemp doc/reports/code-coverage.txt doc/reports/valgrind_output.txt doc/*.log doc/reports/*.txt
DISTCLEANFILES=*~

EXTRA_DIST =
bin_PROGRAMS=
EXTRA_PROGRAMS=

include libraries/Makefile.am
include bin-supported/Makefile.am
include bin-unsupported/Makefile.am
include cppunit/Makefile.am

static: $(EXTRA_PROGRAMS)
CLEANFILES+=$(EXTRA_PROGRAMS)

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

unit_test:

static:
	make -C bin-supported static
	make -C bin-unsupported static

########## DOCS ################
clean-generic: clean-docs
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)
clean-docs:
	rm -rf doc/interface doc/full doc/reports

if HAS_DOXYGEN
all: doc/full/html/index.html doc/interface/html/index.html
endif

doxygen: doc/full/html/index.html doc/interface/html/index.html
doc/full/html/index.html: doc/reports/code-coverage.txt doc/reports/valgrind_output.txt $(shell find doc -name \*.txt)
	cd doc/; doxygen full-documentation.dox > doxygen-full.log

doc/interface/html/index.html: doc/interface-only.dox doc/reports/code-coverage.txt doc/reports/valgrind_output.txt $(shell find doc -name \*.txt)
	cd doc/; doxygen interface-only.dox > doxygen-int.log

doc/reports/code-coverage.txt:
	[ -e doc/reports ] || mkdir -p doc/reports
	@echo '/** \page code_coverage Automatically generated code coverage report' > doc/reports/code-coverage.txt
	@echo '<pre>' >> doc/reports/code-coverage.txt
	@echo 'CODE COVERAGE REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> doc/reports/code-coverage.txt
	@echo "</pre>*/" >> doc/reports/code-coverage.txt

doc/reports/valgrind_output.txt:
	[ -e doc/reports ] || mkdir -p doc/reports
	@echo '/** \page leak_detect Automatically generated leak detection report' > doc/reports/valgrind_output.txt
	@echo '\section leaks Leak detection report' >> doc/reports/valgrind_output.txt
	@echo "Generated for $$(whoami) on $$(date)" >> doc/reports/valgrind_output.txt
	@echo '<pre>' >> doc/reports/valgrind_output.txt
	@echo 'LEAK DETECTION REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> doc/reports/valgrind_output.txt
	@echo "</pre>*/" >> doc/reports/valgrind_output.txt
	@echo '/** \section leak_time Time to run leak detection report.' >> doc/reports/valgrind_output.txt
	@echo '<pre>' >> doc/reports/valgrind_output.txt
	@echo 'LEAK DETECTION REPORT HAS NOT BEEN RUN YET IN THIS SOURCE TREE!' >> doc/reports/valgrind_output.txt
	@echo "</pre>*/" >> doc/reports/valgrind_output.txt
########## END DOCS ################


EXTRA_DIST += \
 libsmbios.spec \
 build/scripts 		\
 build/VC.2005 		\
 COPYING-GPL		\
 COPYING-OSL				\
 include/smbios/SmbiosLowLevel.h		\
 include/smbios/DellRbu.h		\
 include/smbios/ICmosRW.h		\
 include/smbios/IException.h		\
 include/smbios/IFactory.h		\
 include/smbios/IMemory.h		\
 include/smbios/IObserver.h		\
 include/smbios/ISmbios.h		\
 include/smbios/ISmbiosXml.h		\
 include/smbios/ISmi.h		\
 include/smbios/IToken.h		\
 include/smbios/SmbiosDefs.h		\
 include/smbios/RbuLowLevel.h		\
 include/smbios/config/abi/msvc_prefix.hpp		\
 include/smbios/config/abi/msvc_suffix.hpp		\
 include/smbios/config/boost_LICENSE_1_0_txt		\
 include/smbios/config/abi_prefix.hpp		\
 include/smbios/config/abi_suffix.hpp		\
 include/smbios/config/auto_link.hpp		\
 include/smbios/config/compiler/gcc.hpp		\
 include/smbios/config/compiler/visualc.hpp		\
 include/smbios/config/platform/linux.hpp		\
 include/smbios/config/platform/win32.hpp		\
 include/smbios/config/platform/win64.hpp		\
 include/smbios/config/select_compiler_config.hpp		\
 include/smbios/config/posix_features.hpp		\
 include/smbios/config/stdlib/libstdcpp2.hpp		\
 include/smbios/config/stdlib/libstdcpp3.hpp		\
 include/smbios/config/README		\
 include/smbios/config/suffix.hpp		\
 include/smbios/config/user.hpp		\
 include/smbios/config/select_stdlib_config.hpp		\
 include/smbios/config/select_platform_config.hpp		\
 include/smbios/config.hpp		\
 include/smbios/compat.h		\
 include/smbios/SystemInfo.h		\
 include/smbios/types.h		\
 include/smbios/message.h	\
 doc/bios_hdr_file.txt	\
 doc/coding	\
 doc/dell_drivers.txt	\
 doc/DellToken.txt	\
 doc/design	\
 doc/exe-assetTag.txt	\
 doc/exe-dellBiosUpdate.txt	\
 doc/exe-getSystemId.txt	\
 doc/exe-propertyTag.txt	\
 doc/exe-serviceTag.txt	\
 doc/external-specs	\
 doc/getopt	\
 doc/installation.txt	\
 doc/interface-only.dox	\
 doc/mainpage.txt	\
 doc/old_news.txt	\
 doc/smbios23.xml	\
 doc/visio	\
 doc/yum.txt
