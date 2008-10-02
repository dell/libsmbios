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

#if defined(DEBUG_CMOS_C)
#   include <stdio.h>
#   define dprintf(format, args...) do { fprintf(stderr , format , ## args);  } while(0)
#else
#   define dprintf(format, args...) do {} while(0)
#endif

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))

struct callback
{
    cmos_write_callback cb_fn;
    void *userdata;
    struct callback *next;
};

struct cmos_obj
{
    int initialized;
    int (*read_fn)(const struct cmos_obj *m, u8 *byte, u32 indexPort, u32 dataPort, u32 offset);
    int (*write_fn)(const struct cmos_obj *m, u8 byte, u32 indexPort, u32 dataPort, u32 offset);
    void (*free)(struct cmos_obj *this);
    void (*cleanup)(struct cmos_obj *this); // called instead of ->free for singleton
    struct callback cb_list_head;
    void *private_data;
};

// regular one
void __internal init_cmos_struct(struct cmos_obj *m);
void __internal _init_cmos_std_stuff(struct cmos_obj *m);  // base class constructor

// unit test one
void __internal init_cmos_struct_filename(struct cmos_obj *m, const char *fn);

EXTERN_C_END;

#endif /* CMOS_IMPL_H */
