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

#include "smbios_c/system_info.h" // this is the main header to include to use the C interface
#include "getopts.h"

struct options opts[] =
    {
        { 250, "set", "Set Dell Property Tag", "s", 1 },
        { 252, "password", "BIOS setup password", "p", 1 },
        { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

void map_ascii_to_scancode_braindead(char *outputScanCodeBuf, const char *inputAsciiBuf);

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
    }

    char *propertyTag = sysinfo_get_property_ownership_tag();
    printf("Existing Property Ownership Tag: %s\n", propertyTag);
    sysinfo_string_free(propertyTag);

    if(new_tag)
    {
        char *pass_scancode = 0;
        if (password)
        {
            pass_scancode = calloc(1, sizeof(password));
            map_ascii_to_scancode_braindead(pass_scancode, password);
        }

        int ret = sysinfo_set_property_ownership_tag(new_tag, password, pass_scancode);
        if(ret==0)
            printf("changed tag successfully\n");
        else
            printf("tag change failed\n");
    }

    return retval;
}






//--------------------------------------------------------------------
// Global Data Area
//--------------------------------------------------------------------
// maps ASCII number to a scan code
// sorted by ASCII value
static const char asc_map[256] =
    {
        0x03, 0x1E, 0x30, 0x46,  0x20, 0x12, 0x21, 0x22,
        0x0E, 0x0F, 0x1C, 0x25,  0x26, 0x1C, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x07, 0x0C,
        0x39, 0x02, 0x28, 0x04,  0x05, 0x06, 0x08, 0x28,
        0x0A, 0x0B, 0x09, 0x0D,  0x33, 0x0C, 0x34, 0x35,
        0x0B, 0x02, 0x03, 0x04,  0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x27, 0x27,  0x33, 0x0D, 0x34, 0x35,
        0x03, 0x1E, 0x30, 0x2E,  0x20, 0x12, 0x21, 0x22,
        0x23, 0x17, 0x24, 0x25,  0x26, 0x32, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x07, 0x0C,
        0x29, 0x1E, 0x30, 0x2E,  0x20, 0x12, 0x21, 0x22,
        0x23, 0x17, 0x24, 0x25,  0x26, 0x32, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x29, 0x0E,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
    };

//---------------------------------------------------------------------
// KBDMapASCIIToScanCode - Maps a string of ASCII codes to scan code
// bytes
//---------------------------------------------------------------------
void map_ascii_to_scancode_braindead(char *outputScanCodeBuf, const char *inputAsciiBuf)
{
    memset(outputScanCodeBuf, 0, strlen(inputAsciiBuf));
    for (size_t i = 0; i<strlen(inputAsciiBuf); i++)
        outputScanCodeBuf[i] = asc_map[(size_t)(inputAsciiBuf[i])];
}


