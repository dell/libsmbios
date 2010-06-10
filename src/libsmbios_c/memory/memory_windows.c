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

#include <stdio.h>
#include <stdlib.h>

#include "smbios_c/obj/memory.h"
#include "smbios_c/types.h"
#include "memory_impl.h"
#include "common_internal.h"
#include "common_windows.h"

// usually want to include this last
#include "libsmbios_c_intlize.h"

// automatically set to zero
#define CDEFBufSize (64 * 4 * 1024)
u8 * CDEFBlockBuffer; // covers 0xC0000 - 0xDFFFF
HANDLE hPhysMem;

HANDLE OpenMemAccess(PCWSTR filename)
{
    UNICODE_STRING usDevmem;
    OBJECT_ATTRIBUTES oaAttrs;
    NTSTATUS status;
    HANDLE hPhysMem = NULL;

    RtlInitUnicodeString(&usDevmem, filename);
    InitializeObjectAttributes(&oaAttrs, &usDevmem, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenSection(&hPhysMem, SECTION_MAP_READ, &oaAttrs);

    if (!NT_SUCCESS(status))
        hPhysMem = NULL;

    return hPhysMem;
}

int CloseMemAccess(HANDLE hPhysMem)
{
    NTSTATUS status = NtClose(hPhysMem);
    if (!NT_SUCCESS(status))
        return FALSE;

    return TRUE;
}

int MapMem(HANDLE hPhysMem, PVOID pBaseAddr, PDWORD pPhysAddr, PDWORD pSize)
{
    NTSTATUS status;
    PHYSICAL_ADDRESS paAddr;

    * (DWORD *) pBaseAddr = (DWORD) NULL;
    paAddr.HighPart = 0;
    paAddr.LowPart = *pPhysAddr;
    status = NtMapViewOfSection(hPhysMem, NtCurrentProcess(), (PVOID *) pBaseAddr, 0L,
                                *pSize, &paAddr, pSize, ViewShare, 0, PAGE_READONLY);

    if (!NT_SUCCESS(status))
        return FALSE;

    *pPhysAddr = paAddr.LowPart;
    return TRUE;
}

int UnMapMem(PVOID pBaseAddr)
{
    NTSTATUS status = NtUnmapViewOfSection(NtCurrentProcess(), pBaseAddr);
    if (!NT_SUCCESS(status))
        return FALSE;

    return TRUE;
}

static BOOL setPrivilege(LPCTSTR privilegeName, BOOL enable)
{
    HANDLE              hToken;
    HANDLE              hCurrentProcess;
    DWORD               err;
    TOKEN_PRIVILEGES    tkprivs;

    // Get a token for this process.
    hCurrentProcess = GetCurrentProcess();
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        // Get the LUID for the security privilege.
        LookupPrivilegeValue(NULL, privilegeName, &tkprivs.Privileges[0].Luid);
        tkprivs.PrivilegeCount = 1;  // one privilege to set
        tkprivs.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

        // Get the security privilege for this process.
        AdjustTokenPrivileges(hToken, FALSE, &tkprivs, 0, (PTOKEN_PRIVILEGES)NULL, NULL);
    }
    // get the error returned by AdjustTokenPrivileges() or OpenProcessToken()
    err = GetLastError();

    return err == ERROR_SUCCESS;
}

int readPhysicalMemoryMap( HANDLE hPhysMem, u8 *buffer, u64 offset, unsigned int length)
{
    unsigned int totalBytes = 0;
    unsigned int originalSize = length;
    unsigned int pageSize = length;
    int retval = -1;
    char *error = _("Handle to physical memory was not set or could not be opened.");

    if(!hPhysMem)
        goto out_err;

    error = _("Error accessing buffer.");
    if(0 == buffer)
        goto out_err;

    while(totalBytes != originalSize )
    {
        DWORD BaseAddr;
        DWORD PhysAddr = (DWORD)(offset);

        // OK, first get the page that has the requested offset.  This
        // function will round-down physaddr to the nearest page boundary,
        // and the pageSize will change to the size of the page
        error = _("Error mapping physical memory.");
        if (!MapMem(hPhysMem, (PVOID) &BaseAddr, &PhysAddr, (PDWORD)(&pageSize)))
            goto out_err;

        //Find the index into the page based on requested offset
        unsigned int index =  (DWORD)(offset) - PhysAddr;

        // Only continue to copy if the index is within the pageSize and
        // we still need bytes
        while( index < pageSize && totalBytes != originalSize)
        {
            u64 tmp = BaseAddr + index; // extra tmp to satisfy vc++ /w4
            buffer[totalBytes] = *((u8 *)(tmp));
            index++;
            totalBytes++;
        }

        u64 tmp = BaseAddr; // extra tmp to satisfy vc++ /w4
        error = _("Error unmapping physical memory.");
        if (!UnMapMem((PVOID)(tmp)))
            goto out_err;

        // If the inner while loop exited due to the end of the page, we
        // need to update the offset and the remaining size and map
        // another page
        offset = PhysAddr + index;
        pageSize = originalSize-totalBytes;
    }

    retval = 0;

out_err:
    // TODO: set module error status
    return retval;
}

int readPhysicalMemoryDebugSysctl( u8 *buffer, u64 offset, unsigned int length)
{
    MEM_STRUCT mem;
    NTSTATUS status;
    ULONG bytesReturned;
    int retval = -1;
    char *error = 0;

    memset(&mem, 0, sizeof(mem));
    mem.Addr = (DWORD)offset;
    mem.pBuf = buffer;
    mem.NumBytes = (DWORD)length;

    status = ZwSystemDebugControl(DebugSysReadPhysicalMemory,
                &mem, sizeof(mem), 0, 0, &bytesReturned);

    error = _("Could not use Debug Sysctl to read physical memory.");
    if (NT_SUCCESS(status))
        retval = 0;

    // TODO: set error string
    return retval;
}


int fillBufferCache() 
{
    DWORD iSignature = 0x4649524D; //FIRM
    char *error = 0;
    int retval = -1;

    CDEFBlockBuffer = calloc(1, CDEFBufSize);
    error = _("Unable to allocate ram.");
    if (!CDEFBlockBuffer)
        goto out;

    // pass NULL to EnumSystemFirmwareTables. This will return the size needed.
    unsigned int bufsize = EnumSystemFirmwareTables( iSignature, NULL, 0 );
    error = _("EnumSystemFirmwareTables returned 0 for table size.");
    if( bufsize <= 0 )
        goto out;

    DWORD * FirmwareTableEnumBuffer = calloc(1, bufsize);
    EnumSystemFirmwareTables( iSignature, FirmwareTableEnumBuffer, bufsize );

    for (DWORD *ptr=FirmwareTableEnumBuffer; ptr<(FirmwareTableEnumBuffer+bufsize); ++ptr)
        GetSystemFirmwareTable( iSignature, *ptr, CDEFBlockBuffer + (*ptr - 0xC0000), CDEFBufSize - (*ptr - 0xC0000));

    retval = 0;

out:
    return retval;
}

int readmemEnumSystemFirmwareTables( u8 *buffer, u64 offset, unsigned int length)
{
    int retval = -1;

    if (offset < 0xC0000 || offset > 0xFFFFF)
        goto out;

    if (offset + length > 0xFFFFF)
        goto out;

    if (!CDEFBlockBuffer && fillBufferCache() != 0)
        goto out;

    memset( buffer, 0, length );
    memcpy( buffer, &CDEFBlockBuffer[(DWORD)offset - 0xC0000], length );

    retval = 0;

out:
    return retval;
}


void destroy()
{

    // NEVER THROW FROM DESTRUCTOR!
    if(hPhysMem)
    {
        CloseMemAccess(hPhysMem); // if this fails, something bad happened,
    }                       // but there isn't anything we can do about it.

    free(CDEFBlockBuffer);
    CDEFBlockBuffer = NULL;
}


void fillBuffer( u8 *buffer, u64 offset, unsigned int length)
{

    // Use the newer W2K3 SP1 APIs if they are available.  If we first call
    // readPhysicalMemoryMap and readPhysicalMemoryDebugSysctl and then call
    // enumSystemFirmwareTables, the new GetSystemFirmwareTable API of W2K3 SP1
    // returns invalid data.
    if (EnumSystemFirmwareTables && GetSystemFirmwareTable )
        if (0 == readmemEnumSystemFirmwareTables( buffer, offset, length ))
            goto out;

    if (hPhysMem && 0 == readPhysicalMemoryMap( hPhysMem, buffer, offset, length ))
        goto out;

    readPhysicalMemoryDebugSysctl( buffer, offset, length );

out:
    return;
}


__hidden int init_mem_struct(struct memory_access_obj *m)
{
    int retval = -1;
    printf("BEGIN: WINDOWS MEMORY ACCESS NOT IMPLEMENTED YET!!!! \n");

    if (!LoadNtdllFuncs())
        goto out_err;

    // DEBUG privs necessary to call systemdebugcontrol
    // set here to avoid big delays in mem read loops
    setPrivilege(SE_DEBUG_NAME, 1);
    if (!hPhysMem)
        hPhysMem = OpenMemAccess(L"\\device\\physicalmemory");

    printf("END: WINDOWS MEMORY ACCESS NOT IMPLEMENTED YET!!!! \n");
    goto out;

out_err:
    return retval; 

out:
    return retval;
}

