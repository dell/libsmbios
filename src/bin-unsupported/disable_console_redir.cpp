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

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <string>
#include <iostream>
#include <memory>
#include <stdlib.h>

#include "smbios/IMemory.h"
#include "smbios/ISmbios.h"
#include "smbios/IToken.h"
#include "smbios/ICmosRW.h"
#include "smbios/SystemInfo.h"
#include "smbios/IObserver.h"

using namespace std;

// The purpose of this binary was as a temporary workaround to a BIOS bug 
// in the development BIOSen. It is no longer used, but is here as an 
// example of manipulating tokens.

// retval = 0; was 8G system, redir on, turned off
// retval = 1; was 8G system, redir off
// retval = 2; not 8G system
// retval = 3; checksum precheck failed
// retval = 4; error doing any of the above

class already_off : public exception {};
class not_8G : public exception {} ;
class checksum_fail : public exception {} ;
class error_getting_id : public exception {} ;

int
main (int argc, char **argv)
{
    string fileName( "" );
    if( argc > 1 )
        fileName = argv[1];

    string cmosFileName("");
    if( argc > 2 )
        cmosFileName =  argv[2];

    int retval = 0;
    try
    {
        if( fileName != "" )
        {
            memory::MemoryFactory *memoryFactory = memory::MemoryFactory::getFactory();
            memoryFactory->setParameter("memFile", fileName);
            memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
        }

        cmos::CmosRWFactory *cmosFactory = cmos::CmosRWFactory::getFactory();
        if( cmosFileName != "" )
        {
            cmosFactory->setParameter("cmosMapFile", cmosFileName);
            cmosFactory->setMode( cmos::CmosRWFactory::UnitTestMode );
        }

        // First check that we are on a PE1850/2850/2800.
        int id = SMBIOSGetDellSystemId();

        if ( 0 == id )
            throw error_getting_id();

        if ( 0x16c == id )
            goto good;

        if ( 0x16d == id )
            goto good;

        if ( 0x16e == id )
            goto good;

        if ( 0x183 == id )
            goto good;

        throw not_8G();

good:
        // We need to ensure that cmos checksums are correct before we go
        // messing with things, or we could mess up the system.
        cmos::ICmosRW *cmos =
            cmos::CmosRWFactory::getFactory()->getSingleton();
        observer::IObservable *o = dynamic_cast<observer::IObservable*>(cmos);
        bool doUpdate = false;
        // will throw an exception (InvalidChecksum) on failure.
        if( o )
            o->notify(&doUpdate);

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        // this is the token that de-activates redirection.
        int console_redirection_disable_token = 0x401d;

        if( (*tokenTable)[ console_redirection_disable_token ]->isActive() )
            throw already_off();

        (*tokenTable)[ console_redirection_disable_token ]->activate();
    }

    catch( const smbios::InvalidChecksum &e )
    {
        cerr << "Pre-check of CMOS checksum failed. System is in an unknown state"
            << endl
            << "The exact exception was: " 
            << e.what();

        throw checksum_fail();
    }
    catch( const already_off &e )
    {
        retval = 1;    
    }
    catch( const not_8G &e )
    {
        retval = 2;    
    }
    catch( const checksum_fail &e )
    {
        retval = 3;    
    }
    catch ( ... )
    {
        retval = 4;    
    }

    return retval;
}
