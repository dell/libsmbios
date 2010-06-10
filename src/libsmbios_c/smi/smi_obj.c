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
#include "smbios_c/smbios.h"
#include "smbios_c/smi.h"
#include "libsmbios_c_intlize.h"
#include "internal_strl.h"

// private
#include "smi_impl.h"

// forward declarations
void _smi_free(struct dell_smi_obj *m);

// static vars
static struct dell_smi_obj singleton; // auto-init to 0
typedef int (*init_fn)(struct dell_smi_obj *);
static char *module_error_buf; // auto-init to 0

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

static char *smi_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

static void clear_err(const struct dell_smi_obj *this)
{
    fnprintf("\n");
    if (this && this->errstring)
        memset(this->errstring, 0, ERROR_BUFSIZE);
    if(module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

struct dell_smi_obj *dell_smi_factory(int flags, ...)
{
    va_list ap;
    struct dell_smi_obj *toReturn = 0;
    int ret;

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
        ret = fn(toReturn);
        va_end(ap);
    } else
    {
        fnprintf("default init\n");
        ret = init_dell_smi_obj(toReturn);
    }

    if (ret == 0)
        goto out;

    // failed
    fnprintf("failed\n");
    toReturn->initialized = 0;
    toReturn = 0;

out:
    if (toReturn && ! (flags & DELL_SMI_NO_ERR_CLEAR))
        clear_err(toReturn);
    return toReturn;
}


void dell_smi_obj_free(struct dell_smi_obj *m)
{
    fnprintf("\n");
    if (m && m != &singleton)
        _smi_free(m);
}

const char *dell_smi_obj_strerror(struct dell_smi_obj *s)
{
    const char * retval = 0;
    fnprintf("\n");
    if (s)
        retval = s->errstring;
    else
        retval = module_error_buf;

    fnprintf("error string: %s\n", retval);
    return retval;
}

void dell_smi_obj_set_class(struct dell_smi_obj *this, u16 smi_class)
{
    fnprintf(" %d\n", smi_class);
    clear_err(this);
    if(this)
        this->smi_buf.smi_class = smi_class;
}

void dell_smi_obj_set_select(struct dell_smi_obj *this, u16 smi_select)
{
    fnprintf(" %d\n", smi_select);
    clear_err(this);
    if(this)
        this->smi_buf.smi_select = smi_select;
}

void dell_smi_obj_set_arg(struct dell_smi_obj *this, u8 argno, u32 value)
{
    fnprintf(" %d -> 0x%x\n", argno, value);
    clear_err(this);
    if(!this) goto out;
    free(this->physical_buffer[argno]);
    this->physical_buffer[argno] = 0;
    this->physical_buffer_size[argno] = 0;

    this->smi_buf.arg[argno] = value;
out:
    return;
}

u32  dell_smi_obj_get_res(struct dell_smi_obj *this, u8 argno)
{
    clear_err(this);
    u32 retval = 0;
    if (this)
        retval = this->smi_buf.res[argno];
    fnprintf(" %d = 0x%x\n", argno, retval);
    return retval;
}

static u8 * dell_smi_obj_make_buffer_X(struct dell_smi_obj *this, u8 argno, size_t size)
{
    u8 *retval = 0;
    fnprintf("\n");
    clear_err(this);
    if (argno>3 || !this)
        goto out;

    this->smi_buf.arg[argno] = 0;
    free(this->physical_buffer[argno]);
    this->physical_buffer[argno] = calloc(1, size);
    this->physical_buffer_size[argno] = size;
    retval = this->physical_buffer[argno];
out:
    return retval;
}

const char *bufpat = "DSCI";
u8 * dell_smi_obj_make_buffer_frombios_withheader(struct dell_smi_obj *this, u8 argno, size_t size)
{
    // allocate 4 extra bytes to hold size marker at the beginning
    u8 *buf = dell_smi_obj_make_buffer_X(this, argno, size + sizeof(u32));
    fnprintf("\n");
    if(buf)
    {
        // write buffer pattern
        for (int i=0; i<size+4; i++)
            buf[i] = bufpat[i%4];

        // write size of remaining bytes
        memcpy(buf, &size, sizeof(u32));
        buf += sizeof(u32);
    }
    return buf;
}

u8 * dell_smi_obj_make_buffer_frombios_withoutheader(struct dell_smi_obj *this, u8 argno, size_t size)
{
    fnprintf("\n");
    return dell_smi_obj_make_buffer_X(this, argno, size);
}

u8 * dell_smi_obj_make_buffer_frombios_auto(struct dell_smi_obj *this, u8 argno, size_t size)
{
    clear_err(this);
    u8 smbios_ver = 1;
    u8 *retval = 0;
    // look in smbios struct 0xD0 (Revisions and IDs) to find the Dell SMBIOS implementation version
    //  offset 4 of the struct == dell major version
    struct smbios_struct *s = smbios_get_next_struct_by_type(0, 0xd0);
    smbios_struct_get_data(s, &(smbios_ver), 0x04, sizeof(u8));

    fnprintf("dell smbios ver: %d\n", smbios_ver);

    if (smbios_ver >= 2)
        retval = dell_smi_obj_make_buffer_frombios_withheader(this, argno, size);
    else
        retval = dell_smi_obj_make_buffer_frombios_withoutheader(this, argno, size);
    return retval;
}

u8 * dell_smi_obj_make_buffer_tobios(struct dell_smi_obj *this, u8 argno, size_t size)
{
    return dell_smi_obj_make_buffer_X(this, argno, size);
}


int dell_smi_obj_execute(struct dell_smi_obj *this)
{
    fnprintf("\n");
    clear_err(this);
    int retval = -1;
    if(!this)
        goto out;
    this->smi_buf.res[0] = -3; //default to 'not handled'
    if (this->execute)
        retval = this->execute(this);
out:
    return retval;
}

/**************************************************
 *
 * Internal functions
 *
 **************************************************/

void __hidden _smi_free(struct dell_smi_obj *this)
{
    fnprintf("\n");
    this->initialized=0;
    for (int i=0;i<4;++i)
    {
        free(this->physical_buffer[i]);
        this->physical_buffer[i]=0;
        this->physical_buffer_size[i] = 0;
    }
    free(this->errstring);
    this->errstring = 0;
    free(this);
}

int __hidden init_dell_smi_obj_std(struct dell_smi_obj *this)
{
    int retval = 0;
    char *errbuf = 0;
    fnprintf("\n");

    const char *error = _("Failed to find appropriate SMBIOS 0xD4 structure.\n");
    struct smbios_struct *s = smbios_get_next_struct_by_type(0, 0xda);
    if (s) {
        smbios_struct_get_data(s, &(this->command_address), 4, sizeof(u16));
        smbios_struct_get_data(s, &(this->command_code), 6, sizeof(u8));
    }
    else
        goto out_fail;

    error = _("Failed to allocate memory for error string.\n");
    this->errstring = calloc(1, ERROR_BUFSIZE);
    if (!this->errstring)
        goto out_fail;

    this->initialized = 1;
    goto out;

out_fail:
    fnprintf(" out_fail \n");
    retval = -1;
    errbuf = smi_get_module_error_buf();
    if (errbuf){
        fnprintf("error: %s\n", error);
        strlcpy(errbuf, error, ERROR_BUFSIZE);
        fnprintf("smbios_strerror: %s\n", smbios_strerror());
        strlcat(errbuf, smbios_strerror(), ERROR_BUFSIZE);
    }

out:
    return retval;
}



