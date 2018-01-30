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
#include <stdarg.h>
#include <string.h>
//#include <stdio.h>

// public
#include "smbios_c/memory.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"
#include "internal_strl.h"

// private
#include "smbios_impl.h"
#include "libsmbios_c_intlize.h"

// forward declarations

// static vars
static struct smbios_table singleton; // auto-init to 0
static char *module_error_buf; // auto-init to 0

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

static char *smbios_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

static void clear_err(const struct smbios_table *this)
{
    if (this && this->errstring)
        memset(this->errstring, 0, ERROR_BUFSIZE);
    if(module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

struct smbios_table *smbios_table_factory(int flags, ...)
{
    va_list ap;
    struct smbios_table *toReturn = 0;
    int ret;

    fnprintf("\n");

    if (flags==SMBIOS_DEFAULTS)
        flags = SMBIOS_GET_SINGLETON;

    if (flags & SMBIOS_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct smbios_table *)calloc(1, sizeof(struct smbios_table));

    if (toReturn->initialized)
        goto out;

    if (flags & SMBIOS_UNIT_TEST_MODE) {
        va_start(ap, flags);
        const char *filename = va_arg(ap, const char *);
        long len = strlen(filename);
        va_end(ap);
        if (len > 0){
            fnprintf("Input path: %s (%lu)\n", filename, len);
            toReturn->table_path = calloc(1, len + 1);
            if (!toReturn->table_path)
                goto out_clear_memory;
            strcat(toReturn->table_path, filename);
        }
    }

    ret = init_smbios_struct(toReturn);
    if (ret)
        goto out_init_fail;

    if (!(flags & SMBIOS_NO_FIXUPS))
        do_smbios_fixups(toReturn);

    goto out;

out_clear_memory:
    if (!(flags & SMBIOS_GET_SINGLETON))
        free(toReturn);

out_init_fail:
    // fail. init_smbios_* functions are responsible for free-ing memory if they
    // return failure.
    toReturn = 0;

out:
    if (toReturn && ! (flags & SMBIOS_NO_ERR_CLEAR))
        clear_err(toReturn);
    return toReturn;
}

void smbios_table_free(struct smbios_table *this)
{
    if (!this || this == &singleton)
        return;
    free(this->table_path);

    free(this->errstring);
    this->errstring = 0;

    free(this->table);
    this->table = 0;

    this->initialized=0;

    memset(this, 0, sizeof(*this)); // big hammer
    free(this);
}

const char *smbios_table_strerror(const struct smbios_table *m)
{
    const char * retval = 0;
    if (m)
        retval = m->errstring;
    else
        retval = module_error_buf;

    return retval;
}

struct smbios_struct *smbios_table_get_next_struct(const struct smbios_table *table, const struct smbios_struct *cur)
{
    clear_err(table);
    const u8 *data = 0;

    //If we are called on an uninitialized smbiosBuffer, return 0;
    if (!table || 0 == table->table || (cur && 0x7f == cur->type))
        goto out1;

    data = (u8*)table->table;

    // cur == 0, that means we return the first struct
    if (0 == cur)
        goto out1;

    // start out at the end of the cur structure.
    // The only things that sits between us and the next struct
    // are the strings for the cur structure.
    data = (const u8 *)(cur) + smbios_struct_get_length(cur);

    // skip past strings at the end of the formatted structure,
    // go until we hit double NULL "\0"
    // add a check to make sure we don't walk off the buffer end
    // for broken BIOSen.
    // The (3) is to take into account the deref at the end "data[0] ||
    // data[1]", and to take into account the "data += 2" on the next line.
    while (((data - (u8*)table->table) < (table->table_length - 3)) && (*data || data[1]))
        data++;

    // ok, skip past the actual double null.
    data += 2;

    // add code specifically to work around crap bios implementations
    // that do not have the _required_ 0x7f end-of-table entry
    //   note: (4) == sizeof a std header.
    if ( (data - (u8*)table->table) > (table->table_length - 4))
    {
        // really should output some nasty message here... This is very
        // broken
        data = 0;
        goto out1;
    }

out1:
    return (struct smbios_struct *)data;
}

struct smbios_struct *smbios_table_get_next_struct_by_type(const struct smbios_table *table, const struct smbios_struct *cur, u8 type)
{
    do {
        cur = smbios_table_get_next_struct(table, cur);
        if (cur && cur->type == type)
            break;
    } while ( cur );
    return (struct smbios_struct *)cur;
}

struct smbios_struct *smbios_table_get_next_struct_by_handle(const struct smbios_table *table, const struct smbios_struct *cur, u16 handle)
{
    do {
        cur = smbios_table_get_next_struct(table, cur);
        if (cur && cur->handle == handle)
            break;
    } while ( cur );
    return (struct smbios_struct *)cur;
}


u8 smbios_struct_get_type(const struct smbios_struct *s)
{
    if (s)
        return s->type;
    return 0;
}

u8 smbios_struct_get_length(const struct smbios_struct *s)
{
    if (s)
        return s->length;
    return 0;
}

u16 smbios_struct_get_handle(const struct smbios_struct *s)
{
    if (s)
        return s->handle;
    return 0;
}

int smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len)
{
    int retval = -1;

    fnprintf("(%p, %p, %d, %zd)\n", s, dest, offset, len);

    if (!s)
        goto out;

    if (offset > smbios_struct_get_length(s))
        goto out;

    if( offset + len < offset ) // attempt to wraparound... :(
        goto out;

    if( offset + len > smbios_struct_get_length(s) )
        goto out;

    retval = 0;
    memcpy(dest, (const u8 *)(s)+offset, len);

out:
    return retval;
}

const char *smbios_struct_get_string_from_offset(const struct smbios_struct *s, u8 offset)
{
    u8 strnum = 0;
    const char *retval = 0;

    dbg_printf("smbios_struct_get_string_from_offset()\n");

    if (!s)
        goto out;

    if (smbios_struct_get_data(s, &strnum, offset, sizeof(strnum)) >= 0)
    {
        dbg_printf("string offset: %d  which: %d\n", offset, strnum);
        retval = smbios_struct_get_string_number(s, strnum);
    }

out:
    dbg_printf("string: %s\n", retval);
    return retval;
}

const char *smbios_struct_get_string_number(const struct smbios_struct *s, u8 which)
{
    const char *string_pointer = 0;
    const char *retval = 0;

    dbg_printf("smbios_struct_get_string_number(%p, %d)\n", s, which);

    //strings are numbered beginning with 1
    // bail if passed null smbios_struct, or user tries to get string 0
    if (!which || !s)
        goto out;

    string_pointer = (const char *)(s);

    // start out at the end of the header. This is where
    // the first string starts
    string_pointer += smbios_struct_get_length(s);

    for (; which > 1; which--)
    {
        string_pointer += strlen (string_pointer) + 1;

        // if it is still '\0', that means we are
        // at the end of this item and should stop.
        // user gave us a bad index
        if( ! *string_pointer )
            goto out;
    }

    retval = string_pointer;


out:
    return retval;
}

// visitor pattern
void smbios_table_walk(struct smbios_table *table, void (*fn)(const struct smbios_struct *, void *userdata), void *userdata)
{
    clear_err(table);
    const struct smbios_struct *s = smbios_table_get_next_struct(table, 0);
    while(s) {
        fn(s, userdata);
        s = smbios_table_get_next_struct(table, s);
    };
}


/**************************************************
 *
 * Internal functions
 *
 **************************************************/

int __hidden init_smbios_struct(struct smbios_table *m)
{
    char *errbuf;
    char *error = _("Allocation error trying to allocate memory for error string. (ironic, yes?) \n");
    m->initialized = 1;
    m->errstring = calloc(1, ERROR_BUFSIZE);
    if (!m->errstring)
        goto out_fail;

    fnprintf("\n");
    error = _("Could not instantiate SMBIOS table. The errors from the low-level modules were:\n");

    // smbios firmware tables strategy
    if (smbios_get_table_firm_tables(m) >= 0)
        return 0;

    // smbios memory strategy
    if (smbios_get_table_memory(m) >= 0)
        return 0;

    // fall through to failure...

out_fail:
    errbuf = smbios_get_module_error_buf();
    if (errbuf){
        strlcpy(errbuf, error, ERROR_BUFSIZE);
        if (m->errstring)
            strlcat(errbuf, m->errstring, ERROR_BUFSIZE);
    }
    smbios_table_free(m);
    return -1;
}



// validate the smbios table entry point
bool __hidden validate_dmi_tep(const struct dmi_table_entry_point *dmiTEP)
{
    // This code checks for the following:
    //       entry point structure checksum : As per the specs
    //       anchor string : As per the specs
    bool retval = true;

    u8 checksum = 0;
    const u8 *ptr = (const u8*)(dmiTEP);
    // don't overrun dmiTEP if BIOS is buggy... (note sizeof() test here)
    //      added especially to deal with buggy Intel BIOS.
    for( unsigned int i = 0; i < sizeof(*dmiTEP); ++i )
        // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
        checksum = (checksum + ptr[i]) & 0xFF;

    if(memcmp(dmiTEP->anchor,"_DMI_",5)!=0) // Checking intermediate anchor string
        retval = false;  // validation failed

    fnprintf("DMI TEP csum %d.\n", (int)checksum);
    if(checksum) // Checking entry point structure checksum
        retval = false;  // validation failed

    return retval;
}



/* validate the smbios table entry point */
bool __hidden smbios_verify_smbios(const char *buf, long length, long *dmi_length_out)
{
    struct smbios_table_entry_point *ep;
    bool retval = true;

    u8 checksum = 0;
    for(unsigned int i = 0; i < length ; ++i )
        checksum = (checksum + buf[i]) & 0xFF;

    fnprintf("SMBIOS TEP csum %d.\n", (int)checksum);
    if(checksum) // Checking entry point structure checksum
        return false;  // validation failed

    ep = (struct smbios_table_entry_point*) buf;
    retval = validate_dmi_tep( &(ep->dmi));

    *dmi_length_out = ep->dmi.table_length;
    fnprintf("Major version: %d Minor version: %d\n", ep->major_ver, ep->minor_ver);

    return retval;
}

/* validate the smbios3 table entry point */
bool __hidden smbios_verify_smbios3(const char *buf, long length, long *dmi_length_out)
{
    struct smbios_table_entry_point_64 *ep;
    u8 checksum = 0;
    for(unsigned int i = 0; i < length ; ++i )
        checksum = (checksum + buf[i]) & 0xFF;

    fnprintf("SMBIOS TEP csum %d.\n", (int)checksum);
    if(checksum) // Checking entry point structure checksum
        return false;  // validation failed

    ep = (struct smbios_table_entry_point_64*) buf;
    *dmi_length_out = ep->structure_table_length;
    fnprintf("Major version: %d Minor version: %d\n", ep->major_ver, ep->minor_ver);

    return true;
}
