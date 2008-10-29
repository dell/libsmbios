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
 * Return value *must* be de-allocated using sysinfo_string_free(), or memory
 * will leak.
 * @return pointer to buffer containing system property ownership tag string.
 * Deallocate with sysinfo_string_free() when done.
 */
char * DLL_SPEC sysinfo_get_property_ownership_tag();

/** Set system property ownership tag
 * @param newTag buffer holding new tag
 * @param pass_ascii    password as ascii bytes
 * @param pass_scancode password as keyboard scancodes
 * @return  0 on success, -1 general failure, -2 bad password
 */
int DLL_SPEC sysinfo_set_property_ownership_tag(const char *newTag, const char *pass_ascii, const char *pass_scancode);

/** set the system asset tag.
 * Note some systems store password in ascii and some store keyboard scancodes. Thus you must pass both.
 * @param assetTag  null-terminated new asset tag string
 * @param pass_ascii password to use in ascii (can be null for no pass)
 * @param pass_scancode keyboard scancode values for password (can be null for no pass)
 * @return
 *   0 == success
 *  -1 == general failure
 *  -2 == password incorrect
 */
int DLL_SPEC sysinfo_set_asset_tag(const char *assetTag, const char *pass_ascii, const char *pass_scancode);

/** Returns string describing the last error condition.
 * Can return 0. The buffer used is guaranteed to be valid until the next call
 * to any sysinfo_* function. Copy the contents if you need it longer.
 */
const char * DLL_SPEC sysinfo_strerror();

/** Free string.
 * Use this function to deallocate the strings returned by other functions in
 * this header.
 */
void DLL_SPEC  sysinfo_string_free(void *);

EXTERN_C_END;

#endif  /* SYSTEMINFO_H */
