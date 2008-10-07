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

//
//
// ID Byte functions
//
//
__internal u16 get_id_byte_from_mem ()
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


__internal u16 get_id_byte_from_mem_diamond()
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


__internal u16 getIdByteFromOEMItem ()
{
    u16 idWord = 0;
    // search through 0x0B (OEM_Strings_Structure) items
    smbios_for_each_struct_type( s, OEM_Strings ) {
        const char *str = smbios_struct_get_string_number(s, OEM_String_Field_Number);
        if ((!str) && (0 != strncmp (str, Bayonet_Detect_String, strlen(Bayonet_Detect_String))))
            continue;

        //  Id byte is in second string in table 0x0B
        //  the format is "1[NN]", where NN is the idbyte;
        //  note the &str[2] below to skip the 'n['
        str = smbios_struct_get_string_number(s, 2);
        if(str && strlen(str) > 3 && str[0] == '1' && str[1] == '[')
            idWord = strtol( &str[2], NULL, 16 );

        if (idWord)
            break;
    }

    return idWord;
}


__internal u16 get_id_byte_from_rev_and_id_structure ()
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

//The code for detecting ID byte in case of Diamond is left out.
//  need to write a function for it.
struct DellIdByteFunctions
{
    u16 (*f_ptr)();
}
DellIdByteFunctions[] = {
                            {&get_id_byte_from_mem,},       // normal system -- try this last always.


                            {&getIdByteFromOEMItem,},   // bayonet
                            {&get_id_byte_from_mem_diamond,}, // diamond

                            // do this last because this may contain an OEM id
                            // do this as a last resort because it is
                            // unreliable.
                            {&get_id_byte_from_rev_and_id_structure,},   // Dell Smbios Revisions and ID's struct
                        };




int sysinfo_get_dell_system_id()
{
    int systemId = 0;
    int numEntries =
        sizeof (DellIdByteFunctions) / sizeof (DellIdByteFunctions[0]);

    for (int i = 0; i < numEntries; ++i)
    {
        // first function to return non-zero id wins.
        systemId = DellIdByteFunctions[i].f_ptr ();

        if (systemId)
            break;
    }

    return systemId;
}

