bin_PROGRAMS = activateCmosToken  createUnitTestFiles  disable_console_redir  dumpCmos isCmosTokenActive  smitest  stateByteCtl  upBootCtl  getPasswordFormat  ascii2enUS_scancode  probes dellLEDCtl

EXTRA_PROGRAMS = activateCmosTokenS  createUnitTestFilesS  disable_console_redirS  dumpCmosS isCmosTokenActiveS  smitestS  stateByteCtlS  upBootCtlS  getPasswordFormatS  ascii2enUS_scancodeS  probesS dellLEDCtlS

static: $(EXTRA_PROGRAMS)

CLEANFILES=$(EXTRA_PROGRAMS)

AM_LDFLAGS = -L@top_builddir@/libraries/ -lsmbios
INCLUDES = -I@top_builddir@/include -I@top_srcdir@/include

EXTRA_DIST = getopts.h getopts_LICENSE.txt

activateCmosToken_SOURCES = getopts.c activateCmosToken.cpp
createUnitTestFiles_SOURCES = getopts.c createUnitTestFiles.cpp
disable_console_redir_SOURCES = getopts.c disable_console_redir.cpp
dumpCmos_SOURCES = getopts.c dumpCmos.cpp
isCmosTokenActive_SOURCES = getopts.c isCmosTokenActive.cpp
smitest_SOURCES = getopts.c smitest.cpp
stateByteCtl_SOURCES = getopts.c stateByteCtl.cpp
upBootCtl_SOURCES = getopts.c upBootCtl.cpp
getPasswordFormat_SOURCES = getopts.c getPasswordFormat.cpp
ascii2enUS_scancode_SOURCES = getopts.c ascii2enUS_scancode.cpp
probes_SOURCES = getopts.c probes.cpp
dellLEDCtl_SOURCES = getopts.c dellLEDCtl.cpp

#static 
activateCmosTokenS_SOURCES = getopts.c activateCmosToken.cpp
activateCmosTokenS_LDFLAGS = -all-static

createUnitTestFilesS_SOURCES = getopts.c createUnitTestFiles.cpp
createUnitTestFilesS_LDFLAGS = -all-static

disable_console_redirS_SOURCES = getopts.c disable_console_redir.cpp
disable_console_redirS_LDFLAGS = -all-static

dumpCmosS_SOURCES = getopts.c dumpCmos.cpp
dumpCmosS_LDFLAGS = -all-static

isCmosTokenActiveS_SOURCES = getopts.c isCmosTokenActive.cpp
isCmosTokenActiveS_LDFLAGS = -all-static

smitestS_SOURCES = getopts.c smitest.cpp
smitestS_LDFLAGS = -all-static

stateByteCtlS_SOURCES = getopts.c stateByteCtl.cpp
stateByteCtlS_LDFLAGS = -all-static

upBootCtlS_SOURCES = getopts.c upBootCtl.cpp
upBootCtlS_LDFLAGS = -all-static

getPasswordFormatS_SOURCES = getopts.c getPasswordFormat.cpp
getPasswordFormatS_LDFLAGS = -all-static

ascii2enUS_scancodeS_SOURCES = getopts.c ascii2enUS_scancode.cpp
ascii2enUS_scancodeS_LDFLAGS = -all-static

probesS_SOURCES = getopts.c probes.cpp
probesS_LDFLAGS = -all-static

dellLEDCtlS_SOURCES = getopts.c dellLEDCtl.cpp
dellLEDCtlS_LDFLAGS = -all-static
