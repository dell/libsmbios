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


#ifndef C_OBJ_SMBIOS_H
#define C_OBJ_SMBIOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

// abi_prefix should be last header included before declarations
#include "smbios_c/config/abi_prefix.h"

EXTERN_C_BEGIN;

#define SMBIOS_DEFAULTS       0x0000
#define SMBIOS_GET_SINGLETON  0x0001
#define SMBIOS_GET_NEW        0x0002
#define SMBIOS_UNIT_TEST_MODE 0x0004

struct smbios_table;
struct smbios_struct;

// construct
struct smbios_table *smbios_table_factory(int flags, ...);

// destruct
void smbios_table_free(struct smbios_table *);

// not yet implemented
//// format error string
//size_t smbios_fmt_err(struct smbios_table *, char *buf, size_t len);

// visitor pattern
void smbios_table_walk(struct smbios_table *, void (*smbios_table_walk_fn)(const struct smbios_struct *, void *userdata), void *userdata);

// for looping/searching
struct smbios_struct *smbios_table_get_next_struct(const struct smbios_table *, const struct smbios_struct *cur);
struct smbios_struct *smbios_table_get_next_struct_by_type(const struct smbios_table *, const struct smbios_struct *cur, u8 type);
struct smbios_struct *smbios_table_get_next_struct_by_handle(const struct smbios_table *, const struct smbios_struct *cur, u16 handle);

#define smbios_table_for_each_struct(table_name, struct_name)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_table_get_next_struct(table_name, 0);\
            struct_name;\
            struct_name = smbios_table_get_next_struct(table_name, struct_name)\
           )

#define smbios_table_for_each_struct_type(table_name, struct_name, struct_type)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_table_get_next_struct_by_type(table_name, 0, struct_type);\
            struct_name;\
            struct_name = smbios_table_get_next_struct_by_type(table_name, struct_name, struct_type)\
           )

EXTERN_C_END;

// always should be last thing in header file
#include "smbios_c/config/abi_suffix.h"

#endif  /* SMBIOS_H */
