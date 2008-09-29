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
void __internal init_smbios_struct(struct smbios_table *m);
void __internal smbios_table_free(struct smbios_table *this);

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
        smbios_table_free(m);

    // can do special cleanup for singleton, but none necessary atm
}


const struct smbios_struct *smbios_get_next_struct(const struct smbios_struct *cur)
{
#if 0
        const smbios_structure_header *currStruct = (const smbios_structure_header *)(current);
        const u8 *data = 0;

        //If we are called on an uninitialized smbiosBuffer, return 0;
        if (0 == smbiosBuffer || (currStruct && 0x7f == currStruct->type))
            goto out1;

        data = smbiosBuffer;

        // currStruct == 0, that means we return the first struct
        if (0 == currStruct)
            goto out1;

        // start out at the end of the currStruct structure.
        // The only things that sits between us and the next struct
        // are the strings for the currStruct structure.
        data = reinterpret_cast<const u8 *>(currStruct) + currStruct->length;

        // skip past strings at the end of the formatted structure,
        // go until we hit double NULL "\0"
        // add a check to make sure we don't walk off the buffer end
        // for broken BIOSen.
        // The (3) is to take into account the deref at the end "data[0] ||
        // data[1]", and to take into account the "data += 2" on the next line.
        while (((data - smbiosBuffer) < (table_header.dmi.table_length - 3)) && (*data || data[1]))
            data++;

        // ok, skip past the actual double null.
        data += 2;

        // add code specifically to work around crap bios implementations
        // that do not have the _required_ 0x7f end-of-table entry
        //   note: (4) == sizeof a std header.
        if ( (data - smbiosBuffer) > (table_header.dmi.table_length - 4))
        {
            // really should output some nasty message here... This is very
            // broken
            data = 0;
            goto out1;
        }

out1:
        return data;
#endif
        return 0;
    }

const struct smbios_struct *smbios_get_next_struct_bytype(const struct smbios_struct *cur, u8 type) { return 0;}
const struct smbios_struct *smbios_get_next_struct_byhandle(const struct smbios_struct *cur, u16 handle) { return 0;}


/**************************************************
 *
 * Internal functions
 *
 **************************************************/

void __internal smbios_table_free(struct smbios_table *this)
{
    memset(&this->tep, 0, sizeof(this->tep));

    free(this->table);
    this->table = 0;

    this->initialized=0;

    free(this);
}

void __internal init_smbios_struct(struct smbios_table *m)
{
    m->initialized = 1;

    // smbios efi strategy
    if (smbios_get_table_efi(m) < 0)
        return;

    // smbios memory strategy
    if (smbios_get_table_memory(m) < 0)
        return;

    // smbios WMI strategy (windows only)
    if (smbios_get_table_wmi(m) < 0)
        return;

    // smbios firmware tables strategy (windows only)
    if (smbios_get_table_firm_tables(m) < 0)
        return;
}



// validate the smbios table entry point
bool __internal validate_dmi_tep( const struct dmi_table_entry_point *dmiTEP, bool strict )
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

    if(checksum) // Checking entry point structure checksum
        retval = false;  // validation failed

    return retval;
}



// validate the smbios table entry point
bool __internal validate_smbios_tep( const struct smbios_table_entry_point *tempTEP, bool strict)
{
    // This code checks for the following:
    //       entry point structure checksum : As per the specs
    //       smbios major version : As per the specs
    //       Intermediate anchor string : As per the specs
    //
    // This code does not check the following:
    //      intermediate checksum: the main checksum covers the
    //      entire area
    //          and should be sufficient, plus there is a
    //          possibility for
    //          BIOS bugs in this area.
    //
    //      minor version: according to the spec, this parser should
    //      work
    //          with any change in minor version. The spec says this
    //          parser
    //          will break if major version changes, so we check
    //          that.
    //

    bool retval = true;

    u8 checksum = 0;
    const u8 *ptr = (const u8*)(tempTEP);
    // don't overrun tempTEP if BIOS is buggy... (note sizeof() test here)
    //      added especially to deal with buggy Intel BIOS.
    for( unsigned int i = 0; (i < (unsigned int)(tempTEP->eps_length)) && (i < sizeof(*tempTEP)); ++i )
        // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
        checksum = (checksum + ptr[i]) & 0xFF;

    validate_dmi_tep( &(tempTEP->dmi), strict);

    if(checksum) // Checking entry point structure checksum
        retval = false;  // validation failed

    if(tempTEP->major_ver!=0x02)     // Checking smbios major version
        retval = false;  // validation failed

    // Entry Point Length field is at least 0x1f.
    if(tempTEP->eps_length < 0x0f)
        retval = false;  // validation failed

    return retval;
}


int __internal smbios_get_tep_memory(struct smbios_table *table, bool strict)
{
    int retval = 1;
    struct memory *mem = memory_factory(MEMORY_GET_SINGLETON);

    unsigned long fp = E_BLOCK_START;

    // tell the memory subsystem that it can optimize here and
    // keep memory open while we scan rather than open/close/open/close/...
    // for each fillBuffer() call
    memory_suggest_leave_open(mem);

    struct smbios_table_entry_point tempTEP;
    memset(&tempTEP, 0, sizeof(tempTEP));
    while ( (fp + sizeof(tempTEP)) < F_BLOCK_END)
    {
        memory_read(mem, &tempTEP, fp, sizeof(tempTEP));

        // search for promising looking headers
        // first, look for old-style DMI header
        if (memcmp (&tempTEP, "_DMI_", 5) == 0)
        {
            struct dmi_table_entry_point *dmiTEP = (struct dmi_table_entry_point *)(&tempTEP);
            memmove(&(tempTEP.dmi), &dmiTEP, sizeof(struct dmi_table_entry_point));
            // fake the rest of the smbios table entry point...
            tempTEP.major_ver=2;
            tempTEP.minor_ver=0;
            if(validate_dmi_tep(dmiTEP, strict))
                break;
        }

        // then, look for new-style smbios header. This will always
        // occur before _DMI_ in memory
        if ((memcmp (&tempTEP, "_SM_", 4) == 0))
        {
            if(validate_smbios_tep(&tempTEP, strict))
                break;
        }

        fp += 16;
    }

    // dont need memory optimization anymore
    memory_suggest_close(mem);

    // bad stuff happened if we got to here and fp > 0xFFFFFL
    if ((fp + sizeof(tempTEP)) >= F_BLOCK_END)
    {
        retval = 0;
        goto out;
    }

    memcpy( &table->tep, &tempTEP, sizeof(table->tep) );

out:
    return retval;
}


int __internal smbios_get_table_memory(struct smbios_table *m)
{
    struct memory *mem = memory_factory(MEMORY_GET_SINGLETON);
    int retval = -1; //fail

    if (!smbios_get_tep_memory(m, false))
        goto out;

    size_t len = m->tep.dmi.table_length;
    m->table = (struct table*)calloc(1, len);
    retval = memory_read(mem, m->table, m->tep.dmi.table_address, len);
    if (retval == 0)
        goto out;

    // fail
    free(m->table);
    m->table = 0;

out:
    return retval;
}
