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

// public
#include "smbios_c/memory.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/types.h"

#include "memory_impl.h"

void  memory_suggest_leave_open()
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON);
    memory_obj_suggest_leave_open(m);
    memory_obj_free(m);
}

void  memory_suggest_close()
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON);
    memory_obj_suggest_close(m);
    memory_obj_free(m);
}

int  memory_read(void *buffer, u64 offset, size_t length)
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON);
    int retval = memory_obj_read(m, buffer, offset, length);
    memory_obj_free(m);
    return retval;
}

int  memory_write(void *buffer, u64 offset, size_t length)
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON);
    int retval = memory_obj_write(m, buffer, offset, length);
    memory_obj_free(m);
    return retval;
}

s64  memory_search(const char *pat, size_t patlen, u64 start, u64 end, u64 stride)
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON);
    int retval = memory_obj_search(m, pat, patlen, start, end, stride);
    memory_obj_free(m);
    return retval;
}

const char * memory_strerror()
{
    struct memory_access_obj *m = memory_obj_factory(MEMORY_GET_SINGLETON|MEMORY_NO_ERR_CLEAR);
    const char * retval = memory_obj_strerror(m);
    memory_obj_free(m);
    return retval;
}

