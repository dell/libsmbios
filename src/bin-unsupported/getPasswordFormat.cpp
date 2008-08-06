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

#include "smbios/ISmi.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
{
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
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            default:
                break;
            }
            free(args);
        }

        smi::password_format_enum fmt = smi::getPasswordFormat();

        cout << "The password format is: ";

        if( fmt == smi::PW_FORMAT_SCAN_CODE )
            cout << "SCAN CODE" << endl;

        if( fmt == smi::PW_FORMAT_ASCII )
            cout << "ASCII" << endl;
        
        if( fmt == smi::PW_FORMAT_UNKNOWN )
            cout << "UNKNOWN (probably scan code)" << endl;

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
