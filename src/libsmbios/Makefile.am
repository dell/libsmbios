# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES += out/libsmbios.la
out_libsmbios_la_LDFLAGS := -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
out_libsmbios_la_CXXFLAGS := $(AM_CXXFLAGS) -I$(top_srcdir)/src/libsmbios/common

EXTRA_DIST += src/include/smbios

out_libsmbios_la_SOURCES = \
    src/libsmbios/cmos/CmosRW.cpp                   \
    src/libsmbios/cmos/CmosRWFactory.cpp            \
    src/libsmbios/cmos/CmosRW_LinuxIO.cpp           \
    src/libsmbios/common/ascii2enUS_scancode.cpp    \
    src/libsmbios/common/Observer.cpp               \
    src/libsmbios/memory/MemoryImpl.h		\
    src/libsmbios/memory/Memory.cpp                 \
    src/libsmbios/memory/Memory_Linux.cpp           \
    src/libsmbios/rbu/RbuHdr.cpp                    \
    src/libsmbios/rbu/Rbu_Linux.cpp                 \
    src/libsmbios/rbu/RbuImpl.h			\
    src/libsmbios/smbios/SmbiosFactory.cpp          \
    src/libsmbios/smbios/SmbiosItem.cpp             \
    src/libsmbios/smbios/SmbiosStrategy.cpp         \
    src/libsmbios/smbios/SmbiosStrategy_Linux.cpp   \
    src/libsmbios/smbios/SmbiosTable.cpp            \
    src/libsmbios/smbios/SmbiosTableIterator.cpp    \
    src/libsmbios/smbios/SmbiosWorkaround.cpp       \
    src/libsmbios/smbios/StdWorkarounds.h		\
    src/libsmbios/smi/Smi.cpp                       \
    src/libsmbios/smi/SmiFactory.cpp                \
    src/libsmbios/smi/Smi_Linux.cpp                 \
    src/libsmbios/smi/SmiLowLevel.h			\
    src/libsmbios/smi/SmiImpl.h			\
    src/libsmbios/systeminfo/IdByte.cpp             \
    src/libsmbios/systeminfo/SysInfoError.cpp       \
    src/libsmbios/systeminfo/System.cpp             \
    src/libsmbios/systeminfo/SystemDetect.cpp       \
    src/libsmbios/systeminfo/DellMagic.h		\
    src/libsmbios/systeminfo/SystemDetect.h		\
    src/libsmbios/token/checksum.cpp                \
    src/libsmbios/token/Token.cpp                   \
    src/libsmbios/token/TokenD4.cpp                 \
    src/libsmbios/token/TokenD5.cpp                 \
    src/libsmbios/token/TokenD6.cpp                 \
    src/libsmbios/token/TokenDA.cpp                 \
    src/libsmbios/token/TokenTable.cpp              \
    src/libsmbios/token/TokenTableFactory.cpp       \
    src/libsmbios/token/TokenTableIterator.cpp	\
    src/libsmbios/common/ExceptionImpl.h		\
    src/libsmbios/common/CmosRWImpl.h		\
    src/libsmbios/common/SmbiosWorkaroundImpl.h	\
    src/libsmbios/common/FactoryImpl2.h		\
    src/libsmbios/common/SmbiosImpl.h		\
    src/libsmbios/common/TokenLowLevel.h		\
    src/libsmbios/common/miniddk.h			\
    src/libsmbios/common/TokenImpl.h

EXTRA_DIST += \
    src/libsmbios/cmos/CmosRW_WindowsIO.cpp		\
    src/libsmbios/memory/Memory_Windows.cpp		\
    src/libsmbios/smbios/SmbiosStrategy_Windows.cpp	\
    src/libsmbios/smi/Smi_Windows.cpp	
