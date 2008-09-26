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
#include "smbios_c/compat.h"

#include <iomanip>
#include <fstream>
#include <iostream>

#include "testC_smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/memory.h"
#include "smbios_c/cmos.h"
#include "smbios_c/version.h"

#include "outputctl.h"

using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testCsmbios);

void copyFile( string dstFile, string srcFile )
{
    ifstream src(srcFile.c_str(), ios_base::binary);
    ofstream dst(dstFile.c_str(), ios_base::out | ios_base::binary | ios_base::trunc);

    char ch;
    while( src.get(ch)) dst.put(ch);

    if( !src.eof() || !dst ) throw exception();
}

bool fileExists(string fileName)
{
    FILE *fh=0;
    fh=fopen(fileName.c_str(), "rb");
    if(!fh)
        return false;

    fclose(fh);
    return true;
}

void testCsmbios::setUp()
{
    string writeDirectory = getWritableDirectory();

    // copy the memdump.dat file. We do not write to it, but rw open will fail
    // if we do not copy it
    string memdumpOrigFile = getTestDirectory() + "/memdump.dat";
    string memdumpCopyFile = writeDirectory + "/memdump-copy.dat";
    copyFile( memdumpCopyFile, memdumpOrigFile );

    // copy the CMOS file. We are going to write to it and do not wan to mess up
    // the pristine unit test version
    string cmosOrigFile = getTestDirectory() + "/cmos.dat";
    string cmosCopyFile = writeDirectory + "/cmos-copy.dat";
    copyFile( cmosCopyFile, cmosOrigFile );

    memory_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, memdumpCopyFile.c_str());
    cmos_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, cmosCopyFile.c_str());
}

void testCsmbios::tearDown()
{ 
}

void testCsmbios::testSmbiosConstruct()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    u8 buf;
    struct memory *m = 0;
    int ret;
    for (int i=0; i<26; ++i){
        m = memory_factory(MEMORY_GET_SINGLETON);
        buf = '9';
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
        memory_free(m);
    }

    STD_TEST_END("");
}




