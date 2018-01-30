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
#include <string.h>

// public
#include "smbios_c/memory.h"
#include "smbios_c/types.h"
#include "smbios_impl.h"
#include "internal_strl.h"

// private
#include "smbios_impl.h"
#include "libsmbios_c_intlize.h"

/* read in a file, allocate memory for it
   caller must free the memory
 */
int read_file(const char *fname, long minimum, char **out_buffer, long *out_length)
{
    int ret = -1;
    FILE *f;

    f = fopen(fname, "rb");
    if (!f)
        return ret;
    fseek(f, 0, SEEK_END);
    *out_length = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (*out_length < minimum || *out_length < 0) {
        fnprintf("File length: %li\n", *out_length);
        goto out_close_file;
    }
    *out_buffer = malloc(*out_length);
    if (!*out_buffer)
        goto out_close_file;
    ret = fread(*out_buffer, 1, *out_length, f);
    if (ret == 0) {
        fnprintf("Couldn't read file\n");
        ret = -1;
        goto out_free_memory;
    } else {
        if (feof(f) || ferror(f)) {
            fnprintf("Error reading file\n");
            ret = -1;
            goto out_free_memory;
        } else
            ret = 0;
    }
    goto out_close_file;

out_free_memory:
    free(*out_buffer);

out_close_file:
    fclose(f);

    return ret;
}

int __hidden smbios_get_table_firm_tables(struct smbios_table *m)
{
    int retval = -1; //fail
    const char *error = _("Could not open Table Entry Point.");
    const char *dirname;
    char *entry_fname = NULL;
    char *dmi_fname = NULL;
    char *entry_buffer = NULL;
    long entry_length;

    /* standard */
    if (!m->table_path)
        dirname = "/sys/firmware/dmi/tables";
    /* unit test */
    else
        dirname = m->table_path;

    if (asprintf(&entry_fname, "%s/smbios_entry_point", dirname) < 0)
        goto out_err;
    fnprintf("Using %s for entry point\n", entry_fname);

    if (asprintf(&dmi_fname, "%s/DMI", dirname) < 0)
        goto out_free_entry_path;
    fnprintf("Using %s for DMI\n", dmi_fname);

    fnprintf("\n");
    retval = -1;

    if (read_file(entry_fname, 5, &entry_buffer, &entry_length))
        goto out_free_dmi_path;

    error = _("Invalid SMBIOS table signature");
    /* parse SMBIOS structure */
    if (memcmp (entry_buffer, "_SM_", 4) == 0) {
        if (!smbios_verify_smbios (entry_buffer, entry_length, &m->table_length))
            goto out_free_entry_buffer;
    /* parse SMBIOS 3.0 structure */
    } else if (memcmp (entry_buffer, "_SM3_", 5) == 0) {
    if (!smbios_verify_smbios3 (entry_buffer, entry_length, &m->table_length))
        goto out_free_entry_buffer;
    } else
        goto out_free_entry_buffer;

    error = _("Could not read table from memory. ");
    m->table = (struct table*)calloc(1, m->table_length);
    if (!m->table)
        goto out_free_entry_buffer;

    retval = read_file(dmi_fname, m->table_length, (char**) &m->table, &m->table_length);
    if (retval)
        goto out_free_table;
    goto out;

out_free_table:
    fnprintf(" out_free_table\n");
    free(m->table);
    m->table = 0;

out_free_entry_buffer:
    free(entry_buffer);

out_free_dmi_path:
    free(dmi_fname);

out_free_entry_path:
    free(entry_fname);

out_err:
    fnprintf(" out_err\n");
    if (strlen(m->errstring))
        strlcat(m->errstring, "\n", ERROR_BUFSIZE);
    strlcat (m->errstring, error, ERROR_BUFSIZE);
    return retval;

out:
    free(entry_buffer);
    fnprintf(" out: %d\n", retval);
    return retval;
}

/* this method is only designed to work with SMBIOS 2.0
   - it is also what pythong unit tests will use.
   - when the unit tests are changed over to use sysfs files
     then this method should also be dropped
*/
int __hidden smbios_get_tep_memory(struct smbios_table *table, u64 *address, long *length)
{
    int retval = 0;
    unsigned long fp = E_BLOCK_START;
    const char *errstring;
    u8 *block = malloc(sizeof(struct smbios_table_entry_point));
    if (!block)
        goto out_block;

    fnprintf("\n");

    // tell the memory subsystem that it can optimize here and
    // keep memory open while we scan rather than open/close/open/close/...
    // for each fillBuffer() call
    memory_suggest_leave_open();
    errstring = _("Could not read physical memory. Lowlevel error was:\n");
    while ( (fp + sizeof(struct smbios_table_entry_point)) < F_BLOCK_END)
    {
        int ret = memory_read(block, fp, sizeof(struct smbios_table_entry_point));
        if (ret)
            goto out_memerr;

        /* look for SMBIOS 2.x style header */
        if ((memcmp (block, "_SM_", 4) == 0))
        {
            struct smbios_table_entry_point *tempTEP = (struct smbios_table_entry_point *) block;
            errstring = _("Found _SM_ anchor but could not parse SMBIOS structure.");
            dbg_printf("Found _SM_ anchor. Trying to parse SMBIOS structure.\n");
            if(smbios_verify_smbios((char*) tempTEP, tempTEP->eps_length, length)) {
                *address = tempTEP->dmi.table_address;
                break;
            }
        }

        fp += 16;
    }

    // dont need memory optimization anymore
    memory_suggest_close();

    // bad stuff happened if we got to here and fp > 0xFFFFFL
    errstring = _("Did not find smbios table entry point in memory.");
    if ((fp + sizeof(struct smbios_table_entry_point)) >= F_BLOCK_END)
        goto out_notfound;

    retval = 1;
    goto out;

out_memerr:
    fnprintf("out_memerr: %s\n", errstring);
    strlcat (table->errstring, errstring, ERROR_BUFSIZE);
    fnprintf(" ->memory_strerror()\n");
    strlcat (table->errstring, memory_strerror(), ERROR_BUFSIZE);
    goto out;

out_notfound:
    fnprintf("out_notfound\n");
    strlcat (table->errstring, errstring, ERROR_BUFSIZE);
    goto out;

out_block:
    fnprintf("out_block\n");
    return retval;

out:
    free(block);
    fnprintf("out\n");
    return retval;
}

int __hidden smbios_get_table_memory(struct smbios_table *m)
{
    int retval = -1; //fail
    const char *error = _("Could not find Table Entry Point.");
    u64 address;

    fnprintf("\n");

    if (!smbios_get_tep_memory(m, &address, &m->table_length))
        goto out_err;

    error = _("Found table entry point but could not read table from memory. ");
    m->table = (struct table*)calloc(1, m->table_length);
    retval = memory_read(m->table, address, m->table_length);
    if (retval != 0)
        goto out_free_table;

    goto out;
out_free_table:
    fnprintf(" out_free_table\n");
    free(m->table);
    m->table = 0;
out_err:
    fnprintf(" out_err\n");
    if (strlen(m->errstring))
        strlcat(m->errstring, "\n", ERROR_BUFSIZE);
    strlcat (m->errstring, error, ERROR_BUFSIZE);
out:
    fnprintf(" out\n");
    return retval;
}
