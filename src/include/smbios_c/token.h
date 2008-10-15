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


#ifndef C_TOKEN_H
#define C_TOKEN_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define TOKEN_TYPE_D4  0xD4
#define TOKEN_TYPE_D5  0xD5
#define TOKEN_TYPE_D6  0xD6
#define TOKEN_TYPE_DA  0xDA

/** Return token type.
 * tokens can be 0xD4, 0xD5, 0xD6, or 0xDA tokens, depending on the smbios
 * table structure they come from.
 */
int DLL_SPEC token_get_type(u16 id);

/** Check if a token is a boolean-type token.
 * @return true if token is bool, false if otherwise
 */
bool DLL_SPEC token_is_bool(u16 id);
/** Check if a boolean token is currently set.
 * @return true if token is active (set), false if otherwise
 */
bool DLL_SPEC token_is_active(u16 id);
/** Activate a boolean token.
 * @return 0 on success, <0 on failure.
 */
int DLL_SPEC token_activate(u16 id);

/** Check if a token is a string-type token.
 * @return true if token is a string, false otherwise.
 */
bool DLL_SPEC token_is_string(u16 id);

/** Get a new buffer containing the token string value.
 * @param id token id to get
 * @param len pointer to size_t where final string size will be stored. Size
 * does not include final '\\0', but may not always be equal to strlen() if cmos
 * has embedded '\\0' chars. You must use token_free_string() to free this
 * buffer, or memory will leak.
 * @return pointer to allocated buffer (note: use token_free_string() to free
 * this value). 0 on failure.
 */
char * DLL_SPEC token_get_string(u16 id, size_t *len);

/** Set a string token value.
 * @param id token id
 * @param value pointer to new token buffer
 * @param size size of the buffer
 * @return 0 on success, <0 on failure.
 */
int DLL_SPEC token_set_string(u16 id, const char *value, size_t size);

/** Free allocated memory.
 * Use this to free any memory buffer pointers that you get from this module.
 * For example, token_get_string(...)
 */
void DLL_SPEC token_free_string(char *);

/** Get a pointer to the smbios struct containing this token.
 */
const struct smbios_struct * DLL_SPEC token_get_smbios_struct(u16 id);

/** Get a pointer to the actual token structure
 */
const void * DLL_SPEC token_get_ptr(u16 id);

EXTERN_C_END;

// always should be last thing in header file
#include "smbios_c/config/abi_suffix.h"

#endif  /* TOKEN_H */
