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
#include "smbios_c/compat.h"

#include <string.h>
#include <stdlib.h>

#include "smbios_c/smbios.h"
#include "smbios_c/system_info.h"
#include "dell_magic.h"
#include "sysinfo_impl.h"

static char *module_error_buf; // auto-init to 0

LIBSMBIOS_C_DLL_SPEC const char *smbios_get_library_version_string()
{
    sysinfo_clearerr();
    return PACKAGE_VERSION;
}

LIBSMBIOS_C_DLL_SPEC int smbios_get_library_version_major()
{
    sysinfo_clearerr();
    return LIBSMBIOS_RELEASE_MAJOR;
}

LIBSMBIOS_C_DLL_SPEC int smbios_get_library_version_minor()
{
    sysinfo_clearerr();
    return LIBSMBIOS_RELEASE_MINOR;
}

__attribute__((destructor)) static void return_mem(void)
{
    fnprintf("\n");
    free(module_error_buf);
    module_error_buf = 0;
}

__hidden char *sysinfo_get_module_error_buf()
{
    fnprintf("\n");
    if (!module_error_buf)
        module_error_buf = calloc(1, ERROR_BUFSIZE);
    return module_error_buf;
}

LIBSMBIOS_C_DLL_SPEC const char * sysinfo_strerror()
{
    return module_error_buf;
}

__hidden void sysinfo_clearerr()
{
    if (module_error_buf)
        memset(module_error_buf, 0, ERROR_BUFSIZE);
}

void __hidden strip_trailing_whitespace( char *str )
{
    if(!str)
        return;

    if(strlen(str) == 0)
        return;

    size_t ch = strlen(str);
    do
    {
        --ch;
        if( ' ' == str[ch] )
            str[ch] = '\0';
        else
            break;

    } while(ch);
}

__hidden char * smbios_struct_get_string_from_table(u8 type, u8 offset)
{
    const struct smbios_struct *s;
    const char *r;
    char *ret = 0;

    sysinfo_clearerr();
    s = smbios_get_next_struct_by_type(0, type);
    if (!s)
        goto out_err;

    r = smbios_struct_get_string_from_offset(s, offset);
    if (!r)
        goto out_err;

    ret = calloc(1, strlen(r)+1);
    if(!ret)
        goto out_err;

    strcpy(ret, r);
    strip_trailing_whitespace(ret);
    return ret;

out_err:
    return NULL;
}

LIBSMBIOS_C_DLL_SPEC void sysinfo_string_free(void *f)
{
    free(f);
}

LIBSMBIOS_C_DLL_SPEC char *sysinfo_get_vendor_name()
{
    return smbios_struct_get_string_from_table(System_Information_Structure, System_Information_Manufacturer_Offset);
}

LIBSMBIOS_C_DLL_SPEC char *sysinfo_get_system_name()
{
    return smbios_struct_get_string_from_table(System_Information_Structure, System_Information_Product_Name_Offset);
}

LIBSMBIOS_C_DLL_SPEC char *sysinfo_get_bios_version()
{
    return smbios_struct_get_string_from_table(BIOS_Information_Structure, BIOS_Information_Version_Offset);
}
