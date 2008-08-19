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

struct cmos
{
    int initialized;
    int (*read_fn)(struct cmos *m, u32 indexPort, u32 dataPort, u32 offset, u8 *byte);
    int (*write_fn)(struct cmos *m, u32 indexPort, u32 dataPort, u32 offset, u8 byte);
    void (*free)(struct cmos *this);
    void (*cleanup)(struct cmos *this); // called instead of ->free for singleton
    void *private_data;
    int close;
};

EXTERN_C_END;

#endif /* CMOS_IMPL_H */
