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
#include "smbios_c/obj/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/smbios.h"
#include "smbios_c/types.h"
#include "internal_strl.h"
#include "libsmbios_c_intlize.h"

// private
#include "token_impl.h"

// forward declarations
static int init_token_table(struct token_table *);
static void _token_table_free(struct token_table *);

// static vars
static struct token_table singleton; // auto-init to 0
static char *module_error_buf; // auto-init to 0

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

__hidden char *token_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

static void clear_err(const struct token_table *table)
{
    fnprintf("\n");
    if(table && table->errstring)
        memset(table->errstring, 0, ERROR_BUFSIZE);
    if(module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

struct token_table *token_table_factory(int flags, ...)
{
    struct token_table *toReturn = 0;
    int ret;

    fnprintf("\n");

    if (flags==TOKEN_DEFAULTS)
        flags = TOKEN_GET_SINGLETON;

    if (flags & TOKEN_GET_SINGLETON)
        toReturn = &singleton;
    else
        toReturn = calloc(1, sizeof(struct token_table));

    if (toReturn->initialized)
        goto out;

    ret = init_token_table(toReturn);
    if(ret == 0)
        goto out;

    // failed
    memset(toReturn, 0, sizeof(struct token_table));
    token_table_free(toReturn);
    toReturn = 0;

out:
    if (toReturn && ! (flags & TOKEN_NO_ERR_CLEAR))
        clear_err(toReturn);
    return toReturn;
}


void token_table_free(struct token_table *m)
{
    fnprintf("\n");
    if (m && m != &singleton)
        _token_table_free(m);

    // can do special cleanup for singleton, but none necessary atm
}

const char * token_table_strerror(const struct token_table *table)
{
    const char * retval = 0;
    fnprintf("\n");
    if (table)
        retval = table->errstring;
    else
        retval = module_error_buf;

    return retval;
}

const char * token_obj_strerror(const struct token_obj *tok)
{
    const char *ret = 0;
    fnprintf("\n");
    if (tok)
        ret = tok->errstring;
    return ret;
}

const struct token_obj *token_table_get_next(const struct token_table *t, const struct token_obj *cur)
{
    if (!t)
        return 0;

    if (!cur)
        return t->list_head;

    return cur->next;
}

const struct token_obj *token_table_get_next_by_id(const struct token_table *t, const struct token_obj *cur, u16 id)
{
    fnprintf("\n");
    do {
        cur = token_table_get_next(t, cur);
        dbg_printf("look for %d, got %d\n", id, token_obj_get_id(cur));
        if (cur && token_obj_get_id(cur) == id)
            break;
    } while ( cur );
    return cur;
}

#define make_token_obj_fn(ret, defret, callname, retfmt) \
    ret token_obj_##callname (const struct token_obj *t)    \
    {\
        fnprintf("\n"); \
        ret retval = defret;    \
        if (t && t-> callname) retval = t-> callname (t);     \
        fnprintf(" return: " retfmt "\n", retval);  \
        return retval;\
    }

make_token_obj_fn( int, 0, get_type, "0x%04x" )
make_token_obj_fn( u16, 0, get_id, "0x%04x" )
make_token_obj_fn( int, -1, is_active, "%d" )
make_token_obj_fn( int, -1, activate, "%d" )
make_token_obj_fn( bool, 0, is_bool, "%d" )
make_token_obj_fn( bool, 0, is_string, "%d" )


char * token_obj_get_string (const struct token_obj *t, size_t *len)
{
    fnprintf("\n");
    if (t && t->get_string && token_obj_is_string(t))
        return t-> get_string (t, len);
    return 0;
}

int token_obj_set_string(const struct token_obj *t, const char *newstr, size_t size)
{
    fnprintf("\n");
    if (t && t->set_string && token_obj_is_string(t))
        return t->set_string (t, newstr, size);
    return 0;
}

int token_obj_try_password(const struct token_obj *t, const char *pass_ascii, const char *pass_scan)
{
    fnprintf("\n");
    if (t && t->try_password)
        return t->try_password (t, pass_ascii, pass_scan);
    return 0;
}

const struct smbios_struct *token_obj_get_smbios_struct(const struct token_obj *t)
{
    if (t)
        return t->smbios_structure;
    return 0;
}

const void *token_obj_get_ptr(const struct token_obj *t)
{
    if (t)
        return t->token_ptr;
    return 0;
}


/**************************************************
 *
 * Internal functions
 *
 **************************************************/
static void _token_table_free_tokens(struct token_table *this)
{
    struct token_obj *ptr = this->list_head;
    struct token_obj *next = 0;

    while(ptr)
    {
        next = 0;
        if (ptr->next)
            next = ptr->next;

        // token_obj errstring generally points to token_table
        // errstring. dont free if this is the case
        if (ptr->errstring != this->errstring)
            free(ptr->errstring);
        free(ptr);
        ptr = next;
    }

    this->list_head = 0;
}

static void _token_table_free(struct token_table *this)
{
    _token_table_free_tokens(this);

    free(this->errstring);
    this->errstring = 0;

    free(this);
}

void __hidden add_token(struct token_table *t, struct token_obj *o)
{
    struct token_obj *ptr = t->list_head;

    while(ptr && ptr->next)
        ptr = ptr->next;

    if(ptr)
        ptr->next = o;
    else
        t->list_head = o;
}

int init_token_table(struct token_table *t)
{
    int retval = -1, ret;
    const char *error = _("Failed to obtain smbios table.\n");
    struct smbios_table *table = smbios_table_factory(SMBIOS_GET_SINGLETON);
    char *errbuf;
    fnprintf("\n");

    if(!table)
        goto out_tablefail;

    t->smbios_table = table;

    error = _("Memory allocation failure allocating error string.\n");
    t->errstring = calloc(1, ERROR_BUFSIZE);
    if (!t->errstring)
        goto out_allocfail;

    error = _("Error while trying to add 0xD4 tokens.\n");
    ret = add_d4_tokens(t);
    if (ret)
        goto out_tokenfail;

    ret = add_da_tokens(t);
    if (ret)
        goto out_tokenfail;

    t->initialized = 1;
    retval = 0;
    goto out;

out_tokenfail:
    fnprintf("out_tokenfail\n");
    _token_table_free_tokens(t);

out_allocfail:
    fnprintf("out_allocfail\n");
    smbios_table_free(table);

out_tablefail:
    fnprintf("out_tablefail\n");
    errbuf = token_get_module_error_buf();
    if (errbuf){
        strlcpy(errbuf, error, ERROR_BUFSIZE);
        if (t->errstring)
            strlcat(errbuf, t->errstring, ERROR_BUFSIZE);
        if (!table)
            strlcat(errbuf, smbios_table_strerror(0), ERROR_BUFSIZE); // yes, it is null
    }

out:
    fnprintf("out\n");
    return retval;
}


