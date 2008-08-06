# vim:tw=0:et:ts=4:sw=4

dist_sbin_SCRIPTS += src/bin-supported/mkbiospkg.sh

sbin_PROGRAMS += \
    src/assetTag \
    src/getSystemId   \
    src/propertyTag   \
    src/serviceTag    \
    src/verifySmiPassword     \
    src/wakeupCtl     \
    src/dellBiosUpdate    \
    src/dellLcdBrightness     \
    src/dumpSmbios    \
    src/dellWirelessCtl \
    src/dellMediaDirectCtl

EXTRA_PROGRAMS += \
    src/assetTagS \
    src/getSystemIdS   \
    src/propertyTagS   \
    src/serviceTagS    \
    src/verifySmiPasswordS     \
    src/wakeupCtlS     \
    src/dellBiosUpdateS    \
    src/dellLcdBrightnessS     \
    src/dumpSmbiosS    \
    src/dellWirelessCtlS \
    src/dellMediaDirectCtlS


getopts_h=src/bin-supported/getopts.h
getopts_c=src/bin-supported/getopts.c
EXTRA_DIST += $(getopts_h)
CLEANFILES += src/bin-supported/.libs

src_assetTag_SOURCES = src/bin-supported/assetTag.cpp $(getopts_c)
src_getSystemId_SOURCES = src/bin-supported/getSystemId.cpp $(getopts_c)
src_propertyTag_SOURCES = $(getopts_c) src/bin-supported/propertyTag.cpp
src_serviceTag_SOURCES = $(getopts_c) src/bin-supported/serviceTag.cpp
src_verifySmiPassword_SOURCES = $(getopts_c) src/bin-supported/verifySmiPassword.cpp
src_wakeupCtl_SOURCES = $(getopts_c) src/bin-supported/wakeupCtl.cpp
src_dellLcdBrightness_SOURCES = $(getopts_c) src/bin-supported/dellLcdBrightness.cpp
src_dellWirelessCtl_SOURCES = $(getopts_c) src/bin-supported/dellWirelessCtl.cpp
src_dellMediaDirectCtl_SOURCES = $(getopts_c) src/bin-supported/dellMediaDirectCtl.cpp
src_dumpSmbios_SOURCES = $(getopts_c) src/bin-supported/dumpSmbios.cpp

src_assetTag_LDFLAGS = -lsmbios
src_getSystemId_LDFLAGS = -lsmbios
src_propertyTag_LDFLAGS = -lsmbios
src_serviceTag_LDFLAGS = -lsmbios
src_verifySmiPassword_LDFLAGS = -lsmbios
src_wakeupCtl_LDFLAGS = -lsmbios
src_dellLcdBrightness_LDFLAGS = -lsmbios
src_dellWirelessCtl_LDFLAGS = -lsmbios
src_dellMediaDirectCtl_LDFLAGS = -lsmbios
src_dumpSmbios_LDFLAGS = -lsmbios

# BAD BAD BAD programs....  (uses private headers)
src_dellBiosUpdate_SOURCES = $(getopts_c) src/bin-supported/dellBiosUpdate.cpp
src_dellBiosUpdate_LDFLAGS = -lsmbios
src_dellBiosUpdate_CXXFLAGS = -Isrc/libsmbios/common



#static versions
src_assetTagS_SOURCES = src/bin-supported/assetTag.cpp $(getopts_c)
src_getSystemIdS_SOURCES = src/bin-supported/getSystemId.cpp $(getopts_c)
src_propertyTagS_SOURCES = $(getopts_c) src/bin-supported/propertyTag.cpp
src_serviceTagS_SOURCES = $(getopts_c) src/bin-supported/serviceTag.cpp
src_verifySmiPasswordS_SOURCES = $(getopts_c) src/bin-supported/verifySmiPassword.cpp
src_wakeupCtlS_SOURCES = $(getopts_c) src/bin-supported/wakeupCtl.cpp
src_dellLcdBrightnessS_SOURCES = $(getopts_c) src/bin-supported/dellLcdBrightness.cpp
src_dellWirelessCtlS_SOURCES = $(getopts_c) src/bin-supported/dellWirelessCtl.cpp
src_dellMediaDirectCtlS_SOURCES = $(getopts_c) src/bin-supported/dellMediaDirectCtl.cpp
src_dumpSmbiosS_SOURCES = $(getopts_c) src/bin-supported/dumpSmbios.cpp

src_assetTagS_LDFLAGS          = -all-static -lsmbios
src_getSystemIdS_LDFLAGS       = -all-static -lsmbios
src_propertyTagS_LDFLAGS       = -all-static -lsmbios
src_serviceTagS_LDFLAGS        = -all-static -lsmbios
src_verifySmiPasswordS_LDFLAGS = -all-static -lsmbios
src_wakeupCtlS_LDFLAGS         = -all-static -lsmbios
src_dellLcdBrightnessS_LDFLAGS = -all-static -lsmbios
src_dellWirelessCtlS_LDFLAGS   = -all-static -lsmbios
src_dellMediaDirectCtlS_LDFLAGS   = -all-static -lsmbios
src_dumpSmbiosS_LDFLAGS        = -all-static -lsmbios

# BAD again (uses private includes.)
src_dellBiosUpdateS_SOURCES = $(getopts_c) src/bin-supported/dellBiosUpdate.cpp
src_dellBiosUpdateS_LDFLAGS    = -all-static -lsmbios
src_dellBiosUpdateS_CXXFLAGS = -Isrc/libsmbios/common
