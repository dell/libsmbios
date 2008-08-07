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

struct cmos;

struct cmos *cmos_factory(int flags, ...);
int     cmos_read_byte(struct cmos *, u32 indexPort, u32 dataPort, u32 offset, u8 *byte);
int    cmos_write_byte(struct cmos *, u32 indexPort, u32 dataPort, u32 offset, u8 byte);
void   cmos_free(struct cmos *);
const char * cmos_strerr(struct cmos *);

// useful for checksums, etc
typedef void (*cmos_write_callback)(struct cmos *, void *);
void register_write_callback(cmos_write_callback, void *);

EXTERN_C_END;

#endif  /* CMOS_H */
