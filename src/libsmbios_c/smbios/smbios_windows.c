/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2018 Dell Inc.
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

#include <stdio.h>

// public
#include "smbios_c/obj/smi.h"
#include "smbios_c/types.h"
#include "libsmbios_c_intlize.h"
#include "internal_strl.h"

// private
#include "smi_impl.h"

int __hidden smbios_get_table_firm_tables(struct smbios_table *m)
{
    printf("WINDOWS SMBIOS NOT IMPLEMENTED YET!!!! \n");
    return -1;
}

int __hidden smbios_get_table_memory(struct smbios_table *m)
{
    printf("WINDOWS SMBIOS NOT IMPLEMENTED YET!!!! \n");
    return -1;
}