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

// forward declarations
void __internal init_token_table(struct token_table *);
void __internal _token_table_free(struct token_table *);

// static vars
static struct token_table singleton; // auto-init to 0

struct token_table *token_factory(int flags, ...)
{
    struct token_table *toReturn = 0;

    dprintf("DEBUG: token_factory()\n");

    if (flags==TOKEN_DEFAULTS)
        flags = TOKEN_GET_SINGLETON;

    if (flags & TOKEN_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = (struct token_table *)calloc(1, sizeof(struct token_table));

    if (toReturn->initialized)
        goto out;

    init_token_table(toReturn);

out:
    return toReturn;
}


void token_table_free(struct token_table *m)
{
    if (m != &singleton)
        _token_table_free(m);

    // can do special cleanup for singleton, but none necessary atm
}

const struct token_obj *token_get_next(const struct token_table *t, const struct token_obj *cur)
{
    if (!cur)
        return t->list_head;

    return cur->next;
}

const struct token_obj *token_get_next_by_id(const struct token_table *t, const struct token_obj *cur, u16 id)
{
    dprintf("%s\n", __PRETTY_FUNCTION__);
    do {
        cur = token_get_next(t, cur);
        dprintf("look for %d, got %d\n", id, token_obj_get_id(cur));
        if (cur && token_obj_get_id(cur) == id)
            break;
    } while ( cur );
    return cur;
}

#define makeit(ret, defret, callname) \
    ret token_obj_##callname (const struct token_obj *t)    \
    {\
        dprintf("%s\n", __PRETTY_FUNCTION__);       \
        if (t) return t-> callname (t);     \
        return defret;\
    }

makeit( const char *, 0, get_type )
makeit( u16, 0, get_id )
makeit( bool, 0, is_active )
makeit( int, 0, activate )
makeit( char *, 0, get_string )
makeit( bool, 0, is_bool )
makeit( bool, 0, is_string )

int token_obj_set_string(const struct token_obj *t, const char *newstr, size_t size)
{
    if (t)
        return t->set_string (t, newstr, size);
    return 0;
}

int token_obj_try_password(const struct token_obj *t, const char *password)
{
    if (t)
        return t->try_password (t, password);
    return 0;
}


const struct smbios_struct *token_obj_get_smbios_struct(const struct token_obj *t)
{
    return t->smbios_structure;
}

void token_free_string(char *s)
{
    free(s);
}

#define makeit2(ret, defret, callname)\
    ret token_##callname (u16 id)    \
    {\
        struct token_table *table = 0;              \
        const struct token_obj *token = 0;          \
        dprintf("%s\n", __PRETTY_FUNCTION__);       \
        table = token_factory(TOKEN_GET_SINGLETON); \
        if (!table) goto out;                       \
        token = token_get_next_by_id(table, 0, id); \
        if (!token) goto out;                       \
        return token -> callname (token);          \
out:\
        return defret;  \
    }

makeit2(bool, 0, is_bool)
makeit2(bool, 0, is_active)
makeit2(int, 0, activate)
makeit2(bool, 0, is_string)
makeit2(char *, 0, get_string)

int token_set_string(u16 id, const char *newstr, size_t size)
{
    struct token_table *table = 0;
    const struct token_obj *token = 0;
    table = token_factory(TOKEN_GET_SINGLETON);
    if (!table) goto out;
    token = token_get_next_by_id(table, 0, id);
    if (!token) goto out;
    return token -> set_string (token, newstr, size);
out:
    return 0;
}


/**************************************************
 *
 * Internal functions
 *
 **************************************************/

void __internal _token_table_free(struct token_table *this)
{
    struct token_obj *ptr = this->list_head;
    struct token_obj *next = 0;

    smbios_table_free(this->smbios_table);

    while(ptr)
    {
        next = 0;
        if (ptr->next)
            next = ptr->next;

        free(ptr);
        ptr = next;
    }

    this->list_head = 0;
}

void __internal add_token(struct token_table *t, struct token_obj *o)
{
    struct token_obj *ptr = t->list_head;

    while(ptr && ptr->next)
        ptr = ptr->next;

    if(ptr)
        ptr->next = o;
    else
        t->list_head = o;
}

void __internal init_token_table(struct token_table *t)
{
    struct smbios_table *table = smbios_factory(SMBIOS_GET_SINGLETON);

    t->smbios_table = table;

    add_d4_tokens(t);

    t->initialized = 1;
}


