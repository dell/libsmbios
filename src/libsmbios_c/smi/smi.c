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
    fnprintf("\n");
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS | DELL_SMI_NO_ERR_CLEAR);
    const char *retval = dell_smi_obj_strerror(smi);
    dell_smi_obj_free(smi);
    return retval;
}

int dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4])
{
    int retval = -1;
    fnprintf("\n");
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_DEFAULTS);
    if(!smi)
        goto out;

    dell_smi_obj_set_class(smi, smiClass);
    dell_smi_obj_set_select(smi, select);
    dell_smi_obj_set_arg(smi, cbARG1, args[cbARG1]);
    dell_smi_obj_set_arg(smi, cbARG2, args[cbARG2]);
    dell_smi_obj_set_arg(smi, cbARG3, args[cbARG3]);
    dell_smi_obj_set_arg(smi, cbARG4, args[cbARG4]);

    fnprintf("about to _execute\n");
    retval = dell_smi_obj_execute(smi);

    fnprintf(" cbRES1: %d\n", dell_smi_obj_get_res(smi, cbRES1));
    fnprintf(" cbRES2: %d\n", dell_smi_obj_get_res(smi, cbRES2));
    fnprintf(" cbRES3: %d\n", dell_smi_obj_get_res(smi, cbRES3));
    fnprintf(" cbRES4: %d\n", dell_smi_obj_get_res(smi, cbRES4));

    res[cbRES1] = dell_smi_obj_get_res(smi, cbRES1);
    res[cbRES2] = dell_smi_obj_get_res(smi, cbRES2);
    res[cbRES3] = dell_smi_obj_get_res(smi, cbRES3);
    res[cbRES4] = dell_smi_obj_get_res(smi, cbRES4);

out:
    dell_smi_obj_free(smi);
    fnprintf("return retval: %d\n", retval);
    return retval;
}

static int read_setting(u16 select, u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    u32 args[4] = {location, 0,}, res[4] = {0,};
    fnprintf(" select %d, location %x \n", select, location);
    int retval = dell_simple_ci_smi(0, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
    if(curValue)
        *curValue = res[cbARG2];
    if(minValue)
        *minValue = res[cbARG3];
    if(maxValue)
        *maxValue = res[cbARG4];
    fnprintf("retval %d\n", retval);
    return retval;
}

int dell_smi_read_nv_storage         (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    fnprintf("\n");
    return read_setting(0, location, curValue, minValue, maxValue); // 0 = select code for nv storage
}

int dell_smi_read_battery_mode_setting(u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    fnprintf("\n");
    return read_setting(1, location, curValue, minValue, maxValue); // 1 = select code for battery mode
}

int dell_smi_read_ac_mode_setting     (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue)
{
    return read_setting(2, location, curValue, minValue, maxValue); // 2 = select code for ac mode
}

static int write_setting(u16 security_key, u16 select, u32 location, u32 value, u32 *smiret)
{
    u32 args[4] = {location, value, security_key}, res[4] = {0,};
    int retval = dell_simple_ci_smi(1, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
    if(smiret)
        *smiret = res[cbRES1];
    return retval;
}

int dell_smi_write_nv_storage         (u16 security_key, u32 location, u32 value, u32 *smiret)
{
    fnprintf("key(0x%04x)  location(0x%04x)  value(0x%04x)\n", security_key, location, value);
    return write_setting(security_key, 0, location, value, smiret);
}

int dell_smi_write_battery_mode_setting(u16 security_key, u32 location, u32 value, u32 *smiret)
{
    fnprintf("key(0x%04x)  location(0x%04x)  value(0x%04x)\n", security_key, location, value);
    return write_setting(security_key, 1, location, value, smiret);
}

int dell_smi_write_ac_mode_setting     (u16 security_key, u32 location, u32 value, u32 *smiret)
{
    fnprintf("key(0x%04x)  location(0x%04x)  value(0x%04x)\n", security_key, location, value);
    return write_setting(security_key, 2, location, value, smiret);
}



