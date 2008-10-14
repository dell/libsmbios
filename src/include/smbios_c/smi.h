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

void dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);

// prepackaged smi functions
u32 get_security_key(const char *password);


enum { user_password = 9, admin_password = 10 };

/** is user password installed
 * @return 0==password installed
 *  1==password *NOT* installed
 *  2==password installed/can only be modified using admin password
 *  3==password *NOT* installed/can only be installed with admin password
 */
int password_installed(int which);


/** is user password installed
 * @param password  keyboard scan code bytes for password
 * @param security_key  security key is stored here if password is correct
 * @return 0==password correct
 *         2==password incorrect
 *         -1 == completed with error
 *         -2 == function not supported
 */
int verify_password(int which, const char *password, u16 *security_key);

/** change user password
 * @param oldpw old password
 * @param newpw new password
 * @return 0 == password changed
 *      2 == old password incorrect
 *      3 == password cannot be disabled
 *      0x0100 - 0xff00 == password cannot be changed, min length not met, byte 1 contains min password len
 */
int change_password(int which, const char *oldpw, const char *newpw);

/** get user password properties II
 * @return         Return code
 *                0     Completed successfully
 *               -1    Completed with error
 *               -2    Function not supported
 * cbRES2, byte 0 Password installed indicator
 *                1     Password is installed
 *                2     Password is not installed
 *                3     Password disabled by jumper
 * cbRES2, byte 1 Maximum password length (in bytes)
 * cbRES2, byte 2 Minimum new password length (in bytes)
 * cbRES2, byte 3 Password characteristics, see 3.9.2.11 Using the
 *                  User/Administrator Password II Functions for
 *                  additional information.
 * cbRES3, byte 0 Minimum number of alphabetic characters required
 *                  for a new password
 * cbRES3, byte 1 Minimum number of numeric characters required fo
 *                  a new password
 * cbRES3, byte 2 Minimum number of special characters required for 
 *                  new password
 * cbRES3, byte 3 Maximum character repeat count for a new
 *                  password.
 */
int get_password_properties_2(int which, u32 *cbRES2, u32 *cbRES3);

int verify_password_2(const char *password, u16 *security_key);
int change_password_2(const char *oldpw, const char *newpw);

int get_property_ownership_tag(char *tagBuf, size_t size);
int set_property_ownership_tag(u32 security_key, const char *newTag, size_t size);

int read_nv_storage         (u32 location, u32 *minValue, u32 *maxValue);
int read_battery_mode_setting(u32 location, u32 *minValue, u32 *maxValue);
int read_ac_mode_setting     (u32 location, u32 *minValue, u32 *maxValue);

int write_nv_storage         (u32 security_key, u32 location, u32 value);
int write_battery_mode_setting(u32 security_key, u32 location, u32 value);
int write_ac_mode_setting     (u32 security_key, u32 location, u32 value);

EXTERN_C_END;

#endif  /* C_SMI_H */
