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
#include "smbios_c/compat.h"

#include <stdio.h>
#include <stdlib.h>

#include "smbios_c/memory.h"
#include "smbios_c/system_info.h"

#include "getopts.h"

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
    int reqInt = 0;

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
            memory_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, args);
            break;
        case 255:
            printf("Libsmbios version:    %s\n", smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf("Libsmbios:    %s\n", smbios_get_library_version_string());

    //Error handline needs to be implemented for each of these functions
    //we don't want to catch exceptions because we want to test the C calling interface
    //and besides, C calling interface cannot throw exceptions.
    reqInt     = smbios_get_dell_system_id();
    if(reqInt)
        printf("System ID:    0x%04X\n", reqInt);
    else
    {
        printf("Error getting the System ID:    unknown error.\n");
        retval = 1;
    }


    return retval;
}
