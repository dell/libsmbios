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

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

int  memory_read(void *buffer, u64 offset, size_t length);
int  memory_write(void *buffer, u64 offset, size_t length);

// format error string
size_t memory_fmt_err(char *buf, size_t len);

// helper
s64  memory_search(const char *pat, size_t patlen, u64 start, u64 end, u64 stride);

// Following calls must be properly nested in equal pairs
void  memory_suggest_leave_open();
void  memory_suggest_close();

EXTERN_C_END;

#endif  /* MEMORY_H */
