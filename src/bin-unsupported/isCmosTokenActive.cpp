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

using namespace std;

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

    cout << "Running..." << endl;

    int retval = 1;
    try
    {
        if( fileName != "" )
        {
            memory::MemoryFactory *memoryFactory = memory::MemoryFactory::getFactory();
            memoryFactory->setParameter("memFile", fileName);
            memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
        }

        if( cmosFileName != "" )
        {
            cmos::CmosRWFactory *cmosFactory = cmos::CmosRWFactory::getFactory();
            cmosFactory->setParameter("cmosMapFile", cmosFileName);
            cmosFactory->setMode( cmos::CmosRWFactory::UnitTestMode );
        }

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        int iToken = strtoul( token.c_str(), 0, 0 );
        bool active = (*tokenTable)[ iToken ]->isActive();
        cout << *(*tokenTable)[iToken] << endl;
        retval = !active;
    }
    catch ( smbios::IException &e )
    {
        cout << e.what() << endl;
        retval = 1;
    }
    catch ( exception &e )
    {
        cout << e.what() << endl;
        retval = 1;
    }
    catch ( ... )
    {
        cout << "Unknown error occurred. Aborting." << endl;
        retval = 2;
    }


    return retval;
}
