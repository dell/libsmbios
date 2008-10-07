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


#ifndef C_SMBIOS_H
#define C_SMBIOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

struct smbios_struct;

// for looping/searching
struct smbios_struct *smbios_get_next_struct(const struct smbios_struct *cur);
struct smbios_struct *smbios_get_next_struct_by_type(const struct smbios_struct *cur, u8 type);
struct smbios_struct *smbios_get_next_struct_by_handle(const struct smbios_struct *cur, u16 handle);

// visitor pattern
void smbios_walk(void (*fn)(const struct smbios_struct *, void *userdata), void *userdata);

// for() loop helpers
#define smbios_for_each_struct(struct_name)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct(0);\
            struct_name;\
            struct_name = smbios_get_next_struct(struct_name)\
           )

#define smbios_for_each_struct_type(struct_name, struct_type)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct_by_type(0, struct_type);\
            struct_name;\
            struct_name = smbios_get_next_struct_by_type(struct_name, struct_type)\
           )

// smbios_struct accessor functions
u8 smbios_struct_get_type(const struct smbios_struct *);
u8 smbios_struct_get_length(const struct smbios_struct *);
u16 smbios_struct_get_handle(const struct smbios_struct *);
int smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len);
const char *smbios_struct_get_string_from_offset(const struct smbios_struct *s, u8 offset);
const char *smbios_struct_get_string_number(const struct smbios_struct *s, u8 which);

EXTERN_C_END;

#endif  /* SMBIOS_H */
