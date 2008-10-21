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
#include <sys/io.h>
#include <stdlib.h>
#include <errno.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"
#include "internal_strl.h"
#include "libsmbios_c_intlize.h"

// private
#include "cmos_impl.h"

struct linux_data
{
    int last_errno;
    char *errstring;
};

// we do error string stuff really stupid way for now. can try to optimize
// later once everything works.
static const char * linux_strerror(const struct cmos_access_obj *this)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;
    return private_data->errstring;
}

static int linux_read_fn(const struct cmos_access_obj *this, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    outb_p (offset, indexPort);
    *byte = (inb_p (dataPort));
    return 0;
}

static int linux_write_fn(const struct cmos_access_obj *this, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    outb_p (offset, indexPort);
    outb_p (byte, dataPort);
    return 0;
}

static void linux_cleanup(struct cmos_access_obj *this)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;
    free(private_data->errstring);
    private_data->errstring = 0;
    private_data->last_errno = 0;
}

static void linux_free(struct cmos_access_obj *this)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;
    linux_cleanup(this);
    free(private_data);
    this->private_data = 0;
    this->initialized=0;
}

int __internal init_cmos_struct(struct cmos_access_obj *m)
{
    char * errbuf;
    struct linux_data *private_data = 0;
    size_t curstrsize = 0;

    if(iopl(3) < 0)
        goto out_noprivs;

    private_data = (struct linux_data *)calloc(1, sizeof(struct linux_data));
    if (!private_data)
        goto out_allocfail;
    
    // allocate space for error buffer now. Can optimize this later once api
    // settles
    private_data->errstring = calloc(1, ERROR_BUFSIZE);
    if (!private_data->errstring)
        goto out_allocfail;

    m->read_fn = linux_read_fn;
    m->write_fn = linux_write_fn;
    m->free = linux_free;
    m->cleanup = linux_cleanup;
    m->strerror = linux_strerror;

    _init_cmos_std_stuff(m);
    return 0;

out_noprivs:
    fnprintf("out_allocfail:\n");
    errbuf = cmos_get_module_error_buf();
    if (errbuf)
    {
        strlcpy(errbuf, _("Error trying to raise IO Privilege level.\n"), ERROR_BUFSIZE);
        strlcat(errbuf, _("The OS Error string was: "), ERROR_BUFSIZE);
        curstrsize = strlen(errbuf);
        if ((size_t)(ERROR_BUFSIZE - curstrsize - 1) < ERROR_BUFSIZE)
            strerror_r(errno, errbuf + curstrsize, ERROR_BUFSIZE - curstrsize - 1);
        strlcat(errbuf, "\n", ERROR_BUFSIZE);
    }
    linux_free(m);
    return -1;

out_allocfail:
    // if any allocations failed, roll everything back. This should be safe.
    fnprintf("out_allocfail:\n");
    errbuf = cmos_get_module_error_buf();
    if (errbuf)
        strlcpy(errbuf, _("There was an allocation failure while trying to construct the memory object."), ERROR_BUFSIZE);
    fnprintf(" errbuf ->%p (%zd) '%s'\n", errbuf, strlen(errbuf), errbuf);
    linux_free(m);
    return -1;
}
