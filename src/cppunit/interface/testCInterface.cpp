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

#include "testCInterface.h"
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
CPPUNIT_TEST_SUITE_REGISTRATION (testCInterface);

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

static size_t FWRITE(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream); 
    // TODO: handle short write
    if (written < (size * nmemb))
        throw std::exception();
    return written;
}

void testCInterface::setUp()
{
    string writeDirectory = getWritableDirectory();
    string testFile = writeDirectory + "/testmem.dat";
    string testFile2 = writeDirectory + "/testmem2.dat";

    FILE *fd = fopen(testFile.c_str(), "w+");
    for (int i=0; i<26; i++)
    {
        char w = 'a' + i;
        FWRITE(&w, 1, 1, fd);  // void *ptr, size, nmemb, FILE *
    }

    for (int i=0; i<3; i++)
        for (int j=0; j<65536; j++)
            {
                char w = '0' + i;
                FWRITE(&w, 1, 1, fd);  // void *ptr, size, nmemb, FILE *
            }

    fflush(fd);
    fclose(fd);

    memory_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, testFile.c_str());
    cmos_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, testFile.c_str());
}

void testCInterface::tearDown()
{ }

void testCInterface::testMemoryRead()
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


void testCInterface::testMemoryWrite()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct memory *m = memory_factory(MEMORY_GET_SINGLETON);
    u8 buf;
    int ret;

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        buf = buf + 'A' - 'a';
        ret = memory_write(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('A' + i) );
    }

    memory_free(m);

    STD_TEST_END("");
}

void testCInterface::testMemoryReadMultipage()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct memory *m = memory_factory(MEMORY_GET_SINGLETON);
    int ret=0;
    u8 buf[65536*3 + 1] = {0,};

    // 26 == start after alphabet in test file
    ret = memory_read(m, buf, 26, 65536*3);
    CPPUNIT_ASSERT_EQUAL( 0, ret );
    memory_free(m);

    for (int i=0; i<3; i++)
        for (int j=0; j<65536; j++)
            CPPUNIT_ASSERT_EQUAL( (u8)('0' + i) , buf[j+i*65536] );

    STD_TEST_END("");
}



void testCInterface::testMemorySearch()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct memory *m = memory_factory(MEMORY_GET_SINGLETON);
    s64 ret;

    ret = memory_search(m, "abc", 3, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)0, ret );

    ret = memory_search(m, "de", 2, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)3, ret );

    ret = memory_search(m, "nonexistent", 11, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)-1, ret );

    ret = memory_search(m, "00000000", 8, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)26, ret );

    STD_TEST_END("");
}


void testCInterface::testCmosRead()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    u8 buf;
    struct cmos *c = 0;
    int ret;
    for (int i=0; i<26; ++i){
        c = cmos_factory(CMOS_GET_SINGLETON);
        buf = '9';
        ret = cmos_read_byte(c, 0, 0, i, &buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
        cmos_free(c);
    }

    STD_TEST_END("");
}


void testCInterface::testCmosWrite()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct cmos *c = cmos_factory(CMOS_GET_SINGLETON);
    u8 buf;
    int ret;

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(c, 0, 0, i, &buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(c, 0, 0, i, &buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        buf = buf + 'A' - 'a';
        ret = cmos_write_byte(c, 0, 0, i, buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(c, 0, 0, i, &buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('A' + i) );
    }

    // index port 1 (offset 512 + i) should be '0'
    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(c, 1, 0, i, &buf);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('0') );
    }

    cmos_free(c);

    STD_TEST_END("");
}


