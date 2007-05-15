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

#include "smbios/ISmbiosXml.h"
#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
    {
        { 1, "xml", "Dump SMBIOS table structures in XML format", "x", 0 },
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
        memory::MemoryFactory *memoryFactory = 0;
        bool xml = false;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                xml = true;
                break;
            case 254:
                // This is for unit testing. You can specify a file that
                // contains a dump of memory to use instead of writing
                // directly to RAM.
                memoryFactory = memory::MemoryFactory::getFactory();
                memoryFactory->setParameter("memFile", args);
                memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
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

        smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosXmlFactory::getFactory();
        smbios::ISmbiosTable *table = smbiosFactory->getSingleton();

        if(xml)
        {
            cout << "xml not supported yet in this binary." << endl;
            //cout << "<?xml-stylesheet type=\"text/xsl\" version=\"1.0\" encoding=\"UTF-8\"?>\n<SMBIOS>\n";;
            //smbios::toXmlString(*table, cout);
            //cout << "</SMBIOS>" << endl;
        }
        else
            cout << *table << endl;

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
