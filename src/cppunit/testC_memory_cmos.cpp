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

#ifdef DEBUG_TEST_MEMORY_C
#define DEBUG_TEST_OUTPUT
#endif

// compat header should always be first header if including system headers
#include "smbios_c/compat.h"

#include <string.h>
#include <stdio.h>

#include "smbios_c/obj/memory.h"
#include "smbios_c/memory.h"
#include "smbios_c/obj/cmos.h"
#include "smbios_c/cmos.h"

#include "testC_memory_cmos.h"
#include "outputctl.h"
#include "main.h"

using namespace std;

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testCInterface);

#define pagesize (4096 * 16)

void testCInterface::setUp()
{
    string writeDirectory = getWritableDirectory();
    string testFile = writeDirectory + "/testmem.dat";

    DCERR( "open memtest input file." << endl);
    FILE *fd = fopen( testFile.c_str(), "w+" );
    DCERR( " write some junk" << endl);
    for (int i=0; i < pagesize*4 + 1;++i)
        if (fwrite("j", 1, 1, fd) != 1)
            ;
    DCERR( "\tfclose()" << endl);
    fclose(fd);

    DCERR( "init memory singleton" << endl);
    memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, testFile.c_str());
    DCERR( "init cmos singleton" << endl);
    cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, testFile.c_str());

    DCERR( "write alphabet" << endl);
    u8 byte = 'a';
    int offset = 0;
    for (int i=0; i<26; i++)
    {
        byte = 'a' + i;
        memory_write(&byte, offset++, 1);
    }

    DCERR( "write " << pagesize / 1024 << "k blocks of '0', '1', and '2'" << endl);
    for (int i=0; i<3; i++)
    {
        u8 arr[pagesize];
        DCERR( "\tWrite "<< pagesize / 1024 <<  "k '" << i << "'" << endl);
        memset(arr, '0' + i, pagesize);
        memory_write(arr, offset, pagesize);
        offset += pagesize;
    }
    DCERR( "setup done" << endl);
}

void testCInterface::tearDown()
{ }

void testCInterface::testForLeaks()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    for (int i=0;i<1000;++i)
    {
        struct memory_access_obj *m = memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_NEW, "/dev/null");
        struct memory_access_obj *n = memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_NEW, "/dev/null");
        memory_obj_free(m);
        m = 0;
        memory_obj_free(n);
        n = 0;
    }

    for (int i=0;i<1000;++i)
    {
        struct cmos_access_obj *c = cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_NEW, "/dev/null");
        struct cmos_access_obj *d = cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_NEW, "/dev/null");
        cmos_obj_free(c);
        c = 0;
        cmos_obj_free(d);
        d = 0;
    }

    STD_TEST_END("");
}

void testCInterface::testMemoryRead()
{
    DCERR( "START:  " << __PRETTY_FUNCTION__ << endl);
    STD_TEST_START(getTestName().c_str() << "  ");

    u8 buf;
    int ret;
    for (int i=0; i<26; ++i){
        buf = '9';
        ret = memory_read(&buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    STD_TEST_END("");
}

void testCInterface::testMemoryWrite()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    u8 buf;
    int ret;

    for (int i=1; i<26; ++i){
        ret = memory_read(&buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(&buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        buf = buf + 'A' - 'a';
        ret = memory_write(&buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(&buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( (u8)('A' + i), buf );
    }
    STD_TEST_END("");
}

void testCInterface::testMemoryReadMultipage()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    int ret=0;
    u8 buf[pagesize *3 + 1] = {0,};

    // 26 == start after alphabet in test file
    ret = memory_read(buf, 26, pagesize *3);
    CPPUNIT_ASSERT_EQUAL( 0, ret );

    for (int i=0; i<3; i++)
        for (int j=0; j<pagesize; j++)
        {
            if ('0' + i != buf[j+i*pagesize] )
                printf("DEBUG: i(%d) j(%d) buf(%c) '0'+i(%c)", i, j, buf[j+i*pagesize], (u8)('0'+i));
            CPPUNIT_ASSERT_EQUAL( (u8)('0' + i) , buf[j+i*pagesize] );
        }

    STD_TEST_END("");
}



void testCInterface::testMemorySearch()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    s64 ret;

    ret = memory_search("abc", 3, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)0, ret );

    ret = memory_search("de", 2, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)3, ret );

    ret = memory_search("nonexistent", 11, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)-1, ret );

    ret = memory_search("00000000", 8, 0, 4096, 1);
    CPPUNIT_ASSERT_EQUAL( (s64)26, ret );

    STD_TEST_END("");
}


void testCInterface::testCmosRead()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    u8 buf;
    int ret;
    for (int i=0; i<26; ++i){
        buf = '9';
        ret = cmos_read_byte(&buf, 0, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    STD_TEST_END("");
}

int _test_write_callback(const struct cmos_access_obj *c, bool do_update, void *userdata)
{
    //printf("Write detected. do_update: %d  userdata: %d\n", do_update, *(int*)userdata);
    (*(int*)userdata)++;
    return 0;
}

void testCInterface::testCmosWrite()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct cmos_access_obj *c = cmos_obj_factory(CMOS_GET_SINGLETON);
    u8 buf;
    int ret;

    int counter=0;
    cmos_obj_register_write_callback(c, _test_write_callback, &counter, 0);

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(&buf, 0, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(&buf, 0, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        buf = buf + 'A' - 'a';
        ret = cmos_write_byte(buf, 0, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(&buf, 0, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('A' + i) );
    }

    // index port 1 (offset 512 + i) should be '0'
    for (int i=0; i<26; ++i){
        ret = cmos_read_byte(&buf, 1, 0, i);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('0') );
    }

    cmos_obj_free(c);

    CPPUNIT_ASSERT_EQUAL( counter, 26 );

    STD_TEST_END("");
}


