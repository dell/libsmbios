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

#include "smbios_c/cmos.h"
#include "smbios_c/obj/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"


int  cmos_read_byte(u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    struct cmos_access_obj *c = cmos_obj_factory(CMOS_GET_SINGLETON);
    int retval = cmos_obj_read_byte(c, byte, indexPort, dataPort, offset);
    cmos_obj_free(c);
    return retval;
}

int  cmos_write_byte(u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    struct cmos_access_obj *c = cmos_obj_factory(CMOS_GET_SINGLETON);
    int retval = cmos_obj_write_byte(c, byte, indexPort, dataPort, offset);
    cmos_obj_free(c);
    return retval;
}

int cmos_run_callbacks(bool do_update)
{
    struct cmos_access_obj *c = cmos_obj_factory(CMOS_GET_SINGLETON);
    int retval = cmos_obj_run_callbacks(c, do_update);
    cmos_obj_free(c);
    return retval;
}

const char * cmos_strerror()
{
    struct cmos_access_obj *m = cmos_obj_factory(CMOS_GET_SINGLETON | CMOS_NO_ERR_CLEAR);
    const char * retval = cmos_obj_strerror(m);
    cmos_obj_free(m);
    return retval;
}

