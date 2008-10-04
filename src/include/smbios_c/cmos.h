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


#ifndef C_CMOS_H
#define C_CMOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

int    cmos_read_byte (u8 *byte, u32 indexPort, u32 dataPort, u32 offset);
int    cmos_write_byte(u8 byte,  u32 indexPort, u32 dataPort, u32 offset);

// to run all the attached callbacks
// most callbacks currently are for checksums. So, run with do_update = 0
// to get a return code indicating if checksums are all valid
int cmos_run_callbacks(bool do_update);

// not yet implemented
//size_t cmos_fmt_err(const struct cmos_obj *, char *buf, size_t len);

EXTERN_C_END;

#endif  /* CMOS_H */
