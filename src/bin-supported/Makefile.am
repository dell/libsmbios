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

