# vim:tw=0:et:ts=4:sw=4
EXTRA_DIST += src/bin-unsupported/getopts.h src/bin-unsupported/getopts_LICENSE.txt
CLEANFILES += src/bin-unsupported/.libs

sbin_PROGRAMS += out/activateCmosToken
out_activateCmosToken_SOURCES = $(getopts) src/bin-unsupported/activateCmosToken.cpp
out_activateCmosToken_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/createUnitTestFiles
out_createUnitTestFiles_SOURCES = $(getopts) src/bin-unsupported/createUnitTestFiles.cpp
out_createUnitTestFiles_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/disable_console_redir
out_disable_console_redir_SOURCES = $(getopts) src/bin-unsupported/disable_console_redir.cpp
out_disable_console_redir_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/dumpCmos
out_dumpCmos_SOURCES = $(getopts) src/bin-unsupported/dumpCmos.cpp
out_dumpCmos_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/isCmosTokenActive
out_isCmosTokenActive_SOURCES = $(getopts) src/bin-unsupported/isCmosTokenActive.cpp
out_isCmosTokenActive_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/smitest
out_smitest_SOURCES = $(getopts) src/bin-unsupported/smitest.cpp
out_smitest_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/stateByteCtl
out_stateByteCtl_SOURCES = $(getopts) src/bin-unsupported/stateByteCtl.cpp
out_stateByteCtl_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/upBootCtl
out_upBootCtl_SOURCES = $(getopts) src/bin-unsupported/upBootCtl.cpp
out_upBootCtl_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/getPasswordFormat
out_getPasswordFormat_SOURCES = $(getopts) src/bin-unsupported/getPasswordFormat.cpp
out_getPasswordFormat_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/ascii2enUS_scancode
out_ascii2enUS_scancode_SOURCES = $(getopts) src/bin-unsupported/ascii2enUS_scancode.cpp
out_ascii2enUS_scancode_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/probes
out_probes_SOURCES = $(getopts) src/bin-unsupported/probes.cpp
out_probes_LDADD = out/libsmbios.la

sbin_PROGRAMS += out/dellLEDCtl
out_dellLEDCtl_SOURCES = $(getopts) src/bin-unsupported/dellLEDCtl.cpp
out_dellLEDCtl_LDADD = out/libsmbios.la

noinst_PROGRAMS += out/savesmbios
out_savesmbios_SOURCES = $(getopts) src/bin-unsupported/savesmbios.c
out_savesmbios_LDADD = out/libsmbios_c.la


#static
EXTRA_PROGRAMS += out/activateCmosTokenS
out_activateCmosTokenS_SOURCES = $(getopts) src/bin-unsupported/activateCmosToken.cpp
out_activateCmosTokenS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_activateCmosTokenS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/createUnitTestFilesS
out_createUnitTestFilesS_SOURCES = $(getopts) src/bin-unsupported/createUnitTestFiles.cpp
out_createUnitTestFilesS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_createUnitTestFilesS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/disable_console_redirS
out_disable_console_redirS_SOURCES = $(getopts) src/bin-unsupported/disable_console_redir.cpp
out_disable_console_redirS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_disable_console_redirS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/dumpCmosS
out_dumpCmosS_SOURCES = $(getopts) src/bin-unsupported/dumpCmos.cpp
out_dumpCmosS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_dumpCmosS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/isCmosTokenActiveS
out_isCmosTokenActiveS_SOURCES = $(getopts) src/bin-unsupported/isCmosTokenActive.cpp
out_isCmosTokenActiveS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_isCmosTokenActiveS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/smitestS
out_smitestS_SOURCES = $(getopts) src/bin-unsupported/smitest.cpp
out_smitestS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_smitestS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/stateByteCtlS
out_stateByteCtlS_SOURCES = $(getopts) src/bin-unsupported/stateByteCtl.cpp
out_stateByteCtlS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_stateByteCtlS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/upBootCtlS
out_upBootCtlS_SOURCES = $(getopts) src/bin-unsupported/upBootCtl.cpp
out_upBootCtlS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_upBootCtlS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/getPasswordFormatS
out_getPasswordFormatS_SOURCES = $(getopts) src/bin-unsupported/getPasswordFormat.cpp
out_getPasswordFormatS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_getPasswordFormatS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/ascii2enUS_scancodeS
out_ascii2enUS_scancodeS_SOURCES = $(getopts) src/bin-unsupported/ascii2enUS_scancode.cpp
out_ascii2enUS_scancodeS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_ascii2enUS_scancodeS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/probesS
out_probesS_SOURCES = $(getopts) src/bin-unsupported/probes.cpp
out_probesS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_probesS_LDADD = out/libsmbios.la

EXTRA_PROGRAMS += out/dellLEDCtlS
out_dellLEDCtlS_SOURCES = $(getopts) src/bin-unsupported/dellLEDCtl.cpp
out_dellLEDCtlS_LDFLAGS = -all-static $(AM_LDFLAGS)
out_dellLEDCtlS_LDADD = out/libsmbios.la
