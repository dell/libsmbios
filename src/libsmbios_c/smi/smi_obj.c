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
#include <string.h>

// public
#include "smbios_c/obj/smi.h"
#include "smbios_c/smi.h"
#include "smbios_c/types.h"

// private
#include "smi_impl.h"

// forward declarations
void _smi_free(struct dell_smi_obj *m);

// static vars
static struct dell_smi_obj singleton; // auto-init to 0
typedef int (*init_fn)(struct dell_smi_obj *);

struct dell_smi_obj *dell_smi_factory(int flags, ...)
{
    va_list ap;
    struct dell_smi_obj *toReturn = 0;

    fnprintf("\n");

    if (flags==DELL_SMI_DEFAULTS)
        flags = DELL_SMI_GET_SINGLETON;

    if (flags & DELL_SMI_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct dell_smi_obj *)calloc(1, sizeof(struct dell_smi_obj));

    if (toReturn->initialized)
        goto out;

    if (flags & DELL_SMI_UNIT_TEST_MODE)
    {
        va_start(ap, flags);
        init_fn fn = va_arg(ap, init_fn);
        fnprintf("call fn pointer: %p\n", fn);
        fn(toReturn);
        va_end(ap);
    } else
    {
        fnprintf("default init\n");
        init_dell_smi_obj(toReturn);
    }

    init_dell_smi_obj_std(toReturn);
out:
    return toReturn;
}


void dell_smi_obj_free(struct dell_smi_obj *m)
{
    fnprintf("\n");
    if (m != &singleton)
        _smi_free(m);

    // can do special cleanup for singleton, but none necessary atm
}

void dell_smi_obj_set_class(struct dell_smi_obj *this, u16 smi_class)
{
    fnprintf("\n");
    this->smi_class = smi_class;
}

void dell_smi_obj_set_select(struct dell_smi_obj *this, u16 smi_select)
{
    fnprintf("\n");
    this->smi_select = smi_select;
}

void dell_smi_obj_set_arg(struct dell_smi_obj *this, u8 argno, u32 value)
{
    fnprintf("\n");
    free(this->physical_buffers[argno]);
    this->physical_buffers[argno] = 0;

    this->arg[argno] = value;
}

u32  dell_smi_obj_get_res(struct dell_smi_obj *this, u8 argno)
{
    fnprintf("\n");
    return this->res[argno];
}

u8  *dell_smi_obj_make_buffer(struct dell_smi_obj *this, u8 argno, size_t size)
{
    fnprintf("\n");
    if (argno>3)
        return 0;

    free(this->physical_buffers[argno]);
    this->physical_buffers[argno] = calloc(1, size);
    return this->physical_buffers[argno];
}

void dell_smi_obj_execute(struct dell_smi_obj *this)
{
    fnprintf("\n");
    this->execute(this);
}

/**************************************************
 *
 * Internal functions
 *
 **************************************************/

void __internal _smi_free(struct dell_smi_obj *this)
{
    fnprintf("\n");
    this->initialized=0;
    for (int i=0;i<4;++i)
    {
        free(this->physical_buffers[i]);
        this->physical_buffers[i]=0;
    }
    free(this);
}

void __internal init_dell_smi_obj_std(struct dell_smi_obj *this)
{
    fnprintf("\n");
    this->initialized = 1;
    this->res[0] = -3; //default to 'not handled'
}



