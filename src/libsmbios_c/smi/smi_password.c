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

struct smi_password_properties {
    u8 installed, minlen, maxlen, characteristics, minalpha, minnumeric, minspecial, maxrepeat;
};

__hidden int password_installed(int which);
__hidden int verify_password(int which, const char *pass_scancode, u16 *security_key);
__hidden int change_password(int which, const char *oldpw, const char *newpw);
__hidden int get_password_properties_2(int which, struct smi_password_properties *p);
__hidden int verify_password_2(int which, const char *password, size_t maxpwlen, u16 *security_key);
__hidden int change_password_2(int which, const char *oldpw, const char *newpw, size_t maxpwlen);

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

#define SMI_PASSWORD_TYPE_ASCII     1
#define SMI_PASSWORD_TYPE_SCANCODE  0

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

int dell_smi_get_security_key(const char *password, u16 *key)
{
    int tmpret;
    u16 security_key = 0;
    int return_code = -2;

    int pass_to_check[] = {DELL_SMI_PASSWORD_ADMIN, DELL_SMI_PASSWORD_USER};
    int numpass = sizeof(pass_to_check)/sizeof(pass_to_check[0]);

    fnprintf("\n");

    // step 1:  is password installed
    for (int i=0; i<numpass; i++)
    {
        int which = pass_to_check[i];
        fnprintf("check %d\n", which);

        // try new func first
        struct smi_password_properties p = {0,};
        tmpret = get_password_properties_2(which, &p);
        // if function succeeded and password *not* installed, skip
        fnprintf("after get_password_properties_2: tmpret(%d)  p.installed(%d)\n", tmpret, p.installed);
        if (tmpret == 0 && p.installed != 0)
        {
            return_code = 0;
            continue;
        }

        tmpret = password_installed(which);
        fnprintf("after password_installed: tmpret(%d)\n", tmpret);
        if (!(tmpret == 0 || tmpret == 2))
        {
            return_code = 0;
            continue;
        }

        // step 2a: verify admin password (new method)
        tmpret = verify_password_2(which, password, p.maxlen, &security_key);
        fnprintf("after verify_password_2: tmpret(%d)  security_key(%d)\n", tmpret, security_key);
        if (tmpret==0) // correct, security key set
        {
            return_code = 0;
            goto out;
        }
        if (tmpret==2)
            return_code = -1; // incorrect password

        // step 2b: verify admin password (old method)
        tmpret = verify_password(which, password, &security_key);
        fnprintf("after verify_password: tmpret(%d)  security_key(%d)\n", tmpret, security_key);
        if (tmpret==0) // correct, security key set
        {
            return_code = 0;
            goto out;
        }
        if (tmpret==2)
            return_code = -1; // incorrect password

        fnprintf("end of loop\n");
    }

out:
    if (key)
        *key = security_key;
    return return_code;
}



bool dell_smi_is_password_present(int which)
{
    // try new func first
    bool retval = false;
    struct smi_password_properties p = {0,};
    int tmpret = get_password_properties_2(which, &p);
    // if function succeeded and password *not* installed, skip
    if (tmpret == 0 && p.installed == 0)
        goto out;

    // then try old
    tmpret = password_installed(which);
    if (!(tmpret == 0 || tmpret == 2))
        goto out;

    retval = true;
out:
    return retval;
}

int dell_smi_password_verify(int which, const char *password)
{
    int retval = 2;
    struct smi_password_properties p = {0,};
    int tmpret = get_password_properties_2(which, &p);
    if (tmpret == 0 && p.installed != 0)
        // if function succeeded and password *not* installed, skip
        goto out;
    else if (tmpret == 0)
    {
        // else _2 function is valid, so use it.
        tmpret = verify_password_2(which, password, p.maxlen, 0);
        retval = 1;
        if (tmpret==0) // correct, security key set
            goto out;

        retval = 0; // incorrect password
        if (tmpret==2)
            goto out;
    }


    tmpret = password_installed(which);
    if (tmpret == 0 && tmpret == 2)
        // function succeeded and password not installed
        goto out;
    else if (tmpret == 0)
    {
        tmpret = verify_password(which, password, 0);
        retval = 1;
        if (tmpret==0) // correct, security key set
            goto out;
        retval = 0; // incorrect password
        if (tmpret==2)
            goto out;
    }

out:
    return retval;
}

int dell_smi_password_format(int which)
{
    int retval = SMI_PASSWORD_TYPE_SCANCODE;
    struct smi_password_properties p = {0,};
    int tmpret = get_password_properties_2(which, &p);
    if (tmpret == 0)
        if (p.characteristics & 1)
            retval = SMI_PASSWORD_TYPE_ASCII;

    return retval;
}

int dell_smi_password_max_len(int which)
{
    int retval = 8;
    struct smi_password_properties p = {0,};
    int tmpret = get_password_properties_2(which, &p);
    if (tmpret == 0)
        retval = p.maxlen;
    return retval;
}

int dell_smi_password_min_len(int which)
{
    int retval = 0;
    struct smi_password_properties p = {0,};
    int tmpret = get_password_properties_2(which, &p);
    if (tmpret == 0)
        retval = p.minlen;
    return retval;
}

int dell_smi_password_change(int which, const char *oldpass, const char *newpass)
{
    // try old fn first
    int ret = change_password(which, oldpass, newpass);
    if (ret >= 0)
        // either it succeeded or failed, but this function was valid, no need to try other
        goto done;

    // try new fn if that failed.
    int maxlen = dell_smi_password_max_len(which);
    ret = change_password_2(which, oldpass, newpass, maxlen);

done:
    return ret;
}

/*************************************/
/********* private stuff *************/
/*************************************/


int password_installed(int which)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
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
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    u32 arg[2] = {0,};
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_VERIFY_PASSWORD);

    // copy password into arg
    if (password_scancodes)
        for (int i=0; i<strlen(password_scancodes) && i < sizeof(arg); ++i)
            ((u8*)arg)[i] = password_scancodes[i];

    dell_smi_obj_set_arg(smi, cbARG1, arg[0]);
    dell_smi_obj_set_arg(smi, cbARG2, arg[1]);

    dell_smi_obj_execute(smi);
    int retval = dell_smi_obj_get_res(smi, cbRES1);

    if (retval == SMI_PASSWORD_CORRECT && security_key)
        *security_key = (u16)dell_smi_obj_get_res(smi, cbRES2);

    dell_smi_obj_free(smi);
    return retval;
}


int change_password(int which, const char *oldpw_scancode, const char *newpw_scancode)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    u32 arg[4] = {0,};
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_CHANGE_PASSWORD);

    // copy password into arg
    if (oldpw_scancode)
        for (int i=0; i<strlen(oldpw_scancode) && i < sizeof(arg)/2; ++i)
            ((u8*)arg)[i] = oldpw_scancode[i];

    if (newpw_scancode)
        for (int i=0; i<strlen(newpw_scancode) && i < sizeof(arg)/2; ++i)
            ((u8*)arg)[i + sizeof(arg)/2 ] = newpw_scancode[i];

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
        p->name = (from & (0xFF << (byte * 8))) >> (byte * 8); \
    } while (0)

int get_password_properties_2(int which, struct smi_password_properties *p)
{
    int retval = -10;
    struct dell_smi_obj *smi = 0;

    fnprintf("\n");
    if (!p)
        goto out;

    smi = dell_smi_factory(DELL_SMI_DEFAULTS);

    if (!smi)
        goto out_free;

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_GET_PASSWORD_PROPERTIES_II);

    dell_smi_obj_execute(smi);
    retval = dell_smi_obj_get_res(smi, cbRES1);
    if (retval != 0)
        goto out_free;

    getbyte(installed, 0, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(maxlen, 1, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(minlen, 2, dell_smi_obj_get_res(smi, cbRES2));
    getbyte(characteristics, 3, dell_smi_obj_get_res(smi, cbRES2));

    getbyte(minalpha, 0, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(minnumeric, 1, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(minspecial, 2, dell_smi_obj_get_res(smi, cbRES3));
    getbyte(maxrepeat, 3, dell_smi_obj_get_res(smi, cbRES3));

out_free:
    fnprintf("out_free\n");
    dell_smi_obj_free(smi);
out:
    fnprintf("out\n");
    return retval;
}

int verify_password_2(int which, const char *password, size_t maxpwlen, u16 *security_key)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_VERIFY_PASSWORD_II);

    u8 *buf = dell_smi_obj_make_buffer_tobios(smi, cbARG1, maxpwlen);

    if (password)
        strncpy((char *)buf, password, maxpwlen);

    dell_smi_obj_execute(smi);

    int retval = dell_smi_obj_get_res(smi, cbRES1);

    if (retval == SMI_PASSWORD_CORRECT && security_key)
        *security_key = (u16)dell_smi_obj_get_res(smi, cbRES2);

    dell_smi_obj_free(smi);
    return retval;
}

int change_password_2(int which, const char *oldpw, const char *newpw, size_t maxpwlen)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, which);
    dell_smi_obj_set_select(smi, SMI_SELECT_CHANGE_PASSWORD_II);

    u8 *buf = dell_smi_obj_make_buffer_tobios(smi, cbARG1, maxpwlen * 2);

    if (oldpw)
        strncpy((char *)buf, oldpw, maxpwlen);

    if (newpw)
        strncpy((char *)buf + maxpwlen, newpw, maxpwlen);

    dell_smi_obj_execute(smi);

    int retval = dell_smi_obj_get_res(smi, cbRES1);

    dell_smi_obj_free(smi);
    return retval;
}

