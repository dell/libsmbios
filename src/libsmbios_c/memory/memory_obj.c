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
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

// public
#include "smbios_c/obj/memory.h"
#include "smbios_c/types.h"

// private
#include "memory_impl.h"

static struct memory_access_obj singleton; // auto-init to 0

struct memory_access_obj *memory_obj_factory(int flags, ...)
{
    va_list ap;
    struct memory_access_obj *toReturn = 0;

    if (flags==MEMORY_DEFAULTS)
        flags = MEMORY_GET_SINGLETON;

    if (flags & MEMORY_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct memory_access_obj *)calloc(1, sizeof(struct memory_access_obj));

    if (toReturn->initialized)
        goto out;

    if (flags & MEMORY_UNIT_TEST_MODE)
    {
        va_start(ap, flags);
        init_mem_struct_filename(toReturn, va_arg(ap, const char *));
        va_end(ap);
    } else
    {
        init_mem_struct(toReturn);
    }

out:
    return toReturn;
}

void  memory_obj_suggest_leave_open(struct memory_access_obj *this)
{
    this->close--;
}

void  memory_obj_suggest_close(struct memory_access_obj *this)
{
    this->close++;
}

bool  memory_obj_should_close(const struct memory_access_obj *this)
{
    return this->close > 0;
}

int  memory_obj_read(const struct memory_access_obj *m, void *buffer, u64 offset, size_t length)
{
    return m->read_fn(m, (u8 *)buffer, offset, length);
}

int  memory_obj_write(const struct memory_access_obj *m, void *buffer, u64 offset, size_t length)
{
    return m->write_fn(m, (u8 *)buffer, offset, length);
}

void memory_obj_free(struct memory_access_obj *m)
{
    fnprintf("  m(%p)  singleton(%p)\n", m, &singleton);
    if (m != &singleton)
    {
        m->free(m);
        free(m);
    }
    else
        m->cleanup(m);
}

s64  memory_obj_search(const struct memory_access_obj *m, const char *pat, size_t patlen, u64 start, u64 end, u64 stride)
{
    u8 *buf = calloc(1, patlen);
    u64 cur = start;
    memory_obj_suggest_leave_open((struct memory_access_obj *)m);

    memset(buf, 0, patlen);

    while ( (cur + patlen) < end)
    {
        memory_obj_read(m, buf, cur, patlen);

        if (memcmp (buf, pat, patlen) == 0)
            goto out;

        cur += stride;
    }

    // bad stuff happened if we got to here and cur > end
    if ((cur + patlen) >= end)
        cur = -1;

out:
    memory_obj_suggest_close((struct memory_access_obj *)m);
    free(buf);
    return cur;
}

