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

#include <iostream>
#include <string.h>

#include "smbios/ISmi.h"
#include "smbios/SystemInfo.h"
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
{
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
        string password("");
        bool rawPassword=false;

        int c;
        char *args = 0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 249:
                rawPassword = true;
                break;
            case 252:
                password = args;
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

        if((!rawPassword) && (1 == SMBIOSGetSmiPasswordCoding()) && strlen(password.c_str())>0)
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

        cout << "Please wait while verifying password with BIOS..." << endl;
        cout << "Verifying Password: " << flush;

        try
        {
            // try with no password to see if password protection enabled.
            smi::getAuthenticationKey("");
            // only get here if no exception... ie. no password
            cout << "Password NOT installed." << endl;
        }
        catch( const smi::PasswordVerificationFailed &e )
        {
            // ok, now try real password
            smi::getAuthenticationKey(password);
            // if we get here, it is only because no exception thrown above.
            cout << "Password installed. Password PASSED verification." << endl;
        }

        cout << endl;

    }
    catch( const smi::PasswordVerificationFailed &e )
    {
        cout << "Password installed. Password FAILED verification." << endl;
        cout << endl;
        retval = 1;
    }
    catch( const exception &e )
    {
        cerr << endl;
        cerr << "An Error occurred, cannot continue. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "Could not verify password. Common problems are:" << endl;
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
    catch ( ... )
    {
        cerr << endl << "An Unknown Error occurred. Aborting." << endl;
        retval = 2;
    }

    return retval;
}
