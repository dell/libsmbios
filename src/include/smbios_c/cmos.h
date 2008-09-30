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


#ifndef CMOS_H
#define CMOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define CMOS_DEFAULTS       0x0000
#define CMOS_GET_SINGLETON  0x0001
#define CMOS_GET_NEW        0x0002
#define CMOS_UNIT_TEST_MODE 0x0004

struct cmos_obj;

struct cmos_obj *cmos_factory(int flags, ...);
void   cmos_obj_free(struct cmos_obj *);

int     cmos_read_byte(const struct cmos_obj *, u32 indexPort, u32 dataPort, u32 offset, u8 *byte);
int    cmos_write_byte(const struct cmos_obj *, u32 indexPort, u32 dataPort, u32 offset, u8 byte);
size_t cmos_fmt_err(const struct cmos_obj *, char *buf, size_t len);

// useful for checksums, etc
typedef void (*cmos_write_callback)(struct cmos_obj *, void *);
void register_write_callback(cmos_write_callback, void *);

EXTERN_C_END;

#endif  /* CMOS_H */
