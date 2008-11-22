#ifndef _LIBSMBIOS_C_COMMON_WINDOWS_H
#define _LIBSMBIOS_C_COMMON_WINDOWS_H

#include "miniddk.h"

int LoadNtdllFuncs(void);
int EnableDebug(void);
NtClosePtr NtClose;
NtOpenSectionPtr NtOpenSection;
NtMapViewOfSectionPtr NtMapViewOfSection;
NtUnmapViewOfSectionPtr NtUnmapViewOfSection;
RtlInitUnicodeStringPtr RtlInitUnicodeString;
ZwSystemDebugControlPtr ZwSystemDebugControl;
GetSystemFirmwareTablePtr GetSystemFirmwareTable;
EnumSystemFirmwareTablesPtr EnumSystemFirmwareTables;

#endif // _LIBSMBIOS_C_COMMON_WINDOWS_H

