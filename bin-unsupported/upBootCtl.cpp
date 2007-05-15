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
#include "smbios/ICmosRW.h"
#include "smbios/SystemInfo.h"
#include "getopts.h"

using namespace std;

// retval = 0; successfully activated token
// retval = 1; failed cmos checksum pre-check
// retval = 2; failed to set token
// retval = 3; unknown failure

struct options opts[] =
{
    { 1, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
    { 2, "cmos_file", "Debug: CMOS dump file to use instead of physical cmos", "c", 1 },
    { 3, "set", "Set Boot To UP Flag to true", "s", 0 },
    { 4, "reset", "Set Boot To UP Flag to false", "r", 0 },
    { 5, "get", "Set Boot To UP Flag to true", "g", 0 },
    { 0, NULL, NULL, NULL, 0 }
};

int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        int c;
        char *args = 0;
        memory::MemoryFactory *memoryFactory = 0;
        cmos::CmosRWFactory *cmosFactory = 0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                memoryFactory = memory::MemoryFactory::getFactory();
                memoryFactory->setParameter("memFile", args);
                memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
                break;
            case 2:
                cmosFactory = cmos::CmosRWFactory::getFactory();
                cmosFactory->setParameter("cmosMapFile", args);
                cmosFactory->setMode( cmos::CmosRWFactory::UnitTestMode );
                break;
            case 3:
                if( SMBIOSHasBootToUp() )
                    SMBIOSSetBootToUp( 1 );
                exit( retval );
            case 4:
                if( SMBIOSHasBootToUp() )
                    SMBIOSSetBootToUp( 0 );
                exit( retval );
            case 5:
                if( SMBIOSHasBootToUp() )
                    retval = !SMBIOSGetBootToUp(); /* shell logic here */
                exit( retval );
            default:
                break;
            }
            free(args);
        }

        // default if no other params specified
        if( SMBIOSHasBootToUp() )
            retval = !SMBIOSGetBootToUp(); /* shell logic here */

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
