/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// public
#include "smbios_c/types.h"

// private
#include "miniddk.h"
#include "common_windows.h"

#ifdef _MSC_VER
#define NTDLL L"ntdll.dll"
#define KERNEL32 L"kernel32.dll"
#else
#define NTDLL "ntdll.dll"
#define KERNEL32 "kernel32.dll"
#endif

int LoadNtdllFuncs(void)
{

    HMODULE hNtdll;
    HMODULE hKerneldll;

    hNtdll = GetModuleHandle(NTDLL);
    hKerneldll = GetModuleHandle(KERNEL32);

    if (!(hNtdll && hKerneldll))
        return FALSE;

    // should be available across all systems no matter our priv level
    if (!NtOpenSection)
        NtOpenSection        = (NtOpenSectionPtr) GetProcAddress(hNtdll, "NtOpenSection");
    if (!NtOpenSection)
        return FALSE;

    if (!NtClose)
        NtClose              = (NtClosePtr) GetProcAddress(hNtdll, "NtClose");
    if (!NtClose)
        return FALSE;

    if (!NtMapViewOfSection)
        NtMapViewOfSection   = (NtMapViewOfSectionPtr) GetProcAddress(hNtdll, "NtMapViewOfSection");
    if (!NtMapViewOfSection)
        return FALSE;

    if (!NtUnmapViewOfSection)
        NtUnmapViewOfSection = (NtUnmapViewOfSectionPtr) GetProcAddress(hNtdll, "NtUnmapViewOfSection");
    if (!NtUnmapViewOfSection)
        return FALSE;

    if (!RtlInitUnicodeString)
        RtlInitUnicodeString = (RtlInitUnicodeStringPtr) GetProcAddress(hNtdll, "RtlInitUnicodeString");
    if (!RtlInitUnicodeString)
        return FALSE;

    if (!ZwSystemDebugControl)
        ZwSystemDebugControl = (ZwSystemDebugControlPtr) GetProcAddress(hNtdll, "ZwSystemDebugControl");
    if (!ZwSystemDebugControl)
        return FALSE;

    // Donot return false since these APIs are only available on W2K3 SP1 and higher.
    // returning FALSE will break libsmbios on W2K and W2K3( no SP )
    if (!EnumSystemFirmwareTables)
        EnumSystemFirmwareTables = (EnumSystemFirmwareTablesPtr) GetProcAddress(hKerneldll, "EnumSystemFirmwareTables");
    if (!GetSystemFirmwareTable)
        GetSystemFirmwareTable = (GetSystemFirmwareTablePtr) GetProcAddress(hKerneldll, "GetSystemFirmwareTable");

    return TRUE;
}

int EnableDebug(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES TP;
    NTSTATUS status;

    status = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if (!NT_SUCCESS(status))
    {
        return FALSE;
    }

    TP.PrivilegeCount = 1;
    TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    status = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &TP.Privileges[0].Luid);
    if (!NT_SUCCESS(status))
    {
        return FALSE;
    }

    status = AdjustTokenPrivileges(hToken, FALSE, &TP, sizeof(TP), NULL, NULL);
    if (!NT_SUCCESS(status))
    {
        return FALSE;
    }

    return TRUE;
}
