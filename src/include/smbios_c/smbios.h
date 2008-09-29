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


#ifndef SMBIOS_H
#define SMBIOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

EXTERN_C_BEGIN;

#define SMBIOS_DEFAULTS       0x0000
#define SMBIOS_GET_SINGLETON  0x0001
#define SMBIOS_GET_NEW        0x0002
#define SMBIOS_UNIT_TEST_MODE 0x0004

struct smbios_table;
struct smbios_struct;

// construct
struct smbios_table *smbios_factory(int flags, ...);

// destruct
void smbios_free(struct smbios_table *);

// format error string
size_t smbios_fmt_err(struct memory *, char *buf, size_t len);

// for looping/searching
const struct smbios_struct *smbios_get_next_struct(const struct smbios_struct *cur);
const struct smbios_struct *smbios_get_next_struct_bytype(const struct smbios_struct *cur, u8 type);
const struct smbios_struct *smbios_get_next_struct_byhandle(const struct smbios_struct *cur, u16 handle);

// visitor pattern
typedef void (smbios_walk_fn)(struct smbios_struct *);
void smbios_walk(smbios_walk_fn);

u8 smbios_struct_get_type(struct smbios_struct *);
u8 smbios_struct_get_length(struct smbios_struct *);
u16 smbios_struct_get_handle(struct smbios_struct *);
int smbios_struct_get_data(struct smbios_struct *s, void *dest, u8 offset, size_t len);
const char *smbios_get_string(struct smbios_struct *s, u8 offset);


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




EXTERN_C_END;

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* SMBIOS_H */
