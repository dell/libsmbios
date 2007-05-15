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

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************

     UNDER CONSTRUCTION AND NOT EVEN CLOSE TO WORKING. 
     DO NOT ATTEMPT TO COMPILE OR RUN UNLESS YOU INTEND TO FINISH THIS

 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
******************************************************************************/

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "smbios/IToken.h"
#include "smbios/IMemory.h"
#include "smbios/ICmosRW.h"
#include "smbios/ISmi.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/SystemInfo.h" // this is the main header to include to use the C interface
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
    {
        { 1, "token",     "Token ID to check.", NULL, 1 },
        { 2, "is_string", "Returns true if token can be accessed as a string token", NULL, 0 },
        { 3, "is_bool",   "Returns true if token can be accessed as a boolean token", NULL, 0 },
        { 4, "activate",  "Activate boolean token", NULL, 0 },
        { 5, "is_active", "Returns true if boolean token given is active", NULL, 0 },
        { 6, "set_string","Set the value for a string token", NULL, 1 },
        { 7, "get_string","Get the value for a string token", NULL, 0 },

        { 252, "password", "BIOS setup password", "p", 1 },
        { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
        { 253, "cmos_file", "Debug: CMOS dump file to use instead of physical cmos", "c", 1 },
        { 254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        int c=0;
        char *args = 0;

        string password("");
        bool rawPassword=false;

        u32 tokenId = 0;
        string newTokenValue("");

        memory::MemoryFactory *memoryFactory = 0;
        cmos::CmosRWFactory *cmosFactory = 0;

        enum { is_string, is_bool, activate, is_active, set_string, get_string } action = is_active;

        cout << "This binary is not completed or fully debugged." << endl;
        cout << " Please do not use this unless you are working on finishing it. :-)" << endl;
        cout << endl;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                tokenId = strtol(args, NULL, 0); 
                break;
            case 2:
                action = is_string;
                break;
            case 3:
                action = is_bool;
                break;
            case 4:
                action = activate;
                break;
            case 5:
                action = is_active;
                break;
            case 6:
                action = set_string;
                newTokenValue = args;
                break;
            case 7:
                action = get_string;
                break;
            case 252:
                password = args;
                break;
            case 249:
                rawPassword = true;
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
            case 255:
                cout << "Libsmbios version:    " << LIBSMBIOS_RELEASE_VERSION << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
        }

        if((action == activate || action == set_string) && (!rawPassword) && (1 == SMBIOSGetSmiPasswordCoding()) && strlen(password.c_str())>0)
        {
            cerr << endl;
            cerr << "BIOS Password encoding has been detected as SCAN CODE format." << endl;
            cerr << "Automatically changing password from ASCII coding to en_US scancode format." << endl;
            cerr << "Use the --rawpassword option to disable this, for example, if you have " << endl;
            cerr << "another language keyboard, then manually convert the ASCII password to" << endl;
            cerr << "scan code format." << endl;
            cerr << endl;

            char *codedPass = new char[strlen(password.c_str())+1];
            memset(codedPass, 0, strlen(password.c_str())+1);
            SMBIOSMapAsciiTo_en_US_ScanCode(codedPass, password.c_str(), strlen(password.c_str()));
            password = codedPass;
            delete []codedPass;
        }

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        cout << "Token ID: 0x" << hex << tokenId << endl;
        switch(action)
        {
            case is_string:
                cout << "Is String: " << 
                    ((*tokenTable)[ tokenId ]->isString() ? "yes" : "no")
                    << endl;
                break;
            case is_active:
                cout << "Is Active: " << 
                    ((*tokenTable)[ tokenId ]->isActive() ? "yes" : "no")
                    << endl;
                break;
            case activate:
                cout << "Activate: ";
                (*tokenTable)[ tokenId ]->activate();
                cout << "done" << endl;
                break;
            case is_bool:
                cout << "Is Boolean: " << 
                    ((*tokenTable)[ tokenId ]->isBool() ? "yes" : "no")
                    << endl;
                break;
            case set_string:
                cout << "Set String: ";
                (*tokenTable)[ tokenId ]->setString(reinterpret_cast<const u8*>(newTokenValue.c_str()), strlen(newTokenValue.c_str()));
                cout << "done" << endl;
                break;
            case get_string:
                cout << "Get String: "
                    << (*tokenTable)[ tokenId ]->getString() 
                    << endl;
                break;
        }
    }
    catch( const exception &e )
    {
        cerr << endl;
        cerr << "An Error occurred. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "Problem reading or writing tag. Common problems are:" << endl;
        cerr << "    -- Password-protection in the BIOS." << endl;
        cerr << "       Try using the '--password' option to specify the BIOS setup password." << endl;
        cerr << endl;
        cerr << "    -- Insufficient permissions to perform operation." << endl;
        cerr << "       Try running as a more privileged account." << endl;
        cerr << "          Linux  : run as 'root' user" << endl;
        cerr << "          Windows: run as 'administrator' user" << endl;
        cerr << endl;
        cerr << "    -- dcdbas device driver not loaded." << endl;
        cerr << "       Try loading the dcdbas driver" << endl;
        cerr << "          Linux  : insmod dcdbas" << endl;
        cerr << "          Windows: dcdbas driver not yet available." << endl;
        cerr << endl;

        retval = 1;
    }
    catch ( ... )
    {
        cerr << endl;
        cerr << "An Unknown Error occurred. Aborting." << endl;
        cerr << endl;
        retval = 2;
    }

    return retval;
}
