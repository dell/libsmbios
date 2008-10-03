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

#ifndef OBJ_MEMORY_H
#define OBJ_MEMORY_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define MEMORY_DEFAULTS       0x0000
#define MEMORY_GET_SINGLETON  0x0001
#define MEMORY_GET_NEW        0x0002
#define MEMORY_UNIT_TEST_MODE 0x0004

struct memory_access_obj;

// construct
struct memory_access_obj *memory_obj_factory(int flags, ...);

// destruct
void memory_obj_free(struct memory_access_obj *);

int  memory_obj_read(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);
int  memory_obj_write(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);

// format error string
size_t memory_obj_fmt_err(const struct memory_access_obj *, char *buf, size_t len);

// helper
s64  memory_obj_search(const struct memory_access_obj *, const char *pat, size_t patlen, u64 start, u64 end, u64 stride);

// Following calls must be properly nested in equal pairs
void  memory_obj_suggest_leave_open(struct memory_access_obj *);
void  memory_obj_suggest_close(struct memory_access_obj *);

// ask if close flag is set
bool  memory_obj_should_close(const struct memory_access_obj *);

EXTERN_C_END;

#endif  /* MEMORY_H */
