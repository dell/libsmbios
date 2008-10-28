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


