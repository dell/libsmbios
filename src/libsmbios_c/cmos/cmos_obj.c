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
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "cmos_impl.h"
#include "libsmbios_c_intlize.h"
#include "internal_strl.h"

struct cmos_access_obj singleton; // auto-init to 0
static char *module_error_buf; // auto-init to 0

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

char *cmos_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

static void clear_err(const struct cmos_access_obj *this)
{
    if (this && this->errstring)
        memset(this->errstring, 0, ERROR_BUFSIZE);
    if(module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

LIBSMBIOS_C_DLL_SPEC struct cmos_access_obj *cmos_obj_factory(int flags, ...)
{
    va_list ap;
    struct cmos_access_obj *toReturn = 0;
    int ret;

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
        ret = init_cmos_struct_filename(toReturn, va_arg(ap, const char *));
        va_end(ap);
    } else
    {
        ret = init_cmos_struct(toReturn);
    }

    if (ret==0)
        goto out;

    // fail. init_cmos_* functions are responsible for free-ing memory if they
    // return failure.
    toReturn->initialized = 0;
    toReturn = 0;

out:
    if (toReturn  && ! (flags & CMOS_NO_ERR_CLEAR))
        clear_err(toReturn);
    return toReturn;
}

const char *cmos_obj_strerror(const struct cmos_access_obj *m)
{
    const char * retval = 0;
    if (m)
        retval = m->errstring;
    else
        retval = module_error_buf;
    return retval;
}

int  cmos_obj_read_byte(const struct cmos_access_obj *m, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    clear_err(m);
    int retval = -6;  // bad *buffer ptr
    if (!byte)
        goto out;

    retval = -5; // bad memory_access_obj
    if (!m)
        goto out;

    retval = -7; // not implemented
    if (!m->read_fn)
        goto out;

    retval = m->read_fn(m, byte, indexPort, dataPort, offset);

out:
    return retval;
}

int  cmos_obj_write_byte(const struct cmos_access_obj *m, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    clear_err(m);

    int retval = -5; // bad memory_access_obj
    if (!m)
        goto out;

    retval = -7; // not implemented
    if (!m->write_fn)
        goto out;

    ((struct cmos_access_obj *)m)->write_lock++;
    retval = m->write_fn(m, byte, indexPort, dataPort, offset);
    if (m->write_lock == 1)
        cmos_obj_run_callbacks(m, true);
    ((struct cmos_access_obj *)m)->write_lock--;

out:
    return retval;
}

void __hidden _cmos_obj_cleanup(struct cmos_access_obj *m)
{
    if(m->cleanup)
        m->cleanup(m);
}

void __hidden _cmos_obj_free(struct cmos_access_obj *m)
{
    struct callback *ptr = 0;
    struct callback *next = 0;

    ptr = m->cb_list_head;
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

    free(m->errstring);
    m->errstring=0;
    m->initialized=0;

    if(m->free)
        m->free(m);

    memset(m, 0, sizeof(*m)); // big hammer
    free(m);
}

void cmos_obj_free(struct cmos_access_obj *m)
{
    if (!m) goto out;
    _cmos_obj_cleanup(m);
    if (m != &singleton)
        _cmos_obj_free(m);
out:
    return;
}

void cmos_obj_register_write_callback(struct cmos_access_obj *m, cmos_write_callback cb_fn, void *userdata, void (*destructor)(void *))
{
    clear_err(m);
    struct callback *ptr = 0;
    struct callback *new = 0;

    if(!m || !cb_fn)
        goto out;

    fnprintf(" loop\n");

    ptr = m->cb_list_head;
    while(ptr && ptr->next)
    {
        // dont add duplicates
        if (ptr->cb_fn == cb_fn && ptr->userdata == userdata)
            goto out;

        ptr = ptr->next;
    }

    fnprintf(" allocate\n");
    new = calloc(1, sizeof(struct callback));
    new->cb_fn = cb_fn;
    new->userdata = userdata;
    new->destructor = destructor;
    new->next = 0;

    fnprintf(" join %p\n", ptr);
    if (ptr)
        ptr->next = new;
    else
        m->cb_list_head = new;

out:
    return;
}

int cmos_obj_run_callbacks(const struct cmos_access_obj *m, bool do_update)
{
    clear_err(m);
    int retval = -1;
    const struct callback *ptr = 0;

    if (!m)
        goto out;

    fnprintf("\n");
    retval = 0;
    ptr = m->cb_list_head;
    if(!ptr)
        goto out;

    do{
        fnprintf(" ptr->cb_fn %p\n", ptr->cb_fn);
        retval |= ptr->cb_fn(m, do_update, ptr->userdata);
        ptr = ptr->next;
    } while (ptr);

out:
    return retval;
}

int __hidden _init_cmos_std_stuff(struct cmos_access_obj *m)
{
    int retval = 0;
    m->initialized = 1;
    m->cb_list_head = 0;
    char * errbuf;

    // allocate space for error buffer now. Can optimize this later once api
    // settles. possibly only allocate on error (which would be problematic in
    // the case of error==out-of-mem)
    m->errstring = calloc(1, ERROR_BUFSIZE);
    if (!m->errstring)
        goto out_allocfail;
    goto out;

out_allocfail:
    // if any allocations failed, roll everything back. This should be safe.
    fnprintf("out_allocfail:\n");
    errbuf = cmos_get_module_error_buf();
    if (errbuf)
        strlcpy(errbuf, _("There was an allocation failure while trying to construct the cmos object."), ERROR_BUFSIZE);
    retval = -1;

out:
    return retval;
}
