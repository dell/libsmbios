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
#include "smbios/ISmbios.h"
#include "smbios/IToken.h"

#include "smbios/SystemInfo.h"
#include "smbios/IMemory.h"
#include "smbios/SmbiosDefs.h"
#include "ExceptionImpl.h"

#include "SystemDetect.h"

// all our magic numbers
#include "DellMagic.h"

#include <string.h>

// include this last.
#include "smbios/message.h"

using namespace smbios;
using namespace cmos;
using namespace std;

// This is defined in SysInfoError.cpp
extern smbios::Exception<smbios::IException> SysInfoException;

//
//
// Detection functions
//
//

bool couldBeDiamond ()
{
    bool couldBeDiamond = false;

    if(SMBIOSGetDellSystemId() == SYSTEM_ID_DIAMOND )
        couldBeDiamond=true;

    return couldBeDiamond;
}


bool couldBeBayonet ()
{
    //functionEnter( "%s", "" );
    bool couldBeBayonet = false;

    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // crappy msvc compiler workaround
    smbios::ISmbiosTable::const_iterator item ;

    if (0 == table)
        throw InternalErrorImpl();

    // search through 0x0B (OEM_Strings_Structure) items
    for( item = (*table)[OEM_Strings] ; item != table->end(); ++item)
    {
        const char *str = item->getStringByStringNumber (OEM_String_Field_Number); // no need to free retval.
        if ((0 != str) && (0 == strncmp (str, Bayonet_Detect_String, strlen(Bayonet_Detect_String))))
            couldBeBayonet = true;
    }


    //functionLeave( "\t\tretval = %i\n", (int)couldBeBayonet );
    return couldBeBayonet;
}

static bool isStdDellBiosSystem ()
{
    //functionEnter( "%s", "" );
    bool dellSystem = false;
    // OEM String is 5 chars ("Dell\0")
    char OEMString[5] = { 0, };

    memory::IMemory *mem =
        memory::MemoryFactory::getFactory()->getSingleton();

    mem->fillBuffer( reinterpret_cast<u8 *>(OEMString), OEM_String_Location, 4 );

    if (0 == strncmp (OEMString, OEM_Dell_String, 5))
        dellSystem = true;

    //functionLeave( "\t\tretval = %i\n", (int)dellSystem );
    return dellSystem;
}



//
// List of detection functions
//

struct SystemDetectionFunction
{
    bool (*f_ptr)();
}
DellDetectionFunctions[] = {
                               {&isStdDellBiosSystem,},
                               {&couldBeBayonet,},
                               {&couldBeDiamond,},
                           };


//
// The main detection routine
//

int SMBIOSIsDellSystem ()
{
    bool isDell = false;
    int retval = 0;
    //functionEnter( "%s", "" );

    // notice how extensible this is...
    //  We can add new detection functions to the array defined
    //  above at any time...
    //
    // Why not add an 8450 detection routine? Anybody?
    //
    int numEntries =
        sizeof (DellDetectionFunctions) /
        sizeof (DellDetectionFunctions[0]);

    for (int i = 0; i < numEntries; ++i)
    {
        try
        {
            isDell = DellDetectionFunctions[i].f_ptr ();
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }

        if (isDell)
            break;
    }

    //Convert to an int for our C-caller friends
    if(isDell)
    {
        retval = 1;
    }
    else
    {
        retval = 0;
    }
    //functionLeave( "\t\tretval = %i\n", (int)retval );
    return retval;
}


