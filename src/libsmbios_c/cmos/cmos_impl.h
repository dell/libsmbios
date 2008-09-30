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

#ifndef CMOS_IMPL_H
#define CMOS_IMPL_H

#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))

struct cmos_obj
{
    int initialized;
    int (*read_fn)(const struct cmos_obj *m, u32 indexPort, u32 dataPort, u32 offset, u8 *byte);
    int (*write_fn)(const struct cmos_obj *m, u32 indexPort, u32 dataPort, u32 offset, u8 byte);
    void (*free)(struct cmos_obj *this);
    void (*cleanup)(struct cmos_obj *this); // called instead of ->free for singleton
    void *private_data;
};

// regular one
void __internal init_cmos_struct(struct cmos_obj *m);

// unit test one
void __internal init_cmos_struct_filename(struct cmos_obj *m, const char *fn);

EXTERN_C_END;

#endif /* CMOS_IMPL_H */
