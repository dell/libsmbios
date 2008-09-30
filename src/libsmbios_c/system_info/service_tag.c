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

#define LIBSMBIOS_SOURCE
#include "smbios_c/compat.h"

#include <string.h>
#include <stdlib.h>

#include "smbios_c/smbios.h"
#include "smbios_c/version.h"
#include "smbios_c/system_info.h"
#include "dell_magic.h"
#include "_impl.h"


__internal char *getServiceTagFromSysInfo()
{
    return smbios_get_string_from_table(System_Information_Structure, System_Information_Serial_Number_Offset);
}

__internal char *getServiceTagFromSysEncl()
{
    return smbios_get_string_from_table(System_Enclosure_or_Chassis_Structure, System_Enclosure_or_Chassis_Service_Offset);
}

// Code for getting the service tag from one of many locations
struct DellGetServiceTagFunctions
{
    char *(*f_ptr)();
}

/* try dynamic functions first to make sure we get current data. */
DellGetServiceTagFunctions[] = {
                                   //{&getServiceTagFromSMI,},       // SMI Token
                                   //{&getServiceTagFromCMOSToken,}, // CMOS Token
                                   {&getServiceTagFromSysInfo,},   // SMBIOS System Information Item
                                   {&getServiceTagFromSysEncl,},   // SMBIOS System Enclosure Item
                               };

char *smbios_get_service_tag()
{
    char *serviceTag = 0;
    int numEntries =
        sizeof (DellGetServiceTagFunctions) / sizeof (DellGetServiceTagFunctions[0]);

    for (int i = 0; (i < numEntries) && (!serviceTag); ++i)
    {
        // first function to return non-zero id wins.
        serviceTag = DellGetServiceTagFunctions[i].f_ptr ();
    }
    strip_trailing_whitespace(serviceTag);
    return serviceTag;
}






