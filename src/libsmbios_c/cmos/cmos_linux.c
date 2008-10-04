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

#define LIBSMBIOS_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system
#include <sys/io.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"

static int linux_read_fn(const struct cmos_access_obj *this, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    if(iopl(3) < 0)
        return -1;

    outb_p (offset, indexPort);
    return (inb_p (dataPort));
}

static int linux_write_fn(const struct cmos_access_obj *this, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    if(iopl(3) < 0)
        return -1;

    outb_p (offset, indexPort);
    outb_p (byte, dataPort);
    return 0;
}

static void linux_free(struct cmos_access_obj *this)
{
}

static void linux_cleanup(struct cmos_access_obj *this)
{
}

void __internal init_cmos_struct(struct cmos_access_obj *m)
{
    m->private_data = 0;
    m->read_fn = linux_read_fn;
    m->write_fn = linux_write_fn;
    m->free = linux_free;
    m->cleanup = linux_cleanup;

    _init_cmos_std_stuff(m);
}
