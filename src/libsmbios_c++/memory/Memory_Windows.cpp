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

#define LIBSMBIOS_SOURCE
#include "MemoryImpl.h"
#include "miniddk.h"

#include <cstdio>

// include this last
#include "smbios/message.h"

using namespace std;

#ifdef _MSC_VER
#define NTDLL L"ntdll.dll"
#define KERNEL32 L"kernel32.dll"
#else
#define NTDLL "ntdll.dll"
#define KERNEL32 "kernel32.dll"
#endif

namespace memory
{
    MemoryFactoryImpl::MemoryFactoryImpl()
    {
        setParameter("memFile", "\\Device\\PhysicalMemory");
    }


    // Non Member Functions
    NtOpenSectionPtr NtOpenSection = NULL;
    NtClosePtr NtClose = NULL;
    NtMapViewOfSectionPtr NtMapViewOfSection = NULL;
    NtUnmapViewOfSectionPtr NtUnmapViewOfSection = NULL;
    RtlInitUnicodeStringPtr RtlInitUnicodeString = NULL;
    ZwSystemDebugControlPtr ZwSystemDebugControl = NULL;

    EnumSystemFirmwareTablesPtr EnumSystemFirmwareTables = NULL;
    GetSystemFirmwareTablePtr GetSystemFirmwareTable = NULL;
    u8 * CBlockBuffer = NULL;
    u8 * EBlockBuffer = NULL;
    int reopenHint = 1;

    int LoadNtdllFuncs(void)
    {
        HMODULE hNtdll;
        HMODULE hKerneldll;

        hNtdll = GetModuleHandle(NTDLL);
        hKerneldll = GetModuleHandle(KERNEL32);
        if (!(hNtdll && hKerneldll))
            return FALSE;

        // should be available across all systems no matter our priv level
        NtOpenSection        = (NtOpenSectionPtr) GetProcAddress(hNtdll, "NtOpenSection");
        NtClose              = (NtClosePtr) GetProcAddress(hNtdll, "NtClose");
        NtMapViewOfSection   = (NtMapViewOfSectionPtr) GetProcAddress(hNtdll, "NtMapViewOfSection");
        NtUnmapViewOfSection = (NtUnmapViewOfSectionPtr) GetProcAddress(hNtdll, "NtUnmapViewOfSection");
        RtlInitUnicodeString = (RtlInitUnicodeStringPtr) GetProcAddress(hNtdll, "RtlInitUnicodeString");
        ZwSystemDebugControl = (ZwSystemDebugControlPtr) GetProcAddress(hNtdll, "ZwSystemDebugControl");

        // Donot return false since these APIs are only available on W2K3 SP1 and higher.
        // returning FALSE will break libsmbios on W2K and W2K3( no SP )
        EnumSystemFirmwareTables = (EnumSystemFirmwareTablesPtr) GetProcAddress(hKerneldll, "EnumSystemFirmwareTables");
        GetSystemFirmwareTable = (GetSystemFirmwareTablePtr) GetProcAddress(hKerneldll, "GetSystemFirmwareTable");

        return TRUE;
    }

    // MEB: fatal issue
    // MEB: pass in filename.  use getParameterString("memfile") as the
    // parameter. makes it possible to unit test against file.
    HANDLE OpenMemAccess(void)
    {
        UNICODE_STRING usDevmem;
        OBJECT_ATTRIBUTES oaAttrs;
        NTSTATUS status;
        HANDLE hPhysMem = NULL;

        RtlInitUnicodeString(&usDevmem, L"\\device\\physicalmemory");
        InitializeObjectAttributes(&oaAttrs, &usDevmem, OBJ_CASE_INSENSITIVE, NULL, NULL);
        status = NtOpenSection(&hPhysMem, SECTION_MAP_READ, &oaAttrs);

        if (!NT_SUCCESS(status))
        {
            hPhysMem = NULL;
        }

        return hPhysMem;
    }

    int CloseMemAccess(HANDLE hPhysMem)
    {
        NTSTATUS status;

        status = NtClose(hPhysMem);

        if (!NT_SUCCESS(status))
        {
            return FALSE;
        }

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
        {
            hPhysMem = NULL;
            return FALSE;
        }

        *pPhysAddr = paAddr.LowPart;
        return TRUE;
    }

    int UnMapMem(PVOID pBaseAddr)
    {
        NTSTATUS status;

        status = NtUnmapViewOfSection(NtCurrentProcess(), pBaseAddr);

        if (!NT_SUCCESS(status))
        {
            return FALSE;
        }

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

    void readPhysicalMemoryMap( HANDLE hPhysMem, u8 *buffer, u64 offset, unsigned int length)
    {
        unsigned int totalBytes = 0;
        unsigned int originalSize = length;
        unsigned int pageSize = length;

        if(!hPhysMem)
        {
            throw AccessErrorImpl( _("Handle to physical memory was not set or could not be opened.") );
        }

        if(0 == buffer)
        {
            throw AccessErrorImpl( _("Error accessing buffer.") );
        }

        while(totalBytes != originalSize )
        {
            DWORD BaseAddr;
            DWORD PhysAddr = static_cast<DWORD>(offset);

            // OK, first get the page that has the requested offset.  This
            // function will round-down physaddr to the nearest page boundary,
            // and the pageSize will change to the size of the page
            if (!MapMem(hPhysMem, (PVOID) &BaseAddr, &PhysAddr, reinterpret_cast<PDWORD>(&pageSize)))
            {
                throw AccessErrorImpl( _("Error mapping physical memory.") );
            }

            //Find the index into the page based on requested offset
            unsigned int index =  static_cast<DWORD>(offset) - PhysAddr;

            // Only continue to copy if the index is within the pageSize and
            // we still need bytes
            while( index < pageSize && totalBytes != originalSize)
            {
                u64 tmp = BaseAddr + index; // extra tmp to satisfy vc++ /w4
                buffer[totalBytes] = *reinterpret_cast<u8 *>(tmp);
                index++;
                totalBytes++;
            }

            u64 tmp = BaseAddr; // extra tmp to satisfy vc++ /w4
            if (!UnMapMem(reinterpret_cast<PVOID>(tmp)))
            {
                throw AccessErrorImpl( _("Error unmapping physical memory."));
            }

            // If the inner while loop exited due to the end of the page, we
            // need to update the offset and the remaining size and map
            // another page
            offset = PhysAddr + index;
            pageSize = originalSize-totalBytes;
        }
    }

    void readPhysicalMemoryDebugSysctl( u8 *buffer, u64 offset, unsigned int length)
    {
        MEM_STRUCT mem;
        NTSTATUS status;
        ULONG bytesReturned;

        memset(&mem, 0, sizeof(mem));
        mem.Addr = (DWORD)offset;
        mem.pBuf = buffer;
        mem.NumBytes = (DWORD)length;

        status = ZwSystemDebugControl(DebugSysReadPhysicalMemory,
                                      &mem,
                                      sizeof(mem),
                                      0,
                                      0,
                                      &bytesReturned);
        if (! NT_SUCCESS(status) )
        {
            throw AccessErrorImpl( _("Could not use Debug Sysctl to read physical memory."));
        }
    }


    // breaks for requests that span boundaries
    // breaks for requests of mem at offset > 1M
    void enumSystemFirmwareTables( u8 *buffer, u64 offset, unsigned int length)
    {
        // The following code is added for faster access. Otherewise, it takes libsmbios
        // approx 3-5minutes to get Service tag etc.
        if( offset >= 0xC0000 && offset < 0xDFFFF && CBlockBuffer != NULL )
        {
            memset( buffer, 0, length );
            memcpy( buffer, &CBlockBuffer[(DWORD)offset - 0xC0000], length );
            return;
        }

        // The following code is added for faster access. Otherewise, it takes libsmbios
        // approx 3-5minutes to get Service tag etc.
        if( offset >= 0xE0000 && offset < 0xFFFFF && EBlockBuffer != NULL ) // optimization
        {
            memset( buffer, 0, length );
            memcpy( buffer, &EBlockBuffer[(DWORD)offset - 0xE0000], length );
            return;
        }

        DWORD iSignature = 0x46000000 | 0x00490000 | 0x00005200 | 0x0000004D ; //FIRM

        // pass NULL to EnumSystemFirmwareTables. This will return the size needed.
        unsigned int iBufferSizeNeeded = EnumSystemFirmwareTables( iSignature, NULL, 0 );
        if( iBufferSizeNeeded > 0 )
        {
            DWORD * FirmwareTableEnumBuffer = new DWORD[iBufferSizeNeeded];
            ULONG i=0;
            for( i = 0; i < iBufferSizeNeeded; i++ )  // zero out the buffer. Otherwise, the function sometimes fails.
            {
                FirmwareTableEnumBuffer[i] = 0;
            }

            EnumSystemFirmwareTables( iSignature, FirmwareTableEnumBuffer, iBufferSizeNeeded );
            DWORD FirmwareTableNameToUse = 0;
            for( i = 0; i < iBufferSizeNeeded; i++ )
            {
                if( FirmwareTableEnumBuffer[i] > 0 && FirmwareTableEnumBuffer[i] <= offset && offset <= FirmwareTableEnumBuffer[i] + 128 * 1024 )
                {
                    FirmwareTableNameToUse = FirmwareTableEnumBuffer[i];
                }
            }
            delete [] FirmwareTableEnumBuffer;

            if( FirmwareTableNameToUse == 0 )
            {
                throw AccessErrorImpl( _("Could not locate a table which can be used.") );
            }


            iBufferSizeNeeded = GetSystemFirmwareTable( iSignature, FirmwareTableNameToUse, NULL, 0 );
            if( iBufferSizeNeeded > 0 )
            {
                u8 * MemoryAtRequestedOffSet = NULL;
                MemoryAtRequestedOffSet = new u8[iBufferSizeNeeded];
                if( MemoryAtRequestedOffSet != NULL )
                {
                    memset( MemoryAtRequestedOffSet, 0, iBufferSizeNeeded );
                    memset( buffer, 0, length );
                    GetSystemFirmwareTable( iSignature, FirmwareTableNameToUse, MemoryAtRequestedOffSet, iBufferSizeNeeded );
                    memcpy( buffer, &MemoryAtRequestedOffSet[(DWORD)offset - FirmwareTableNameToUse], length );
                    if( FirmwareTableNameToUse == 0xC0000 )
                    {
                        CBlockBuffer = MemoryAtRequestedOffSet; // save for later use
                    }
                    else
                        if( FirmwareTableNameToUse == 0xE0000 )
                        {
                            EBlockBuffer = MemoryAtRequestedOffSet; // save for later use
                        }
                        else
                        {
                            delete MemoryAtRequestedOffSet;
                        }
                }
                else
                {
                    throw AccessErrorImpl( _("Failed to allocate memory for Firmware table.") );
                }
            }
            else
            {
                throw AccessErrorImpl( _("GetSystemFirmwareTable returned 0 for table length.") );
            }
        }
        else
        {
            throw AccessErrorImpl( _("EnumSystemFirmwareTables returned 0 for table size.") );
        }
    }


    MemoryOsSpecific::MemoryOsSpecific( const string )
            : IMemory()
    {
        HANDLE hPhysMem = NULL;

        if (!LoadNtdllFuncs())
        {
            //printf("Could not find functions in dll!\n");
            //TODO change exception
            throw AccessErrorImpl( _("Could not load ntdll functions!") );
        }

        // DEBUG privs necessary to call systemdebugcontrol
        // set here to avoid big delays in mem read loops
        setPrivilege(SE_DEBUG_NAME, 1);

        hPhysMem = OpenMemAccess();

        osData = static_cast<HANDLE *>(hPhysMem);
    }

    MemoryOsSpecific::~MemoryOsSpecific()
    {
        HANDLE hPhysMem = static_cast<HANDLE>(osData);

        // NEVER THROW FROM DESTRUCTOR!
        if(hPhysMem)
        {
            CloseMemAccess(hPhysMem); // if this fails, something bad happened,
        }                       // but there isn't anything we can do about it.

        delete CBlockBuffer;   // safe to delete NULL
        CBlockBuffer = NULL;

        delete EBlockBuffer;   // safe to delete NULL
        EBlockBuffer = NULL;
    }

    void MemoryOsSpecific::fillBuffer( u8 *buffer, u64 offset, unsigned int length) const
    {

        if( EnumSystemFirmwareTables && GetSystemFirmwareTable )
        {
            // Use the newer W2K3 SP1 APIs if they are available.
            // If we first call readPhysicalMemoryMap and readPhysicalMemoryDebugSysctl
            // and then call enumSystemFirmwareTables, the new GetSystemFirmwareTable API
            // of W2K3 SP1 returns invalid data.
            enumSystemFirmwareTables( buffer, offset, length );
        }
        else
        {
            HANDLE hPhysMem = static_cast<HANDLE>(osData);
            if(hPhysMem)
            {
                readPhysicalMemoryMap( hPhysMem, buffer, offset, length );
            }
            else
            {
                readPhysicalMemoryDebugSysctl( buffer, offset, length );
            }
        }
    }

    u8 MemoryOsSpecific::getByte( u64 offset ) const
    {
        u8 value = 0;
        fillBuffer(&value, offset, 1);
        return value;
    }

    void MemoryOsSpecific::putByte( u64 , u8 ) const
    {
        throw smbios::NotImplementedImpl( _("writing to physical memory is not implemented on Windows yet.") );
    }


    // yes, I know these do absolutely nothing right now. 
    int MemoryOsSpecific::incReopenHint()
    {
        return ++reopenHint;
    }
    int MemoryOsSpecific::decReopenHint()
    {
        return --reopenHint;
    }
}
