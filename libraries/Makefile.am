# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES = libraries/libsmbios.la libraries/libsmbiosxml.la
libraries_libsmbios_la_LDFLAGS = -version-info $(LIBSMBIOS_LIBTOOL_CURRENT):$(LIBSMBIOS_LIBTOOL_REVISION):$(LIBSMBIOS_LIBTOOL_AGE)
libraries_libsmbios_la_CXXFLAGS = -I$(top_srcdir)/libraries/common -I$(top_builddir)/libraries/common 

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

libraries_libsmbios_la_SOURCES = \
    libraries/cmos/CmosRW.cpp                   \
    libraries/cmos/CmosRWFactory.cpp            \
    libraries/cmos/CmosRW_LinuxIO.cpp           \
    libraries/common/ascii2enUS_scancode.cpp    \
    libraries/common/Observer.cpp               \
    libraries/memory/Memory.cpp                 \
    libraries/memory/Memory_Linux.cpp           \
    libraries/rbu/RbuHdr.cpp                    \
    libraries/rbu/Rbu_Linux.cpp                 \
    libraries/smbios/SmbiosFactory.cpp          \
    libraries/smbios/SmbiosItem.cpp             \
    libraries/smbios/SmbiosStrategy.cpp         \
    libraries/smbios/SmbiosStrategy_Linux.cpp   \
    libraries/smbios/SmbiosTable.cpp            \
    libraries/smbios/SmbiosTableIterator.cpp    \
    libraries/smbios/SmbiosWorkaround.cpp       \
    libraries/smi/Smi.cpp                       \
    libraries/smi/SmiFactory.cpp                \
    libraries/smi/Smi_Linux.cpp                 \
    libraries/systeminfo/IdByte.cpp             \
    libraries/systeminfo/SysInfoError.cpp       \
    libraries/systeminfo/System.cpp             \
    libraries/systeminfo/SystemDetect.cpp       \
    libraries/token/checksum.cpp                \
    libraries/token/Token.cpp                   \
    libraries/token/TokenD4.cpp                 \
    libraries/token/TokenD5.cpp                 \
    libraries/token/TokenD6.cpp                 \
    libraries/token/TokenDA.cpp                 \
    libraries/token/TokenTable.cpp              \
    libraries/token/TokenTableFactory.cpp       \
    libraries/token/TokenTableIterator.cpp      

EXTRA_DIST += \
	libraries/cmos/CmosRW_WindowsIO.cpp	\
	libraries/memory/Memory_Windows.cpp	\
	libraries/smbios/SmbiosStrategy_Windows.cpp	\
	libraries/smi/Smi_Windows.cpp	\
	libraries/rbu/RbuImpl.h		\
	libraries/common/ExceptionImpl.h		\
	libraries/common/CmosRWImpl.h		\
	libraries/common/SmbiosWorkaroundImpl.h		\
	libraries/common/FactoryImpl2.h		\
	libraries/common/SmbiosImpl.h		\
	libraries/common/TokenLowLevel.h		\
	libraries/common/miniddk.h		\
	libraries/memory/MemoryImpl.h		\
	libraries/smbios/StdWorkarounds.h		\
	libraries/smi/SmiLowLevel.h		\
	libraries/smi/SmiImpl.h		\
	libraries/systeminfo/DellMagic.h		\
	libraries/systeminfo/SystemDetect.h		\
	libraries/common/TokenImpl.h		\
	libraries/xml_libxerces/SmbiosXmlImpl.h		\
	libraries/xml_libxerces/XmlUtils.h		\
	libraries/xml_libxml2/SmbiosXmlImpl.h		\
	libraries/xml_libxml2/XmlUtils.h	


libraries_libsmbiosxml_la_CXXFLAGS = $(shell xml2-config --cflags) -DLIBXML2=1 -DLIBXERCES=2 -DXMLUTILS=1 -I$(top_srcdir)/libraries/common -I$(top_builddir)/libraries/common 
libraries_libsmbiosxml_la_SOURCES = libraries/xml_libxml2/SmbiosXml.cpp libraries/xml_libxml2/XmlUtils.cpp
libraries_libsmbiosxml_la_LIBADD = -lsmbios
libraries_libsmbiosxml_la_LDFLAGS = $(shell xml2-config --libs) -Llibraries/ -version-info $(LIBSMBIOSXML_LIBTOOL_CURRENT):$(LIBSMBIOSXML_LIBTOOL_REVISION):$(LIBSMBIOSXML_LIBTOOL_AGE)

# make sure libsmbios is built before libsmbiosxml
libraries_libsmbiosxml_la_DEPENDENCIES = libraries/libsmbios.la

libraries/common/StdSmbiosXml.h: Makefile.am $(top_srcdir)/doc/smbios23.xml $(top_srcdir)/build/scripts/makeXmlHeader.py
	@echo Building $@
	@mkdir -p $(shell dirname $@)
	@chmod ug+w $$(dirname $@)
	@python $(top_srcdir)/build/scripts/makeXmlHeader.py $(top_srcdir)/doc/smbios23.xml $@.new
	@diff -q $@ $@.new >/dev/null 2>&1 || mv -f $@.new $@
	@rm -f $@.new


dist-hook: libraries/common/StdSmbiosXml.h
	mkdir -p $(distdir)/include/smbios $(distdir)/libraries/common/
	[ ! -e $(top_srcdir)/libraries/common/StdSmbiosXml.h ] || cp -f $(top_srcdir)/libraries/common/StdSmbiosXml.h $(distdir)/libraries/common/
	[ ! -e $(top_builddir)/libraries/common/StdSmbiosXml.h ] || cp -f $(top_builddir)/libraries/common/StdSmbiosXml.h $(distdir)/libraries/common/

$(libraries_libsmbios_la_OBJECTS):
$(libraries_libsmbiosxml_la_OBJECTS): libraries/common/StdSmbiosXml.h

CLEANFILES+=libraries/common/StdSmbiosXml.h libraries/.libs
