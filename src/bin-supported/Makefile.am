# vim:tw=0:et:ts=4:sw=4
dist_sbin_SCRIPTS += src/bin-supported/mkbiospkg.sh
getopts=src/bin-supported/getopts.c src/bin-supported/getopts.h
CLEANFILES += src/bin-supported/.libs

sbin_PROGRAMS += out/assetTag
out_assetTag_SOURCES = src/bin-supported/assetTag.cpp $(getopts)
out_assetTag_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/getSystemId
out_getSystemId_SOURCES = src/bin-supported/getSystemId.cpp $(getopts)
out_getSystemId_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/propertyTag
out_propertyTag_SOURCES = $(getopts) src/bin-supported/propertyTag.cpp
out_propertyTag_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/serviceTag
out_serviceTag_SOURCES = $(getopts) src/bin-supported/serviceTag.cpp
out_serviceTag_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/verifySmiPassword
out_verifySmiPassword_SOURCES = $(getopts) src/bin-supported/verifySmiPassword.cpp
out_verifySmiPassword_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/wakeupCtl
out_wakeupCtl_SOURCES = $(getopts) src/bin-supported/wakeupCtl.cpp
out_wakeupCtl_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/dellLcdBrightness
out_dellLcdBrightness_SOURCES = $(getopts) src/bin-supported/dellLcdBrightness.cpp
out_dellLcdBrightness_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/dellWirelessCtl
out_dellWirelessCtl_SOURCES = $(getopts) src/bin-supported/dellWirelessCtl.cpp
out_dellWirelessCtl_LDADD = out/libsmbios.la

if BUILD_MEDIA_DIRECT
sbin_PROGRAMS += out/dellMediaDirectCtl
out_dellMediaDirectCtl_SOURCES = $(getopts) src/bin-supported/dellMediaDirectCtl.cpp
out_dellMediaDirectCtl_LDADD = out/libsmbios.la
endif

noinst_PROGRAMS += out/dumpSmbios
out_dumpSmbios_SOURCES = $(getopts) src/bin-supported/dumpSmbios.cpp
out_dumpSmbios_LDADD = out/libsmbios.la

# BAD BAD BAD programs....  (uses private headers)
sbin_PROGRAMS += out/dellBiosUpdate
out_dellBiosUpdate_SOURCES = $(getopts) src/bin-supported/dellBiosUpdate.cpp
out_dellBiosUpdate_LDADD = out/libsmbios.la
out_dellBiosUpdate_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios/common

# C
noinst_PROGRAMS += out/getSystemId_lite
out_getSystemId_lite_SOURCES = src/bin-supported/getSystemId_lite.c $(getopts)
out_getSystemId_lite_LDADD = out/libsmbios_c.la

EXTRA_PROGRAMS += out/getSystemId_liteS
out_getSystemId_liteS_SOURCES = src/bin-supported/getSystemId_lite.c $(getopts)
out_getSystemId_liteS_LDADD = out/libsmbios_c.la
out_getSystemId_liteS_LDFLAGS = -static

## BAD again (uses private includes.)
EXTRA_PROGRAMS += out/dellBiosUpdateS
out_dellBiosUpdateS_SOURCES = $(getopts) src/bin-supported/dellBiosUpdate.cpp
out_dellBiosUpdateS_LDADD = out/libsmbios.la
out_dellBiosUpdateS_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios/common
out_dellBiosUpdateS_LDFLAGS = -static -all-static

EXTRA_PROGRAMS += out/assetTagS
out_assetTagS_SOURCES = src/bin-supported/assetTag.cpp $(getopts)
out_assetTagS_LDADD = out/libsmbios.la
out_assetTagS_LDFLAGS = -static -all-static

if BUILD_MEDIA_DIRECT
EXTRA_PROGRAMS += out/dellMediaDirectCtlS
out_dellMediaDirectCtlS_SOURCES = $(getopts) src/bin-supported/dellMediaDirectCtl.cpp
out_dellMediaDirectCtlS_LDADD = out/libsmbios.la
out_dellMediaDirectCtlS_LDFLAGS = -all-static
endif

EXTRA_PROGRAMS += out/getSystemIdS
out_getSystemIdS_SOURCES = src/bin-supported/getSystemId.cpp $(getopts)
out_getSystemIdS_LDADD = out/libsmbios.la
out_getSystemIdS_LDFLAGS = -static -all-static

EXTRA_PROGRAMS += out/propertyTagS
out_propertyTagS_SOURCES = $(getopts) src/bin-supported/propertyTag.cpp
out_propertyTagS_LDADD = out/libsmbios.la
out_propertyTagS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/serviceTagS
out_serviceTagS_SOURCES = $(getopts) src/bin-supported/serviceTag.cpp
out_serviceTagS_LDADD = out/libsmbios.la
out_serviceTagS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/verifySmiPasswordS
out_verifySmiPasswordS_SOURCES = $(getopts) src/bin-supported/verifySmiPassword.cpp
out_verifySmiPasswordS_LDADD = out/libsmbios.la
out_verifySmiPasswordS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/wakeupCtlS
out_wakeupCtlS_SOURCES = $(getopts) src/bin-supported/wakeupCtl.cpp
out_wakeupCtlS_LDADD = out/libsmbios.la
out_wakeupCtlS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/dellLcdBrightnessS
out_dellLcdBrightnessS_SOURCES = $(getopts) src/bin-supported/dellLcdBrightness.cpp
out_dellLcdBrightnessS_LDADD = out/libsmbios.la
out_dellLcdBrightnessS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/dellWirelessCtlS
out_dellWirelessCtlS_SOURCES = $(getopts) src/bin-supported/dellWirelessCtl.cpp
out_dellWirelessCtlS_LDADD = out/libsmbios.la
out_dellWirelessCtlS_LDFLAGS = -all-static

EXTRA_PROGRAMS += out/dumpSmbiosS
out_dumpSmbiosS_SOURCES = $(getopts) src/bin-supported/dumpSmbios.cpp
out_dumpSmbiosS_LDADD = out/libsmbios.la
out_dumpSmbiosS_LDFLAGS = -all-static

