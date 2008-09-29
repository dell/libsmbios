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

struct memory
{
    int initialized;
    int (*read_fn)(const struct memory *this, u8 *buffer, u64 offset, size_t length);
    int (*write_fn)(const struct memory *this, u8 *buffer, u64 offset, size_t length);
    void (*free)(struct memory *this);
    void (*cleanup)(struct memory *this); // called instead of ->free for singleton
    void *private_data;
    int close;
};

EXTERN_C_END;

#endif /* MEMORY_IMPL_H */
