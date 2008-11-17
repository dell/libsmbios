/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
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

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system
#include <stdlib.h>
//#include <string.h>

// public
#include "smbios_c/obj/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/smbios.h"
#include "smbios_c/token.h"
#include "smbios_c/types.h"

// private
#include "token_impl.h"

const char * token_strerror()
{
    const char *retval = 0;
    struct token_table *table = token_table_factory(TOKEN_DEFAULTS | TOKEN_NO_ERR_CLEAR);
    fnprintf("\n");
    if (table)
        retval = token_table_strerror(table);
    return retval;
}

#define make_token_fn(ret, defret, callname)\
    ret token_##callname (u16 id)    \
    {\
        struct token_table *table = 0;              \
        const struct token_obj *token = 0;          \
        fnprintf("\n"); \
        table = token_table_factory(TOKEN_DEFAULTS); \
        if (!table) goto out;                       \
        token = token_table_get_next_by_id(table, 0, id); \
        if (!token) goto out;                       \
        return token_obj_##callname (token);                    \
out:\
        return defret;  \
    }

make_token_fn(int, 0, get_type)
make_token_fn(bool, 0, is_bool)
make_token_fn(int, 0, is_active)
make_token_fn(int, 0, activate)
make_token_fn(bool, 0, is_string)
make_token_fn(const void *, 0, get_ptr)
make_token_fn(const struct smbios_struct *, 0, get_smbios_struct)

char * token_get_string (u16 id, size_t *len)
{
    struct token_table *table = 0;
    const struct token_obj *token = 0;
    fnprintf("\n");
    table = token_table_factory(TOKEN_DEFAULTS);
    if (!table) goto out;
    token = token_table_get_next_by_id(table, 0, id);
    if (!token) goto out;
    return token_obj_get_string(token, len);
out:
    return 0;
}

void token_string_free(char *s)
{
    fnprintf("\n");
    free(s);
}

int token_set_string(u16 id, const char *newstr, size_t size)
{
    struct token_table *table = 0;
    const struct token_obj *token = 0;
    fnprintf("\n");
    table = token_table_factory(TOKEN_DEFAULTS);
    if (!table) goto out;
    token = token_table_get_next_by_id(table, 0, id);
    if (!token) goto out;
    return token -> set_string (token, newstr, size);
out:
    return 0;
}

int token_try_password(u16 id, const char *pass_ascii, const char *pass_scancode)
{
    struct token_table *table = 0;
    const struct token_obj *token = 0;
    fnprintf("\n");
    table = token_table_factory(TOKEN_DEFAULTS);
    if (!table) goto out;
    token = token_table_get_next_by_id(table, 0, id);
    if (!token) goto out;
    return token -> try_password (token, pass_ascii, pass_scancode);
out:
    return 0;
}



