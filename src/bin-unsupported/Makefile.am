# vim:tw=0:et:ts=4:sw=4
sbin_PROGRAMS += \
    src/activateCmosToken  \
    src/createUnitTestFiles  \
    src/disable_console_redir  \
    src/dumpCmos \
    src/isCmosTokenActive  \
    src/smitest  \
    src/stateByteCtl  \
    src/upBootCtl  \
    src/getPasswordFormat  \
    src/ascii2enUS_scancode  \
    src/probes \
    src/dellLEDCtl

EXTRA_PROGRAMS += \
    src/activateCmosTokenS  \
    src/createUnitTestFilesS  \
    src/disable_console_redirS  \
    src/dumpCmosS \
    src/isCmosTokenActiveS  \
    src/smitestS  \
    src/stateByteCtlS  \
    src/upBootCtlS  \
    src/getPasswordFormatS  \
    src/ascii2enUS_scancodeS  \
    src/probesS \
    src/dellLEDCtlS

EXTRA_DIST += src/bin-unsupported/getopts.h src/bin-unsupported/getopts_LICENSE.txt
CLEANFILES += src/bin-unsupported/.libs

src_activateCmosToken_SOURCES = $(getopts_c) src/bin-unsupported/activateCmosToken.cpp
src_createUnitTestFiles_SOURCES = $(getopts_c) src/bin-unsupported/createUnitTestFiles.cpp
src_disable_console_redir_SOURCES = $(getopts_c) src/bin-unsupported/disable_console_redir.cpp
src_dumpCmos_SOURCES = $(getopts_c) src/bin-unsupported/dumpCmos.cpp
src_isCmosTokenActive_SOURCES = $(getopts_c) src/bin-unsupported/isCmosTokenActive.cpp
src_smitest_SOURCES = $(getopts_c) src/bin-unsupported/smitest.cpp
src_stateByteCtl_SOURCES = $(getopts_c) src/bin-unsupported/stateByteCtl.cpp
src_upBootCtl_SOURCES = $(getopts_c) src/bin-unsupported/upBootCtl.cpp
src_getPasswordFormat_SOURCES = $(getopts_c) src/bin-unsupported/getPasswordFormat.cpp
src_ascii2enUS_scancode_SOURCES = $(getopts_c) src/bin-unsupported/ascii2enUS_scancode.cpp
src_probes_SOURCES = $(getopts_c) src/bin-unsupported/probes.cpp
src_dellLEDCtl_SOURCES = $(getopts_c) src/bin-unsupported/dellLEDCtl.cpp

src_activateCmosToken_LDFLAGS=  -lsmbios
src_createUnitTestFiles_LDFLAGS= -lsmbios
src_disable_console_redir_LDFLAGS= -lsmbios
src_dumpCmos_LDFLAGS= -lsmbios
src_isCmosTokenActive_LDFLAGS= -lsmbios
src_smitest_LDFLAGS= -lsmbios
src_stateByteCtl_LDFLAGS= -lsmbios
src_upBootCtl_LDFLAGS= -lsmbios
src_getPasswordFormat_LDFLAGS= -lsmbios
src_ascii2enUS_scancode_LDFLAGS= -lsmbios
src_probes_LDFLAGS= -lsmbios
src_dellLEDCtl_LDFLAGS= -lsmbios


#static 
src_activateCmosTokenS_SOURCES = $(getopts_c) src/bin-unsupported/activateCmosToken.cpp
src_activateCmosTokenS_LDFLAGS = -all-static -lsmbios

src_createUnitTestFilesS_SOURCES = $(getopts_c) src/bin-unsupported/createUnitTestFiles.cpp
src_createUnitTestFilesS_LDFLAGS = -all-static -lsmbios

src_disable_console_redirS_SOURCES = $(getopts_c) src/bin-unsupported/disable_console_redir.cpp
src_disable_console_redirS_LDFLAGS = -all-static -lsmbios

src_dumpCmosS_SOURCES = $(getopts_c) src/bin-unsupported/dumpCmos.cpp
src_dumpCmosS_LDFLAGS = -all-static -lsmbios

src_isCmosTokenActiveS_SOURCES = $(getopts_c) src/bin-unsupported/isCmosTokenActive.cpp
src_isCmosTokenActiveS_LDFLAGS = -all-static -lsmbios

src_smitestS_SOURCES = $(getopts_c) src/bin-unsupported/smitest.cpp
src_smitestS_LDFLAGS = -all-static -lsmbios

src_stateByteCtlS_SOURCES = $(getopts_c) src/bin-unsupported/stateByteCtl.cpp
src_stateByteCtlS_LDFLAGS = -all-static -lsmbios

src_upBootCtlS_SOURCES = $(getopts_c) src/bin-unsupported/upBootCtl.cpp
src_upBootCtlS_LDFLAGS = -all-static -lsmbios

src_getPasswordFormatS_SOURCES = $(getopts_c) src/bin-unsupported/getPasswordFormat.cpp
src_getPasswordFormatS_LDFLAGS = -all-static -lsmbios

src_ascii2enUS_scancodeS_SOURCES = $(getopts_c) src/bin-unsupported/ascii2enUS_scancode.cpp
src_ascii2enUS_scancodeS_LDFLAGS = -all-static -lsmbios

src_probesS_SOURCES = $(getopts_c) src/bin-unsupported/probes.cpp
src_probesS_LDFLAGS = -all-static -lsmbios

src_dellLEDCtlS_SOURCES = $(getopts_c) src/bin-unsupported/dellLEDCtl.cpp
src_dellLEDCtlS_LDFLAGS = -all-static -lsmbios
