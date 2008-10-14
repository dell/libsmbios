/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
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

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

#include <string.h>

// public
#include "smbios_c/obj/smi.h"
#include "smbios_c/smi.h"

// private
#include "smi_impl.h"



__internal int password_installed(int which);
__internal int verify_password(int which, const char *password, u16 *security_key);
__internal int change_password(int which, const char *oldpw, const char *newpw);
__internal int get_password_properties_2(int which, u8 *installed, u8 *minlen, u8 *maxlen, u8 *characteristics, u8 *minalpha, u8 *minnumeric, u8 *minspecial, u8 *maxrepeat);
__internal int verify_password_2(int which, const char *password, size_t maxpwlen, u16 *security_key);
__internal int change_password_2(int which, const char *oldpw, const char *newpw, size_t maxpwlen);

#define SMI_PASSWORD_INSTALLED 0
#define SMI_PASSWORD_NOT_INSTALLED 1
#define SMI_PASSWORD_INSTALLED_ONLY_ADMIN_MOD 2
#define SMI_PASSWORD_NOT_INSTALLED_ONLY_ADMIN_MOD 3

#define SMI_PASSWORD_CORRECT 0
#define SMI_PASSWORD_INCORRECT 2
#define SMI_PASSWORD_CHANGED 0

#define SMI_PASSWORD_CHARACTERISTIC_BIT_ASCII             0
#define SMI_PASSWORD_CHARACTERISTIC_BIT_ALPHANUMERIC_ONLY 1
#define SMI_PASSWORD_CHARACTERISTIC_BIT_CANNOT_DELETE     2

#define SMI_SELECT_PASSWORD_INSTALLED   0
#define SMI_SELECT_VERIFY_PASSWORD      1
#define SMI_SELECT_CHANGE_PASSWORD      2
#define SMI_SELECT_GET_PASSWORD_PROPERTIES_II      3
#define SMI_SELECT_VERIFY_PASSWORD_II  4
#define SMI_SELECT_CHANGE_PASSWORD_II  5


/*
An application that will attempt to set information via any Security-Key-protected Calling Interface
function must first acquire a proper Security Key. It does this by performing the following steps:
1. Check to see if an Administrator Password is set (Class 10, Selector 0 or 3). If yes, go to 2;
    otherwise, go to 3.
2. Verify the Administrator Password (Class 10 Selector 1 or 4). If the password is verified
    (cbRES1 == 0), read the Security Key from cbRES2, and use it on subsequent set functions
    where it is required. If the password does not verify (cbRES1 == -1), repeat step 2 until it
    does verify; otherwise, subsequent set functions protected by the Administrator Password will
    be rejected by the BIOS if it supports the Security Key feature.
3. Check to see if a User Password is set (Class 9, Selector 0 or 3). If yes, go to 4; otherwise,
    no Security Key will be needed to change data through the Calling Interface, and the caller
    can use any value at all for the Security Key when using any Security-Key-protected Calling
    Interface function.
4. Verify the User Password (Class 9 Selector 1 or 4). If the password is verified (cbRES1 ==
    0), read the Security Key from cbRES2, and use it on subsequent set functions where it is
    required. If the password does not verify (cbRES1 == -1), repeat step 4 until it does verify;
   otherwise, subsequent set functions protected by the User Password will be rejected by the
   BIOS if it supports the Security Key feature.
*/




int password_installed(int which)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_PASSWORD_INSTALLED);
    dell_smi_obj_set_arg(smi, cbARG1, 1);
    dell_smi_obj_execute(smi);
    int retval = dell_smi_obj_get_res(smi, cbRES1);
    dell_smi_obj_free(smi);
    return retval;
}

int verify_password(int which, const char *password_scancodes, u16 *security_key)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    u32 arg[2] = {0,};
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_VERIFY_PASSWORD);

    // copy password into arg
    for (int i=0; i<strlen(password_scancodes) && i < sizeof(arg); ++i)
        ((u8*)arg)[i] = password_scancodes[i];

    dell_smi_obj_set_arg(smi, cbARG1, arg[0]);
    dell_smi_obj_set_arg(smi, cbARG2, arg[1]);

    dell_smi_obj_execute(smi);
    int retval = dell_smi_obj_get_res(smi, cbRES1);

    if (retval == SMI_PASSWORD_CORRECT)
        *security_key = (u16)dell_smi_obj_get_res(smi, cbRES2);

    dell_smi_obj_free(smi);
    return retval;
}


int change_password(int which, const char *oldpw_scancode, const char *newpw_scancode)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    u32 arg[4] = {0,};
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_CHANGE_PASSWORD);

    // copy password into arg
    for (int i=0; i<strlen(oldpw_scancode) && i < sizeof(arg)/2; ++i)
        ((u8*)arg)[i] = oldpw_scancode[i];

    for (int i=sizeof(arg)/2; i<strlen(newpw_scancode) && i < sizeof(arg); ++i)
        ((u8*)arg)[i] = newpw_scancode[i];

    dell_smi_obj_set_arg(smi, cbARG1, arg[0]);
    dell_smi_obj_set_arg(smi, cbARG2, arg[1]);
    dell_smi_obj_set_arg(smi, cbARG3, arg[2]);
    dell_smi_obj_set_arg(smi, cbARG4, arg[3]);

    dell_smi_obj_execute(smi);
    int retval = dell_smi_obj_get_res(smi, cbRES1);

    dell_smi_obj_free(smi);
    return retval;
}

#define getbyte(name, byte, from) \
    do {\
        if (name)   \
            *name = (from & (0xFF << (byte * 8))) >> (byte * 8); \
    } while (0)

int get_password_properties_2(int which, u8 *installed, u8 *minlen, u8 *maxlen, u8 *characteristics, u8 *minalpha, u8 *minnumeric, u8 *minspecial, u8 *maxrepeat)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_GET_PASSWORD_PROPERTIES_II);

    dell_smi_obj_execute(smi);
    int retval = dell_smi_obj_get_res(smi, cbRES1);
    if (retval != 0)
        goto out;

    getbyte(installed, 0, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(maxlen, 1, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(minlen, 2, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(characteristics, 3, dell_smi_obj_get_res(smi, cbRES2));

    getbyte(minalpha, 0, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(minnumeric, 1, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(minspecial, 2, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(maxrepeat, 3, dell_smi_obj_get_res(smi, cbRES3));

out:
    dell_smi_obj_free(smi);
    return retval;
}

int verify_password_2(int which, const char *password, size_t maxpwlen, u16 *security_key)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_VERIFY_PASSWORD_II);

    u8 *buf = dell_smi_obj_make_buffer_tobios(smi, cbARG1, maxpwlen);

    strncpy((char *)buf, password, maxpwlen);

    dell_smi_obj_execute(smi);

    int retval = dell_smi_obj_get_res(smi, cbRES1);

    if (retval == SMI_PASSWORD_CORRECT)
        *security_key = (u16)dell_smi_obj_get_res(smi, cbRES2);

    dell_smi_obj_free(smi);
    return retval;
}

int change_password_2(int which, const char *oldpw, const char *newpw, size_t maxpwlen)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_CHANGE_PASSWORD_II);

    u8 *buf = dell_smi_obj_make_buffer_tobios(smi, cbARG1, maxpwlen * 2);

    strncpy((char *)buf, oldpw, maxpwlen);
    strncpy((char *)buf + maxpwlen, newpw, maxpwlen);

    dell_smi_obj_execute(smi);

    int retval = dell_smi_obj_get_res(smi, cbRES1);

    dell_smi_obj_free(smi);
    return retval;
}

