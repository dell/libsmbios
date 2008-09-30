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
size_t smbios_fmt_err(struct smbios_table *, char *buf, size_t len);

// for looping/searching
const struct smbios_struct *smbios_get_next_struct(const struct smbios_table *, const struct smbios_struct *cur);
const struct smbios_struct *smbios_get_next_struct_by_type(const struct smbios_table *, const struct smbios_struct *cur, u8 type);
const struct smbios_struct *smbios_get_next_struct_by_handle(const struct smbios_table *, const struct smbios_struct *cur, u16 handle);

// visitor pattern
typedef void (*smbios_walk_fn)(const struct smbios_struct *, void *userdata);
void smbios_walk(smbios_walk_fn, void *userdata);

u8 smbios_struct_get_type(const struct smbios_struct *);
u8 smbios_struct_get_length(const struct smbios_struct *);
u16 smbios_struct_get_handle(const struct smbios_struct *);
int smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len);
const char *smbios_get_string_from_offset(const struct smbios_struct *s, u8 offset);
const char *smbios_get_string_number(const struct smbios_struct *s, u8 which);

#define smbios_for_each_struct(table_name, struct_name)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct(table_name, 0);\
            struct_name;\
            struct_name = smbios_get_next_struct(table_name, struct_name)\
           )

#define smbios_for_each_struct_type(table_name, struct_name, struct_type)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct_by_type(table_name, 0, struct_type);\
            struct_name;\
            struct_name = smbios_get_next_struct_by_type(table_name, struct_name, struct_type)\
           )

EXTERN_C_END;

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* SMBIOS_H */
