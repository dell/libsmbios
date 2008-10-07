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
#include <stdarg.h>
#include <stdlib.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"

struct cmos_access_obj singleton; // auto-init to 0

struct cmos_access_obj *cmos_obj_factory(int flags, ...)
{
    va_list ap;
    struct cmos_access_obj *toReturn = 0;

    if (flags==CMOS_DEFAULTS)
        flags = CMOS_GET_SINGLETON;

    if (flags & CMOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct cmos_access_obj *)calloc(1, sizeof(struct cmos_access_obj));

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

int  cmos_obj_read_byte(const struct cmos_access_obj *m, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    return m->read_fn(m, byte, indexPort, dataPort, offset);
}

int  cmos_obj_write_byte(const struct cmos_access_obj *m, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    int temp = m->write_fn(m, byte, indexPort, dataPort, offset);
    cmos_obj_run_callbacks(m, true);
    return temp;
}

void __internal _cmos_obj_free(struct cmos_access_obj *m)
{
    struct callback *ptr = m->cb_list_head;
    struct callback *next = 0;

    // free callback list
    while(ptr)
    {
        next = 0;
        if (ptr->next)
            next = ptr->next;

        if (ptr->destructor)
            ptr->destructor(ptr->userdata);
        free(ptr);
        ptr = next;
    }

    m->cb_list_head = 0;

    m->free(m);
}

void cmos_obj_free(struct cmos_access_obj *m)
{
    if (m != &singleton)
        _cmos_obj_free(m);
    else
        m->cleanup(m);
}

void cmos_obj_register_write_callback(struct cmos_access_obj *m, cmos_write_callback cb_fn, void *userdata, void (*destructor)(void *))
{
    struct callback *ptr = m->cb_list_head;
    struct callback *new = 0;
    dprintf("%s\n", __PRETTY_FUNCTION__);

    dprintf("%s - loop\n", __PRETTY_FUNCTION__);
    while(ptr && ptr->next)
    {
        // dont add duplicates
        if (ptr->cb_fn == cb_fn && ptr->userdata == userdata)
            goto out;

        ptr = ptr->next;
    }

    dprintf("%s - allocate\n", __PRETTY_FUNCTION__);
    new = calloc(1, sizeof(struct callback));
    new->cb_fn = cb_fn;
    new->userdata = userdata;
    new->destructor = destructor;
    new->next = 0;

    dprintf("%s - join %p\n", __PRETTY_FUNCTION__, ptr);
    if (ptr)
        ptr->next = new;
    else
        m->cb_list_head = new;

out:
    return;
}

int cmos_obj_run_callbacks(const struct cmos_access_obj *m, bool do_update)
{
    int retval = 0;
    const struct callback *ptr = m->cb_list_head;
    dprintf("%s\n", __PRETTY_FUNCTION__);
    if(!ptr)
        goto out;

    do{
        dprintf("%s - ptr->cb_fn %p\n", __PRETTY_FUNCTION__, ptr->cb_fn);
        retval |= ptr->cb_fn(m, do_update, ptr->userdata);
        ptr = ptr->next;
    } while (ptr);

out:
    return retval;
}

void __internal _init_cmos_std_stuff(struct cmos_access_obj *m)
{
    m->initialized = 1;
    m->cb_list_head = 0;
}
