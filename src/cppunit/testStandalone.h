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


#ifndef _TESTSTANDALONE_H
#define _TESTSTANDALONE_H

#include "smbios/compat.h"

#include <cppunit/extensions/HelperMacros.h>
#include <typeinfo>
#include <string>

#include "smbios/ISmbios.h"
#include "smbios/ICmosRW.h"
#include "smbios/IToken.h"
#include "smbios/SystemInfo.h"

#include "XmlUtils.h"

extern int global_argc;
extern char ** global_argv;

class testStandalone  : public CppUnit::TestFixture
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
        return "standalone";
    }
    virtual std::string getTestDirectory()
    {
        //return DEFAULT_TEST_DIR;
        return global_argv[1];
    };

public:
    virtual void setUp();
    virtual void tearDown();

    // base smbios test
    void testSmbiosTableBase();
    void testSmbiosTableBase_iterNextItem();
    void testSmbiosTableBase_findItemByType();
    void testSmbiosTableBase_findItemByHandle();

    // table tests
    void testTable_Subscript();
    void testEntryCount ();
    void testConstIterator ();
    void testSubscriptOperator1 ();
    void testSubscriptOperator2 ();
    void testSubscriptOperator3 ();
    void testSmbiosXml();

    // item tests
    void testStreamify();
    void testEachItemAccessors();
    void testItemIdentity();
    void testGetBoundaries();

    // memory tests
    void testMemoryBadFiles();
    void testMemoryFuncs();

    // cmos token tests
    void testCmosConstructor();

    // smi tests
    void testSmi_callingInterface();
    void testSmi_callingInterface_physaddr();

    // testInput.xml tests
    void testServiceTagWriting();
    void testLibraryVersion();

    // other
    void testStateBytes();

    // Exception Tests
    void testException();

    // Exception Tests
    void testNonXml();


    // make sure to put this at the end...
    CPPUNIT_TEST_SUITE (testStandalone);

    CPPUNIT_TEST (testSmbiosTableBase);
    CPPUNIT_TEST (testSmbiosTableBase_iterNextItem);
    CPPUNIT_TEST (testSmbiosTableBase_findItemByType);
    CPPUNIT_TEST (testSmbiosTableBase_findItemByHandle);

    CPPUNIT_TEST (testTable_Subscript);
    CPPUNIT_TEST (testEntryCount);
    CPPUNIT_TEST (testConstIterator);
    CPPUNIT_TEST (testSubscriptOperator1);
    CPPUNIT_TEST (testSubscriptOperator2);
    CPPUNIT_TEST (testSubscriptOperator3);
    CPPUNIT_TEST (testSmbiosXml);

    CPPUNIT_TEST (testStreamify);
    CPPUNIT_TEST (testItemIdentity);
    CPPUNIT_TEST (testEachItemAccessors);
    CPPUNIT_TEST (testGetBoundaries);


    CPPUNIT_TEST (testMemoryBadFiles);
    CPPUNIT_TEST (testMemoryFuncs);

    CPPUNIT_TEST (testCmosConstructor);

    CPPUNIT_TEST (testSmi_callingInterface);
    CPPUNIT_TEST (testSmi_callingInterface_physaddr);

    CPPUNIT_TEST (testLibraryVersion);
    CPPUNIT_TEST (testException);

    CPPUNIT_TEST (testNonXml);

    CPPUNIT_TEST_SUITE_END ();
};

#endif
