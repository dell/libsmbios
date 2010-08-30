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
#include <string.h> // memset
#include <stdlib.h>

#ifdef sun
#  include <sys/sysi86.h>
#  include <sys/smbios.h>
#  define iopl(x) sysi86(SI86V86, V86SC_IOPL, 0x3000)
#else
#  include <sys/io.h>
#endif

#include "smbios/ISmbios.h"
#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/SystemInfo.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

#if defined(DEBUG_MEDIA_DIRECT)
#   undef DCOUT DCERR
#   include <iostream>
#   define DCOUT  _dbg_iostream_out(cout, line)
#   define DCERR  _dbg_iostream_out(cerr, line)
#endif

using namespace std;

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define __asm__ asm
#endif

struct options opts[] =
    {
        { 1, "info", "Display Media Direct Info", "i", 0 },
        { 2, "remove", "Remove Media Direct Xloader BIOS Support", "r", 0 },
        { 3, 0, "Install Xloader Blocks", "b", 0 },
        { 4, "lowlba", "Low 32-bits of LBA for Xloader blocks", 0, 0 },
        { 5, "highlba", "High 32-bits of LBA for Xloader blocks", 0, 0 },
        { 254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };


#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct mediaDirectTable {
    u8 signature[4];
    u8 versionMinor;
    u8 versionMajor;
    u8 checksum;
    u8 portIndex;
    u8 portValue;
}
LIBSMBIOS_PACKED_ATTR;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

struct smiRegs
{
    u32 eax; // output only
    u32 ebx; // output only
    u32 ecx; // output only
    u32 esi; // input/output
    u32 edi; // input/output
};

enum {
    E_BLOCK_START = 0xE0000UL,
    F_BLOCK_START = 0xF0000UL,
    F_BLOCK_END = 0xFFFFFUL
};

void getMediaDirectTable(mediaDirectTable *mdTable);
void printMDInfo(mediaDirectTable *mdTable);
void removeXloader(mediaDirectTable *mdTable);
void installXloader(mediaDirectTable *mdTable, u32 highLba, u32 lowLba);

int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        int c=0;
        char *args = 0;
        memory::MemoryFactory *memoryFactory = 0;
        bool lowInit = false, highInit = false;
        u32 lowLba = 0, highLba = 0;

        memoryFactory = memory::MemoryFactory::getFactory();
        mediaDirectTable mdTable;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:  // -i, --info
                getMediaDirectTable(&mdTable);
                printMDInfo(&mdTable);
                break;
            case 2:  // -r, --remove
                getMediaDirectTable(&mdTable);
                printMDInfo(&mdTable);
                removeXloader(&mdTable);
                break;
            case 3: // -b, --break-my-machine
                if( ! (lowInit && highInit )) {
                    cerr << "Need low/high LBA. (cmdline order matters)" << endl;
                    break;
                }
                getMediaDirectTable(&mdTable);
                installXloader(&mdTable, lowLba, highLba);
                printMDInfo(&mdTable);
                break;
            case 4: // --lowlba
                lowInit = 1;
                lowLba = strtol(args, 0, 0);
                break;
            case 5: // --highlba
                highInit = 1;
                highLba = strtol(args, 0, 0);
                break;
            case 254: // -m FILE, --memory FILE
                // This is for unit testing. You can specify a file that
                // contains a dump of memory to use instead of writing
                // directly to RAM.
                memoryFactory->setParameter("memFile", args);
                memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
                break;
            case 255: // -v, --version
                cout << "Libsmbios version:    " << SMBIOSGetLibraryVersionString() << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
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

class MyError : public std::exception
{
    private:
        std::string s;
    public:
        MyError(std::string s_) : s(s_) { };
        ~MyError() throw() { };
        const char* what() const throw() { return s.c_str(); };
};


void _callSmi(smiRegs *r, u8 port)
{
    iopl(3);

    __asm__ __volatile__ (
           //   magic    port
        "outb   %%al,    %%dx     \n\t"

        : /* output args */
          "=a" (r->eax),
          "=b" (r->ebx),
          "=c" (r->ecx),
          "=S" (r->esi),
          "=D" (r->edi)
        : /* input args */
          "0" (r->eax),
          "1" (r->ebx),
          "2" (r->ecx),
          "3" (r->esi),
          "4" (r->edi),
          "d" (port)
        /* no clobber */
    );
}

void callSmi(mediaDirectTable *md, smiRegs *r, u8 function, u8 subFunction)
{
	// set AL = smi port
    // set AH = function
    r->eax = function << 8 | md->portValue;

    // set BL = subfunction
    r->ebx = subFunction;

    _callSmi(r, md->portIndex);

    if (r->eax == 0){
        // success
    } else if( (s32)r->eax == -1 ) {
        // failure
        DCERR("smi failure" << endl);
        throw "smi failure";  // TODO: make this a regular class-based exception
    } else {
        DCERR("SMI NOT SUPPORTED" << endl);
        throw "SMI NOT SUPPORTED";  // TODO: make this a regular class-based exception
    }
}

#define BIT_IS_SET(bit, value)  ( (value) & (1<<bit) )

void printMDInfo(mediaDirectTable *mdTable)
{
    smiRegs r = {0,};

    cout << "Media Direct Info:" << endl;
    cout << "\tBIOS MD Version: " << (int)mdTable->versionMajor << "." << (int)mdTable->versionMinor << endl;

    // call info smi
    callSmi(mdTable, &r, 0x01 ,0);

    //BX[16-5]=   - RESERVED FOR FUTURE USE
    //BX[0]   = 0 - system is NOT “Media Direct” capable
    //        = 1 - system is “Media Direct” capable
    cout << "\tMedia Direct Capable           : " <<  (BIT_IS_SET(0, r.ebx) ?  "yes" : "no") << endl;

    //BX[1]   = 0 - user did NOT press the MD button to start the system
    //        = 1 - user pressed the MD button to start the system
    cout << "\tSystem Start via MD Button     : " <<  (BIT_IS_SET(1, r.ebx) ?  "yes" : "no") << endl;

    //BX[2]   = 0 - BIOS does NOT support the Vista HotStart feature
    //        = 1 - BIOS supports the Vista HotStart feature
    cout << "\tBIOS Support for Vista HotStart: " <<  (BIT_IS_SET(2, r.ebx) ?  "yes" : "no") << endl;

    //BX[3]   = 0 - Pretty Boot mode is NOT active
    //        = 1 - Pretty Boot mode is active
    cout << "\tPretty Boot Active             : " <<  (BIT_IS_SET(3, r.ebx) ?  "yes" : "no") << endl;

    //BX[4]   = 0 - BIOS does NOT support xloder extended functions
    //        = 1 -BIOS supports the xloader extended functions
    cout << "\tBIOS Supports extended xloader : " <<  (BIT_IS_SET(4, r.ebx) ?  "yes" : "no") << endl;

    callSmi(mdTable, &r, 0x02 ,0);
    cout << "\tBIOS Configuration Changed     : " <<  (BIT_IS_SET(0, r.ebx) ?  "yes" : "no") << endl;

    callSmi(mdTable, &r, 0x04 ,0);

    cout << "\tXloader Configured             : " <<  (BIT_IS_SET(0, r.ebx) ?  "yes" : "no") << endl;

	cout << hex;
    // following info is only valid if "Xloader Configured", above, is yes.
	cout << "\tXloader Revision               : " <<  r.ecx << endl;
	cout << "\tXloader Low 32-bit LBA         : " <<  r.esi << endl;
	cout << "\tXloader High 32-bit LBA        : " <<  r.edi << endl;
    cout << dec;

    cout << "DONE." << endl;
}

void installXloader(mediaDirectTable *mdTable, u32 lowLba, u32 highLba)
{
    smiRegs r = {0,};
    r.esi = lowLba;
    r.edi = highLba;
    cout << "Installing Xloader sector information" << endl;
	cout << "\tXloader Low 32-bit LBA         : " <<  r.esi << endl;
	cout << "\tXloader High 32-bit LBA        : " <<  r.edi << endl;
    callSmi(mdTable, &r, 0x04 ,1);
    cout << "DONE." << endl;
}

void removeXloader(mediaDirectTable *mdTable)
{
    smiRegs r = {0,};
    cout << "Removing Xloader sector information from BIOS" << endl;
    callSmi(mdTable, &r, 0x04 ,2);
    cout << "DONE." << endl;
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
            for(unsigned int i=0; i<sizeof(*mdTable); ++i){
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





