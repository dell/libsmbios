// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
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


#ifndef SMBIOSLOWLEVEL_H
#define SMBIOSLOWLEVEL_H

// compat header should always be first header
#include "smbios/compat.h"

#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

/*****************
 * The smbiosLowlevel namespace has the low-level structures
 * that appear in memory.
 * */
namespace smbiosLowlevel
{

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
    struct smbios_structure_header
    {
        u8 type;
        u8 length;
        u16 handle;
    }
    LIBSMBIOS_PACKED_ATTR;

    struct dmi_table_entry_point
    {
        u8 anchor[5];
        u8 checksum;
        u16 table_length;
        u32 table_address;
        u16 table_num_structs;
        u8 smbios_bcd_revision;
    }
    LIBSMBIOS_PACKED_ATTR;

    struct smbios_table_entry_point
    {
        u8 anchor[4];
        u8 checksum;
        u8 eps_length;
        u8 major_ver;
        u8 minor_ver;
        u16 max_struct_size;
        u8 revision;
        u8 formatted_area[5];
        struct dmi_table_entry_point dmi;
        u8 padding_for_Intel_BIOS_bugs;
    } LIBSMBIOS_PACKED_ATTR;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* SMBIOSLOWLEVEL_H */
