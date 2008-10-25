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

// system

#include "smbios_c/obj/token.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/obj/cmos.h"
#include "smbios_c/cmos.h"

#include "testC_token.h"
#include "outputctl.h"
#include "main.h"
#include "XmlUtils.h"

using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testCtoken);

void testCtoken::setUp()
{
    setupForUnitTesting(getTestDirectory(), getWritableDirectory());

    doc = 0;
    parser = 0;
    InitXML();
    parser = xmlutils::getParser();
    string testInput = getTestDirectory() + "/testInput.xml";
    compatXmlReadFile(parser, doc, testInput.c_str());
}

void testCtoken::tearDown()
{
    reset();
    if (parser)
        xmlFreeParser(parser);

    if (doc)
        xmlFreeDoc(doc);

    FiniXML();
}


// checkSkipTest for Skipping known BIOS Bugs.
void
testCtoken::checkSkipTest(string testName)
{
    if(!doc)
        return;

    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *testsToSkip = xmlutils::findElement(xmlDocGetRootElement(doc),"testsToSkip","","");
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *test = xmlutils::findElement(testsToSkip,"test","name",testName);

        if(test)
            throw skip_test();
    }
    catch (const skip_test &)
    {
        throw;
    }
    catch (const exception &)
    {
        //Do Nothing
    }
}


// testInput.xml tests
string testCtoken::getTestInputString( string toFind, string section )
{
    if (!doc)
        throw skip_test();

    string foundString = "";

    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *systeminfo = xmlutils::findElement( xmlDocGetRootElement(doc), section, "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *sysName = xmlutils::findElement( systeminfo, toFind, "", "" );
        foundString = xmlutils::getNodeText( sysName );
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    return foundString;
}

void testCtoken::testTokenConstruct()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    struct token_table *table = token_table_factory(TOKEN_GET_SINGLETON);

    token_table_for_each(table, token){
        token_obj_get_type(token);
        token_obj_get_id(token);
    }

    token_table_free(table);

    STD_TEST_END("");
}


void testCtoken::testTokenChecksums()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    struct token_table *table = token_table_factory(TOKEN_GET_SINGLETON);

    int ret = cmos_run_callbacks(false);

    token_table_free(table);

    CPPUNIT_ASSERT_EQUAL(ret, 0);

    STD_TEST_END("");
}





