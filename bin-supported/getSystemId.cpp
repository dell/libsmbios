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
#include "smbios/compat.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <memory>

#include "smbios/IMemory.h"
#include "smbios/SystemInfo.h"
#include "smbios/version.h"
#include "getopts.h"

using namespace std;

struct options opts[] =
    {
        {
            254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1
        },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

int
main (int argc, char **argv)
{
    int retval = 0;
    const char *str    = 0;
    int         reqInt = 0;
    memory::MemoryFactory *memoryFactory = 0;

    int c=0;
    char *args = 0;
    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 254:
            // This is for unit testing. You can specify a file that
            // contains a dump of memory to use instead of writing
            // directly to RAM.
            memoryFactory = memory::MemoryFactory::getFactory();
            memoryFactory->setParameter("memFile", args);
            memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
            break;
        case 255:
            cout << "Libsmbios version:    " << LIBSMBIOS_RELEASE_VERSION << endl;
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf("Libsmbios:    " LIBSMBIOS_RELEASE_VERSION "\n" );

    //Error handline needs to be implemented for each of these functions
    //we don't want to catch exceptions because we want to test the C calling interface
    //and besides, C calling interface cannot throw exceptions.
    reqInt     = SMBIOSGetDellSystemId();
    if(reqInt)
        printf("System ID:    0x%04X\n", reqInt);
    else
    {
        printf("Error getting the System ID:    %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }


    str    = SMBIOSGetServiceTag();
    if(str)
    {
        printf("Service Tag:  %s\n", str);
        printf("Express Service Code: %lld\n", strtoll(str, NULL, 36));
    }
    else
    {
        printf("Error getting the Service Tag:  %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }
    SMBIOSFreeMemory(str);


    str   = SMBIOSGetSystemName();
    if(str)
        printf("Product Name: %s\n", str);
    else
    {
        printf("Error getting the Product Name: %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }
    SMBIOSFreeMemory(str);


    str   = SMBIOSGetBiosVersion();
    if(str)
        printf("BIOS Version: %s\n", str);
    else
    {
        printf("Error getting the BIOS Version: %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }
    SMBIOSFreeMemory(str);


    str   = SMBIOSGetVendorName();
    if(str)
        printf("Vendor:       %s\n", str);
    else
    {
        printf("Error getting the Vendor:       %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }
    SMBIOSFreeMemory(str);


    reqInt    = SMBIOSIsDellSystem();
    printf("Is Dell:      %d\n", reqInt);

    return retval;
}
