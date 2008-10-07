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

void dell_smi_calling_interface_simple(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);
u32  dell_smi_get_authentication_key(const char *pass);

enum password_format_enum { PW_FORMAT_UNKNOWN, PW_FORMAT_SCAN_CODE, PW_FORMAT_ASCII };
enum password_format_enum dell_smi_get_password_format();

EXTERN_C_END;

#endif  /* C_SMI_H */
