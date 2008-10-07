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

#ifndef MEMORY_IMPL_H
#define MEMORY_IMPL_H

#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#if defined(DEBUG_MEMORY_C)
#   include <stdio.h>
#   undef dbg_printf
#   define dbg_printf _dbg_printf
#endif

struct memory_access_obj
{
    int initialized;
    int (*read_fn)(const struct memory_access_obj *this, u8 *buffer, u64 offset, size_t length);
    int (*write_fn)(const struct memory_access_obj *this, u8 *buffer, u64 offset, size_t length);
    void (*free)(struct memory_access_obj *this);
    void (*cleanup)(struct memory_access_obj *this); // called instead of ->free for singleton
    void *private_data;
    int close;
};

void __internal init_mem_struct(struct memory_access_obj *m);
void __internal init_mem_struct_filename(struct memory_access_obj *m, const char *fn);

EXTERN_C_END;

#endif /* MEMORY_IMPL_H */
