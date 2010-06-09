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


#ifndef C_SMBIOS_H
#define C_SMBIOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

struct smbios_struct;

/** Function for looping over smbios table structures.
 * Returns a pointer to the next smbios structure. You can cast this structure
 * to a specific smbios structure, or you can use the generic access methods to
 * pull data out of the structure. Returns 0 on end of table.
 *  @param cur  pointer to current structure, or 0 to begin at the start of the table.
 *  @return  Pointer to next smbios structure. returns 0 on end of table.
 */
LIBSMBIOS_C_DLL_SPEC struct smbios_struct * smbios_get_next_struct(const struct smbios_struct *cur);


/** Function for looping over smbios table structures by type.
 * Returns a pointer to the next smbios structure with a given type. You can
 * cast this structure to a specific smbios structure, or you can use the
 * generic access methods to pull data out of the structure. Returns 0 on end
 * of table.
 *  @param cur  pointer to current structure, or 0 to begin at the start of the table.
 *  @param type  only return smbios structures matching type
 *  @return  Pointer to next smbios structure. returns 0 on end of table.
 */
LIBSMBIOS_C_DLL_SPEC struct smbios_struct * smbios_get_next_struct_by_type(const struct smbios_struct *cur, u8 type);

/** Function for looping over smbios table structures by handle.
 * Returns a pointer to the next smbios structure with a given handle. You can
 * cast this structure to a specific smbios structure, or you can use the
 * generic access methods to pull data out of the structure. Returns 0 on end
 * of table.
 *  @param cur  pointer to current structure, or 0 to begin at the start of the table.
 *  @param handle  only return smbios structures matching handle
 *  @return  Pointer to next smbios structure. returns 0 on end of table.
 */
LIBSMBIOS_C_DLL_SPEC struct smbios_struct * smbios_get_next_struct_by_handle(const struct smbios_struct *cur, u16 handle);

/** Call a named function for each smbios structure.
 * Calls the given function for each smbios table structure. Passes a pointer
 * to the structure as well as the userdata pointer given.
 * @param fn  pointer to the function to call
 * @param userdata  opaque pointer that will be passed to the funciton
 */
LIBSMBIOS_C_DLL_SPEC void smbios_walk(void (*fn)(const struct smbios_struct *, void *userdata), void *userdata);

/** looping helper macro.
 * This macro makes it easy to loop over each structure in the smbios table
 */
#define smbios_for_each_struct(struct_name)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct(0);\
            struct_name;\
            struct_name = smbios_get_next_struct(struct_name)\
           )

/** looping helper macro.
 * This macro makes it easy to loop over specific structure types in the smbios
 * table
 */
#define smbios_for_each_struct_type(struct_name, struct_type)  \
        for(    \
            const struct smbios_struct *struct_name = smbios_get_next_struct_by_type(0, struct_type);\
            struct_name;\
            struct_name = smbios_get_next_struct_by_type(struct_name, struct_type)\
           )

/** Returns the structure type of a given smbios structure. */
LIBSMBIOS_C_DLL_SPEC u8 smbios_struct_get_type(const struct smbios_struct *);

/** Returns the structure length of a given smbios structure. */
LIBSMBIOS_C_DLL_SPEC u8 smbios_struct_get_length(const struct smbios_struct *);

/** Returns the structure handle of a given smbios structure. */
LIBSMBIOS_C_DLL_SPEC u16 smbios_struct_get_handle(const struct smbios_struct *);

/** Copy data out of the smbios structure.
 * Does bounds-checking to ensure that structure overflows do not happen.
 * @param s  pointer to structure to access
 * @param dest  pointer to user-allocated buffer to fill
 * @param offset offset in structure
 * @param len  length of data to copy into buffer
 * @return returns 0 on success, <0 on failure.
 */
LIBSMBIOS_C_DLL_SPEC int smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len);

/** get string from smbios structure.
 * Most smbios structures have specific offsets that contain a string number.
 * This function will look up the offset and retrieve the pointed-to string.
 * @param s pointer to smbios structure
 * @param offset  offset containing string pointer
 * @return returns a pointer to the string, or 0 on failure.
 */
LIBSMBIOS_C_DLL_SPEC const char * smbios_struct_get_string_from_offset(const struct smbios_struct *s, u8 offset);

/** get string from smbios structure.
 * Retrieves string N from the end of a smbios structure.
 * @param s pointer to smbios structure
 * @param which string number to return
 */
LIBSMBIOS_C_DLL_SPEC const char * smbios_struct_get_string_number(const struct smbios_struct *s, u8 which);

/** Returns string describing the last error condition.
 * Can return 0. The buffer used is guaranteed to be valid until the next call
 * to any smbios_* function. Copy the contents if you need it longer.
 */
LIBSMBIOS_C_DLL_SPEC const char * smbios_strerror();

EXTERN_C_END;

#endif  /* SMBIOS_H */
