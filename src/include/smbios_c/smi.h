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

#ifndef C_SMI_H
#define C_SMI_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

enum {
    CB_ARG1 = 0,
    CB_ARG2 = 1,
    CB_ARG3 = 2,
    CB_ARG4 = 3,
    CB_RES1 = 0,
    CB_RES2 = 1,
    CB_RES3 = 2,
    CB_RES4 = 3,
};

void dell_smi_calling_interface_simple(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);

EXTERN_C_END;

#endif  /* C_SMI_H */
