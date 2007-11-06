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
#include <iomanip>
#include <memory>       // auto_ptr
#include <stdlib.h>     // strtoul
#include <set>
#include <stdio.h>        // fopen()

//#include <unistd.h>    // getopt()

#include "smbios/ICmosRW.h"
#include "smbios/IMemory.h"
#include "smbios/ISmbios.h"

// fix this... this is a namespace violation
#include "../libraries/common/TokenLowLevel.h"

using namespace std;
using namespace smbios;
using namespace cmos;

void dumpMemory( const string &memDumpFile );
void dumpCmos( const string &cmosOutput, set<int> indexPorts );

int
main (int argc, char **argv)
{
    (void)argc;
    (void)argv;

    string memdumpFile = "memdump.dat";
    string cmosdumpFile = "cmos.dat";

    // TODO
    // put cmdline parsing here.
    //

    int retval = 0;
    try 
    {
        // do this first, it will raise an exception on error and prevent creation of zero-len
        // dat files.
        const smbios::ISmbiosTable *table = 
            smbios::SmbiosFactory::getFactory()->getSingleton();

        FILE *fd;
        fd = fopen( memdumpFile.c_str(), "w+" );
        fclose(fd);
        fd = fopen( cmosdumpFile.c_str(), "w+" );
        fclose(fd);
    
        // main code begins...
        set<int> indexPorts;
    
        for( smbios::ISmbiosTable::iterator item = (*table)[0xD4] ; item != table->end(); ++item)
        {
            const u8 *ptr = 0;
            try  // wrap in try{} to avoid mem leaks on exception
            {
                size_t size = 0;  //getBuffer puts size of returned buffer here.
                ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
                const indexed_io_access_structure *io_struct =
                    reinterpret_cast<const indexed_io_access_structure *>(ptr);
                indexPorts.insert( io_struct->indexPort  );
            }
            catch ( ... )
            {
                // make sure this is always in sync with below
                delete [] const_cast<u8*>(ptr);
                ptr = 0;
                throw;
            }
            // make sure this is always in sync with the above.
            delete [] const_cast<u8*>(ptr);
            ptr = 0;
        }
    
        cout << "Dumping memory to file: " << memdumpFile << endl;
        dumpMemory( memdumpFile );
    
        cout << "Dumping cmos to file: " << cmosdumpFile << endl;
        dumpCmos( cmosdumpFile, indexPorts );
    }
    catch( const smbios::IException &e )
    {
        cerr << "An Error occurred. The Error message is: " << endl << e.what() << endl;
        retval = 2;
    }
    catch ( ... )
    {
        cerr << "An Unknown Error occurred. Aborting." << endl;
        retval = 3;
    }

    return retval;
}


void dumpMemory( const string &memDumpFile )
{
    // make one connected to real memory
    auto_ptr<memory::IMemory> realMem(memory::MemoryFactory::getFactory()->makeNew());

    // make a file-backed mem obj.
    memory::MemoryFactory::getFactory()->setParameter("memFile", memDumpFile);
    memory::MemoryFactory::getFactory()->setMode( memory::MemoryFactory::UnitTestMode );
    auto_ptr<memory::IMemory> fileMem(memory::MemoryFactory::getFactory()->makeNew());

    // dump the first meg of mem.
    for (unsigned int i=0xA0000; i<=0xFFFFF; ++i )
    {
        //      0xFFFFF
        fileMem->putByte( i, realMem->getByte(i) );
    }
}

void dumpCmos( const string &cmosOutput, set<int> indexPorts )
{
    // Get normal CMOS object. This will read from regular CMOS.
    //  We do not use the singleton interface here on purpose.
    //  Notice how we put them in an auto_ptr, though.
    cmos::CmosRWFactory *factory = cmos::CmosRWFactory::getFactory();
    auto_ptr<cmos::ICmosRW> cmosIo(factory->makeNew());

    // Get the CMOS object that will be connected to the output file.
    factory->setMode( cmos::CmosRWFactory::UnitTestMode );
    factory->setParameter("cmosMapFile", cmosOutput);
    auto_ptr<cmos::ICmosRW> cmosFile(factory->makeNew());

    for (set<int>::iterator i = indexPorts.begin(); i != indexPorts.end(); ++i )
    {
        int port = (*i);
        for (int j=0; j<0x80; ++j)
        {
            // read from CMOS
            unsigned char c = cmosIo->readByte(port, port+1, j);

            // write it into the file.
            cmosFile->writeByte(port, port+1, j, c);
        }
    }
}

