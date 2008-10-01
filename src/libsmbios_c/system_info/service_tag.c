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

__internal char *getServiceTagFromCMOSToken()
{
    struct smbios_table *table = smbios_factory(SMBIOS_GET_SINGLETON);
    const struct smbios_struct *s;
    char *tempval = 0;

    if (!table)
        goto out;

    s = smbios_get_next_struct_by_type(table, 0, 0xD4);
    if (!s)
        goto out;

#if 0
    // Step 1: Get tag from CMOS
    tempval = new char[SVC_TAG_LEN_MAX + 1];
    memset(tempval, '\0', SVC_TAG_LEN_MAX + 1);
    // will throw an exception if not found.
    (*table)[Cmos_Service_Token]->getString(reinterpret_cast<u8*>(tempval), SVC_TAG_CMOS_LEN_MAX + 1);

    // Step 2: Decode 7-char tag from 5-char CMOS value
    dell_decode_service_tag( tempval, SVC_TAG_LEN_MAX + 1 );

    // Step 3: Make sure checksum is good before returning value
    u16 indexPort, dataPort;
    u8  location;

    smbios::IToken *token = &(*((*table)[ Cmos_Service_Token ]));
    dynamic_cast< smbios::ICmosToken * >(token)->getCMOSDetails( &indexPort, &dataPort, &location );

    u8 csum = 0;
    ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

    for( u32 i = 0; i < SVC_TAG_CMOS_LEN_MAX; i++)
    {
        // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
        csum = (csum + cmos->readByte( indexPort, dataPort, location + i )) & 0xFF;
    }

    // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
    csum = (csum - cmos->readByte( indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX )) & 0xFF;
    if( csum ) // bad (should be zero)
        throw "Bad checksum";
#endif

out:
    return tempval;
}

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
                                   {&getServiceTagFromCMOSToken,}, // CMOS Token
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
