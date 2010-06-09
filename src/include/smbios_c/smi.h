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

#ifndef C_SMI_H
#define C_SMI_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

enum {
    cbARG1 = 0,
    cbARG2 = 1,
    cbARG3 = 2,
    cbARG4 = 3,
    cbRES1 = 0,
    cbRES2 = 1,
    cbRES3 = 2,
    cbRES4 = 3,
};

LIBSMBIOS_C_DLL_SPEC const char *dell_smi_strerror();

LIBSMBIOS_C_DLL_SPEC int dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);

// not yet implemented
LIBSMBIOS_C_DLL_SPEC int dell_adv_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4], const u8 *buffer[4], const size_t buffer_size[4]);

LIBSMBIOS_C_DLL_SPEC int dell_smi_read_nv_storage         (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);
LIBSMBIOS_C_DLL_SPEC int dell_smi_read_battery_mode_setting(u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);
LIBSMBIOS_C_DLL_SPEC int dell_smi_read_ac_mode_setting     (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);

LIBSMBIOS_C_DLL_SPEC int dell_smi_write_nv_storage         (u16 security_key, u32 location, u32 value, u32 *smiret);
LIBSMBIOS_C_DLL_SPEC int dell_smi_write_battery_mode_setting(u16 security_key, u32 location, u32 value, u32 *smiret);
LIBSMBIOS_C_DLL_SPEC int dell_smi_write_ac_mode_setting     (u16 security_key, u32 location, u32 value, u32 *smiret);

// password related functions
enum { DELL_SMI_PASSWORD_ANY = 0, DELL_SMI_PASSWORD_USER = 9, DELL_SMI_PASSWORD_ADMIN = 10, DELL_SMI_PASSWORD_OWNER = 12 };
enum { DELL_SMI_PASSWORD_FMT_SCANCODE = 0, DELL_SMI_PASSWORD_FMT_ASCII = 1 };
LIBSMBIOS_C_DLL_SPEC int dell_smi_password_format(int which);
LIBSMBIOS_C_DLL_SPEC int dell_smi_get_security_key(const char *password, u16 *security_key);
LIBSMBIOS_C_DLL_SPEC bool dell_smi_is_password_present(int which);
LIBSMBIOS_C_DLL_SPEC int dell_smi_password_verify(int which, const char *password);
LIBSMBIOS_C_DLL_SPEC int dell_smi_password_max_len(int which);
LIBSMBIOS_C_DLL_SPEC int dell_smi_password_min_len(int which);
LIBSMBIOS_C_DLL_SPEC int dell_smi_password_change(int which, const char *oldpass, const char *newpass);

EXTERN_C_END;

#endif  /* C_SMI_H */
