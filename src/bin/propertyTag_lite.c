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
#include <string.h>

#include "smbios_c/smi.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios_c/system_info.h" // this is the main header to include to use the C interface
#include "getopts.h"

#define PROP_TAG_SIZE 80

struct options opts[] =
    {
        {
            250, "set", "Set Dell Property Tag", "s", 1
        },
        { 252, "password", "BIOS setup password", "p", 1 },
        { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

int
main (int argc, char **argv)
{
    int retval = 0;

    int c=0;
    char *args = 0;

    bool setVal = false;
    const char *password = 0;
    const char *new_tag = 0;
    bool rawPassword=false;

    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 250:
            setVal = 1;
            // Property tag can be at most 80 chars (plus '\0')
            new_tag = args;
            break;
        case 252:
            password = args;
            break;
        case 249:
            rawPassword = true;
            break;
        case 255:
            printf("Libsmbios version:\t%s", smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    char propertyTag[PROP_TAG_SIZE + 1] = { 0, };
    get_property_ownership_tag(propertyTag, PROP_TAG_SIZE);
    printf("Existing Property Ownership Tag: %s\n", propertyTag);

    if(new_tag)
    {
        int ret = set_property_ownership_tag(0, new_tag, strlen(new_tag));
        if(ret==0)
            printf("changed tag successfully\n");
        else
            printf("tag change failed\n");
    }

    return retval;
}
