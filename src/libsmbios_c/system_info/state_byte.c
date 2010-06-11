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
#include "smbios_c/token.h"
#include "smbios_c/smbios.h"

#include "dell_magic.h"
#include "sysinfo_impl.h"

LIBSMBIOS_C_DLL_SPEC int sysinfo_has_nvram_state_bytes()
{
    int retval = 1;
    if ( 0xD4 != token_get_type( NvramByte1_Token )  )
        retval = 0;
    if ( 0xD4 != token_get_type( NvramByte2_Token )  )
        retval = 0;
    return retval;
}


// user =
//      0x0000 = DSA
//      0x8000 = OM Toolkit
//      0x9000 = open
//      0xA000 = open
//      0xB000 = open
//      0xC000 = open
//      0xD000 = open
//      0xE000 = open
//      0xF000 = expand to whole byte
LIBSMBIOS_C_DLL_SPEC int sysinfo_get_nvram_state_bytes( int user )
{
    char *string;
    int retval = 0;

    string = token_get_string( NvramByte1_Token, 0 );
    if (string)
        retval = ((u8 *)string)[0];
    free(string);

    string = token_get_string( NvramByte2_Token, 0 );
    if (string)
        retval |= ((u8 *)string)[0] << 8;
    free(string);

    if( user == 0x0000 )  // DSA
    {
        if( (retval & 0x8000) != user )
        {
            retval = 0;  // user doesn't match, return default
        }
        retval &= ~0x8000; // mask user bits
    }
    else
    {
        if ((user & 0xF000) == 0xF000 ) // probably will never be used
        {
            if( (retval & 0xFF00) != user )
            {
                retval = 0;// user doesn't match, return default
            }
            retval &= ~0xFF00; // mask user bits
        }
        else
        {
            if( (retval & 0xF000) != user ) // Toolkit (or users 0x9 - 0xE)
            {
                retval = 0;// user doesn't match, return default
            }
            retval &= ~0xF000; // mask user bits
        }
    }
    return retval;
}

LIBSMBIOS_C_DLL_SPEC void sysinfo_set_nvram_state_bytes(int user, int value)
{
    if ( user == 0x0000 ) // DSA
    {
        value &= ~0x8000;  // mask user bits
        value |= user;     // set user
    }
    else if( (user & 0xF000) == 0xF000 )
    {
        value &= ~0xFF00;   // mask user bits
        value |= user;      // set user
    }
    else
    {
        value &= ~0xF000;   // mask user bits
        value |= user;      // set user
    }

    char *tempdata = (char*)(&value);
    token_set_string( NvramByte1_Token, tempdata, 1);
    token_set_string( NvramByte2_Token, tempdata + 1, 1);

    return;
}



