// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
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


#ifndef _TESTCINTERFACE_H
#define _TESTCINTERFACE_H

#include "smbios_c/compat.h"

#include <cppunit/extensions/HelperMacros.h>
#include <typeinfo>
#include <string>

extern int global_argc;
extern char ** global_argv;

class testCInterface  : public CppUnit::TestFixture
{
protected:
    virtual std::string getWritableDirectory()
    {
        //return DEFAULT_TEST_DIR;
        return global_argv[1];
    };


    virtual std::string getTestName()
    {
        //return TEST_DIR;
        return "testC_memory_cmos";
    }

public:
    virtual void setUp();
    virtual void tearDown();

    // base smbios test
    void testMemoryRead();
    void testMemoryWrite();
    void testMemoryReadMultipage();
    void testMemorySearch();
    void testCmosRead();
    void testCmosWrite();
    void testForLeaks();


    // make sure to put this at the end...
    CPPUNIT_TEST_SUITE (testCInterface);

    CPPUNIT_TEST (testMemoryRead);
    CPPUNIT_TEST (testMemoryReadMultipage);
    CPPUNIT_TEST (testMemorySearch);
    CPPUNIT_TEST (testCmosRead);
    CPPUNIT_TEST (testCmosWrite);
    CPPUNIT_TEST (testMemoryWrite);
    CPPUNIT_TEST (testForLeaks);

    CPPUNIT_TEST_SUITE_END ();
};

#endif
