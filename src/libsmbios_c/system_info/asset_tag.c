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
#include "smbios_c/compat.h"

#include <string.h>
#include <stdlib.h>

#include "smbios_c/obj/token.h"
#include "smbios_c/token.h"
#include "smbios_c/cmos.h"
#include "smbios_c/smbios.h"
#include "smbios_c/smi.h"
#include "smbios_c/system_info.h"
#include "dell_magic.h"
#include "sysinfo_impl.h"


static char *getAssetTagFromSysEncl()
{
    fnprintf("\n");
    return smbios_struct_get_string_from_table(System_Enclosure_or_Chassis_Structure, System_Enclosure_or_Chassis_Asset_Offset);
}

// not static so we can use it in unit test, but not part of public API.
// you have been warned.
char *getAssetTagFromToken()
{
    const struct smbios_struct *s;
    char *tag = 0;
    u16 indexPort, dataPort;
    u8  location;
    u8 csum = 0;
    u8 byte;
    int ret;

    fnprintf("\n");

    // Step 1: Get tag from CMOS
    fnprintf("- get string\n");
    size_t len = 0;
    tag = token_get_string(Cmos_Asset_Token, &len);  // allocates mem
    if (!tag)
        goto out_err;

    // Step 3: Make sure checksum is good before returning value
    fnprintf("- csum\n");
    s = token_get_smbios_struct(Cmos_Asset_Token);
    indexPort = ((struct indexed_io_access_structure*)s)->indexPort;
    dataPort = ((struct indexed_io_access_structure*)s)->dataPort;
    location = ((struct indexed_io_token *)token_get_ptr(Cmos_Asset_Token))->location;

    // calc checksum
    for( u32 i = 0; i < ASSET_TAG_CMOS_LEN_MAX; i++)
    {
        ret = cmos_read_byte(&byte, indexPort, dataPort, location + i);
        if (ret<0)
            goto out_err;

        csum += byte;
    }

    // get checksum byte
    ret = cmos_read_byte(&byte, indexPort, dataPort, location + ASSET_TAG_CMOS_LEN_MAX);
    if (ret<0)
        goto out_err;

    fnprintf("- got: %x  calc: %x\n", csum, byte);
    if ((u8)(csum + byte)) // bad (should be zero)
        goto out_err;

    fnprintf("GOT CMOS TAG: %s\n", tag);
    fnprintf("- out\n");

    return tag;

out_err:
    fnprintf("- out_err\n");
    free(tag);
    tag = 0;
    return NULL;
}

static char *getAssetTagFromSMI()
{
    fnprintf("\n");
    return getTagFromSMI( 0 ); /* Read asset tag select code */
}

// Code for getting the asset tag from one of many locations
/* try dynamic functions first to make sure we get current data. */
static struct DellAssetTagFunctions
{
    char *(*f_ptr)();
} DellAssetTagFunctions[] = {
                              {&getAssetTagFromSysEncl,}, // SMBIOS System Information Item
                              {&getAssetTagFromToken,},   // SMBIOS CMOS Token
                              {&getAssetTagFromSMI,},     // SMI
                          };

LIBSMBIOS_C_DLL_SPEC char *sysinfo_get_asset_tag()
{
    char *assetTag = 0;
    int numEntries =
        sizeof (DellAssetTagFunctions) / sizeof (DellAssetTagFunctions[0]);

    sysinfo_clearerr();
    fnprintf("\n");
    for (int i = 0; (i < numEntries) && (!assetTag); ++i)
    {
        fnprintf("Call fn pointer %p\n", DellAssetTagFunctions[i].f_ptr);
        // first function to return non-zero id with strlen()>0 wins.
        assetTag = DellAssetTagFunctions[i].f_ptr ();
        fnprintf("got result: %p\n", assetTag);
        if (assetTag)
        {
            strip_trailing_whitespace(assetTag);
            if (!strlen(assetTag))
            {
                fnprintf("string is zero len, returning as not specified\n");
                /*
                 * In case one of the function returns an empty string (zero len),
                 * we would be returning the value "Not Specified" to the caller.
                 */
                assetTag = realloc(assetTag, ASSET_TAG_NOT_SPECIFIED_LEN);
                if (assetTag)
                    strncpy(assetTag, ASSET_TAG_NOT_SPECIFIED, ASSET_TAG_NOT_SPECIFIED_LEN - 1);
                goto out;
            }
        }
    }

out:
    return assetTag;
}


//
// SET FUNCTIONS
//

static int setAssetTagUsingCMOSToken(const char *newTag, const char *pass_ascii, const char *pass_scancode)
{
    const struct smbios_struct *s;
    u16 indexPort, dataPort;
    u8  location, csum = 0, byte;
    int retval = -1, ret;

    UNREFERENCED_PARAMETER(pass_ascii);
    UNREFERENCED_PARAMETER(pass_scancode);
    fnprintf("\n");

    // Step 1: write tag to CMOS
    fnprintf("- set string\n");
    ret = token_set_string(Cmos_Asset_Token, newTag, strlen(newTag));
    if (ret)
        goto out;

    // Step 2: Write checksum
    fnprintf("- csum\n");
    s = token_get_smbios_struct(Cmos_Asset_Token);
    indexPort = ((struct indexed_io_access_structure*)s)->indexPort;
    dataPort = ((struct indexed_io_access_structure*)s)->dataPort;
    location = ((struct indexed_io_token *)token_get_ptr(Cmos_Asset_Token))->location;

    // calc checksum
    for( u32 i = 0; i < ASSET_TAG_CMOS_LEN_MAX; i++)
    {
        ret = cmos_read_byte(&byte, indexPort, dataPort, location + i);
        if (ret)
            goto out;

        csum += byte;
    }

    // write checksum byte
    ret = cmos_write_byte(~csum + 1, indexPort, dataPort, location + ASSET_TAG_CMOS_LEN_MAX);
    if (ret<0)
        goto out;

    retval = 0;

out:
    fnprintf("- out\n");
    return retval;
}


static int setAssetTagUsingSMI(const char *newTag, const char *pass_ascii, const char *pass_scancode)
{
    int retval = 0, ret;
    u16 security_key = 0;
    const char *whichpw = pass_scancode;
    if (dell_smi_password_format(DELL_SMI_PASSWORD_ADMIN) == DELL_SMI_PASSWORD_FMT_ASCII)
        whichpw=pass_ascii;
    ret = dell_smi_get_security_key(whichpw, &security_key);
    retval = -2;
    if (ret)  // bad password
        goto out;

    ret = setTagUsingSMI( 1, newTag, security_key); /* Write asset tag select code */
    retval = -1;
    if (ret)
        goto out;

    retval = 0;

out:
    return retval;
}

// Code for getting the service tag from one of many locations
static struct DellSetAssetTagFunctions
{
    int (*f_ptr)(const char *, const char *, const char *);
} DellSetAssetTagFunctions[] = {
                                 {&setAssetTagUsingSMI},   // SMBIOS System Information Item
                                 {&setAssetTagUsingCMOSToken},   // SMBIOS System Information Item
                             };

LIBSMBIOS_C_DLL_SPEC int sysinfo_set_asset_tag(const char *assetTag, const char *pass_ascii, const char *pass_scancode)
{
    int ret = -1;
    int numEntries = sizeof (DellSetAssetTagFunctions) / sizeof (DellSetAssetTagFunctions[0]);

    sysinfo_clearerr();
    fnprintf("\n");
    for (int i = 0; (i < numEntries) && (ret != 0); ++i)
    {
        fnprintf("Call fn pointer %p\n", DellSetAssetTagFunctions[i].f_ptr);
        // first function to return non-zero id with strlen()>0 wins.
        ret = DellSetAssetTagFunctions[i].f_ptr (assetTag, pass_ascii, pass_scancode);
    }
    return ret;
}



