# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:

lib_LTLIBRARIES = libsmbios.la libsmbiosxml.la
libsmbios_la_LDFLAGS = -version-info @LIBSMBIOS_LIBTOOL_CURRENT@:@LIBSMBIOS_LIBTOOL_REVISION@:@LIBSMBIOS_LIBTOOL_AGE@

INCLUDES = -I$(top_builddir)/include -I$(top_builddir)/libraries/common -I$(top_srcdir)/include -I$(top_srcdir)/libraries/common 

libsmbios_la_SOURCES = \
    cmos/CmosRW.cpp                   \
    cmos/CmosRWFactory.cpp            \
    cmos/CmosRW_LinuxIO.cpp           \
    common/ascii2enUS_scancode.cpp    \
    common/Observer.cpp               \
    memory/Memory.cpp                 \
    memory/Memory_Linux.cpp           \
    rbu/RbuHdr.cpp                    \
    rbu/Rbu_Linux.cpp                 \
    smbios/SmbiosFactory.cpp          \
    smbios/SmbiosItem.cpp             \
    smbios/SmbiosStrategy.cpp         \
    smbios/SmbiosStrategy_Linux.cpp   \
    smbios/SmbiosTable.cpp            \
    smbios/SmbiosTableIterator.cpp    \
    smbios/SmbiosWorkaround.cpp       \
    smi/Smi.cpp                       \
    smi/SmiFactory.cpp                \
    smi/Smi_Linux.cpp                 \
    systeminfo/IdByte.cpp             \
    systeminfo/SysInfoError.cpp       \
    systeminfo/System.cpp             \
    systeminfo/SystemDetect.cpp       \
    token/checksum.cpp                \
    token/Token.cpp                   \
    token/TokenD4.cpp                 \
    token/TokenD5.cpp                 \
    token/TokenD6.cpp                 \
    token/TokenDA.cpp                 \
    token/TokenTable.cpp              \
    token/TokenTableFactory.cpp       \
    token/TokenTableIterator.cpp      

EXTRA_DIST = \
	cmos/CmosRW_WindowsIO.cpp	\
	memory/Memory_Windows.cpp	\
	smbios/SmbiosStrategy_Windows.cpp	\
	smi/Smi_Windows.cpp	\
	rbu/RbuLowLevel.h		\
	rbu/RbuImpl.h		\
	common/ExceptionImpl.h		\
	common/CmosRWImpl.h		\
	common/SmbiosWorkaroundImpl.h		\
	common/FactoryImpl2.h		\
	common/SmbiosImpl.h		\
	common/TokenLowLevel.h		\
	common/miniddk.h		\
	memory/MemoryImpl.h		\
	smbios/StdWorkarounds.h		\
	smi/SmiLowLevel.h		\
	smi/SmiImpl.h		\
	systeminfo/DellMagic.h		\
	systeminfo/SystemDetect.h		\
	common/TokenImpl.h		\
	xml_libxerces/SmbiosXmlImpl.h		\
	xml_libxerces/XmlUtils.h		\
	xml_libxml2/SmbiosXmlImpl.h		\
	xml_libxml2/XmlUtils.h	


libsmbiosxml_la_CXXFLAGS = $(shell xml2-config --cflags) -DLIBXML2=1 -DLIBXERCES=2 -DXMLUTILS=1
libsmbiosxml_la_SOURCES = xml_libxml2/SmbiosXml.cpp  xml_libxml2/XmlUtils.cpp
libsmbiosxml_la_LIBADD = -lsmbios
libsmbiosxml_la_LDFLAGS = $(shell xml2-config --libs) -version-info @LIBSMBIOSXML_LIBTOOL_CURRENT@:@LIBSMBIOSXML_LIBTOOL_REVISION@:@LIBSMBIOSXML_LIBTOOL_AGE@

# make sure libsmbios is built before libsmbiosxml
libsmbiosxml_la_DEPENDENCIES = libsmbios.la

common/StdSmbiosXml.h: Makefile.am $(top_srcdir)/doc/smbios23.xml $(top_srcdir)/build/scripts/makeXmlHeader.py
	@echo Building $@
	@mkdir -p $(shell dirname $@)
	@chmod ug+w $$(dirname $@)
	@python $(top_srcdir)/build/scripts/makeXmlHeader.py $(top_srcdir)/doc/smbios23.xml $@.new
	@diff -q $@ $@.new >/dev/null 2>&1 || mv -f $@.new $@
	@rm -f $@.new

RELEASE_NAME=@RELEASE_NAME@
RELEASE_MAJOR=@RELEASE_MAJOR@
RELEASE_MINOR=@RELEASE_MINOR@
RELEASE_SUBLEVEL=@RELEASE_SUBLEVEL@
RELEASE_EXTRALEVEL=@RELEASE_EXTRALEVEL@

FILE_RELEASE_STRING=$(shell grep LIBSMBIOS_RELEASE_VERSION $$( ([ ! -e $(top_builddir)/include/smbios/version.h ] && echo $(top_srcdir)/include/smbios/version.h) || echo $(top_builddir)/include/smbios/version.h) 2>/dev/null )
GEN_RELEASE_STRING=\#define LIBSMBIOS_RELEASE_VERSION "$(RELEASE_MAJOR).$(RELEASE_MINOR).$(RELEASE_SUBLEVEL)"

# leading space required for automake
 ifneq ($(FILE_RELEASE_STRING),$(GEN_RELEASE_STRING))
 .PHONY: ../include/smbios/version.h
 endif

../include/smbios/version.h: $(top_builddir)/Makefile
	@echo Building $@
	@mkdir -p $(shell dirname $@)
	@chmod ug+w $$(dirname $@)
	@echo '#ifndef SMBIOS_VERSION_H'  > $@.new
	@echo '#define SMBIOS_VERSION_H'  >> $@.new
	@echo ' // This is an automatically generated file, do not edit.'  >> $@.new
	@echo ' // This file is generated in common.mk'  >> $@.new
	@echo ''  >> $@.new
	@echo '$(GEN_RELEASE_STRING)'  >> $@.new
	@echo ' '  >> $@.new
	@echo '#endif'  >> $@.new
	@diff -q $@ $@.new >/dev/null 2>&1 || mv -f $@.new $@
	@rm -f $@.new

dist-hook: ../include/smbios/version.h common/StdSmbiosXml.h
	mkdir -p $(distdir)/../include/smbios $(distdir)/common/
	[ ! -e $(top_builddir)/include/smbios/version.h ] || cp -f $(top_builddir)/include/smbios/version.h $(distdir)/../include/smbios
	[ ! -e $(top_srcdir)/include/smbios/version.h ] || cp -f $(top_srcdir)/include/smbios/version.h $(distdir)/../include/smbios
	[ ! -e $(top_srcdir)/libraries/common/StdSmbiosXml.h ] || cp -f $(top_srcdir)/libraries/common/StdSmbiosXml.h $(distdir)/common/
	[ ! -e $(top_builddir)/libraries/common/StdSmbiosXml.h ] || cp -f $(top_builddir)/libraries/common/StdSmbiosXml.h $(distdir)/common/

$(libsmbios_la_OBJECTS):  $(top_builddir)/include/smbios/version.h 
$(libsmbiosxml_la_OBJECTS):  $(top_builddir)/include/smbios/version.h common/StdSmbiosXml.h

CLEANFILES=$(top_builddir)/include/smbios/version.h common/StdSmbiosXml.h
