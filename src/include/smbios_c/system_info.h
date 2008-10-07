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


#ifndef C_SYSTEM_INFO_H
#define C_SYSTEM_INFO_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

// abi_prefix should be last header included before declarations
#include "smbios_c/config/abi_prefix.h"

EXTERN_C_BEGIN;

//////////////////////////////////////////////////////////////////////////
//
// Stable API section.
//
// All of the functions in this section have a strong guarantee that we
// will not break API compatibility.
//
//////////////////////////////////////////////////////////////////////////

//! Return a string representing the version of the libsmbios library.
/** Returns the current version of the SMBIOS library as a string
 */
const char *smbios_get_library_version_string();
int smbios_get_library_version_major();
int smbios_get_library_version_minor();


//! Return the Dell System ID Byte or Word
/** The Dell System ID is a unique number allocated to each Dell System
 * (server, desktop, workstation, or laptop) that uniquely identifies that
 * system within Dell's product line.
 */
int   smbios_get_dell_system_id();
char *smbios_get_vendor_name();
char *smbios_get_system_name();
char *smbios_get_bios_version();
char *smbios_get_asset_tag();
char *smbios_get_service_tag();
void smbios_string_free(void *);

EXTERN_C_END;

// always should be last thing in header file
#include "smbios_c/config/abi_suffix.h"

#endif  /* SYSTEMINFO_H */
