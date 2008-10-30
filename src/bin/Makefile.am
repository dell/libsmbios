# vim:tw=0:et:ts=4:sw=4
dist_sbin_SCRIPTS += src/bin/mkbiospkg.sh
CLEANFILES += src/bin/.libs
EXTRA_DIST += src/bin/getopts_LICENSE.txt

# convenience lib for getopt
noinst_LTLIBRARIES += out/libgetopt.la
out_libgetopt_la_SOURCES = src/bin/getopts.c src/bin/getopts.h
out_libgetopt_la_CXXFLAGS = $(AM_CXXFLAGS)
out_libgetopt_la_LDFLAGS = -static

dist_sbin_SCRIPTS += src/bin/tokenCtl
DATA_HOOK_REPLACE += $(sbindir)/tokenCtl
pkgdatadir = $(datadir)/smbios-utils
nodist_pkgdata_DATA = doc/token_list.csv doc/token_blacklist.csv

dist_sbin_SCRIPTS += src/bin/verifySmiPassword
DATA_HOOK_REPLACE += $(sbindir)/verifySmiPassword

sbin_PROGRAMS += out/assetTag
out_assetTag_SOURCES = src/bin/assetTag.cpp 
out_assetTag_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/getSystemId
out_getSystemId_SOURCES = src/bin/getSystemId.cpp 
out_getSystemId_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/propertyTag
out_propertyTag_SOURCES = src/bin/propertyTag.cpp
out_propertyTag_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/serviceTag
out_serviceTag_SOURCES = src/bin/serviceTag.cpp
out_serviceTag_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/wakeupCtl
out_wakeupCtl_SOURCES = src/bin/wakeupCtl.cpp
out_wakeupCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/dellLcdBrightness
out_dellLcdBrightness_SOURCES = src/bin/dellLcdBrightness.cpp
out_dellLcdBrightness_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/dellWirelessCtl
out_dellWirelessCtl_SOURCES = src/bin/dellWirelessCtl.cpp
out_dellWirelessCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

if BUILD_MEDIA_DIRECT
sbin_PROGRAMS += out/dellMediaDirectCtl
out_dellMediaDirectCtl_SOURCES = src/bin/dellMediaDirectCtl.cpp
out_dellMediaDirectCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)
endif

# BAD BAD BAD programs....  (uses private headers)
sbin_PROGRAMS += out/dellBiosUpdate
out_dellBiosUpdate_SOURCES = src/bin/dellBiosUpdate.cpp
out_dellBiosUpdate_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)
out_dellBiosUpdate_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios_c++/common

# C
noinst_PROGRAMS += out/getSystemId_lite
out_getSystemId_lite_SOURCES = src/bin/getSystemId_lite.c 
out_getSystemId_lite_LDADD = out/libsmbios_c.la out/libgetopt.la $(AM_LDADD)

noinst_PROGRAMS += out/propertyTag_lite
out_propertyTag_lite_SOURCES = src/bin/propertyTag_lite.c
out_propertyTag_lite_LDADD = out/libsmbios_c.la out/libgetopt.la $(AM_LDADD)

noinst_PROGRAMS += out/smitest
out_smitest_SOURCES = src/bin/smitest.cpp
out_smitest_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/stateByteCtl
out_stateByteCtl_SOURCES = src/bin/stateByteCtl.cpp
out_stateByteCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/upBootCtl
out_upBootCtl_SOURCES = src/bin/upBootCtl.cpp
out_upBootCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

noinst_PROGRAMS += out/ascii2enUS_scancode
out_ascii2enUS_scancode_SOURCES = src/bin/ascii2enUS_scancode.cpp
out_ascii2enUS_scancode_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

noinst_PROGRAMS += out/probes
out_probes_SOURCES = src/bin/probes.cpp
out_probes_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/dellLEDCtl
out_dellLEDCtl_SOURCES = src/bin/dellLEDCtl.cpp
out_dellLEDCtl_LDADD = out/libsmbios.la out/libgetopt.la $(AM_LDADD)

sbin_PROGRAMS += out/smbios_cmos_save_to_file
out_smbios_cmos_save_to_file_SOURCES = src/bin/smbios_cmos_save_to_file.c
out_smbios_cmos_save_to_file_LDADD = out/libsmbios_c.la out/libgetopt.la $(AM_LDADD)


