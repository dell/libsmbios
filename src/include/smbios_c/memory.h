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


#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define MEMORY_DEFAULTS       0x0000
#define MEMORY_GET_SINGLETON  0x0001
#define MEMORY_GET_NEW        0x0002
#define MEMORY_UNIT_TEST_MODE 0x0004

struct memory;

struct memory *memory_factory(int flags, ...);
int  memory_read(struct memory *, u8 *buffer, u64 offset, size_t length);
int  memory_write(struct memory *, u8 *buffer, u64 offset, size_t length);
void memory_free(struct memory *);
s64  memory_search(struct memory *, const char *pat, size_t patlen, u64 start, u64 end, u64 stride);
const char * memory_strerr(struct memory *);

// Following calls must be properly nested in equal pairs
void  memory_suggest_leave_open(struct memory *);
void  memory_suggest_close(struct memory *);

// ask if close flag is set
bool  memory_should_close(struct memory *);

EXTERN_C_END;

#endif  /* MEMORY_H */
