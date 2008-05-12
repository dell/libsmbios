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

#include "smbios/ISmbios.h"
#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

#if defined(DEBUG_MEDIA_DIRECT)
#   define DCOUT(line) do { cout << line; } while(0)
#   define DCERR(line) do { cerr << line; } while(0)
#else
#   define DCOUT(line) do {} while(0)
#   define DCERR(line) do {} while(0)
#endif

using namespace std;

struct options opts[] =
    {
        { 254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };


#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct mediaDirectTable {
    u8 signature[4];
    u8 versionMajor;
    u8 versionMinor;
    u8 checksum;
    u8 portIndex;
    u8 portValue;
}
LIBSMBIOS_PACKED_ATTR;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

enum {
    E_BLOCK_START = 0xE0000UL,
    F_BLOCK_START = 0xF0000UL,
    F_BLOCK_END = 0xFFFFFUL
};

void getMediaDirectTable(mediaDirectTable *mdTable);

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

        memoryFactory = memory::MemoryFactory::getFactory();
        mediaDirectTable mdTable;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 254:
                // This is for unit testing. You can specify a file that
                // contains a dump of memory to use instead of writing
                // directly to RAM.
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

        DCOUT("Hello World" << endl);
        getMediaDirectTable(&mdTable);

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

class MyError : public std::exception
{
    private:
        std::string s;
    public:
        MyError(std::string s_) : s(s_) { };
        ~MyError() throw() { };
        const char* what() const throw() { return s.c_str(); };
};


struct smiRegs
{
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
    u8  port;
    u8  magic;
};

void callSmi(smiRegs *r)
{
    asm volatile (
        ""
        "outb %b0,%w1"
        ""
        : /* no output args */
        : "a" (r->magic),
          "d" (r->port),
          "b" (r->ebx),
          "c" (r->ecx)
        : "memory"
    );
}

void getMediaDirectTable(mediaDirectTable *mdTable)
{
    memory::IMemory *mem = memory::MemoryFactory::getFactory()->getSingleton();
    unsigned long fp = F_BLOCK_START;

    DCERR("getMediaDirectTable() Memory scan for Media Direct table." << endl);

    // tell the memory subsystem that it can optimize here and 
    // keep memory open while we scan rather than open/close/open/close/... 
    // for each fillBuffer() call
    // 
    // this would be safer if we used spiffy c++ raii technique here
    mem->decReopenHint();

    while ( (fp + sizeof(*mdTable)) < F_BLOCK_END)
    {
        memset(mdTable, 0, sizeof(*mdTable));
        mem->fillBuffer(
            reinterpret_cast<u8 *>(mdTable),
            fp,
            sizeof(*mdTable)
        );

        // search for promising looking headers
        // first, look for old-style DMI header
        if (memcmp (mdTable->signature, "MD20", 4) == 0)
        {
            DCERR("Found MD20 anchor. Trying to parse legacy DMI structure." << endl);
            u8 checksum=0;
            for(int i=0; i<sizeof(*mdTable); ++i){
                u8 byte = reinterpret_cast<u8*>(mdTable)[i];
                checksum += byte;
                DCERR("byte " << dec << i << " = " << hex << (int)byte << endl);
                DCERR("   checksum: " << hex << (int)checksum << endl);
            }

            if(checksum == 0) // check the checksum
            {
                DCERR("Found valid Media Direct entry point at offset: " << fp << endl);
                break;
            }
        }

        fp += 4;
    }

    // dont need memory optimization anymore
    mem->incReopenHint();

    // bad stuff happened if we got to here and fp > 0xFFFFFL
    if ((fp + sizeof(*mdTable)) >= F_BLOCK_END)
    {
        memset(mdTable, 0, sizeof(*mdTable));
        throw MyError("Unable to find table entry point.");
    }
}





