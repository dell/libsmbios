/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2005 David Greaves <david@dgreaves.com>
 * Licensed under the GNU General Public License as published 
 * by the Free Software Foundation; either version 2 of the License, 
 * or (at your option) any later version.
 *
 * Derived from activeCmosToken.cpp by Michael Brown
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 */

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <string>
#include <iostream>
#include <stdlib.h>

#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/IToken.h"
#include "smbios/ICmosRW.h"
#include "smbios/IObserver.h"
#include "smbios/SystemInfo.h"
#include "getopts.h"

using namespace std;

// This program sets and activates the BIOS Wakeup timer on an SC420 (and...??)
// Called with no paramters it reports the current Wakeup timer state.

// retval = 0; successfully set wakeup time
// retval = 1; failed cmos checksum pre-check
// retval = 2; failed to set time
// retval = 3; unknown failure

#define WAKE_HOUR_LOCN (0x2b)
#define WAKE_MIN_LOCN  (0x2c)

#define WAKE_DISABLE   (0x28)
#define WAKE_EVERYDAY  (0x29)
#define WAKE_WEEKDAY   (0x2A)

struct options opts[] =
    {
        { 1, "hour",     "set wakeup hour", NULL, 1 },
        { 2, "minute",   "set wakeup minute", NULL, 1 },
        { 3, "everyday", "set wakeup for everyday", NULL, 0 },
        { 4, "weekday",  "set wakeup for weekdays only", NULL, 0 },
        { 5, "disable",  "disable wakeups", NULL, 0 },
        { 252, "password", "BIOS setup password", "p", 1 },
        { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
        { 253, "cmos_file", "Debug: CMOS dump file to use instead of physical cmos", "c", 1 },
        { 254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

void getCurrentWakeup(smbios::ITokenTable *tokenTable, string prefix)
{
    u8 hour_c[2] = {0};
    u8 minute_c[2] = {0};
    (*tokenTable)[ WAKE_HOUR_LOCN ]->getString(hour_c,2);
    (*tokenTable)[ WAKE_MIN_LOCN ]->getString(minute_c,2);
    cout << prefix << " wakeup type: " <<
            ((*tokenTable)[ WAKE_EVERYDAY ]->isActive()?"Every day":"") <<
            ((*tokenTable)[ WAKE_WEEKDAY  ]->isActive()?"Week days":"") <<
            ((*tokenTable)[ WAKE_DISABLE  ]->isActive()?"Disabled":"") <<
            endl;

    if (!(*tokenTable)[WAKE_DISABLE]->isActive() )
    {
        cout << prefix << " wakeup time: "  << 
            hex << static_cast<int>(hour_c[0])   << ":" <<
                   static_cast<int>(minute_c[0]) << dec << endl;
    }
}

void checkExistingChecksums(bool doUpdate=false)
{
    // We need to ensure that cmos checksums are correct before we go
    // messing with things, or we could mess up the system.
    cmos::ICmosRW *cmos =
        cmos::CmosRWFactory::getFactory()->getSingleton();
    observer::IObservable *o = dynamic_cast<observer::IObservable*>(cmos);
    // will throw an exception on failure.
    if( o )
        o->notify(&doUpdate);
}

int
main (int argc, char **argv)
{
    int retval = 0; // success
    try
    {
        int wake_hour=0, wake_min=0;
        
        int wake_hour_bcd = 0, wake_min_bcd = 0;
        bool doSet=false;
        int token=0;

        string password("");
        bool rawPassword=false;

        int c=0;
        char *args = 0;
        memory::MemoryFactory *memoryFactory = 0;
        cmos::CmosRWFactory *cmosFactory = 0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:  // hour
                wake_hour = strtoul(args, 0, 0);
                if (wake_hour < 0 or wake_hour >23) 
                {
                    cerr << "Wake hour: " << args << " must be between 0 and 23" << endl;
                    throw "bad data";
                }
                wake_hour_bcd = ((wake_hour / 10) <<4) + (wake_hour % 10);
                break;
            case 2: // minute
                wake_min = strtoul(args, 0, 0);
                if (wake_min < 0 or wake_min >59) 
                {
                    cerr << "Wake min: " << args << " must be between 0 and 59" << endl;
                    throw "bad data";
                }
                wake_min_bcd = ((wake_min / 10) << 4) + (wake_min % 10);
                break;
            case 3: // everyday
                doSet = true;
                token = WAKE_EVERYDAY;
                break;
            case 4: // weekday
                doSet = true;
                token = WAKE_WEEKDAY;
                break;
            case 5: // disable
                doSet = true;
                token = WAKE_DISABLE;
                wake_hour_bcd=0;
                wake_min_bcd=0;
                break;
            case 254:
                // This is for unit testing. You can specify a file that
                // contains a dump of memory to use instead of writing
                // directly to RAM.
                memoryFactory = memory::MemoryFactory::getFactory();
                memoryFactory->setParameter("memFile", args);
                memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
                break;
            case 253:
                // ditto, except for CMOS
                cmosFactory = cmos::CmosRWFactory::getFactory();
                cmosFactory->setParameter("cmosMapFile", args);
                cmosFactory->setMode( cmos::CmosRWFactory::UnitTestMode );
                break;
            case 252:
                password = args;
                break;
            case 249:
                rawPassword = true;
                break;
            case 255:
                cout << "Libsmbios version:    " << SMBIOSGetLibraryVersionString() << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
        }

        checkExistingChecksums();

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        // get wakeup...
        getCurrentWakeup(tokenTable, "Current");

        if( doSet )
        {
            // set wakeup...
            (*tokenTable)[ WAKE_HOUR_LOCN ]->setString( reinterpret_cast<const u8*>(&wake_hour_bcd), 1);  
            (*tokenTable)[ WAKE_MIN_LOCN ]->setString( reinterpret_cast<const u8*>(&wake_min_bcd), 1);
            (*tokenTable)[ token ]->activate();

            getCurrentWakeup(tokenTable, "Set");
        }
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


