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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"

void __internal _do_callbacks(const struct cmos_obj *);

struct cmos_obj singleton; // auto-init to 0

struct cmos_obj *cmos_factory(int flags, ...)
{
    va_list ap;
    struct cmos_obj *toReturn = 0;

    if (flags==CMOS_DEFAULTS)
        flags = CMOS_GET_SINGLETON;
            
    if (flags & CMOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct cmos_obj *)calloc(1, sizeof(struct cmos_obj));

    if (toReturn->initialized)
        goto out;

    if (flags & CMOS_UNIT_TEST_MODE)
    {
        va_start(ap, flags);
        init_cmos_struct_filename(toReturn, va_arg(ap, const char *));
        va_end(ap);
    } else 
    {
        init_cmos_struct(toReturn);
    }

out:
    return toReturn;
}

int  cmos_read_byte(const struct cmos_obj *m, u32 indexPort, u32 dataPort, u32 offset, u8 *byte)
{
    return m->read_fn(m, indexPort, dataPort, offset, byte);
}

int  cmos_write_byte(const struct cmos_obj *m, u32 indexPort, u32 dataPort, u32 offset, u8 byte)
{
    int temp = m->write_fn(m, indexPort, dataPort, offset, byte);
    _do_callbacks(m);
    return temp;
}

void cmos_obj_free(struct cmos_obj *m)
{
    if (m != &singleton)
        m->free(m);
    else
        m->cleanup(m);
}

void register_write_callback(struct cmos_obj *m, cmos_write_callback cb_fn, void *userdata)
{
    struct callback *ptr = &(m->cb_list_head);
    struct callback *new = 0;

    while(ptr->next)
    {
        // dont add duplicates
        if (ptr->cb_fn == cb_fn && ptr->userdata == userdata)
            goto out;

        ptr = ptr->next;
    }

    new = calloc(1, sizeof(struct callback));
    new->cb_fn = cb_fn;
    new->userdata = userdata;
    new->next = 0;
    ptr->next = new;

out:
    return;
}

void __internal _do_callbacks(const struct cmos_obj *m)
{
    for(const struct callback *ptr = &(m->cb_list_head); ptr->next; ptr = ptr->next)
        if(ptr->cb_fn)
            ptr->cb_fn(m, ptr->userdata);
}

void __internal _init_cmos_std_stuff(struct cmos_obj *m)
{
    m->initialized = 1;
    m->cb_list_head.cb_fn = 0;
    m->cb_list_head.userdata = 0;
    m->cb_list_head.next = 0;
}
