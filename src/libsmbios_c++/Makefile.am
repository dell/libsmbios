# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES += out/libsmbios.la
out_libsmbios_la_LDFLAGS := -version-info $(LIBSMBIOS_CPLUSPLUS_LIBTOOL_CURRENT):$(LIBSMBIOS_CPLUSPLUS_LIBTOOL_REVISION):$(LIBSMBIOS_CPLUSPLUS_LIBTOOL_AGE) $(AM_LDFLAGS)
out_libsmbios_la_CPPFLAGS := $(AM_CPPFLAGS) -I$(top_srcdir)/src/libsmbios_c++/common

EXTRA_DIST += src/include/smbios

out_libsmbios_la_SOURCES = \
    src/libsmbios_c++/cmos/CmosRW.cpp                   \
    src/libsmbios_c++/cmos/CmosRWFactory.cpp            \
    src/libsmbios_c++/common/ascii2enUS_scancode.cpp    \
    src/libsmbios_c++/common/Observer.cpp               \
    src/libsmbios_c++/memory/MemoryImpl.h		\
    src/libsmbios_c++/memory/Memory.cpp                 \
    src/libsmbios_c++/smbios/SmbiosFactory.cpp          \
    src/libsmbios_c++/smbios/SmbiosItem.cpp             \
    src/libsmbios_c++/smbios/SmbiosStrategy.cpp         \
    src/libsmbios_c++/smbios/SmbiosTable.cpp            \
    src/libsmbios_c++/smbios/SmbiosTableIterator.cpp    \
    src/libsmbios_c++/smbios/SmbiosWorkaround.cpp       \
    src/libsmbios_c++/smbios/StdWorkarounds.h		\
    src/libsmbios_c++/smi/Smi.cpp                       \
    src/libsmbios_c++/smi/SmiFactory.cpp                \
    src/libsmbios_c++/smi/SmiLowLevel.h			\
    src/libsmbios_c++/smi/SmiImpl.h			\
    src/libsmbios_c++/systeminfo/IdByte.cpp             \
    src/libsmbios_c++/systeminfo/SysInfoError.cpp       \
    src/libsmbios_c++/systeminfo/System.cpp             \
    src/libsmbios_c++/systeminfo/SystemDetect.cpp       \
    src/libsmbios_c++/systeminfo/DellMagic.h		\
    src/libsmbios_c++/systeminfo/SystemDetect.h		\
    src/libsmbios_c++/token/checksum.cpp                \
    src/libsmbios_c++/token/Token.cpp                   \
    src/libsmbios_c++/token/TokenD4.cpp                 \
    src/libsmbios_c++/token/TokenD5.cpp                 \
    src/libsmbios_c++/token/TokenD6.cpp                 \
    src/libsmbios_c++/token/TokenDA.cpp                 \
    src/libsmbios_c++/token/TokenTable.cpp              \
    src/libsmbios_c++/token/TokenTableFactory.cpp       \
    src/libsmbios_c++/token/TokenTableIterator.cpp	\
    src/libsmbios_c++/common/ExceptionImpl.h		\
    src/libsmbios_c++/common/CmosRWImpl.h		\
    src/libsmbios_c++/common/SmbiosWorkaroundImpl.h	\
    src/libsmbios_c++/common/FactoryImpl2.h		\
    src/libsmbios_c++/common/SmbiosImpl.h		\
    src/libsmbios_c++/common/TokenLowLevel.h		\
    src/libsmbios_c++/common/miniddk.h			\
    src/libsmbios_c++/common/TokenImpl.h

SOLARIS_SOURCES = \
    src/libsmbios_c++/cmos/CmosRW_SolarisIO.cpp           \
    src/libsmbios_c++/memory/Memory_Solaris.cpp           \
    src/libsmbios_c++/smbios/SmbiosStrategy_Solaris.cpp   \
    src/libsmbios_c++/smi/Smi_Solaris.cpp

LINUX_SOURCES = \
    src/libsmbios_c++/cmos/CmosRW_LinuxIO.cpp           \
    src/libsmbios_c++/memory/Memory_Linux.cpp           \
    src/libsmbios_c++/smbios/SmbiosStrategy_Linux.cpp   \
    src/libsmbios_c++/smi/Smi_Linux.cpp

WINDOWS_SOURCES = \
    src/libsmbios_c++/cmos/CmosRW_WindowsIO.cpp		\
    src/libsmbios_c++/memory/Memory_Windows.cpp		\
    src/libsmbios_c++/smbios/SmbiosStrategy_Windows.cpp	\
    src/libsmbios_c++/smi/Smi_Windows.cpp

if BUILD_WINDOWS
out_libsmbios_la_SOURCES += $(WINDOWS_SOURCES)
EXTRA_DIST += $(LINUX_SOURCES) $(SOLARIS_SOURCES)
endif

if BUILD_LINUX
out_libsmbios_la_SOURCES += $(LINUX_SOURCES)
EXTRA_DIST += $(WINDOWS_SOURCES) $(SOLARIS_SOURCES)
endif

if BUILD_SOLARIS
out_libsmbios_la_SOURCES += $(SOLARIS_SOURCES)
EXTRA_DIST += $(WINDOWS_SOURCES) $(LINUX_SOURCES)
endif
