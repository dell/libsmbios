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

bool token_is_bool(u16 id);
bool token_is_active(u16 id);
int token_activate(u16 id);

bool token_is_string(u16 id);
char *token_get_string(u16 id);
int token_set_string(u16 id, const char *, size_t size);

#ifndef TOKEN_FREE_STRING
#define TOKEN_FREE_STRING
void token_free_string(char *);
#endif

const struct smbios_struct *token_get_smbios_struct(u16 id);

EXTERN_C_END;

// always should be last thing in header file
#include "smbios_c/config/abi_suffix.h"

#endif  /* TOKEN_H */
