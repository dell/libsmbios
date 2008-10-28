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

const char *dell_smi_strerror()
{
    const char *retval = 0;
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    if (smi) retval = dell_smi_obj_strerror(smi);
    dell_smi_obj_free(smi);
    return retval;
}

int dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4])
{
    int retval = -1;
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    if(!smi)
        goto out;

    dell_smi_obj_set_class(smi, smiClass);
    dell_smi_obj_set_select(smi, select);
    dell_smi_obj_set_arg(smi, cbARG1, args[cbARG1]);
    dell_smi_obj_set_arg(smi, cbARG2, args[cbARG2]);
    dell_smi_obj_set_arg(smi, cbARG3, args[cbARG3]);
    dell_smi_obj_set_arg(smi, cbARG4, args[cbARG4]);

    dell_smi_obj_execute(smi);

    fnprintf(" cbRES1: %d\n", dell_smi_obj_get_res(smi, cbRES1));
    fnprintf(" cbRES2: %d\n", dell_smi_obj_get_res(smi, cbRES2));
    fnprintf(" cbRES3: %d\n", dell_smi_obj_get_res(smi, cbRES3));
    fnprintf(" cbRES4: %d\n", dell_smi_obj_get_res(smi, cbRES4));

    res[cbRES1] = dell_smi_obj_get_res(smi, cbRES1);
    res[cbRES2] = dell_smi_obj_get_res(smi, cbRES2);
    res[cbRES3] = dell_smi_obj_get_res(smi, cbRES3);
    res[cbRES4] = dell_smi_obj_get_res(smi, cbRES4);

    dell_smi_obj_free(smi);
    retval = 0;

out:
    return retval;
}

// prepackaged smi functions
#define PROPERTY_TAG_LEN 80
int sysinfo_get_property_ownership_tag(char *tagBuf, size_t size)
{
    int retval = -2;
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    fnprintf("\n");
    if (!smi)
        goto out;

    dell_smi_obj_set_class(smi, 20); //class 20 == property tag
    dell_smi_obj_set_select(smi, 0); // 0 == read
    // allocate one extra byte to ensure it is zero terminated
    u8 *buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG1, PROPERTY_TAG_LEN + 1);

    fnprintf("dell_smi_obj_execute()\n");
    dell_smi_obj_execute(smi);

    retval = dell_smi_obj_get_res(smi, cbRES1);
    if (retval != 0)
        goto out;

    fnprintf("copy to return value\n");
    memset(tagBuf, 0, size);
    strncpy(tagBuf, (const char*)buf, size); // strncpy with size guaranteed to be ok
    tagBuf[size-1] = '\0';
    fnprintf("tag: -->%s<--\n", tagBuf);

out:
    fnprintf(" - out\n");
    dell_smi_obj_free(smi);
    return retval;
}

int sysinfo_set_property_ownership_tag(const char *newTag, const char *pass_ascii, const char *pass_scancode)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    u16 security_key = 0;
    const char *whichpw = pass_scancode;
    u8 *buf;
    int retval = -2;

    if (!smi)
        goto out;

    fnprintf(" new tag request: '%s'\n", newTag);

    if (dell_smi_password_format(DELL_SMI_PASSWORD_ADMIN) == DELL_SMI_PASSWORD_FMT_ASCII)
        whichpw=pass_ascii;
    int ret = dell_smi_get_security_key(whichpw, &security_key);
    if (ret)  // bad password
        goto out;

    dell_smi_obj_set_class(smi, 20); //class 20 == property tag
    dell_smi_obj_set_select(smi, 1); // 1 == write
    buf = dell_smi_obj_make_buffer_tobios(smi, cbARG1, PROPERTY_TAG_LEN); // max property tag size
    dell_smi_obj_set_arg(smi, cbARG2, security_key);
    strncpy((char *)buf, newTag, PROPERTY_TAG_LEN);

    fnprintf("dell_smi_obj_execute()\n");
    dell_smi_obj_execute(smi);

    retval = dell_smi_obj_get_res(smi, cbRES1);

    fnprintf(" - out\n");
    dell_smi_obj_free(smi);

out:
    return retval;
}

static int read_setting(u16 select, u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    u32 args[4] = {location, 0,}, res[4] = {0,};
    int retval = dell_simple_ci_smi(0, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
    if(curValue)
        *curValue = res[cbARG2];
    if(minValue)
        *minValue = res[cbARG3];
    if(maxValue)
        *maxValue = res[cbARG4];
    return retval;
}

int dell_smi_read_nv_storage         (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    return read_setting(0, location, curValue, minValue, maxValue); // 0 = select code for nv storage
}

int dell_smi_read_battery_mode_setting(u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    return read_setting(1, location, curValue, minValue, maxValue); // 1 = select code for battery mode
}

int dell_smi_read_ac_mode_setting     (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    return read_setting(2, location, curValue, minValue, maxValue); // 2 = select code for ac mode
}

static int write_setting(u16 security_key, u16 select, u32 location, u32 value)
{
    u32 args[4] = {location, value, security_key}, res[4] = {0,};
    dell_simple_ci_smi(0, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
    return res[cbRES1];
}

int dell_smi_write_nv_storage         (u16 security_key, u32 location, u32 value)
{
    return write_setting(security_key, 0, location, value);
}

int dell_smi_write_battery_mode_setting(u16 security_key, u32 location, u32 value)
{
    return write_setting(security_key, 1, location, value);
}

int dell_smi_write_ac_mode_setting     (u16 security_key, u32 location, u32 value)
{
    return write_setting(security_key, 2, location, value);
}



