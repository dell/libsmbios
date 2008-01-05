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

#include "smbios/ISmi.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/SystemInfo.h" // this is the main header to include to use the C interface
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
    {
        {
            250, "set", "Set Dell Property Tag", "s", 1
        },
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

        bool setVal = false;
        string password("");
        string newTag("");
        bool rawPassword=false;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 250:
                setVal = 1;
                // Property tag can be at most 80 chars (plus '\0')
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

#define PROP_TAG_SIZE 80
        char propertyTag[PROP_TAG_SIZE + 1] =
            {
                0,
            };
        smi::getPropertyOwnershipTag(propertyTag, PROP_TAG_SIZE);
        cout << "Existing Property Ownership Tag: " << propertyTag << endl;

        if (setVal)
        {
            cout << "Changing Property Ownership Tag: " << newTag << endl;
            try
            {
                smi::setPropertyOwnershipTag(password, newTag.c_str(), strlen(newTag.c_str()));
                cout << "Change Successful. The changes may not take effect until reboot, depending on system type." << endl;
            }
            catch( const smi::PasswordVerificationFailed &)
            {
                cerr << endl;
                if( strlen(password.c_str())>0 )
                {
                    cerr << "Could not set tag. BIOS setup password is enabled but the password" << endl;
                    cerr << "given was not accepted by BIOS as the correct password." << endl;
                    cerr << "    -- Verify the password and try again." << endl;
                }
                else
                {
                    cerr << "Could not set tag. BIOS setup password is enabled but no password was given." << endl;
                    cerr << "    -- Try using the '--password' option to specify the BIOS setup password." << endl;
                }
                cerr << endl;
                retval = 1;
            }
            catch( const smi::SmiExecutedWithError &)
            {
                cerr << endl;
                cerr << "Could not set tag. Common problems are:" << endl;
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
                cerr << endl;
                retval = 1;
            }
        }
    }
    catch( const smi::UnsupportedSmi &e )
    {
        cerr << endl;
        cerr << "An Error occurred. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "It appears that this system does not support Property Ownership Tags." << endl;
        cerr << endl;

        retval = 1;
    }
    catch( const exception &e )
    {
        cerr << endl;
        cerr << "An Error occurred. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "Problem reading or writing tag. Common problems are:" << endl;
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
