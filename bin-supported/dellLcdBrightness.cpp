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

#include "smbios/ISmi.h"
#include "smbios/IToken.h"
#include "smbios/SystemInfo.h"
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
{
    { 2,  "value",    "set new value",          "v", 1 },
    { 4,  "battery",  "read battery setting",   "b", 0 },
    { 5,  "ac",       "read ac setting",        "a", 0 },
    { 252, "password", "BIOS setup password", "p", 1 },
    { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
    { 255, "version", "Display libsmbios version information", NULL, 0 },
    { 0, NULL, NULL, NULL, 0 }
};

typedef u32 (*readfn)(u32 location, u32 *minValue, u32 *maxValue);
typedef u32 (*writefn)(const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue);

int
main (int argc, char **argv)
{
    int retval = 0;

    u8 location=0;
    u32 curValue=0;
    u32 minValue=0;
    u32 maxValue=0;

    bool set=false;
    u32 newValue=0;

    enum { batteryMode, acMode } mode = batteryMode;
    readfn readFunction = &smi::readBatteryModeSetting;
    writefn writeFunction = &smi::writeBatteryModeSetting;
    const char *batStr = "Battery";
    const char *acStr = "AC";
    const char *defStr = batStr;

    string password("");
    bool rawPassword = false;
    const int DELL_LCD_BRIGHNESS_TOKEN = 0x007d;

    try
    {
        char *args = 0;
        int c=0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 2:
                set=true;
                newValue = strtol(args, NULL, 0);
                break;

            case 4:
                mode = batteryMode;
                readFunction = &smi::readBatteryModeSetting;
                writeFunction = &smi::writeBatteryModeSetting;
                defStr = batStr;
                break;

            case 5:
                mode = acMode;
                readFunction = &smi::readACModeSetting;
                writeFunction = &smi::writeACModeSetting;
                defStr = acStr;
                break;

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

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        smbios::IToken *token = &(*((*tokenTable)[ DELL_LCD_BRIGHNESS_TOKEN ]));
        dynamic_cast< smbios::ISmiToken * >(token)->getSmiDetails( static_cast<u16*>(0), static_cast<u8*>(0), &location );

        cout << hex;
        cout << "Read " << defStr << " Mode Setting" << endl;
        curValue = readFunction(location, &minValue, &maxValue);
        cout << "    current: " << curValue << endl;
        cout << "        min: " << minValue << endl;
        cout << "        max: " << maxValue << endl << endl;;
        if(set && newValue >= minValue && newValue <= maxValue)
        {
            cout << "Write " << defStr << " Mode Setting" << endl;
            curValue = writeFunction(password, location, newValue, &minValue, &maxValue);
            cout << "    current: " << curValue << endl;
            cout << "        min: " << minValue << endl;
            cout << "        max: " << maxValue << endl << endl;;
        } else if(set) {
            cout << endl;
            cout << "Tried to set brighness level outside of allowable range." << endl;
            cout << endl;
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
