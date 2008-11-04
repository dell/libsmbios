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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>

#include "smbios_c/obj/cmos.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/memory.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/system_info.h"

#include "getopts.h"

#define _(String) gettext(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

// retval = 0; successfully activated token
// retval = 1; failed cmos checksum pre-check
// retval = 2; failed to set token
// retval = 3; unknown failure

struct options opts[] =
{
    { 1, "memory_file",  N_("Debug: Memory dump file to use instead of physical memory"), "m", 1 },
    { 2, "cmos_file",  N_("Debug: CMOS dump file to use instead of physical cmos"), "c", 1 },
    { 3, "set",  N_("Set Boot To UP Flag to true"), "s", 0 },
    { 4, "clear",  N_("Set Boot To UP Flag to false"), "c", 0 },
    { 5, "get",  N_("Set Boot To UP Flag to true"), "g", 0 },
    { 255, "version", N_("Display libsmbios version information"), "v", 0 },
    { 0, NULL, NULL, NULL, 0 }
};

int
main (int argc, char **argv)
{
    int retval = 0;
    int flag = 2;

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    textdomain(GETTEXT_PACKAGE);

    int c;
    char *args = 0;
    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 1:
            memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, args);
            break;
        case 2:
            cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, args);
            break;
        case 3:
            flag = 1; // fall through
        case 4:
            if( sysinfo_has_up_boot_flag() )
                sysinfo_set_up_boot_flag( flag );
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

    // default if no other params specified
    if( sysinfo_has_up_boot_flag() )
    {
        if(sysinfo_get_up_boot_flag())
        {
                retval = 0;
                printf(_("UP Boot Flag SET\n"));
        }
        else
        {
                retval = 1;
                printf(_("UP Boot Flag NOT SET\n"));
        }
    }


    exit(retval);
}
