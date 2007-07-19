# vim:tw=0:et:ts=4:sw=4
bin_PROGRAMS += \
    bin-unsupported/activateCmosToken  \
    bin-unsupported/createUnitTestFiles  \
    bin-unsupported/disable_console_redir  \
    bin-unsupported/dumpCmos \
    bin-unsupported/isCmosTokenActive  \
    bin-unsupported/smitest  \
    bin-unsupported/stateByteCtl  \
    bin-unsupported/upBootCtl  \
    bin-unsupported/getPasswordFormat  \
    bin-unsupported/ascii2enUS_scancode  \
    bin-unsupported/probes \
    bin-unsupported/dellLEDCtl

EXTRA_PROGRAMS += \
    bin-unsupported/activateCmosTokenS  \
    bin-unsupported/createUnitTestFilesS  \
    bin-unsupported/disable_console_redirS  \
    bin-unsupported/dumpCmosS \
    bin-unsupported/isCmosTokenActiveS  \
    bin-unsupported/smitestS  \
    bin-unsupported/stateByteCtlS  \
    bin-unsupported/upBootCtlS  \
    bin-unsupported/getPasswordFormatS  \
    bin-unsupported/ascii2enUS_scancodeS  \
    bin-unsupported/probesS \
    bin-unsupported/dellLEDCtlS

EXTRA_DIST += bin-unsupported/getopts.h bin-unsupported/getopts_LICENSE.txt

bin_unsupported_activateCmosToken_SOURCES = $(getopts_c) bin-unsupported/activateCmosToken.cpp
bin_unsupported_createUnitTestFiles_SOURCES = $(getopts_c) bin-unsupported/createUnitTestFiles.cpp
bin_unsupported_disable_console_redir_SOURCES = $(getopts_c) bin-unsupported/disable_console_redir.cpp
bin_unsupported_dumpCmos_SOURCES = $(getopts_c) bin-unsupported/dumpCmos.cpp
bin_unsupported_isCmosTokenActive_SOURCES = $(getopts_c) bin-unsupported/isCmosTokenActive.cpp
bin_unsupported_smitest_SOURCES = $(getopts_c) bin-unsupported/smitest.cpp
bin_unsupported_stateByteCtl_SOURCES = $(getopts_c) bin-unsupported/stateByteCtl.cpp
bin_unsupported_upBootCtl_SOURCES = $(getopts_c) bin-unsupported/upBootCtl.cpp
bin_unsupported_getPasswordFormat_SOURCES = $(getopts_c) bin-unsupported/getPasswordFormat.cpp
bin_unsupported_ascii2enUS_scancode_SOURCES = $(getopts_c) bin-unsupported/ascii2enUS_scancode.cpp
bin_unsupported_probes_SOURCES = $(getopts_c) bin-unsupported/probes.cpp
bin_unsupported_dellLEDCtl_SOURCES = $(getopts_c) bin-unsupported/dellLEDCtl.cpp

bin_unsupported_activateCmosToken_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_createUnitTestFiles_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_disable_console_redir_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_dumpCmos_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_isCmosTokenActive_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_smitest_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_stateByteCtl_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_upBootCtl_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_getPasswordFormat_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_ascii2enUS_scancode_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_probes_LDFLAGS= -Llibraries/ -lsmbios
bin_unsupported_dellLEDCtl_LDFLAGS= -Llibraries/ -lsmbios


#static 
bin_unsupported_activateCmosTokenS_SOURCES = $(getopts_c) bin-unsupported/activateCmosToken.cpp
bin_unsupported_activateCmosTokenS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_createUnitTestFilesS_SOURCES = $(getopts_c) bin-unsupported/createUnitTestFiles.cpp
bin_unsupported_createUnitTestFilesS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_disable_console_redirS_SOURCES = $(getopts_c) bin-unsupported/disable_console_redir.cpp
bin_unsupported_disable_console_redirS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_dumpCmosS_SOURCES = $(getopts_c) bin-unsupported/dumpCmos.cpp
bin_unsupported_dumpCmosS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_isCmosTokenActiveS_SOURCES = $(getopts_c) bin-unsupported/isCmosTokenActive.cpp
bin_unsupported_isCmosTokenActiveS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_smitestS_SOURCES = $(getopts_c) bin-unsupported/smitest.cpp
bin_unsupported_smitestS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_stateByteCtlS_SOURCES = $(getopts_c) bin-unsupported/stateByteCtl.cpp
bin_unsupported_stateByteCtlS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_upBootCtlS_SOURCES = $(getopts_c) bin-unsupported/upBootCtl.cpp
bin_unsupported_upBootCtlS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_getPasswordFormatS_SOURCES = $(getopts_c) bin-unsupported/getPasswordFormat.cpp
bin_unsupported_getPasswordFormatS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_ascii2enUS_scancodeS_SOURCES = $(getopts_c) bin-unsupported/ascii2enUS_scancode.cpp
bin_unsupported_ascii2enUS_scancodeS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_probesS_SOURCES = $(getopts_c) bin-unsupported/probes.cpp
bin_unsupported_probesS_LDFLAGS = -all-static -Llibraries/ -lsmbios

bin_unsupported_dellLEDCtlS_SOURCES = $(getopts_c) bin-unsupported/dellLEDCtl.cpp
bin_unsupported_dellLEDCtlS_LDFLAGS = -all-static -Llibraries/ -lsmbios
