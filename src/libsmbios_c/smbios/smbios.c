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

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system
#include <stdlib.h>
#include <string.h>

// public
#include "smbios_c/memory.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"

// private
#include "smbios_impl.h"

// forward declarations

// visitor pattern
void smbios_walk(void (*smbios_walk_fn)(const struct smbios_struct *, void *userdata), void *userdata)
{
    struct smbios_table *table = smbios_table_factory(SMBIOS_DEFAULTS);
    smbios_table_walk(table, smbios_walk_fn, userdata);
    smbios_table_free(table);
}

// for looping/searching
struct smbios_struct *smbios_get_next_struct(const struct smbios_struct *cur)
{
    struct smbios_table *table = smbios_table_factory(SMBIOS_DEFAULTS);
    struct smbios_struct *ret = smbios_table_get_next_struct(table, cur);
    smbios_table_free(table);
    return ret;
}

struct smbios_struct *smbios_get_next_struct_by_type(const struct smbios_struct *cur, u8 type)
{
    struct smbios_table *table = smbios_table_factory(SMBIOS_DEFAULTS);
    struct smbios_struct *ret = smbios_table_get_next_struct_by_type(table, cur, type);
    smbios_table_free(table);
    return ret;
}

struct smbios_struct *smbios_get_next_struct_by_handle(const struct smbios_struct *cur, u16 handle)
{
    struct smbios_table *table = smbios_table_factory(SMBIOS_DEFAULTS);
    struct smbios_struct *ret = smbios_table_get_next_struct_by_handle(table, cur, handle);
    smbios_table_free(table);
    return ret;
}

const char *smbios_strerror(const struct smbios_struct *cur)
{
    char *ret;
    struct smbios_table *table = smbios_table_factory(SMBIOS_DEFAULTS | SMBIOS_NO_ERR_CLEAR);
    if (table) {
        /* leak */
        ret = strdup(smbios_table_strerror(table));
        smbios_table_free(table);
    } else {
        ret = "";
    }
    return ret;
}
