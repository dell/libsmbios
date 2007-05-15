bin_PROGRAMS = assetTag getSystemId  propertyTag  serviceTag  verifySmiPassword  wakeupCtl  tokenCtl  dellBiosUpdate  dellLcdBrightness dumpSmbios dellWirelessCtl

EXTRA_PROGRAMS = assetTagS getSystemIdS  propertyTagS  serviceTagS  verifySmiPasswordS  wakeupCtlS  tokenCtlS  dellBiosUpdateS  dellLcdBrightnessS dumpSmbiosS dellWirelessCtlS

static: $(EXTRA_PROGRAMS)

CLEANFILES=$(EXTRA_PROGRAMS)

AM_LDFLAGS = -L@top_builddir@/libraries/ -lsmbios
INCLUDES = -I@top_builddir@/include -I@top_builddir@/libraries/common -I@top_srcdir@/include -I@top_srcdir@/libraries/common 
EXTRA_DIST = getopts.h

assetTag_SOURCES = assetTag.cpp getopts.c
getSystemId_SOURCES = getSystemId.cpp getopts.c
propertyTag_SOURCES = getopts.c propertyTag.cpp
serviceTag_SOURCES = getopts.c serviceTag.cpp
verifySmiPassword_SOURCES = getopts.c verifySmiPassword.cpp
wakeupCtl_SOURCES = getopts.c wakeupCtl.cpp
tokenCtl_SOURCES = getopts.c tokenCtl.cpp
dellBiosUpdate_SOURCES = getopts.c dellBiosUpdate.cpp
dellLcdBrightness_SOURCES = getopts.c dellLcdBrightness.cpp
dellWirelessCtl_SOURCES = getopts.c dellWirelessCtl.cpp

#libsmbiosxml progs
dumpSmbios_SOURCES = getopts.c dumpSmbios.cpp
dumpSmbios_LDFLAGS = -lsmbiosxml
dumpSmbios_DEPENDENCIES = @top_builddir@/libraries/libsmbiosxml.la


#static versions
assetTagS_SOURCES = assetTag.cpp getopts.c
getSystemIdS_SOURCES = getSystemId.cpp getopts.c
propertyTagS_SOURCES = getopts.c propertyTag.cpp
serviceTagS_SOURCES = getopts.c serviceTag.cpp
verifySmiPasswordS_SOURCES = getopts.c verifySmiPassword.cpp
wakeupCtlS_SOURCES = getopts.c wakeupCtl.cpp
tokenCtlS_SOURCES = getopts.c tokenCtl.cpp
dellBiosUpdateS_SOURCES = getopts.c dellBiosUpdate.cpp
dellLcdBrightnessS_SOURCES = getopts.c dellLcdBrightness.cpp
dellWirelessCtlS_SOURCES = getopts.c dellWirelessCtl.cpp

#libsmbiosxml progs
dumpSmbiosS_SOURCES = getopts.c dumpSmbios.cpp
dumpSmbiosS_LDFLAGS = -lsmbiosxml -all-static
dumpSmbiosS_DEPENDENCIES = @top_builddir@/libraries/libsmbiosxml.la

assetTagS_LDFLAGS = -all-static
getSystemIdS_LDFLAGS = -all-static
propertyTagS_LDFLAGS = -all-static
serviceTagS_LDFLAGS = -all-static
verifySmiPasswordS_LDFLAGS = -all-static
wakeupCtlS_LDFLAGS = -all-static
tokenCtlS_LDFLAGS = -all-static
dellBiosUpdateS_LDFLAGS = -all-static
dellLcdBrightnessS_LDFLAGS = -all-static
dellWirelessCtlS_LDFLAGS = -all-static
