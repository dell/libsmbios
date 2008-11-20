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

#include <iostream>
#include <sstream>

/* stuff for WMI. needs SDK, so is optional. */
#ifdef LIBSMBIOS_WIN_USE_WMI
#   ifndef _WIN32_DCOM
#   define _WIN32_DCOM
#   endif

// this pragma statment will automatically add the 'wbemuuid.lib' library
// to program library link list
#  pragma comment(lib, "wbemuuid")

#   include <objbase.h>
#   include <wbemcli.h>
#endif

#include "smbios/IMemory.h"
#include "SmbiosImpl.h"
#include "miniddk.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

#ifdef _MSC_VER
#define NTDLL L"ntdll.dll"
#define KERNEL32 L"kernel32.dll"
#else
#define NTDLL "ntdll.dll"
#define KERNEL32 "kernel32.dll"
#endif

namespace smbios
{
    // Non Member Functions
    GetSystemFirmwareTablePtr GetSystemFirmwareTable = 0;

    int LoadNtdllFuncs(void)
    {
        HMODULE hKerneldll;

        hKerneldll = GetModuleHandle(KERNEL32);
        if (!hKerneldll)
            return FALSE;

        // Donot return false since these APIs are only available on W2K3 SP1 and higher.
        // returning FALSE will break libsmbios on W2K and W2K3( no SP )
        GetSystemFirmwareTable = (GetSystemFirmwareTablePtr) GetProcAddress(hKerneldll, "GetSystemFirmwareTable");

        return TRUE;
    }


    bool SmbiosWinGetFirmwareTableStrategy::getSmbiosTable(const u8 **smbiosBuffer, smbiosLowlevel::smbios_table_entry_point *table_header, bool )
    {
        // new throws exception, no need to test.
        u8 *newSmbiosBuffer = 0;

        DWORD iSignature =             'R'; //RSMB
        iSignature = iSignature << 8 | 'S';
        iSignature = iSignature << 8 | 'M';
        iSignature = iSignature << 8 | 'B';

        if( !GetSystemFirmwareTable )
            if( !LoadNtdllFuncs() )
                throw ParseExceptionImpl( _("Could not load dll functions.") );

        if( !GetSystemFirmwareTable )
            throw ParseExceptionImpl( _("Could not access GetSystemFirmwareTable() API.") );

        int iBufferSizeNeeded = GetSystemFirmwareTable( iSignature, 0, 0, 0 );
        if( iBufferSizeNeeded <= 0 )
            throw ParseExceptionImpl( _("GetSystemFirmwareTable returned 0 for table length.") );

        newSmbiosBuffer = new u8[iBufferSizeNeeded];
        if( ! newSmbiosBuffer )
            throw ParseExceptionImpl( _("Failed to allocate memory for Firmware table.") );
        memset( newSmbiosBuffer, 0, sizeof(u8) * iBufferSizeNeeded );

        // populate buffer with table.
        // Note that this is not the actual smbios table:
        // From MS: The layout of the data returned by the raw SMBIOS table
        //          provider is identical to the data returned by the
        //          MSSmBIOS_RawSMBiosTables WMI class.  The
        //          MSSmBIOS_RawSMBiosTables class is located in the root\wmi
        //          namespace.
        GetSystemFirmwareTable( iSignature, 0, newSmbiosBuffer, iBufferSizeNeeded );

        // Have to manually set values in header because we cannot get table entry point using this api
        table_header->dmi.table_length = static_cast<u16>(iBufferSizeNeeded);
        table_header->major_ver = newSmbiosBuffer[1];
        table_header->minor_ver = newSmbiosBuffer[2];
        table_header->dmi.table_num_structs = 9999;

        // get rid of header that MS tacks on.
#       define MS_RSMB_HEADER_SIZE 8

        memmove(newSmbiosBuffer, newSmbiosBuffer + MS_RSMB_HEADER_SIZE, iBufferSizeNeeded - MS_RSMB_HEADER_SIZE);
        memset( newSmbiosBuffer + iBufferSizeNeeded - MS_RSMB_HEADER_SIZE, 0, MS_RSMB_HEADER_SIZE);

        //delete old one, if necessary
        if( *smbiosBuffer )
        {
            memset (const_cast<u8 *>(*smbiosBuffer), 0, sizeof (**smbiosBuffer));
            delete [] const_cast<u8 *>(*smbiosBuffer);
            *smbiosBuffer = 0;
        }

        *smbiosBuffer = (const u8 *) newSmbiosBuffer;
        return true;
    }



#ifdef LIBSMBIOS_WIN_USE_WMI
    /* NON-MEMBER functions to help parse WMI data. */
    static void GetWMISMBIOSEntry( IWbemClassObject **pSmbios )
    {
        BSTR                    path = SysAllocString(L"root\\wmi");
        BSTR                    className = SysAllocString(L"MSSmBios_RawSMBiosTables");
        ULONG                   uReturned = 1;
        HRESULT                 hr = S_FALSE;
        IWbemLocator            *pLocator = NULL;
        IWbemServices           *pNamespace = NULL;
        IEnumWbemClassObject    *pEnumSMBIOS = NULL;

        try
        {
            hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE,
                                       NULL, EOAC_SECURE_REFS, NULL );

            /* No need to throw exception on failure for CoInitializeSecurity() because other user may have already
               initialized it properly.
             */

            hr = CoCreateInstance( CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLocator );
    
            if (! SUCCEEDED( hr ) )
                throw InternalErrorImpl(_("CoCreateInstance() failed for locator. "
                            "Check that the security levels have been "
                            "properly initialized."
                            ));
    
            hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace );
            pLocator->Release();
    
            if( WBEM_S_NO_ERROR != hr )
                throw InternalErrorImpl(_("ConnectServer() failed for namespace"));
    
            hr = pNamespace->CreateInstanceEnum( className, 0, NULL, &pEnumSMBIOS );
            pNamespace->Release();
    
            if (! SUCCEEDED( hr ) )
                throw InternalErrorImpl(_("CreateInstanceEnum() failed for MSSmBios_RawSMBiosTables"));
    
            hr = pEnumSMBIOS->Next( 4000, 1, pSmbios, &uReturned );
            pEnumSMBIOS->Release();
    
            if ( 1 != uReturned )
                throw InternalErrorImpl(_("Next() failed for pEnumSMBIOS"));
        }
        catch(const exception &)
        {
            SysFreeString(className);
            SysFreeString(path);
            throw;
        }
    }


    static void GetWMISMBIOSTable( IWbemClassObject *pSmbios, WMISMBIOSINFO &smbiosData )
    {
        BSTR                propName;
        CIMTYPE             type;
        VARIANT             pVal;
        SAFEARRAY           *parray = NULL;

        if ( NULL == pSmbios )
            throw ParseExceptionImpl( _("GetWMISMBIOSTable: NULL pointer to SMBIOS Entry specified.") );

        VariantInit(&pVal);

        propName = SysAllocString(L"SMBiosData");
        pSmbios->Get( propName, 0L, &pVal, &type, NULL);
        SysFreeString(propName);

        if ( ( VT_UI1 | VT_ARRAY ) != pVal.vt )
            throw ParseExceptionImpl( _("GetWMISMBIOSTable: SMBiosData returned unknown entry type.") );

        parray = V_ARRAY(&pVal);

        smbiosData.bufferSize = parray->rgsabound[0].cElements;

        if ( smbiosData.bufferSize == 0 )
            throw ParseExceptionImpl( _("GetWMISMBIOSTable: Buffer size was zero.") );

        smbiosData.buffer = new u8[smbiosData.bufferSize];
        if ( ! smbiosData.buffer )
            throw ParseExceptionImpl( _("GetWMISMBIOSTable: Failed to allocate memory for SMBIOS table.") );

        memcpy(smbiosData.buffer, (u8 *)parray->pvData, smbiosData.bufferSize);
    }

    static void GetWMISMBIOSVersion( IWbemClassObject *pSmbios, u8 *majorVersion, u8 *minorVersion )
    {
        BSTR            propName;
        HRESULT         hr = S_OK;
        CIMTYPE         type;
        VARIANT         pVal;

        if ( NULL == pSmbios )
            throw ParseExceptionImpl( _("GetWMISMBIOSVersion: null pointer passed as pSmbios.") );

        VariantInit( &pVal );
        propName = SysAllocString( L"SmbiosMajorVersion" ); 
        hr = pSmbios->Get( propName, 0L, &pVal, &type, NULL );
        SysFreeString( propName );

        if ( ! SUCCEEDED( hr ) || VT_UI1 != pVal.vt ) 
            throw ParseExceptionImpl( _("GetWMISMBIOSVersion: problem accessing WMI SmbiosMajorVersion.") );

        if(majorVersion)
            *majorVersion = V_UI1(&pVal); 

        VariantClear( &pVal ); 
        propName = SysAllocString( L"SmbiosMinorVersion" ); 
        hr = pSmbios->Get( propName, 0L, &pVal, &type, NULL );
        SysFreeString( propName );

        if ( !SUCCEEDED( hr ) || pVal.vt != VT_UI1 ) 
            throw ParseExceptionImpl( _("GetWMISMBIOSVersion: problem accessing WMI SmbiosMinorVersion.") );

        if(minorVersion)
            *minorVersion = V_UI1(&pVal); 
    }

    static void GetWMISMBIOSData( WMISMBIOSINFO &smbiosData )
    {
        IWbemClassObject    *pSmbios = NULL;

        try
        {
            if (! SUCCEEDED( CoInitialize(0) ) )
                throw InternalErrorImpl( _("Could not initialize COM.") );

            GetWMISMBIOSEntry( &pSmbios );
            GetWMISMBIOSTable( pSmbios, smbiosData );
            GetWMISMBIOSVersion( pSmbios, &smbiosData.majorVersion, &smbiosData.minorVersion );
        }
        catch(const exception &)
        {
            delete [] smbiosData.buffer;
            smbiosData.buffer = 0;
            throw;
        }
        CoUninitialize();
    }


    bool SmbiosWinWMIStrategy::getSmbiosTable(const u8 **smbiosBuffer, smbiosLowlevel::smbios_table_entry_point *table_header, bool )
    {
        // new throws exception, no need to test.
        WMISMBIOSINFO wmi_smbiosData;
        memset(&wmi_smbiosData, 0, sizeof(wmi_smbiosData));

        GetWMISMBIOSData( wmi_smbiosData );

        if( wmi_smbiosData.bufferSize <= 0 || ! wmi_smbiosData.buffer )
            throw ParseExceptionImpl( _("getSmbiosTable(): GetWMISMBIOSData returned 0 for buffer size.") );

        // fake version information for now.
        table_header->dmi.table_length = static_cast<u16>(wmi_smbiosData.bufferSize);
        table_header->major_ver = wmi_smbiosData.majorVersion;
        table_header->minor_ver = wmi_smbiosData.minorVersion;
        table_header->dmi.table_num_structs = 9999;

        //delete old one, if necessary
        if( *smbiosBuffer )
        {
            memset (const_cast<u8 *>(*smbiosBuffer), 0, sizeof (**smbiosBuffer));
            delete [] const_cast<u8 *>(*smbiosBuffer);
            *smbiosBuffer = 0;
        }

        *smbiosBuffer = (const u8 *) wmi_smbiosData.buffer;
        return true;
    }
#else
    bool SmbiosWinWMIStrategy::getSmbiosTable(const u8 **, smbiosLowlevel::smbios_table_entry_point *, bool )
    {
        return false;
    }
#endif /* LIBSMBIOS_WIN_USE_WMI */
}

