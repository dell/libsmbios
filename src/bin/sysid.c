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
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "smbios/SystemInfo.h"
#include "getopts.h"

struct options opts[] =
    {
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

int
main (int argc, char **argv)
{
    int retval = 0;
    const char *str    = 0;
    int         reqInt = 0;

    int c=0;
    char *args = 0;
    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 255:
            printf( "Libsmbios version:    "  SMBIOSGetLibraryVersionString() "\n");
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf("Libsmbios:    " SMBIOSGetLibraryVersionString() "\n" );

    //Error handline needs to be implemented for each of these functions
    //we don't want to catch exceptions because we want to test the C calling interface
    //and besides, C calling interface cannot throw exceptions.
    reqInt     = SMBIOSGetDellSystemId();
    if(reqInt)
        printf("System ID:    0x%04X\n", reqInt);
    else
    {
        printf("Error getting the System ID   : %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }


    str    = SMBIOSGetServiceTag();
    if(str)
        printf("Service Tag:  %s\n", str);
    else
    {
        printf("Error getting the Service Tag : %s\n", SMBIOSGetSysInfoErrorString());
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
        printf("Error getting the Vendor      : %s\n", SMBIOSGetSysInfoErrorString());
        retval = 1;
    }
    SMBIOSFreeMemory(str);


    reqInt    = SMBIOSIsDellSystem();
    printf("Is Dell:      %d\n", reqInt);

    return retval;
}
