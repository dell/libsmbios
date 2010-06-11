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

#include "smbios_c/cmos.h"
#include "smbios_c/memory.h"
#include "smbios_c/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/token.h"
#include "smbios_c/system_info.h"

#include "dell_magic.h"
#include "sysinfo_impl.h"

#define UP_ANCHOR     "_UP_"
#define UP_ANCHOR_LEN     4

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct up_info
{
    char anchor[4];
    u16  stuff1; // anybody know what this is?
    u8   offset;
    u16  stuff2; // anybody know what this is?
    u8   flag;
}
LIBSMBIOS_C_PACKED_ATTR;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

__hidden bool get_up_offset_and_flag(struct up_info *up)
{
    s64 offset = memory_search( UP_ANCHOR, UP_ANCHOR_LEN,  0xF0000UL, 0xFFFFFUL, 16);
    if (!offset)
        offset = memory_search( UP_ANCHOR, UP_ANCHOR_LEN,  0xF0000UL, 0xFFFFFUL, 1);

    if (offset!=0 && offset!=-1)
        memory_read(up, (u64)offset, sizeof(*up));

    fnprintf("offset 0x%llx", offset);
    return (offset!=0 && offset!=-1);
}

__hidden int up_boot_helper(int flag)
{
    // flag:0 == get
    // flag:1 == set true
    // flag:2 == set false

    // retval = -1: error trying to perform function
    // retval = 0: NO BOOT TO UP CAPABILITY
    // retval = 1 && set; set to value
    // retval = 2 && !set; UP not active
    // retval = 3 && !set; UP Active
    int retval = 0;

    struct up_info up;
    memset(&up, 0, sizeof(up));

    bool found = get_up_offset_and_flag(&up);
    if(!found)
        goto out;

    // instantiate token table to set up checksum observers
    token_table_factory(TOKEN_DEFAULTS);

    // find 0xD4 token
    struct smbios_struct *s = smbios_get_next_struct_by_type(0, 0xD4);
    if(!s)
        goto out_err;

    // copy index/data port
    struct indexed_io_access_structure *d4 = 0;
    d4 = (struct indexed_io_access_structure *)s;
    u32 indexPort = d4->indexPort;
    u32 dataPort = d4->dataPort;

    // read byte
    u8 byte;
    int ret = cmos_read_byte(&byte, indexPort, dataPort, up.offset);
    if (ret<0)
        goto out_err;

    fnprintf("up offset: 0x%04x\n", up.offset);
    fnprintf("up flag  : 0x%04x\n", up.flag);
    fnprintf("read byte: 0x%04x\n", byte);

    if(flag==0) {
        if( (byte & up.flag) == up.flag )
            retval = 3;

        if( (byte & up.flag) != up.flag )
            retval = 2;
    } else {
        byte |= up.flag;
        if(flag==2)
                byte &= ~up.flag;

        ret = cmos_write_byte(byte, indexPort, dataPort, up.offset);
        if (ret<0)
            goto out_err;

        retval = 1;
    }

    goto out;

out_err:
    retval = -1;
    goto out;

out:
    return retval;
}

LIBSMBIOS_C_DLL_SPEC int sysinfo_has_up_boot_flag()
{
    return up_boot_helper(0);
}

LIBSMBIOS_C_DLL_SPEC int sysinfo_get_up_boot_flag()
{
    return up_boot_helper(0) - 2;
}

LIBSMBIOS_C_DLL_SPEC int sysinfo_set_up_boot_flag(int state)
{
    return up_boot_helper( state==1? 1:2 );
}
