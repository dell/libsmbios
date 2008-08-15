# vim:tw=0:et:ts=4:sw=4

dist_sbin_SCRIPTS += src/bin-supported/mkbiospkg.sh

sbin_PROGRAMS += \
    out/assetTag \
    out/getSystemId   \
    out/propertyTag   \
    out/serviceTag    \
    out/verifySmiPassword     \
    out/wakeupCtl     \
    out/dellBiosUpdate    \
    out/dellLcdBrightness     \
    out/dumpSmbios    \
    out/dellWirelessCtl \
    out/dellMediaDirectCtl

EXTRA_PROGRAMS += \
    out/assetTagS \
    out/getSystemIdS   \
    out/propertyTagS   \
    out/serviceTagS    \
    out/verifySmiPasswordS     \
    out/wakeupCtlS     \
    out/dellBiosUpdateS    \
    out/dellLcdBrightnessS     \
    out/dumpSmbiosS    \
    out/dellWirelessCtlS \
    out/dellMediaDirectCtlS

getopts_h=src/bin-supported/getopts.h
getopts_c=src/bin-supported/getopts.c
EXTRA_DIST += $(getopts_h)
CLEANFILES += src/bin-supported/.libs

out_assetTag_SOURCES = src/bin-supported/assetTag.cpp $(getopts_c)
out_getSystemId_SOURCES = src/bin-supported/getSystemId.cpp $(getopts_c)
out_propertyTag_SOURCES = $(getopts_c) src/bin-supported/propertyTag.cpp
out_serviceTag_SOURCES = $(getopts_c) src/bin-supported/serviceTag.cpp
out_verifySmiPassword_SOURCES = $(getopts_c) src/bin-supported/verifySmiPassword.cpp
out_wakeupCtl_SOURCES = $(getopts_c) src/bin-supported/wakeupCtl.cpp
out_dellLcdBrightness_SOURCES = $(getopts_c) src/bin-supported/dellLcdBrightness.cpp
out_dellWirelessCtl_SOURCES = $(getopts_c) src/bin-supported/dellWirelessCtl.cpp
out_dellMediaDirectCtl_SOURCES = $(getopts_c) src/bin-supported/dellMediaDirectCtl.cpp
out_dumpSmbios_SOURCES = $(getopts_c) src/bin-supported/dumpSmbios.cpp

out_assetTag_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_getSystemId_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_propertyTag_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_serviceTag_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_verifySmiPassword_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_wakeupCtl_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_dellLcdBrightness_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_dellWirelessCtl_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_dellMediaDirectCtl_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_dumpSmbios_LDFLAGS = $(AM_LDFLAGS) -lsmbios

# BAD BAD BAD programs....  (uses private headers)
out_dellBiosUpdate_SOURCES = $(getopts_c) src/bin-supported/dellBiosUpdate.cpp
out_dellBiosUpdate_LDFLAGS = $(AM_LDFLAGS) -lsmbios
out_dellBiosUpdate_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios/common



#static versions
out_assetTagS_SOURCES = src/bin-supported/assetTag.cpp $(getopts_c)
out_getSystemIdS_SOURCES = src/bin-supported/getSystemId.cpp $(getopts_c)
out_propertyTagS_SOURCES = $(getopts_c) src/bin-supported/propertyTag.cpp
out_serviceTagS_SOURCES = $(getopts_c) src/bin-supported/serviceTag.cpp
out_verifySmiPasswordS_SOURCES = $(getopts_c) src/bin-supported/verifySmiPassword.cpp
out_wakeupCtlS_SOURCES = $(getopts_c) src/bin-supported/wakeupCtl.cpp
out_dellLcdBrightnessS_SOURCES = $(getopts_c) src/bin-supported/dellLcdBrightness.cpp
out_dellWirelessCtlS_SOURCES = $(getopts_c) src/bin-supported/dellWirelessCtl.cpp
out_dellMediaDirectCtlS_SOURCES = $(getopts_c) src/bin-supported/dellMediaDirectCtl.cpp
out_dumpSmbiosS_SOURCES = $(getopts_c) src/bin-supported/dumpSmbios.cpp

out_assetTagS_LDFLAGS          = $(AM_LDFLAGS) -all-static -lsmbios
out_getSystemIdS_LDFLAGS       = $(AM_LDFLAGS) -all-static -lsmbios
out_propertyTagS_LDFLAGS       = $(AM_LDFLAGS) -all-static -lsmbios
out_serviceTagS_LDFLAGS        = $(AM_LDFLAGS) -all-static -lsmbios
out_verifySmiPasswordS_LDFLAGS = $(AM_LDFLAGS) -all-static -lsmbios
out_wakeupCtlS_LDFLAGS         = $(AM_LDFLAGS) -all-static -lsmbios
out_dellLcdBrightnessS_LDFLAGS = $(AM_LDFLAGS) -all-static -lsmbios
out_dellWirelessCtlS_LDFLAGS   = $(AM_LDFLAGS) -all-static -lsmbios
out_dellMediaDirectCtlS_LDFLAGS   = $(AM_LDFLAGS) -all-static -lsmbios
out_dumpSmbiosS_LDFLAGS        = $(AM_LDFLAGS) -all-static -lsmbios

# BAD again (uses private includes.)
out_dellBiosUpdateS_SOURCES = $(getopts_c) src/bin-supported/dellBiosUpdate.cpp
out_dellBiosUpdateS_LDFLAGS    = $(AM_LDFLAGS) -all-static -lsmbios
out_dellBiosUpdateS_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios/common
