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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"


struct ut_data
{
    char *filename;
    FILE *fd;
    int rw;
};

static int UT_read_fn(const struct cmos_access_obj *this, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    int retval = -1;

    fnprintf("%x %x %d\n", indexPort, dataPort, offset);

    // for unit testing, index into a file by indexPort
    offset = indexPort * 256 + offset;

    if (!private_data->fd)
    {
        // fopen portable to Windows if "b" is added to mode.
        fnprintf("reopen\n");
        private_data->rw=0;
        private_data->fd = fopen( private_data->filename, "rb" ); // open for read to start
        if (!private_data->fd)
            goto err_out;
    }

    // FSEEK is a macro defined in config/ for portability
    fnprintf("seek %d (%x)\n", offset, offset);
    retval = -2;
    int ret = FSEEK(private_data->fd, offset, 0);
    if (ret)
        goto err_out;

    fnprintf("read\n");
    size_t bytesRead = fread( byte, 1, 1, private_data->fd );

    // TODO: handle short reads
    retval = -3;
    fnprintf("short? %zd\n", bytesRead);
    if ((1 != bytesRead))
        goto err_out;

    retval = 0;
    goto out;

err_out:

out:
    // always close
    if (private_data->fd)
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    fnprintf("retval %d read\n", retval);
    return retval;
}

static int UT_write_fn(const struct cmos_access_obj *this, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    int retval = -1;

    // for unit testing, index into a file by indexPort
    offset = indexPort * 256 + offset;

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

out:
    // always close
    if (private_data->fd)
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static void UT_free(struct cmos_access_obj *this)
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

static void UT_cleanup(struct cmos_access_obj *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    if (private_data->fd)
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    private_data->rw = 0;
}


int init_cmos_struct_filename(struct cmos_access_obj *m, const char *fn)
{
    struct ut_data *priv_ut = (struct ut_data *)calloc(1, sizeof(struct ut_data));
    priv_ut->filename = (char *)calloc(1, strlen(fn) + 1);
    strcpy(priv_ut->filename, fn);

    m->private_data = priv_ut;
    m->free = UT_free;
    m->read_fn = UT_read_fn;
    m->write_fn = UT_write_fn;
    m->cleanup = UT_cleanup;

    _init_cmos_std_stuff(m);

    return 0;
}
