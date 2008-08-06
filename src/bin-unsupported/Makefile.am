# vim:tw=0:et:ts=4:sw=4
sbin_PROGRAMS += \
    out/activateCmosToken  \
    out/createUnitTestFiles  \
    out/disable_console_redir  \
    out/dumpCmos \
    out/isCmosTokenActive  \
    out/smitest  \
    out/stateByteCtl  \
    out/upBootCtl  \
    out/getPasswordFormat  \
    out/ascii2enUS_scancode  \
    out/probes \
    out/dellLEDCtl

EXTRA_PROGRAMS += \
    out/activateCmosTokenS  \
    out/createUnitTestFilesS  \
    out/disable_console_redirS  \
    out/dumpCmosS \
    out/isCmosTokenActiveS  \
    out/smitestS  \
    out/stateByteCtlS  \
    out/upBootCtlS  \
    out/getPasswordFormatS  \
    out/ascii2enUS_scancodeS  \
    out/probesS \
    out/dellLEDCtlS

EXTRA_DIST += src/bin-unsupported/getopts.h src/bin-unsupported/getopts_LICENSE.txt
CLEANFILES += src/bin-unsupported/.libs

out_activateCmosToken_SOURCES = $(getopts_c) src/bin-unsupported/activateCmosToken.cpp
out_createUnitTestFiles_SOURCES = $(getopts_c) src/bin-unsupported/createUnitTestFiles.cpp
out_disable_console_redir_SOURCES = $(getopts_c) src/bin-unsupported/disable_console_redir.cpp
out_dumpCmos_SOURCES = $(getopts_c) src/bin-unsupported/dumpCmos.cpp
out_isCmosTokenActive_SOURCES = $(getopts_c) src/bin-unsupported/isCmosTokenActive.cpp
out_smitest_SOURCES = $(getopts_c) src/bin-unsupported/smitest.cpp
out_stateByteCtl_SOURCES = $(getopts_c) src/bin-unsupported/stateByteCtl.cpp
out_upBootCtl_SOURCES = $(getopts_c) src/bin-unsupported/upBootCtl.cpp
out_getPasswordFormat_SOURCES = $(getopts_c) src/bin-unsupported/getPasswordFormat.cpp
out_ascii2enUS_scancode_SOURCES = $(getopts_c) src/bin-unsupported/ascii2enUS_scancode.cpp
out_probes_SOURCES = $(getopts_c) src/bin-unsupported/probes.cpp
out_dellLEDCtl_SOURCES = $(getopts_c) src/bin-unsupported/dellLEDCtl.cpp

out_activateCmosToken_LDFLAGS=  -lsmbios
out_createUnitTestFiles_LDFLAGS= -lsmbios
out_disable_console_redir_LDFLAGS= -lsmbios
out_dumpCmos_LDFLAGS= -lsmbios
out_isCmosTokenActive_LDFLAGS= -lsmbios
out_smitest_LDFLAGS= -lsmbios
out_stateByteCtl_LDFLAGS= -lsmbios
out_upBootCtl_LDFLAGS= -lsmbios
out_getPasswordFormat_LDFLAGS= -lsmbios
out_ascii2enUS_scancode_LDFLAGS= -lsmbios
out_probes_LDFLAGS= -lsmbios
out_dellLEDCtl_LDFLAGS= -lsmbios


#static 
out_activateCmosTokenS_SOURCES = $(getopts_c) src/bin-unsupported/activateCmosToken.cpp
out_activateCmosTokenS_LDFLAGS = -all-static -lsmbios

out_createUnitTestFilesS_SOURCES = $(getopts_c) src/bin-unsupported/createUnitTestFiles.cpp
out_createUnitTestFilesS_LDFLAGS = -all-static -lsmbios

out_disable_console_redirS_SOURCES = $(getopts_c) src/bin-unsupported/disable_console_redir.cpp
out_disable_console_redirS_LDFLAGS = -all-static -lsmbios

out_dumpCmosS_SOURCES = $(getopts_c) src/bin-unsupported/dumpCmos.cpp
out_dumpCmosS_LDFLAGS = -all-static -lsmbios

out_isCmosTokenActiveS_SOURCES = $(getopts_c) src/bin-unsupported/isCmosTokenActive.cpp
out_isCmosTokenActiveS_LDFLAGS = -all-static -lsmbios

out_smitestS_SOURCES = $(getopts_c) src/bin-unsupported/smitest.cpp
out_smitestS_LDFLAGS = -all-static -lsmbios

out_stateByteCtlS_SOURCES = $(getopts_c) src/bin-unsupported/stateByteCtl.cpp
out_stateByteCtlS_LDFLAGS = -all-static -lsmbios

out_upBootCtlS_SOURCES = $(getopts_c) src/bin-unsupported/upBootCtl.cpp
out_upBootCtlS_LDFLAGS = -all-static -lsmbios

out_getPasswordFormatS_SOURCES = $(getopts_c) src/bin-unsupported/getPasswordFormat.cpp
out_getPasswordFormatS_LDFLAGS = -all-static -lsmbios

out_ascii2enUS_scancodeS_SOURCES = $(getopts_c) src/bin-unsupported/ascii2enUS_scancode.cpp
out_ascii2enUS_scancodeS_LDFLAGS = -all-static -lsmbios

out_probesS_SOURCES = $(getopts_c) src/bin-unsupported/probes.cpp
out_probesS_LDFLAGS = -all-static -lsmbios

out_dellLEDCtlS_SOURCES = $(getopts_c) src/bin-unsupported/dellLEDCtl.cpp
out_dellLEDCtlS_LDFLAGS = -all-static -lsmbios
