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

#ifndef C_OBJ_SMI_H
#define C_OBJ_SMI_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#define DELL_SMI_DEFAULTS       0x0000
#define DELL_SMI_GET_SINGLETON  0x0001
#define DELL_SMI_GET_NEW        0x0002
#define DELL_SMI_UNIT_TEST_MODE 0x0004
#define DELL_SMI_NO_ERR_CLEAR   0x0008

struct dell_smi_obj;

// construct
LIBSMBIOS_C_DLL_SPEC struct dell_smi_obj *dell_smi_factory(int flags, ...);

// destruct
LIBSMBIOS_C_DLL_SPEC void dell_smi_obj_free(struct dell_smi_obj *);

LIBSMBIOS_C_DLL_SPEC const char *dell_smi_obj_strerror(struct dell_smi_obj *);

LIBSMBIOS_C_DLL_SPEC void dell_smi_obj_set_class(struct dell_smi_obj *, u16 );
LIBSMBIOS_C_DLL_SPEC void dell_smi_obj_set_select(struct dell_smi_obj *, u16 );
LIBSMBIOS_C_DLL_SPEC void dell_smi_obj_set_arg(struct dell_smi_obj *, u8 argno, u32 value);
LIBSMBIOS_C_DLL_SPEC u32  dell_smi_obj_get_res(struct dell_smi_obj *, u8 argno);
LIBSMBIOS_C_DLL_SPEC u8  *dell_smi_obj_make_buffer_frombios_auto(struct dell_smi_obj *, u8 argno, size_t size);
LIBSMBIOS_C_DLL_SPEC u8  *dell_smi_obj_make_buffer_frombios_withheader(struct dell_smi_obj *, u8 argno, size_t size);
LIBSMBIOS_C_DLL_SPEC u8  *dell_smi_obj_make_buffer_frombios_withoutheader(struct dell_smi_obj *, u8 argno, size_t size);
LIBSMBIOS_C_DLL_SPEC u8  *dell_smi_obj_make_buffer_tobios(struct dell_smi_obj *, u8 argno, size_t size);
LIBSMBIOS_C_DLL_SPEC int  dell_smi_obj_execute(struct dell_smi_obj *);

EXTERN_C_END;

#endif  /* C_SMI_H */
