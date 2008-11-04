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

// usually want to include this last
#include "libsmbios_c_intlize.h"

static struct memory_access_obj singleton; // auto-init to 0
static char *module_error_buf; // auto-init to 0

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

char *memory_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

static void clear_err(const struct memory_access_obj *this)
{
    if (this && this->errstring)
        memset(this->errstring, 0, ERROR_BUFSIZE);
    if(module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

struct memory_access_obj *memory_obj_factory(int flags, ...)
{
    va_list ap;
    struct memory_access_obj *toReturn = 0;
    int ret;

    fnprintf("1\n");
    return_mem();

    fnprintf("2\n");
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
        ret = init_mem_struct_filename(toReturn, va_arg(ap, const char *));
        va_end(ap);
    } else
    {
        ret = init_mem_struct(toReturn);
    }

    if (ret == 0)
        goto out;

    // init_mem_* functions are responsible for free-ing memory if they return
    // failure
    toReturn->initialized = 0;
    toReturn = 0;

out:
    if (toReturn && ! (flags & MEMORY_NO_ERR_CLEAR))
        clear_err(toReturn);
    return toReturn;
}

void  memory_obj_suggest_leave_open(struct memory_access_obj *this)
{
    clear_err(this);
    if (this)
        this->close--;
}

void  memory_obj_suggest_close(struct memory_access_obj *this)
{
    clear_err(this);
    if (this)
        this->close++;
}

bool  memory_obj_should_close(const struct memory_access_obj *this)
{
    clear_err(this);
    if (this)
        return this->close > 0;
    return true;
}

int  memory_obj_read(const struct memory_access_obj *m, void *buffer, u64 offset, size_t length)
{
    clear_err(m);
    int retval = -6;  // bad *buffer ptr
    if (!m)
        retval = -5; // bad memory_access_obj

    if (m && buffer)
        retval = m->read_fn(m, (u8 *)buffer, offset, length);

    return retval ;
}

int  memory_obj_write(const struct memory_access_obj *m, void *buffer, u64 offset, size_t length)
{
    clear_err(m);
    int retval = -6;  // bad *buffer ptr
    if (!m)
        retval = -5; // bad memory_access_obj

    if (m && buffer)
        retval = m->write_fn(m, (u8 *)buffer, offset, length);

    return retval;
}

const char *memory_obj_strerror(const struct memory_access_obj *m)
{
    const char * retval = 0;
    if (m)
        retval = m->errstring;
    else
        retval = module_error_buf;

    return retval;
}

void memory_obj_free(struct memory_access_obj *m)
{
    fnprintf("  m(%p)  singleton(%p)\n", m, &singleton);
    if (!m) goto out;
    if (m->cleanup)
        m->cleanup(m);
    if (m != &singleton)
    {
        if (m->free)
            m->free(m);
        memset(m, 0, sizeof(*m)); // big hammer
        free(m);
    }
out:
    return;
}

s64  memory_obj_search(const struct memory_access_obj *m, const char *pat, size_t patlen, u64 start, u64 end, u64 stride)
{
    u8 *buf = calloc(1, patlen);
    u64 cur = start;
    int ret;
    memory_obj_suggest_leave_open((struct memory_access_obj *)m);

    memset(buf, 0, patlen);

    while ( (cur + patlen) < end)
    {
        ret = memory_obj_read(m, buf, cur, patlen);
        if(ret != 0)
            goto err_out;

        if (memcmp (buf, pat, patlen) == 0)
            goto out;

        cur += stride;
    }

    // bad stuff happened if we got to here and cur > end
    if ((cur + patlen) >= end)
        goto err_out;

    goto out;

err_out:
    cur = -1;

out:
    memory_obj_suggest_close((struct memory_access_obj *)m);
    free(buf);
    return cur;
}

