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


#ifndef _TESTCTOKEN_H
#define _TESTCTOKEN_H

#include "smbios_c/compat.h"

#include <cppunit/extensions/HelperMacros.h>
#include <typeinfo>
#include <string>

#include "XmlUtils.h"

extern int global_argc;
extern char ** global_argv;

class testCtoken  : public CppUnit::TestFixture
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
        return global_argv[2];
    }

    virtual std::string getTestDirectory()
    {
        //return DEFAULT_TEST_DIR;
        return global_argv[1];
    };

    std::string getTestInputString( std::string toFind, std::string section="systemInfo" );

    void checkSkipTest( std::string testName);

    // parser owns all XML entities. When it is deleted, everything
    // goes with it.
    XML_NAMESPACE DOMBuilder *parser;

    // The doc is owned by the parser. We do not have to clean it up
    // it is deleted when the parser is released. We keep a ref
    // here for speed purposes
    XML_NAMESPACE DOMDocument *doc;


public:
    virtual void setUp();
    virtual void tearDown();

    // base smbios test
    void testTokenConstruct();
    void testTokenChecksums();

    // make sure to put this at the end...
    CPPUNIT_TEST_SUITE (testCtoken);

    CPPUNIT_TEST (testTokenConstruct);
    CPPUNIT_TEST (testTokenChecksums);

    CPPUNIT_TEST_SUITE_END ();
};

#endif
