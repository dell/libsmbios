# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES = libraries/libsmbios.la libraries/libsmbios_c.la
libraries_libsmbios_la_LDFLAGS = -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
libraries_libsmbios_la_CXXFLAGS = -I$(top_srcdir)/libraries/common -I$(top_builddir)/libraries/common

libraries_libsmbios_c_la_LDFLAGS = -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
libraries_libsmbios_c_la_CXXFLAGS =

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

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
	libraries/common/TokenImpl.h


CLEANFILES+=libraries/.libs
