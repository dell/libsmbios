// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
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

#ifndef SMBIOS_IMPL_H
#define SMBIOS_IMPL_H

#include "smbios_c/compat.h"
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define __internal __attribute__((visibility("internal")))
#define __hidden __attribute__((visibility("hidden")))

#if defined(DEBUG_SMBIOS_C)
#   define dprintf(format, args...) do { fprintf(stdout , format , ## args);  } while(0)
#else
#   define dprintf(format, args...) do {} while(0)
#endif

#define E_BLOCK_START 0xE0000UL
#define F_BLOCK_START 0xF0000UL
#define F_BLOCK_END   0xFFFFFUL

struct smbios_table
{
    int initialized;
    struct smbios_table_entry_point tep;
    struct table *table;
};

int __internal smbios_get_table_memory(struct smbios_table *m);

#if 0
int __internal smbios_get_table_efi(struct smbios_table *m);
int __internal smbios_get_table_wmi(struct smbios_table *m);
int __internal smbios_get_table_firm_tables(struct smbios_table *m);
#else
#define smbios_get_table_efi(m)         (-1)
#define smbios_get_table_wmi(m)         (-1)
#define smbios_get_table_firm_tables(m) (-1)
#endif

EXTERN_C_END;

#endif /* SMBIOS_IMPL_H */
