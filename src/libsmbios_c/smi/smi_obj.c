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
#include <stdlib.h>
#include <string.h>

// public
#include "smbios_c/obj/smi.h"
#include "smbios_c/smi.h"
#include "smbios_c/types.h"

// private
#include "smi_impl.h"

// forward declarations

// static vars
static struct dell_smi_obj dell_smi singleton; // auto-init to 0

struct dell_smi_obj *dell_smi_factory(int flags, ...)
{
    struct dell_smi_obj dell_smi *toReturn = 0;

    dprintf("DEBUG: dell_smi_factory()\n");

    if (flags==SMBIOS_DEFAULTS)
        flags = SMBIOS_GET_SINGLETON;

    if (flags & SMBIOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct dell_smi_obj *)calloc(1, sizeof(struct dell_smi_obj));

    if (toReturn->initialized)
        goto out;

    _ini_smi(toReturn);

out:
    return toReturn;
}


void dell_smi_obj_free(struct dell_smi_obj *m)
{
    if (m != &singleton)
        _smi_free(m);

    // can do special cleanup for singleton, but none necessary atm
}


/**************************************************
 *
 * Internal functions
 *
 **************************************************/

void __internal _smi_free(struct dell_smi_obj *this)
{
    this->initialized=0;
    free(this);
}

void __internal _init_smi(struct dell_smi_obj *m)
{
    fnprintf("\n");
    m->initialized = 1;

}



