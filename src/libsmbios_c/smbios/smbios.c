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
#include "smbios_c/memory.h"
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"

// private
#include "smbios_impl.h"

// forward declarations
void init_smbios_struct(struct smbios_table *m);

// static vars
static struct smbios_table singleton; // auto-init to 0

struct smbios_table *smbios_factory(int flags, ...)
{
    struct smbios_table *toReturn = 0;

    if (flags==SMBIOS_DEFAULTS)
        flags = SMBIOS_GET_SINGLETON;

    if (flags & SMBIOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct smbios_table *)calloc(1, sizeof(struct smbios_table));

    if (toReturn->initialized)
        goto out;

    init_smbios_struct(toReturn);

out:
    return toReturn;
}


void smbios_free(struct smbios_table *m)
{
    if (m != &singleton)
        m->free(m);
    else
        m->cleanup(m);
}

static void smbios_table_free(struct smbios_table *this)
{
    //struct ut_data *private_data = (struct ut_data *)this->private_data;
}

static void smbios_table_cleanup(struct smbios_table *this)
{
    //struct ut_data *private_data = (struct ut_data *)this->private_data;
}

void init_smbios_struct(struct smbios_table *m)
{
    //struct ut_data *priv_ut = (struct ut_data *)calloc(1, sizeof(struct ut_data));
    //m->private_data = priv_ut;
    m->free = smbios_table_free;
    m->cleanup = smbios_table_cleanup;
    m->initialized = 1;
}

