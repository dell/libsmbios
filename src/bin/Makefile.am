# vim:tw=0:et:ts=4:sw=4
dist_sbin_SCRIPTS += src/bin/mkbiospkg.sh
CLEANFILES += src/bin/.libs
EXTRA_DIST += src/bin/getopts_LICENSE.txt

# convenience lib for getopt
noinst_LTLIBRARIES += out/libgetopt.la
out_libgetopt_la_SOURCES = src/bin/getopts.c src/bin/getopts.h
out_libgetopt_la_CXXFLAGS = $(AM_CXXFLAGS)
out_libgetopt_la_LDFLAGS = -static



sbin_PROGRAMS += out/assetTag
out_assetTag_SOURCES = src/bin/assetTag.cpp 
out_assetTag_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/getSystemId
out_getSystemId_SOURCES = src/bin/getSystemId.cpp 
out_getSystemId_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/propertyTag
out_propertyTag_SOURCES = src/bin/propertyTag.cpp
out_propertyTag_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/serviceTag
out_serviceTag_SOURCES = src/bin/serviceTag.cpp
out_serviceTag_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/verifySmiPassword
out_verifySmiPassword_SOURCES = src/bin/verifySmiPassword.cpp
out_verifySmiPassword_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/wakeupCtl
out_wakeupCtl_SOURCES = src/bin/wakeupCtl.cpp
out_wakeupCtl_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/dellLcdBrightness
out_dellLcdBrightness_SOURCES = src/bin/dellLcdBrightness.cpp
out_dellLcdBrightness_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/dellWirelessCtl
out_dellWirelessCtl_SOURCES = src/bin/dellWirelessCtl.cpp
out_dellWirelessCtl_LDADD = out/libsmbios.la out/libgetopt.la

if BUILD_MEDIA_DIRECT
sbin_PROGRAMS += out/dellMediaDirectCtl
out_dellMediaDirectCtl_SOURCES = src/bin/dellMediaDirectCtl.cpp
out_dellMediaDirectCtl_LDADD = out/libsmbios.la out/libgetopt.la
endif

noinst_PROGRAMS += out/dumpSmbios
out_dumpSmbios_SOURCES = src/bin/dumpSmbios.cpp
out_dumpSmbios_LDADD = out/libsmbios.la out/libgetopt.la

# BAD BAD BAD programs....  (uses private headers)
sbin_PROGRAMS += out/dellBiosUpdate
out_dellBiosUpdate_SOURCES = src/bin/dellBiosUpdate.cpp
out_dellBiosUpdate_LDADD = out/libsmbios.la out/libgetopt.la
out_dellBiosUpdate_CXXFLAGS = $(AM_CXXFLAGS) -Isrc/libsmbios/common

# C
noinst_PROGRAMS += out/getSystemId_lite
out_getSystemId_lite_SOURCES = src/bin/getSystemId_lite.c 
out_getSystemId_lite_LDADD = out/libsmbios_c.la out/libgetopt.la




sbin_PROGRAMS += out/activateCmosToken
out_activateCmosToken_SOURCES = src/bin/activateCmosToken.cpp
out_activateCmosToken_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/createUnitTestFiles
out_createUnitTestFiles_SOURCES = src/bin/createUnitTestFiles.cpp
out_createUnitTestFiles_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/disable_console_redir
out_disable_console_redir_SOURCES = src/bin/disable_console_redir.cpp
out_disable_console_redir_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/dumpCmos
out_dumpCmos_SOURCES = src/bin/dumpCmos.cpp
out_dumpCmos_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/isCmosTokenActive
out_isCmosTokenActive_SOURCES = src/bin/isCmosTokenActive.cpp
out_isCmosTokenActive_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/smitest
out_smitest_SOURCES = src/bin/smitest.cpp
out_smitest_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/stateByteCtl
out_stateByteCtl_SOURCES = src/bin/stateByteCtl.cpp
out_stateByteCtl_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/upBootCtl
out_upBootCtl_SOURCES = src/bin/upBootCtl.cpp
out_upBootCtl_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/getPasswordFormat
out_getPasswordFormat_SOURCES = src/bin/getPasswordFormat.cpp
out_getPasswordFormat_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/ascii2enUS_scancode
out_ascii2enUS_scancode_SOURCES = src/bin/ascii2enUS_scancode.cpp
out_ascii2enUS_scancode_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/probes
out_probes_SOURCES = src/bin/probes.cpp
out_probes_LDADD = out/libsmbios.la out/libgetopt.la

sbin_PROGRAMS += out/dellLEDCtl
out_dellLEDCtl_SOURCES = src/bin/dellLEDCtl.cpp
out_dellLEDCtl_LDADD = out/libsmbios.la out/libgetopt.la

noinst_PROGRAMS += out/savesmbios
out_savesmbios_SOURCES = src/bin/savesmbios.c
out_savesmbios_LDADD = out/libsmbios_c.la out/libgetopt.la


