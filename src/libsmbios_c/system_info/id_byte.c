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

#include "smbios_c/system_info.h"
#include "smbios_c/memory.h"
#include "smbios_c/smbios.h"

#include "dell_magic.h"
#include "sysinfo_impl.h"

//
//
// ID Byte functions
//
//
__hidden u16 get_id_byte_from_mem ()
{
    u16 tempWord = 0;
    u16 idWord = 0;
    char strBuf[DELL_SYSTEM_STRING_LEN] = { 0, };
    int ret;

    struct two_byte_structure tbs;

    // Step 1: Check that "Dell System" is present at the proper offset
    ret = memory_read(strBuf, DELL_SYSTEM_STRING_LOC, DELL_SYSTEM_STRING_LEN-1);
    if (ret<0) goto out;

    if( strncmp( strBuf, DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) != 0 )
        goto out;

    // Step 2: fill the id structs
    ret = memory_read(&tbs, TWO_BYTE_STRUCT_LOC, sizeof(struct two_byte_structure) );
    if (ret<0) goto out;

    // Step 3: check the checksum of one-byte struct
    //    update: checksum is not reliable, so don't use it...

    // Step 4: Check one byte ID
    tempWord = tbs.system_id;

    // Step 5: if 0xFE, then it is a double byte (word) ID.
    // *  -- byte at 0xFE845 is 0xFE
    if (0xFE == tempWord)
    {
        // Step 6: check two byte struct checksum
        // *  -- three bytes at 0xFE845 sum to 0x00 but are not all 0x00.
        //*  -- extension checksum is 0

        // Step 7: get ID.
        tempWord = tbs.two_byte_id;
    }

    idWord = tempWord;

out:
    return idWord;
}


__hidden u16 get_id_byte_from_mem_diamond()
{
    u16 idWord = 0;
    char strBuf[DELL_SYSTEM_STRING_LEN] = { 0, };
    int ret;

    // Step 1: Check that "Dell System" is present at the proper offset
    ret = memory_read(strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_1, DELL_SYSTEM_STRING_LEN - 1);

    if( ret>=0 && strncmp( strBuf, DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 )
    {
        u8 idByte = 0;
        ret = memory_read(&idByte, ID_BYTE_LOC_DIAMOND_1, sizeof(idByte));
        if( ret>=0 && SYSTEM_ID_DIAMOND == idByte )
        {
            idWord = SYSTEM_ID_DIAMOND;
            goto out;
        }
    }

    ret = memory_read(strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_2, DELL_SYSTEM_STRING_LEN - 1);
    if( (ret>=0) && (strncmp( strBuf, DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 ))
    {
        u8 idByte = 0;
        ret = memory_read(&idByte, ID_BYTE_LOC_DIAMOND_2, sizeof(idByte));
        if( ret>=0 && SYSTEM_ID_DIAMOND == idByte )
        {
            idWord = SYSTEM_ID_DIAMOND;
            goto out;
        }
    }

out:
    return idWord;
}


__hidden const char * get_dell_oem_string_by_tag (int tag)
{
    // search through 0x0B (OEM_Strings_Structure) items
    smbios_for_each_struct_type( s, OEM_Strings ) {
        // first string must be "Dell System" per spec
        const char *str = smbios_struct_get_string_number(s, 1);
        if ((!str) || (0 != strncmp (str, DELL_SYSTEM_STRING, strlen(DELL_SYSTEM_STRING)))) {
            str = 0;
            continue;
        }

        int i=2; // start searching string table from second string (first was searched above)
        while ( (str = smbios_struct_get_string_number(s, i++)) ){
            char *endptr = 0;
            long strtag = strtol(str, &endptr, 10);
            if(strlen(str) > 3 && strtag == tag && endptr[0] == '[')
                return str;
        }
    }
    return 0;
}

__hidden u16 get_dell_id_byte_from_oem_item ()
{
    u16 idWord = 0;
    // Tag # for oem string table Dell ID tag is '1'
    // see docs for dell oem strings table (0x0b)
    const char *str = get_dell_oem_string_by_tag(1);

    //  Id byte is in second string in table 0x0B
    //  the format is "1[NN]", where NN is the idbyte;
    //  note the &str[2] below to skip the 'n['
    if(str && strlen(str) > 3 && str[0] == '1' && str[1] == '[')
        idWord = strtol( &str[2], NULL, 16 );

    return idWord;
}

__hidden u16 get_oem_id_byte_from_oem_item ()
{
    u16 idWord = 0;
    // Tag # for oem string table reseller ID tag is '7'
    // see docs for dell oem strings table (0x0b)
    const char *str = get_dell_oem_string_by_tag(7);

    //  Id byte is in second string in table 0x0B
    //  the format is "1[NN]", where NN is the idbyte;
    //  note the &str[2] below to skip the 'n['
    if(str && strlen(str) > 3 && str[0] == '7' && str[1] == '[')
        idWord = strtol( &str[2], NULL, 16 );

    return idWord;
}

__hidden u16 get_id_byte_from_rev_and_id_structure ()
{
    u16 idWord = 0;
    // search through 0x0B (Revisions_and_IDs_Structure)
    smbios_for_each_struct_type( s, Dell_Revisions_and_IDs ) {
        //If byte field is 0xFE, we need to look in the extension field
        smbios_struct_get_data(s, &idWord, 0x06, 1);
        if( 0xFE == idWord )
        {
            smbios_struct_get_data(s, &idWord, 0x08, 2);
        }
    }
    return idWord;
}

static struct DellIdByteFunctions
{
    const char *const name;
    u16 (*f_ptr)();
}
DellIdByteFunctions[] = {
        {"get_id_byte_from_mem_diamond", &get_id_byte_from_mem_diamond,},
        {"get_dell_id_byte_from_oem_item", &get_dell_id_byte_from_oem_item,},  // 0x0b structure, tag 1
        {"get_id_byte_from_rev_and_id_structure", &get_id_byte_from_rev_and_id_structure,}, // 0xd0 structure
        {"get_id_byte_from_mem", &get_id_byte_from_mem,},
    };

static struct DellIdByteFunctions
DellOemIdByteFunctions[] = {
        {"get_id_byte_from_mem_diamond", &get_id_byte_from_mem_diamond,}, // diamond is old and never oem, so this is safe
        // try to get OEM ID first
        {"get_oem_id_byte_from_oem_item", &get_oem_id_byte_from_oem_item,},  // 0x0b structure, tag 7
        // on older systems, the id in the rev and id structure got replaced with OEM ID
        {"get_id_byte_from_rev_and_id_structure", &get_id_byte_from_rev_and_id_structure,}, // 0xd0 structure
        // on older systems, the id in mem structure got replaced with OEM ID
        {"get_id_byte_from_mem", &get_id_byte_from_mem,},
        // then fall back to Dell ID if OEM ID not present
        // This will never hit, as one of the previous two functions will always
        // return a value before we get to here. But better safe than sorry
        {"get_dell_id_byte_from_oem_item", &get_oem_id_byte_from_oem_item,},  // 0x0b structure, tag 1
    };


LIBSMBIOS_C_DLL_SPEC int sysinfo_get_dell_oem_system_id()
{
    int systemId = 0;
    int numEntries =
        sizeof (DellOemIdByteFunctions) / sizeof (DellOemIdByteFunctions[0]);

    sysinfo_clearerr();
    for (int i = 0; i < numEntries; ++i)
    {
        fnprintf("calling id_byte function: %s\n", DellIdByteFunctions[i].name);
        // first function to return non-zero id wins.
        systemId = DellOemIdByteFunctions[i].f_ptr ();

        if (systemId)
            break;
    }

    return systemId;
}

LIBSMBIOS_C_DLL_SPEC int sysinfo_get_dell_system_id()
{
    int systemId = 0;
    int numEntries =
        sizeof (DellIdByteFunctions) / sizeof (DellIdByteFunctions[0]);

    sysinfo_clearerr();
    for (int i = 0; i < numEntries; ++i)
    {
        fnprintf("calling id_byte function: %s\n", DellIdByteFunctions[i].name);
        // first function to return non-zero id wins.
        systemId = DellIdByteFunctions[i].f_ptr ();

        if (systemId)
            break;
    }

    return systemId;
}

