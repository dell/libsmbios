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

#include "smbios/ISmi.h"  // only needed if you want to use fake input (memdump.dat)
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
{
    { 1,  "location", "which location to read", "l", 1 },
    { 2,  "value",    "set new value",          "v", 1 },
    { 3,  "nv",       "read nvram",             "n", 0 },
    { 4,  "battery",  "read battery setting",   "b", 0 },
    { 5,  "ac",       "read ac setting",        "a", 0 },
    { 6,  "sysstat",  "read system status",     "s", 0 },
    { 7,  "display",  "get basic display type", "d", 0 },
    { 8,  "chdisp",   "set active display",     "c", 1 },
    { 11, "password",   "password to use", "p", 1 },
    { 0, NULL, NULL, NULL, 0 }
};

int
main (int argc, char **argv)
{
    int retval = 0;

    int location=-1;
    u32 curValue=0;
    u32 minValue=0;
    u32 maxValue=0;
    bool set=false;
    u32 newValue=0;

    char *password=static_cast<char *>(malloc(0));

    try
    {
        char *args = 0;
        int c=0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                location = strtol(args, NULL, 0);
                break;
            case 2:
                set=true;
                newValue = strtol(args, NULL, 0);
                break;
            case 3:
                cout << hex;
                cout << "Read NV Storage" << endl;
                curValue = smi::readNVStorage(location, &minValue, &maxValue);
                cout << "   Location: " << location << endl;
                cout << "    current: " << curValue << endl;
                cout << "        min: " << minValue << endl;
                cout << "        max: " << maxValue << endl << endl;;
                if(set)
                {
                    cout << "Write NV Storage" << endl;
                    curValue = smi::writeNVStorage(password, location, newValue, &minValue, &maxValue);
                    cout << "   Location: " << location << endl;
                    cout << "    current: " << curValue << endl;
                    cout << "        min: " << minValue << endl;
                    cout << "        max: " << maxValue << endl << endl;;
                }
                break;
            case 4:
                cout << hex;
                cout << "Read Battery Mode Setting" << endl;
                curValue = smi::readBatteryModeSetting(location, &minValue, &maxValue);
                cout << "   Location: " << location << endl;
                cout << "    current: " << curValue << endl;
                cout << "        min: " << minValue << endl;
                cout << "        max: " << maxValue << endl << endl;;
                if(set)
                {
                    cout << "Write Battery Mode Setting" << endl;
                    curValue = smi::writeBatteryModeSetting(password, location, newValue, &minValue, &maxValue);
                    cout << "   Location: " << location << endl;
                    cout << "    current: " << curValue << endl;
                    cout << "        min: " << minValue << endl;
                    cout << "        max: " << maxValue << endl << endl;;
                }
                break;
            case 5:
                cout << "Read AC Mode Setting" << endl;
                curValue = smi::readACModeSetting(location, &minValue, &maxValue);
                cout << hex;
                cout << "   Location: " << location << endl;
                cout << "    current: " << curValue << endl;
                cout << "        min: " << minValue << endl;
                cout << "        max: " << maxValue << endl << endl;;
                if(set)
                {
                    cout << "Write AC Mode Setting" << endl;
                    curValue = smi::writeACModeSetting(password, location, newValue, &minValue, &maxValue);
                    cout << "   Location: " << location << endl;
                    cout << "    current: " << curValue << endl;
                    cout << "        min: " << minValue << endl;
                    cout << "        max: " << maxValue << endl << endl;;
                }
                break;
            case 6:
                curValue = smi::readSystemStatus(&maxValue);
                cout << hex;
                cout << "System Status" << endl;
                cout << "   Failing Sensor Count : " << curValue << endl;
                cout << "   Failing Sensor Handle: " << maxValue << endl << endl;
                break;
            case 7:
                {
                u32 type=0, resolution=0, mem=0, horiz=0, vert=0;
                smi::getDisplayType(type, resolution, mem);
                cout << hex;
                cout << "Display Properties" << endl;
                cout << "   Type           : " << static_cast<int>(type) << endl;
                cout << "   Resolution code: " << static_cast<int>(resolution) << endl;
                cout << "   Number of 256kb Mem chunks: " << static_cast<int>(mem) << endl;

                smi::getPanelResolution(horiz, vert);
                cout << "   Horizontal Resolution: " << dec << static_cast<int>(horiz) << endl;
                cout << "   Vertical Resolution  : " << dec << static_cast<int>(vert) << endl;

                smi::getActiveDisplays(type);
                cout << "   Active Code: " << hex << (type & 0x01) << dec << endl;
                cout << "   Laptop LCD Active  : " << static_cast<bool>((type & 0x01)) << endl;
                cout << "   External TV Active : " << static_cast<bool>((type & 0x02)) << endl;
                cout << "   External DVI Active: " << static_cast<bool>((type & 0x04)) << endl;
                cout << endl;
                }
                break;
            case 8:
                newValue = strtol(args, NULL, 0);
                cout << "setting active display code: " << hex << static_cast<int>(newValue) << endl;
                smi::setActiveDisplays(newValue);
                break;
            case 11:
                free(password);
                password = 0;
                password = strndup(args, 256);
                cout << "using password if necessary: " << password << endl;
                break;
            default:
                break;
            }
            free(args);
        }
        

        free(password);
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
