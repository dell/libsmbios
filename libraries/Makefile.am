# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES = libraries/libsmbios.la libraries/libsmbios_c.la
libraries_libsmbios_la_LDFLAGS = -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
libraries_libsmbios_la_CXXFLAGS = -I$(top_srcdir)/libraries/common -I$(top_builddir)/libraries/common

libraries_libsmbios_c_la_LDFLAGS = -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
libraries_libsmbios_c_la_CXXFLAGS =

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

include_HEADERS = \
    include/smbios_c/config  			\
    include/smbios_c/compat.h  			\
    include/smbios_c/config.h  			\
    include/smbios_c/memory.h  			\
    include/smbios_c/types.h  			\
 include/smbios/config  			\
 include/smbios/config.hpp		\
 include/smbios/SmbiosLowLevel.h		\
 include/smbios/DellRbu.h		\
 include/smbios/ICmosRW.h		\
 include/smbios/IException.h		\
 include/smbios/IFactory.h		\
 include/smbios/IMemory.h		\
 include/smbios/IObserver.h		\
 include/smbios/ISmbios.h		\
 include/smbios/ISmi.h		\
 include/smbios/IToken.h		\
 include/smbios/SmbiosDefs.h		\
 include/smbios/RbuLowLevel.h		\
 include/smbios/compat.h		\
 include/smbios/SystemInfo.h		\
 include/smbios/types.h		\
 include/smbios/message.h	

libraries_libsmbios_c_la_SOURCES = \
    libraries/memory_c/memory_impl.h		\
    libraries/memory_c/memory_linux.c		\
    libraries/memory_c/memory.c

libraries_libsmbios_la_SOURCES = \
    libraries/cmos/CmosRW.cpp                   \
    libraries/cmos/CmosRWFactory.cpp            \
    libraries/cmos/CmosRW_LinuxIO.cpp           \
    libraries/common/ascii2enUS_scancode.cpp    \
    libraries/common/Observer.cpp               \
    libraries/memory/MemoryImpl.h		\
    libraries/memory/Memory.cpp                 \
    libraries/memory/Memory_Linux.cpp           \
    libraries/rbu/RbuHdr.cpp                    \
    libraries/rbu/Rbu_Linux.cpp                 \
    libraries/rbu/RbuImpl.h			\
    libraries/smbios/SmbiosFactory.cpp          \
    libraries/smbios/SmbiosItem.cpp             \
    libraries/smbios/SmbiosStrategy.cpp         \
    libraries/smbios/SmbiosStrategy_Linux.cpp   \
    libraries/smbios/SmbiosTable.cpp            \
    libraries/smbios/SmbiosTableIterator.cpp    \
    libraries/smbios/SmbiosWorkaround.cpp       \
    libraries/smbios/StdWorkarounds.h		\
    libraries/smi/Smi.cpp                       \
    libraries/smi/SmiFactory.cpp                \
    libraries/smi/Smi_Linux.cpp                 \
    libraries/smi/SmiLowLevel.h			\
    libraries/smi/SmiImpl.h			\
    libraries/systeminfo/IdByte.cpp             \
    libraries/systeminfo/SysInfoError.cpp       \
    libraries/systeminfo/System.cpp             \
    libraries/systeminfo/SystemDetect.cpp       \
    libraries/systeminfo/DellMagic.h		\
    libraries/systeminfo/SystemDetect.h		\
    libraries/token/checksum.cpp                \
    libraries/token/Token.cpp                   \
    libraries/token/TokenD4.cpp                 \
    libraries/token/TokenD5.cpp                 \
    libraries/token/TokenD6.cpp                 \
    libraries/token/TokenDA.cpp                 \
    libraries/token/TokenTable.cpp              \
    libraries/token/TokenTableFactory.cpp       \
    libraries/token/TokenTableIterator.cpp	\
    libraries/common/ExceptionImpl.h		\
    libraries/common/CmosRWImpl.h		\
    libraries/common/SmbiosWorkaroundImpl.h	\
    libraries/common/FactoryImpl2.h		\
    libraries/common/SmbiosImpl.h		\
    libraries/common/TokenLowLevel.h		\
    libraries/common/miniddk.h			\
    libraries/common/TokenImpl.h

EXTRA_DIST += \
    libraries/cmos/CmosRW_WindowsIO.cpp		\
    libraries/memory/Memory_Windows.cpp		\
    libraries/smbios/SmbiosStrategy_Windows.cpp	\
    libraries/smi/Smi_Windows.cpp	

CLEANFILES+=libraries/.libs
