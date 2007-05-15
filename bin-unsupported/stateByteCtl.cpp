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
#include <iomanip>
#include <stdlib.h>

#include "smbios/IMemory.h"
#include "smbios/ICmosRW.h"
#include "smbios/SystemInfo.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

// retval = 0; successfully activated token
// retval = 1; failed cmos checksum pre-check
// retval = 2; failed to set token
// retval = 3; unknown failure

struct options opts[] =
{
    { 1, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
    { 2, "cmos_file", "Debug: CMOS dump file to use instead of physical cmos", "c", 1 },
    { 3, "set", "Set CMOS state byte to new value", "s", 1 },
    { 4, "owner", "Set state byte owner", "o", 1 },
    { 0, NULL, NULL, NULL, 0 }
};

class myException: public exception {
    public:
        myException(const std::string &initMsg): msg(initMsg) {};
        virtual ~myException() throw() {};
        virtual const char *what() const throw() { return msg.c_str(); };
    private:
        std::string msg;
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
        int owner = 0;
        bool set = false;
        int newvalue = 0;
        bool ownerset = false;
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
                set = true;
                newvalue = strtoul( args, 0, 0 );
                break;
            case 4:
                ownerset = true;
                owner = strtoul( args, 0, 0 );
                break;
            default:
                break;
            }
            free(args);
        }

        if( !ownerset )
            throw myException( _("You must set 'owner'.") );

        if( set )
            if( SMBIOSHasNvramStateBytes() )
                SMBIOSSetNvramStateBytes( newvalue, owner );

        if( SMBIOSHasNvramStateBytes() )
            retval = SMBIOSGetNvramStateBytes( owner ); 

        cout << hex << "State Byte: 0x" << setw(4) << setfill('0') << retval << endl;
    }
    catch( const smbios::IException &e )
    {
        cerr << "An Error occurred. The Error message is: " << endl << e.what() << endl;
        retval = 1;
    }
    catch( const exception &e )
    {
        cerr << "An Error occurred. The Error message is: " << endl << e.what() << endl;
        retval = 1;
    }
    catch ( ... )
    {
        cerr << "An Unknown Error occurred. Aborting." << endl;
        retval = 2;
    }

    return retval;
}
