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


#ifndef C_OBJ_CMOS_H
#define C_OBJ_CMOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"

#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define CMOS_DEFAULTS       0x0000
#define CMOS_GET_SINGLETON  0x0001
#define CMOS_GET_NEW        0x0002
#define CMOS_UNIT_TEST_MODE 0x0004
#define CMOS_NO_ERR_CLEAR   0x0008

// forward declaration to reduce header file deps
struct cmos_access_obj;

LIBSMBIOS_C_DLL_SPEC struct cmos_access_obj *cmos_obj_factory(int flags, ...);
LIBSMBIOS_C_DLL_SPEC void   cmos_obj_free(struct cmos_access_obj *);

LIBSMBIOS_C_DLL_SPEC int     cmos_obj_read_byte(const struct cmos_access_obj *, u8 *byte, u32 indexPort, u32 dataPort, u32 offset);
LIBSMBIOS_C_DLL_SPEC int    cmos_obj_write_byte(const struct cmos_access_obj *, u8 byte,  u32 indexPort, u32 dataPort, u32 offset);

// format error string
LIBSMBIOS_C_DLL_SPEC const char *cmos_obj_strerror(const struct cmos_access_obj *m);

// useful for checksums, etc
typedef int (*cmos_write_callback)(const struct cmos_access_obj *, bool, void *);
LIBSMBIOS_C_DLL_SPEC void cmos_obj_register_write_callback(struct cmos_access_obj *, cmos_write_callback, void *, void (*destruct)(void *));
LIBSMBIOS_C_DLL_SPEC int cmos_obj_run_callbacks(const struct cmos_access_obj *m, bool do_update);

EXTERN_C_END;

#endif  /* CMOS_H */
