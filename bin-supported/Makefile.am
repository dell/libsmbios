# vim:tw=0:et:ts=4:sw=4

sbin_PROGRAMS += \
    bin-supported/assetTag \
    bin-supported/getSystemId   \
    bin-supported/propertyTag   \
    bin-supported/serviceTag    \
    bin-supported/verifySmiPassword     \
    bin-supported/wakeupCtl     \
    bin-supported/tokenCtl      \
    bin-supported/dellBiosUpdate    \
    bin-supported/dellLcdBrightness     \
    bin-supported/dumpSmbios    \
    bin-supported/dellWirelessCtl 

EXTRA_PROGRAMS += \
    bin-supported/assetTagS \
    bin-supported/getSystemIdS   \
    bin-supported/propertyTagS   \
    bin-supported/serviceTagS    \
    bin-supported/verifySmiPasswordS     \
    bin-supported/wakeupCtlS     \
    bin-supported/tokenCtlS      \
    bin-supported/dellBiosUpdateS    \
    bin-supported/dellLcdBrightnessS     \
    bin-supported/dumpSmbiosS    \
    bin-supported/dellWirelessCtlS 


getopts_h=bin-supported/getopts.h
getopts_c=bin-supported/getopts.c
EXTRA_DIST += $(getopts_h)
CLEANFILES += bin-supported/.libs

bin_supported_assetTag_SOURCES = bin-supported/assetTag.cpp $(getopts_c)
bin_supported_getSystemId_SOURCES = bin-supported/getSystemId.cpp $(getopts_c)
bin_supported_propertyTag_SOURCES = $(getopts_c) bin-supported/propertyTag.cpp
bin_supported_serviceTag_SOURCES = $(getopts_c) bin-supported/serviceTag.cpp
bin_supported_verifySmiPassword_SOURCES = $(getopts_c) bin-supported/verifySmiPassword.cpp
bin_supported_wakeupCtl_SOURCES = $(getopts_c) bin-supported/wakeupCtl.cpp
bin_supported_tokenCtl_SOURCES = $(getopts_c) bin-supported/tokenCtl.cpp
bin_supported_dellLcdBrightness_SOURCES = $(getopts_c) bin-supported/dellLcdBrightness.cpp
bin_supported_dellWirelessCtl_SOURCES = $(getopts_c) bin-supported/dellWirelessCtl.cpp
bin_supported_dumpSmbios_SOURCES = $(getopts_c) bin-supported/dumpSmbios.cpp

bin_supported_assetTag_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_getSystemId_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_propertyTag_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_serviceTag_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_verifySmiPassword_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_wakeupCtl_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_tokenCtl_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_dellLcdBrightness_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_dellWirelessCtl_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_dumpSmbios_LDFLAGS = -Llibraries/ -lsmbios

# BAD BAD BAD programs....  (uses private headers)
bin_supported_dellBiosUpdate_SOURCES = $(getopts_c) bin-supported/dellBiosUpdate.cpp
bin_supported_dellBiosUpdate_LDFLAGS = -Llibraries/ -lsmbios
bin_supported_dellBiosUpdate_CXXFLAGS = -Ilibraries/common



#static versions
bin_supported_assetTagS_SOURCES = bin-supported/assetTag.cpp $(getopts_c)
bin_supported_getSystemIdS_SOURCES = bin-supported/getSystemId.cpp $(getopts_c)
bin_supported_propertyTagS_SOURCES = $(getopts_c) bin-supported/propertyTag.cpp
bin_supported_serviceTagS_SOURCES = $(getopts_c) bin-supported/serviceTag.cpp
bin_supported_verifySmiPasswordS_SOURCES = $(getopts_c) bin-supported/verifySmiPassword.cpp
bin_supported_wakeupCtlS_SOURCES = $(getopts_c) bin-supported/wakeupCtl.cpp
bin_supported_tokenCtlS_SOURCES = $(getopts_c) bin-supported/tokenCtl.cpp
bin_supported_dellLcdBrightnessS_SOURCES = $(getopts_c) bin-supported/dellLcdBrightness.cpp
bin_supported_dellWirelessCtlS_SOURCES = $(getopts_c) bin-supported/dellWirelessCtl.cpp
bin_supported_dumpSmbiosS_SOURCES = $(getopts_c) bin-supported/dumpSmbios.cpp

bin_supported_assetTagS_LDFLAGS          = -all-static -Llibraries/ -lsmbios
bin_supported_getSystemIdS_LDFLAGS       = -all-static -Llibraries/ -lsmbios
bin_supported_propertyTagS_LDFLAGS       = -all-static -Llibraries/ -lsmbios
bin_supported_serviceTagS_LDFLAGS        = -all-static -Llibraries/ -lsmbios
bin_supported_verifySmiPasswordS_LDFLAGS = -all-static -Llibraries/ -lsmbios
bin_supported_wakeupCtlS_LDFLAGS         = -all-static -Llibraries/ -lsmbios
bin_supported_tokenCtlS_LDFLAGS          = -all-static -Llibraries/ -lsmbios
bin_supported_dellLcdBrightnessS_LDFLAGS = -all-static -Llibraries/ -lsmbios
bin_supported_dellWirelessCtlS_LDFLAGS   = -all-static -Llibraries/ -lsmbios
bin_supported_dumpSmbiosS_LDFLAGS        = -all-static -Llibraries/ -lsmbios

# BAD again (uses private includes.)
bin_supported_dellBiosUpdateS_SOURCES = $(getopts_c) bin-supported/dellBiosUpdate.cpp
bin_supported_dellBiosUpdateS_LDFLAGS    = -all-static -Llibraries/ -lsmbios
bin_supported_dellBiosUpdateS_CXXFLAGS = -Ilibraries/common
