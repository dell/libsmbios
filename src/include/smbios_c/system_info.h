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

EXTERN_C_BEGIN;

//////////////////////////////////////////////////////////////////////////
//
// Stable API section.
//
// All of the functions in this section have a strong guarantee that we
// will not break API compatibility.
//
//////////////////////////////////////////////////////////////////////////

/** Return a string representing the version of the libsmbios library.
 * This string is statically allocated in the library, so there is no need to
 * free it when done.
 */
const char * DLL_SPEC smbios_get_library_version_string();
//! Return a number representing the major version of the libsmbios library.
int DLL_SPEC  smbios_get_library_version_major();
//! Return a number representing the minor version of the libsmbios library.
int DLL_SPEC smbios_get_library_version_minor();


//! Return the Dell System ID Byte or Word
/** The Dell System ID is a unique number allocated to each Dell System
 * (server, desktop, workstation, or laptop) that uniquely identifies that
 * system within Dell's product line.
 */
int  DLL_SPEC  sysinfo_get_dell_system_id();

/** Return a buffer containing the system vendor name.
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing vendor name. Deallocate with
 * sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_vendor_name();

/** Return a buffer containing the system name.
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing system name. Deallocate with
 * sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_system_name();

/** Return a buffer containing the system bios version string.
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing system bios version string. Deallocate
 * with sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_bios_version();

/** Return a buffer containing the system asset tag string.
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing system asset tag string. Deallocate
 * with sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_asset_tag();

/** Return a buffer containing the system service tag string.
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing system service tag string. Deallocate
 * with sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_service_tag();

/** copy property ownership tag into user-supplied buffer.
 * @param tagBuf  user must allocate 81-byte null-filled buffer to hold tag
 * @param size  indicates size of buffer. If smaller buffer is passed, result may be truncated
 * @return 0 on success
 */
int DLL_SPEC sysinfo_get_property_ownership_tag(char *tagBuf, size_t size);

/** Set system property ownership tag
 * @param security_key  if system is password protected, this must be passed
 * @param newTag buffer holding new tag
 * @param size size of buffer
 * @return  0 on success
 */
int DLL_SPEC sysinfo_set_property_ownership_tag(u16 security_key, const char *newTag, size_t size);

/** Free string.
 * Use this function to deallocate the strings returned by other functions in
 * this header.
 */
void DLL_SPEC  sysinfo_string_free(void *);

EXTERN_C_END;

#endif  /* SYSTEMINFO_H */
