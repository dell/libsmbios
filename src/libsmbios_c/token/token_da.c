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
#include <string.h>

// public
#include "smbios_c/obj/cmos.h"
#include "smbios_c/cmos.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/smi.h"
#include "smbios_c/types.h"

// private
#include "token_impl.h"

// helpers so we dont get line lengths from heck.
#define cast_token(t)  ((struct calling_interface_token *)(t->token_ptr))
#define cast_struct(t) ((struct calling_interface_structure *)token_obj_get_smbios_struct(t))

static int _da_get_type(const struct token_obj *t)
{
    fnprintf("\n");
    return 0xDA;
}

static int _da_get_id(const struct token_obj *t)
{
    fnprintf("\n");
    return cast_token(t)->tokenId;
}

// pretend DA tokens are bool.
// if user wants to get fancy, use the raw smi write_nv_data functions...

static int _da_is_bool(const struct token_obj *t)
{
    fnprintf("\n");
    return true;
}

static int _da_is_string(const struct token_obj *t)
{
    fnprintf("\n");
    return false;
}

static int _da_is_active(const struct token_obj *t)
{
    fnprintf("\n");
    int ret = false;
    if (cast_token(t)->value == dell_smi_read_nv_storage(cast_token(t)->location, 0, 0))
        ret = true;
    return ret;
}

// wonky to get around GCC error: "cast from pointer to integer of different size"
// as well as "warning: dereferencing type-punned pointer will break strict-aliasing rules"
// should be just security_key = (u16)t->private_data;
union void_u16 {
    void *ptr;
    u16   val;
};

static int _da_activate(const struct token_obj *t)
{
    fnprintf("\n");
    union void_u16 indirect;
    indirect.ptr = t->private_data;
    dell_smi_write_nv_storage(indirect.val, cast_token(t)->location, cast_token(t)->value);
    return 0;
}

static int _da_try_password(const struct token_obj *t, const char *pass_ascii, const char *pass_scan)
{
    union void_u16 indirect;
    indirect.ptr = t->private_data;
    const char *whichpw = pass_scan;
    if (dell_smi_password_format(DELL_SMI_PASSWORD_ADMIN) == DELL_SMI_PASSWORD_FMT_ASCII)
        whichpw=pass_ascii;
    return dell_smi_get_security_key(whichpw, &(indirect.val));
}

void __internal init_da_token(struct token_obj *t)
{
    fnprintf("\n");
    t->get_type = _da_get_type;
    t->get_id = _da_get_id;
    t->is_bool = _da_is_bool;
    t->is_string = _da_is_string;
    t->is_active = _da_is_active;
    t->activate = _da_activate;
    t->get_string = 0;
    t->get_string_len = 0;
    t->set_string = 0;
    t->try_password = _da_try_password;
    t->private_data = 0;
}

void __internal add_da_tokens(struct token_table *t)
{
    fnprintf("\n");
    smbios_table_for_each_struct_type(t->smbios_table, s, 0xDA) {
        struct calling_interface_structure *d4_struct = (struct calling_interface_structure*)s;
        struct calling_interface_token *token = d4_struct->tokens;


        while (token->tokenId != TokenTypeEOT) {
            struct token_obj *n = calloc(1, sizeof(struct token_obj));
            if (!n)
                goto out_err;

            n->token_ptr = token;
            n->smbios_structure = s;
            init_da_token(n);
            add_token(t, n);
            token++;
        }
    }
    goto out;
out_err:
    // really should do something here
out:
    return;
}



