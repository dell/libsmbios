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

#define LIBSMBIOS_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system
#include <stdlib.h>
//#include <string.h>

// public
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"

// private
#include "token_impl.h"

int __internal _d4_get_id(struct token_obj *t)
{
    return ((struct indexed_io_token *)t)->tokenId;
}

void __internal init_d4_token(struct token_obj *t)
{
    t->get_id = _d4_get_id;
    get_flags = 0;
    is_active = 0;
    activate = 0;
    get_string = 0;
    set_string = 0;
    try_password = 0;
}

void __internal add_d4_tokens(struct token_table *t)
{
    smbios_for_each_struct_type(t->smbios_table, s, 0xD4) {
        struct indexed_io_access_structure *d4_struct = (struct indexed_io_access_structure*)s;
        struct indexed_io_token *token = d4_struct->tokens;

        while (token->tokenId != TokenTypeEOT) {
            struct token_obj *n = calloc(1, sizeof(struct token_obj));
            if (!n)
                goto out;

            n->token_ptr = token;
            n->smbios_structure = s;
            init_d4_token(n);
            add_token(t, n);
            token++;
        }
    }
out:
    return;
}



