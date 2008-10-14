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

void dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4])
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);

    dell_smi_obj_set_class(smi, smiClass);
    dell_smi_obj_set_select(smi, select);
    dell_smi_obj_set_arg(smi, cbARG1, args[cbARG1]);
    dell_smi_obj_set_arg(smi, cbARG2, args[cbARG2]);
    dell_smi_obj_set_arg(smi, cbARG3, args[cbARG3]);
    dell_smi_obj_set_arg(smi, cbARG4, args[cbARG4]);

    dell_smi_obj_execute(smi);

    res[cbRES1] = dell_smi_obj_get_res(smi, cbRES1);
    res[cbRES2] = dell_smi_obj_get_res(smi, cbRES2);
    res[cbRES3] = dell_smi_obj_get_res(smi, cbRES3);
    res[cbRES4] = dell_smi_obj_get_res(smi, cbRES4);

    dell_smi_obj_free(smi);
}

// prepackaged smi functions
int get_property_ownership_tag(char *tagBuf, size_t size)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, 20); //class 20 == property tag
    dell_smi_obj_set_select(smi, 0); // 0 == read
    u8 *buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG1, 80);

    fnprintf("dell_smi_obj_execute()\n");
    dell_smi_obj_execute(smi);

    fnprintf(" cbRES1: %d\n", dell_smi_obj_get_res(smi, cbRES1));
    fnprintf(" cbRES2: %d\n", dell_smi_obj_get_res(smi, cbRES2));
    fnprintf(" cbRES3: %d\n", dell_smi_obj_get_res(smi, cbRES3));
    fnprintf(" cbRES4: %d\n", dell_smi_obj_get_res(smi, cbRES4));

    int retval = dell_smi_obj_get_res(smi, cbRES1);
    if (retval != 0)
        goto out;

    fnprintf("copy to return value\n");
    memset(tagBuf, 0, size);
    strncpy( tagBuf, (const char*)buf, size < 80 ? size:80);
    tagBuf[size-1] = '\0';

out:
    fnprintf(" - out\n");
    dell_smi_obj_free(smi);
    return retval;
}

u32 get_security_key(const char *password)
{
    return 0;
}

int set_property_ownership_tag(u32 security_key, const char *newTag, size_t size)
{
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    fnprintf("\n");

    dell_smi_obj_set_class(smi, 20); //class 20 == property tag
    dell_smi_obj_set_select(smi, 1); // 1 == write
    u8 *buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG1, 80);
    dell_smi_obj_set_arg(smi, cbARG2, security_key);
    strncpy((char *)buf, newTag, 80);

    fnprintf("dell_smi_obj_execute()\n");
    dell_smi_obj_execute(smi);

    fnprintf(" cbRES1: %d\n", dell_smi_obj_get_res(smi, cbRES1));
    fnprintf(" cbRES2: %d\n", dell_smi_obj_get_res(smi, cbRES2));
    fnprintf(" cbRES3: %d\n", dell_smi_obj_get_res(smi, cbRES3));
    fnprintf(" cbRES4: %d\n", dell_smi_obj_get_res(smi, cbRES4));

    int retval = dell_smi_obj_get_res(smi, cbRES1);

    fnprintf(" - out\n");
    dell_smi_obj_free(smi);
    return retval;
}

static int read_setting(u16 select, u32 location, u32 *minValue, u32 *maxValue)
{
    u32 args[4] = {location, 0,}, res[4] = {0,};
    dell_simple_ci_smi(0, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
    if(minValue)
        *minValue = res[2];
    if(maxValue)
        *maxValue = res[3];
    return res[1]; // current value
}

int read_nv_storage         (u32 location, u32 *minValue, u32 *maxValue)
{
    return read_setting(0, location, minValue, maxValue); // 0 = select code for nv storage
}

int read_battery_mode_setting(u32 location, u32 *minValue, u32 *maxValue)
{
    return read_setting(1, location, minValue, maxValue); // 1 = select code for battery mode
}

int read_ac_mode_setting     (u32 location, u32 *minValue, u32 *maxValue)
{
    return read_setting(2, location, minValue, maxValue); // 2 = select code for ac mode
}


int writeNVStorage         (const char *password, u32 location, u32 value)
{
    return 0;
}

int writeBatteryModeSetting(const char *password, u32 location, u32 value)
{
    return 0;
}

int writeACModeSetting     (const char *password, u32 location, u32 value)
{
    return 0;
}



