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

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <stdio.h>
#include <conio.h>

#include "CmosRWImpl.h"
#include "miniddk.h"

using namespace std;

#ifdef _MSC_VER
#define NTDLL L"ntdll.dll"
#else
#define NTDLL "ntdll.dll"
#endif

namespace cmos
{

    // Non Member Functions
    ZwSystemDebugControlPtr ZwSystemDebugControl;

    int LoadNtdllFuncs(void)
    {
        HMODULE hNtdll;

        hNtdll = GetModuleHandle(NTDLL);
        if (!hNtdll)
            return FALSE;

        ZwSystemDebugControl = (ZwSystemDebugControlPtr) GetProcAddress(hNtdll, "ZwSystemDebugControl");
        if (!ZwSystemDebugControl)
            return FALSE;

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
    //
    // CmosRWIo functions
    //

    // REGULAR CONSTRUCTOR
    CmosRWIo::CmosRWIo ()
            : ICmosRW(), Suppressable()
    {

        if (!LoadNtdllFuncs())
            throw smbios::NotImplementedImpl("Error loading DLLs needed to call functions to read CMOS.");

        if (!EnableDebug())
            throw smbios::NotImplementedImpl("Error loading DLLs needed to call functions to read CMOS.");
    }

    // COPY CONSTRUCTOR
    CmosRWIo::CmosRWIo (const CmosRWIo &)
            : ICmosRW(), Suppressable()
    {}

    // OVERLOADED ASSIGNMENT
    CmosRWIo & CmosRWIo::operator = (const CmosRWIo &)
    {
        return *this;
    }

    // TODO: need to throw exception on problem with iopl
    //
    u8 CmosRWIo::readByte (u32 indexPort, u32 dataPort, u32 offset) const
    {
        NTSTATUS status;
        u8 Buf;

        IO_STRUCT io;

        memset(&io, 0, sizeof(io));
        io.Addr = indexPort;
        io.pBuf = &offset;
        io.NumBytes = 1;
        io.Reserved4 = 1;
        io.Reserved6 = 1;

        status = ZwSystemDebugControl(DebugSysWriteIoSpace, &io, sizeof(io), NULL, 0, NULL);
        if (!NT_SUCCESS(status))
          throw smbios::NotImplementedImpl();

        memset(&io, 0, sizeof(io));
        io.Addr = dataPort;
        io.pBuf = &Buf;
        io.NumBytes = 1;
        io.Reserved4 = 1;
        io.Reserved6 = 1;

        status = ZwSystemDebugControl(DebugSysReadIoSpace, &io, sizeof(io), NULL, 0, NULL);
        if (!NT_SUCCESS(status))
          throw smbios::NotImplementedImpl();

        return Buf;
    }

    // TODO: need to throw exception on problem with iopl
    //
    void CmosRWIo::writeByte (u32 indexPort, u32 dataPort, u32 offset, u8 byte) const
    {
        (void) indexPort;
        (void) dataPort;
        (void) offset;
        (void) byte;
        throw smbios::NotImplementedImpl();

// enable this whenever we get write support on windows.
#if 0
        if(! isNotifySuppressed() )
        {
            notify();
        }
#endif
    }

}
