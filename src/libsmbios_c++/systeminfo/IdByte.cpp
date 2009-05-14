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
#include "smbios/compat.h"

#include <string.h>

#include "smbios/ISmbios.h"
#include "smbios/IToken.h"

#include "smbios/SystemInfo.h"
#include "smbios/IMemory.h"
#include "smbios/SmbiosDefs.h"
#include "ExceptionImpl.h"

#include "SystemDetect.h"
#include "DellMagic.h"

// should always be included last.
#include "smbios/message.h"

using namespace smbios;
using namespace cmos;
using namespace std;

extern smbios::Exception<smbios::IException> SysInfoException;

//
//
// ID Byte functions
//
//
static u16 getIdByteFromMem ()
{
    u16 tempWord = 0;
    u16 idWord = 0;
    memory::IMemory *mem = 0;

    struct two_byte_structure tbs;

    mem = memory::MemoryFactory::getFactory()->getSingleton();

    if( 0 == mem )
        throw InternalErrorImpl();

    // Step 1: Check that "Dell System" is present at the proper offset
    u8 strBuf[DELL_SYSTEM_STRING_LEN] = { 0, };
    mem->fillBuffer( strBuf, DELL_SYSTEM_STRING_LOC, DELL_SYSTEM_STRING_LEN - 1 );
    if( strncmp( reinterpret_cast<char*>(strBuf), DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) != 0 )
        goto out;

    // Step 2: fill the id structs
    mem->fillBuffer( reinterpret_cast<u8 *>(&tbs), TWO_BYTE_STRUCT_LOC, sizeof(two_byte_structure) );

    // Step 3: check the checksum of one-byte struct
    //    update: checksum is not reliable, so don't use it...

    // Step 4: Check one byte ID
    tempWord = tbs.system_id;

    // Step 5: if 0xFE, then it is a double byte (word) ID.
    // *  -- byte at 0xFE845 is 0xFE
    if (0xFE == tempWord)
{
        // Step 6: check two byte struct checksum
        // *  -- three bytes at 0xFE845 sum to 0x00 but are not all 0x00.
        //*  -- extension checksum is 0

        // Step 7: get ID.
        tempWord = tbs.two_byte_id;
    }

    idWord = tempWord;

out:
    return idWord;
}

static u16 getIdByteFromMem_Diamond ()
{
    u16 idWord = 0;
    memory::IMemory *mem = 0;
    u8 strBuf[DELL_SYSTEM_STRING_LEN] = { 0, };

    mem = memory::MemoryFactory::getFactory()->getSingleton();

    if( 0 == mem )
        throw InternalErrorImpl();

    // Step 1: Check that "Dell System" is present at the proper offset
    mem->fillBuffer( strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_1, DELL_SYSTEM_STRING_LEN - 1 );
    if( strncmp( reinterpret_cast<char*>(strBuf), DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 )
        if( SYSTEM_ID_DIAMOND == mem->getByte( ID_BYTE_LOC_DIAMOND_1 ) )
            idWord = SYSTEM_ID_DIAMOND;

    mem->fillBuffer( strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_2, DELL_SYSTEM_STRING_LEN - 1 );
    if( strncmp( reinterpret_cast<char*>(strBuf), DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 )
        if( SYSTEM_ID_DIAMOND == mem->getByte( ID_BYTE_LOC_DIAMOND_2 ) )
            idWord = SYSTEM_ID_DIAMOND;

    return idWord;
}

static u16 getIdByteFromOEMItem ()
{
    //functionEnter( "%s", "" );
    u16 idWord = 0;
    const smbios::ISmbiosTable *table = 0;
    smbios::ISmbiosTable::const_iterator item;
    if (!couldBeBayonet())
        goto out;

    table = smbios::SmbiosFactory::getFactory()->getSingleton();

    if (0 == table)
        throw InternalErrorImpl();

    // search through 0x0B (OEM_Strings) items
    for( item = (*table)[OEM_Strings] ; item != table->end(); ++item)
    {
        const char *str = item->getStringByStringNumber (2);
        //isBayonet = true;
        //  Id byte is in second string in table 0x0B
        //  the format is "n[NN]", where NN is the idbyte;
        //  note the &str[2] below to skip the 'n['
        if( 0 != str )
            // quiet vc.net warning using cast
            idWord = static_cast<u16>(strtol( &str[2], NULL, 16 ));
    }

out:
    //functionLeave( "\t\tretval = %i\n", (int)idWord );
    return idWord;
}

static u16 getIdByteFromRevItem ()
{
    //functionEnter( "%s", "" );
    u16 idWord = 0;
    const smbios::ISmbiosTable *table = 0;
    smbios::ISmbiosTable::const_iterator item;

    table = smbios::SmbiosFactory::getFactory()->getSingleton();

    if (0 == table)
        throw InternalErrorImpl();

    // search through 0x0B (Revisions_and_IDs_Structure)
    for( item = (*table)[Dell_Revisions_and_IDs]; item != table->end(); ++item)
    {
        //If byte field is 0xFE, we need to look in the extension field
        idWord = getU8_FromItem(*item, 0x06);
        if( 0xFE == idWord )
        {
            idWord = getU16_FromItem(*item, 0x08);
        }
    }
    //functionLeave( "\t\tretval = %i\n", (int)idWord );
    return idWord;
}

//The code for detecting ID byte in case of Diamond is left out.
//  need to write a function for it.
static struct DellIdByteFunctions
{
    u16 (*f_ptr)();
}
DellIdByteFunctions[] = {
                            {&getIdByteFromMem,},       // normal system -- try this last always.

                            {&getIdByteFromOEMItem,},   // bayonet
                            {&getIdByteFromMem_Diamond,}, // diamond

                            // do this last because this may contain an OEM id
                            // do this as a last resort because it is
                            // unreliable.
                            {&getIdByteFromRevItem,},   // Dell Smbios Revisions and ID's struct
                        };




int SMBIOSGetDellSystemId()
{
    //functionEnter( "%s", "" );
    int systemId = 0;
    int numEntries =
        sizeof (DellIdByteFunctions) / sizeof (DellIdByteFunctions[0]);

    for (int i = 0; i < numEntries; ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            systemId = DellIdByteFunctions[i].f_ptr ();
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
        if (0 != systemId)
        {
            break;
        }
    }

    return systemId;
}

