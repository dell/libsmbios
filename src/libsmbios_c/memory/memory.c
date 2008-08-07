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
#include "smbios_c/types.h"

// private
#include "memory_impl.h"

#if !LIBSMBIOS_C_USE_MEMORY_MMAP
#define MEM_INIT_FUNCTION init_mem_struct_filename
#endif

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))

void __internal init_mem_struct(struct memory *m);
void __internal MEM_INIT_FUNCTION(struct memory *m, const char *fn);

static struct memory singleton; // auto-init to 0

struct memory *memory_factory(int flags, ...)
{
    va_list ap;
    struct memory *toReturn = 0;

    if (flags==MEMORY_DEFAULTS)
        flags = MEMORY_GET_SINGLETON;
            
    if (flags & MEMORY_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct memory *)calloc(1, sizeof(struct memory));

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

int  memory_read(struct memory *m, u8 *buffer, u64 offset, size_t length)
{
    return m->read_fn(m, buffer, offset, length);
}

int  memory_write(struct memory *m, u8 *buffer, u64 offset, size_t length)
{
    return m->write_fn(m, buffer, offset, length);
}

void memory_free(struct memory *m)
{
    if (m != &singleton)
        m->free(m);
    else
        m->cleanup(m);
}

s64  memory_search(struct memory *m, const char *pat, size_t patlen, u64 start, u64 end, u64 stride)
{
    u8 *buf = calloc(1, patlen);
    u64 cur = start;
    m->close = m->close - 1;

    memset(buf, 0, patlen);

    while ( (cur + patlen) < end)
    {
        memory_read(m, buf, cur, patlen);
        
        if (memcmp (buf, pat, patlen) == 0)
            goto out;

        cur += stride;
    }

    // bad stuff happened if we got to here and cur > end
    if ((cur + patlen) >= end)
        cur = -1;

out:
    m->close = m->close + 1;
    free(buf);
    return cur;
}


// UNIT TEST

struct ut_data
{
    char *filename;
    FILE *fd;
    int mem_errno;
    int rw;
};

static int UT_read_fn(struct memory *this, u8 *buffer, u64 offset, size_t length)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    private_data->mem_errno = errno = 0;
    int retval = -1;

    if (!private_data->fd)
    {
        // fopen portable to Windows if "b" is added to mode.
        private_data->rw=0;
        private_data->fd = fopen( private_data->filename, "rb" ); // open for read to start
        if (!private_data->fd)
            goto err_out;
    }

    // FSEEK is a macro defined in config/ for portability
    retval = -2;
    int ret = FSEEK(private_data->fd, offset, 0);
    if (ret)
        goto err_out;

    size_t bytesRead = fread( buffer, 1, length, private_data->fd );

    // TODO: handle short reads
    retval = -3;
    if ((length != bytesRead))
        goto err_out;

    retval = 0;
    goto out;

err_out:
    private_data->mem_errno = errno;

out:
    // close on error, or if close hint
    if (private_data->fd && ((this->close>0) || retval))
    {
        fflush(private_data->fd);
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static int UT_write_fn(struct memory *this, u8 *buffer, u64 offset, size_t length)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    private_data->mem_errno = errno = 0;
    int retval = -1;

    if(!private_data->rw || !private_data->fd)
    {
        if(private_data->fd)
            fclose(private_data->fd);

        private_data->fd = fopen( private_data->filename, "r+b" ); // re-open for write
        if(!private_data->fd)
            goto err_out;

        private_data->rw=1;
    }

    // FSEEK is a macro defined in config/ for portability
    int ret = FSEEK(private_data->fd, offset, 0);
    if(ret)
        goto err_out;

    size_t bytesWritten = fwrite( buffer, length, 1, private_data->fd );
    if( 1 != bytesWritten )
        goto err_out;

    retval = 0;
    goto out;

err_out:
    private_data->mem_errno = errno;

out:
    // close on error, or if close hint
    if (private_data->fd && ((this->close>0) || retval))
    {
        fflush(private_data->fd);
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static void UT_free(struct memory *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    if (private_data->filename)
    {
        free(private_data->filename);
        private_data->filename = 0;
    }
    if (private_data->fd)
    {
        fflush(private_data->fd);
        fclose(private_data->fd);
    }
    free(private_data);
    this->private_data = 0;
}

static void UT_cleanup(struct memory *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    if (private_data->fd)
    {
        fflush(private_data->fd);
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    private_data->mem_errno = 0;
    private_data->rw = 0;
}


void MEM_INIT_FUNCTION(struct memory *m, const char *fn)
{
    struct ut_data *priv_ut = (struct ut_data *)calloc(1, sizeof(struct ut_data));
    priv_ut->filename = (char *)calloc(1, strlen(fn) + 1);
    strcpy(priv_ut->filename, fn);

    m->private_data = priv_ut;
    m->free = UT_free;
    m->read_fn = UT_read_fn;
    m->write_fn = UT_write_fn;
    m->cleanup = UT_cleanup;
    m->close = 1;
    m->initialized = 1;
}


