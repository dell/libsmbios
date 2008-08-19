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
#include <string.h>

#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/ICmosRW.h"  // only needed if you want to use fake input (cmos.dat)
#include "smbios/SystemInfo.h" // this is the main header to include to use the C interface
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
    {
        {
            254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1
        },
        { 253, "cmos_file", "Debug: CMOS dump file to use instead of physical cmos", "c", 1 },
        { 250, "set", "Set Dell Asset Tag", "s", 1 },
        { 252, "password", "BIOS setup password", "p", 1 },
        { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
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
        memory::MemoryFactory *memoryFactory = 0;
        cmos::CmosRWFactory *cmosFactory = 0;

        bool setVal = false;
        string password("");
        string newTag("");
        bool rawPassword=false;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
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
            case 250:
                setVal = 1;
                // Asset tag can be at most 10 chars (plus '\0')
                newTag = args;
                break;
            case 252:
                password = args;
                break;
            case 249:
                rawPassword = true;
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

        if(setVal && (!rawPassword) && (1 == SMBIOSGetSmiPasswordCoding()) && strlen(password.c_str())>0)
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

        const char *oldTag = SMBIOSGetAssetTag();
        if(!oldTag)
        {
            cerr << endl;
            const char *str = SMBIOSGetSysInfoErrorString();
            if(str)
            {
                cerr << "An Error occurred. The Error message is: " << endl;
                cerr << "    " << str << endl;
                cerr << endl;
            }

            cerr << "Could not read tag. Common problems are:" << endl;
            cerr << "    -- Insufficient permissions to perform operation." << endl;
            cerr << "       Try running as a more privileged account." << endl;
            cerr << "          Linux  : run as 'root' user" << endl;
            cerr << "          Windows: run as 'administrator' user" << endl;
            cerr << endl;
            cerr << "    -- dcdbas device driver not loaded." << endl;
            cerr << "       Try loading the dcdbas driver" << endl;
            cerr << "          Linux  : modprobe dcdbas" << endl;
            cerr << "          Windows: dcdbas driver not yet available." << endl;
            cerr << endl;
            retval = 1;
            // don't bother trying to set, as it will not work.
            setVal = false;
        }
        else
        {
            cout << "Existing Asset Tag: " << oldTag << endl;
            SMBIOSFreeMemory(oldTag);
        }

        if (setVal)
        {
            cout << "Changing Asset Tag: " << newTag << endl;
            int res = SMBIOSSetAssetTag(password.c_str(), newTag.c_str(), strlen(newTag.c_str()));
            if(!res)
            {
                cout << "Change Successful. The changes may not take effect until reboot, depending on system type." << endl;
            }
            else
            {
                cerr << endl;
                const char *str = SMBIOSGetSysInfoErrorString();
                if(str)
                {
                    cerr << "An Error occurred. The Error message is: " << endl;
                    cerr << "    " << str << endl;
                    cerr << endl;
                }
                cerr << "Could not set tag. Common problems are:" << endl;
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
                cerr << "          Linux  : modprobe dcdbas" << endl;
                cerr << "          Windows: dcdbas driver not yet available." << endl;
                retval = 1;
            }
        }
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
