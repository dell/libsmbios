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
#include "smbios_c/types.h"

// private
#include "token_impl.h"

// helpers so we dont get line lengths from heck.
#define cast_token(t)  ((struct indexed_io_token *)(t->token_ptr))
#define cast_struct(t) ((struct indexed_io_access_structure *)token_obj_get_smbios_struct(t))

static int _d4_get_type(const struct token_obj *t)
{
    return 0xD4;
}

static int _d4_get_id(const struct token_obj *t)
{
    dbg_printf("_d4_get_id\n");
    return cast_token(t)->tokenId;
}

static int _d4_is_bool(const struct token_obj *t)
{
    return cast_token(t)->andMask != 0;
}

static int _d4_is_string(const struct token_obj *t)
{
    return cast_token(t)->andMask == 0;
}

static int _d4_is_active(const struct token_obj *t)
{
    bool retval = false;
    u8 byte=0;
    int ret;

    if (! _d4_is_bool(t))
        goto out_err;

    ret = cmos_read_byte(&byte,
                  cast_struct(t)->indexPort,
                  cast_struct(t)->dataPort,
                  cast_token(t)->location
              );
    if(ret<0) goto out_err;

    if( (byte & (~cast_token(t)->andMask)) == cast_token(t)->orValue  )
        retval = true;

    goto out;

out_err:
    // really should do something here.

out:
    return retval;
}

static int _d4_activate(const struct token_obj *t)
{
    int retval = -1;
    u8 byte = 0;
    int ret;

    if (! _d4_is_bool(t))
        goto out_err;

    ret = cmos_read_byte(&byte,
                  cast_struct(t)->indexPort,
                  cast_struct(t)->dataPort,
                  cast_token(t)->location
              );
    if(ret<0) goto out_err;

    byte = byte & cast_token(t)->andMask;
    byte = byte | cast_token(t)->orValue;

    ret = cmos_write_byte(byte,
        cast_struct(t)->indexPort,
        cast_struct(t)->dataPort,
        cast_token(t)->location
        );
    if(ret<0) goto out_err;

    retval = 0;
    goto out;

out_err:
    // really should do something here

out:
    return retval;
}

static int _d4_get_string_len(const struct token_obj *t)
{
    // strings always at least 1, no matter what our buggy tables say.
    return cast_token(t)->stringLength ? cast_token(t)->stringLength : 1;
}

static char * _d4_get_string(const struct token_obj *t, size_t *len)
{
    u8 *retval = 0;
    size_t strSize = 0;

    dbg_printf("_d4_get_string()\n");

    dbg_printf("_d4_get_string() - is string?\n");
    if (! _d4_is_string(t))
        goto out_err;

    strSize = _d4_get_string_len(t);
    if(len)
        *len = strSize;

    dbg_printf("_d4_get_string() - alloc string %ld bytes\n", strSize + 1);
    retval = calloc(1, strSize+1);
    if (!retval)
        goto out_err;

    for (int i=0; i<strSize; ++i){
        dbg_printf("_d4_get_string() - read byte %d/%ld\n", i+1, strSize);
        int ret = cmos_read_byte(retval + i,
                  cast_struct(t)->indexPort,
                  cast_struct(t)->dataPort,
                  cast_token(t)->location + i
              );
        if(ret<0) goto out_err;
    }
    goto out;

out_err:
    dbg_printf("_d4_get_string() - out_err\n");
    free(retval);
    retval = 0;

out:
    dbg_printf("_d4_get_string() - out\n");
    return (char *)retval;
}

static int _d4_set_string(const struct token_obj *t, const char *str, size_t size)
{
    u8 retval = 0;
    size_t strSize = _d4_get_string_len(t);
    char *targetBuffer = calloc(1, strSize);

    if (!targetBuffer)
        goto out_err;

    if (! _d4_is_string(t))
        goto out_err;

    memset(targetBuffer, 0, strSize);
    memcpy( targetBuffer, str, size < strSize ? size : strSize );

    for (int i=0; i<strSize; ++i){
        int ret = cmos_write_byte(targetBuffer[i],
                  cast_struct(t)->indexPort,
                  cast_struct(t)->dataPort,
                  cast_token(t)->location + i
              );
        if(ret<0) goto out_err;
    }

    goto out;

out_err:
    retval = -1;

out:
    free(targetBuffer);
    return retval;
}

void __internal init_d4_token(struct token_obj *t)
{
    t->get_type = _d4_get_type;
    t->get_id = _d4_get_id;
    //t->get_flags = _d4_get_flags;
    t->is_bool = _d4_is_bool;
    t->is_string = _d4_is_string;
    t->is_active = _d4_is_active;
    t->activate = _d4_activate;
    t->get_string = _d4_get_string;
    t->get_string_len = _d4_get_string_len;
    t->set_string = _d4_set_string;
    t->try_password = 0;
    t->private_data = 0;
}

void setup_d4_checksum(struct indexed_io_access_structure *d4_struct)
{
    struct checksum_details *d = 0;
    struct cmos_access_obj *c = cmos_obj_factory(CMOS_GET_SINGLETON);

    if (!c)
        goto out_err;

    // if all zeros, there is no checksum
    if (!(d4_struct->checkedRangeStartIndex || d4_struct->checkedRangeEndIndex || d4_struct->checkValueIndex))
        goto out;

    d = calloc(1, sizeof(struct checksum_details));

    d->csumloc   = d4_struct->checkValueIndex;
    d->csumlen   = sizeof(u16);
    d->start     = d4_struct->checkedRangeStartIndex;
    d->end       = d4_struct->checkedRangeEndIndex;
    d->indexPort = d4_struct->indexPort;
    d->dataPort  = d4_struct->dataPort;
    d->checkType = d4_struct->checkType;

    switch(d->checkType) {
        case CHECK_TYPE_BYTE_CHECKSUM:
            d->csum_fn = byteChecksum;
            d->csumlen = sizeof(u8);
            break;
        case CHECK_TYPE_WORD_CHECKSUM:
            d->csum_fn = wordChecksum;
            break;
        case CHECK_TYPE_WORD_CHECKSUM_N:
            d->csum_fn = wordChecksum_n;
            break;
        case CHECK_TYPE_WORD_CRC:
            d->csum_fn = wordCrc;
            break;
        default:
            break;
    }

    cmos_obj_register_write_callback(c, update_checksum, d, free);
    goto out;
out_err:
    // really should do something here

out:
    return;
}

void __internal add_d4_tokens(struct token_table *t)
{
    smbios_table_for_each_struct_type(t->smbios_table, s, 0xD4) {
        struct indexed_io_access_structure *d4_struct = (struct indexed_io_access_structure*)s;
        struct indexed_io_token *token = d4_struct->tokens;

        setup_d4_checksum(d4_struct);

        while (token->tokenId != TokenTypeEOT) {
            struct token_obj *n = calloc(1, sizeof(struct token_obj));
            if (!n)
                goto out_err;

            n->token_ptr = token;
            n->smbios_structure = s;
            init_d4_token(n);
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



