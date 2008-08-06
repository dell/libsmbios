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
#include "smbios/ISmi.h"
#include "smbios/IToken.h"
#include "smbios/ICmosRW.h"
#include "smbios/IObserver.h"

using namespace std;

// retval = 0; successfully activated token
// retval = 1; failed cmos checksum pre-check
// retval = 2; failed to set token
// retval = 3; unknown failure

int
main (int argc, char **argv)
{
    string token( "" );
    if( argc > 1 )
        token = argv[1];

    string fileName( "" );
    if( argc > 2 )
        fileName = argv[2];

    string cmosFileName("");
    if( argc > 3 )
        cmosFileName =  argv[3];

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
    
        // We need to ensure that cmos checksums are correct before we go
        // messing with things, or we could mess up the system.
        cmos::ICmosRW *cmos =
            cmos::CmosRWFactory::getFactory()->getSingleton();
        observer::IObservable *o = dynamic_cast<observer::IObservable*>(cmos);
        bool doUpdate = false;
        // will throw an exception on failure.
        if( o )
            o->notify(&doUpdate);

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        int iToken = strtoul( token.c_str(), 0, 0 );
        while(1)
        {
            try
            {
                (*tokenTable)[ iToken ]->activate();
                break;
            }
            catch (const smbios::NeedAuthentication &e)
            {
                // icky... try compiled in hardcoded password. uncomment to use
                //smbios::IToken *token = &(*((*tokenTable)[ iToken ]));
                //dynamic_cast< smbios::IProtectedToken * >(token)->tryPassword("foo");
                cerr << "Token is a protected type and requires authentication. Use --password option. (which is not implemented yet, sorry.)";
                break;
            }
            catch (const smi::SmiException &e)
            {
                cerr << "Exception trying to run SMI: " << e.what()<< endl;
                break;
            }
            catch (...)
            {
                cerr << "unknown exception." << endl;
                break;
            }
        }
        cout << *(*tokenTable)[iToken] << endl;

    }
    catch( const smbios::InvalidChecksum &e )
    {
        cerr << "Pre-check of CMOS checksum failed. System is in an unknown state, unable to continue."
            << endl
            << "The exact exception was: " 
            << e.what();

        retval = 1;
    }
    catch( const smbios::IException &e )
    {
        cerr << "An Error occurred. The Error message is: " << endl << e.what() << endl;
        retval = 2;
    }
    catch ( ... )
    {
        cerr << "An Unknown Error occurred. Aborting." << endl;
        retval = 3;
    }

    return retval;
}
