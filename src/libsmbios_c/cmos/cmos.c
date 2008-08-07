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

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))

void __internal init_cmos_struct(struct cmos *m);
void __internal init_cmos_struct_filename(struct cmos *m, const char *fn);

struct cmos singleton; // auto-init to 0

struct cmos *cmos_factory(int flags, ...)
{
    va_list ap;
    struct cmos *toReturn = 0;

    if (flags==CMOS_DEFAULTS)
        flags = CMOS_GET_SINGLETON;
            
    if (flags & CMOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct cmos *)calloc(1, sizeof(struct cmos));

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

int  cmos_read_byte(struct cmos *m, u32 indexPort, u32 dataPort, u32 offset, u8 *byte)
{
    return m->read_fn(m, indexPort, dataPort, offset, byte);
}

int  cmos_write_byte(struct cmos *m, u32 indexPort, u32 dataPort, u32 offset, u8 byte)
{
    return m->write_fn(m, indexPort, dataPort, offset, byte);
}

void cmos_free(struct cmos *m)
{
    if (m != &singleton)
        m->free(m);
    else
        m->cleanup(m);
}


// UNIT TEST

struct ut_data
{
    char *filename;
    FILE *fd;
    int cmos_errno;
    int rw;
};

static int UT_read_fn(struct cmos *this, u32 indexPort, u32 dataPort, u32 offset, u8 *byte)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    private_data->cmos_errno = errno = 0;
    int retval = -1;

    // for unit testing, index into a file by indexPort
    offset = indexPort * 512 + offset;

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

    size_t bytesRead = fread( byte, 1, 1, private_data->fd );

    // TODO: handle short reads
    retval = -3;
    if ((1 != bytesRead))
        goto err_out;

    retval = 0;
    goto out;

err_out:
    private_data->cmos_errno = errno;

out:
    // close on error, or if close hint
    if (private_data->fd && ((this->close>0) || retval))
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static int UT_write_fn(struct cmos *this, u32 indexPort, u32 dataPort, u32 offset, u8 byte)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    private_data->cmos_errno = errno = 0;
    int retval = -1;

    // for unit testing, index into a file by indexPort
    offset = indexPort * 512 + offset;

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

    size_t bytesWritten = fwrite( &byte, 1, 1, private_data->fd );
    if( 1 != bytesWritten )
        goto err_out;

    retval = 0;
    goto out;

err_out:
    private_data->cmos_errno = errno;

out:
    // close on error, or if close hint
    if (private_data->fd && ((this->close>0) || retval))
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static void UT_free(struct cmos *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    if (private_data->filename)
    {
        free(private_data->filename);
        private_data->filename = 0;
    }
    if (private_data->fd)
    {
        fclose(private_data->fd);
    }
    free(private_data);
    this->private_data = 0;
}

static void UT_cleanup(struct cmos *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    if (private_data->fd)
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    private_data->cmos_errno = 0;
    private_data->rw = 0;
}


void init_cmos_struct_filename(struct cmos *m, const char *fn)
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


void init_cmos_struct(struct cmos *m)
{
}
