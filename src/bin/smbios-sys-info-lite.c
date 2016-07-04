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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#include "smbios_c/obj/memory.h"
#include "smbios_c/system_info.h"
#include "smbios_c/smbios.h"

#include "getopts.h"

#define _(String) gettext(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

/*  0x0B is the OEM Strings smbios structure    */
#define SMBIOS_TBL_OEM_Strings      0x0B

struct options opts[] =
    {
        { 254, "memory_file", N_("Debug: Memory dump file to use instead of physical memory"), "m", 1 },
        { 255, "version", N_("Display libsmbios version information"), "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

/*  Print out all the OEM strings   */
static void print_oem_strings()
{
    int i=0;
    smbios_for_each_struct_type(s, SMBIOS_TBL_OEM_Strings) {
        const char *str = 0;
        i=1; // SMBIOS strings always start at index 1.
        while(1) {
            str = smbios_struct_get_string_number(s, i);
            if(!str) break;
            printf(_("OEM String %d: %s\n"), i, str);
            i++;
        }
    }
}

int
main (int argc, char **argv)
{
    int reseller_sysid = 0, sysid = 0;
    char *str;

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    textdomain(GETTEXT_PACKAGE);

    int c=0;
    char *args = 0;
    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 254:
            // This is for unit testing. You can specify a file that
            // contains a dump of memory to use instead of writing
            // directly to RAM.
            memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, args);
            break;
        case 255:
            printf( _("Libsmbios version:    %s\n"), smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf(_("Libsmbios:    %s\n"), smbios_get_library_version_string());

    //Error handling needs to be implemented for each of these functions
    sysid     = sysinfo_get_dell_system_id();
    if(sysid)
        printf(_("System ID:    0x%04X\n"), sysid);
    else
    {
        printf(_("Error getting the System ID:    unknown error.\n"));
    }

    reseller_sysid     = sysinfo_get_dell_oem_system_id();
    if(reseller_sysid != sysid)
    {
        if(reseller_sysid)
            printf(_("OEM System ID:    0x%04X\n"), reseller_sysid);
        else
        {
            printf(_("Error getting the System ID:    unknown error.\n"));
        }
    }

    str    = sysinfo_get_service_tag();
    if(str)
    {
        printf(_("Service Tag:  %s\n"), str);
        printf(_("Express Service Code: %lld\n"), strtoll(str, NULL, 36));
    }
    else
    {
        printf(_("Error getting the Service Tag:  unknown error\n"));
    }
    sysinfo_string_free(str);

    str    = sysinfo_get_asset_tag();
    if(str)
        printf(_("Asset Tag:  %s\n"), str);
    else
    {
        printf(_("Error getting the Asset Tag:  unknown error\n"));
    }
    sysinfo_string_free(str);

    str   = sysinfo_get_system_name();
    if(str)
        printf(_("Product Name: %s\n"), str);
    else
    {
        printf(_("Error getting the System Name:    unknown error.\n"));
    }
    sysinfo_string_free(str);

    str   = sysinfo_get_bios_version();
    if(str)
        printf(_("BIOS Version: %s\n"), str);
    else
    {
        printf(_("Error getting the BIOS Version:    unknown error.\n"));
    }
    sysinfo_string_free(str);

    str   = sysinfo_get_vendor_name();
    if(str)
        printf(_("Vendor:       %s\n"), str);
    else
    {
        printf(_("Error getting the Vendor:    unknown error.\n"));
    }
    sysinfo_string_free(str);

    printf(_("Is Dell:      %d\n"), (sysid!=0));

    print_oem_strings();

    return 0;
}
