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


#ifndef TOKEN_H
#define TOKEN_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

EXTERN_C_BEGIN;

#define TOKEN_DEFAULTS       0x0000
#define TOKEN_GET_SINGLETON  0x0001
#define TOKEN_GET_NEW        0x0002
#define TOKEN_UNIT_TEST_MODE 0x0004

struct token_table;
struct token_obj;

// construct
struct token_table *token_factory(int flags, ...);

// destruct
void token_table_free(struct token_table *);

// format error string
size_t token_fmt_err(struct token_table *, char *buf, size_t len);

// for looping/searching
const struct token_obj *token_get_next(const struct token_table *, const struct token_obj *cur);
const struct token_obj *token_get_next_by_id(const struct token_table *, const struct token_obj *cur, u16 id);

u16 token_obj_get_id(const struct token_obj *);

#define token_for_each(table_name, struct_name)  \
        for(    \
            const struct token_obj *struct_name = token_get_next(table_name, 0);\
            struct_name;\
            struct_name = token_get_next(table_name, struct_name)\
           )

#define token_for_each_id(table_name, struct_name, id)  \
        for(    \
            const struct token_obj *struct_name = token_get_next_id(table_name, 0, id);\
            struct_name;\
            struct_name = token_get_next_id(table_name, struct_name, id)\
           )

const char * token_obj_get_type(const struct token_obj *);
int token_obj_is_bool(const struct token_obj *);
int token_obj_is_active(const struct token_obj *);
int token_obj_activate(const struct token_obj *);

int token_obj_is_string(const struct token_obj *);
char* token_obj_get_string(const struct token_obj *);
int token_obj_set_string(const struct token_obj *, const char *);

const struct smbios_struct *token_obj_get_smbios_struct(const struct token_obj *);
int token_obj_try_password(const struct token_obj *, const char *);

void token_free_string(char *);

int token_is_bool(u16 id);
int token_is_active(u16 id);
int token_activate(u16 id);

int token_is_string(u16 id);
const char *token_get_string(u16 id);
int token_set_string(u16 id, const char *);


EXTERN_C_END;

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* TOKEN_H */
