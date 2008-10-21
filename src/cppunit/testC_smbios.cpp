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

#include "testC_smbios.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/obj/cmos.h"
#include "smbios_c/system_info.h"

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
CPPUNIT_TEST_SUITE_REGISTRATION (testCsmbios);

void testCsmbios::setUp()
{
    setupForUnitTesting(getTestDirectory(), getWritableDirectory());

    doc = 0;
    parser = 0;
    InitXML();
    parser = xmlutils::getParser();
    string testInput = getTestDirectory() + "/testInput.xml";
    compatXmlReadFile(parser, doc, testInput.c_str());
}

void testCsmbios::tearDown()
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
testCsmbios::checkSkipTest(string testName)
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
string testCsmbios::getTestInputString( string toFind, string section )
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

void test_walk_fn(const struct smbios_struct *s, void *userdata)
{
    (*(u32 *)userdata)++;
}

void testCsmbios::testSmbiosConstruct()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    u32 structure_count = 0;
    smbios_for_each_struct(s) {
        u32 data=0;
        smbios_struct_get_data(s, &data, 0, sizeof(u8));
        CPPUNIT_ASSERT_EQUAL( (u32)smbios_struct_get_type(s), data );

        data = 0;
        smbios_struct_get_data(s, &data, 1, sizeof(u8));
        CPPUNIT_ASSERT_EQUAL( (u32)smbios_struct_get_length(s), data );

        data = 0;
        smbios_struct_get_data(s, &data, 2, sizeof(u16));
        CPPUNIT_ASSERT_EQUAL( (u32)smbios_struct_get_handle(s), data );

        structure_count ++;
    }

    u32 alt_count = 0;
    smbios_walk(test_walk_fn, &alt_count);

    CPPUNIT_ASSERT_EQUAL(alt_count, structure_count);

    STD_TEST_END("");
}

void testCsmbios::testVariousAccessors()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    const struct smbios_struct *s = smbios_get_next_struct_by_type(0, 0x00); // 0x00 == BIOS structure

    string biosVendorStr="";
    string versionStr="";
    string releaseStr="";

    if (!doc)
        throw skip_test();

    // pull info out of xml
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *smbios = xmlutils::findElement( xmlDocGetRootElement(doc), "smbios", "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *biosInfo = xmlutils::findElement( smbios, "biosInformation", "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *vendor = xmlutils::findElement( biosInfo, "vendor", "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *version = xmlutils::findElement( biosInfo, "version", "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *release = xmlutils::findElement( biosInfo, "release", "", "" );
        biosVendorStr = xmlutils::getNodeText( vendor );
        versionStr = xmlutils::getNodeText( version );
        releaseStr = xmlutils::getNodeText( release );
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    string biosVendorStrSmbios( smbios_struct_get_string_from_offset(s, 4) ); // BIOS VENDOR
    string versionStrSmbios( smbios_struct_get_string_from_offset(s, 5) ); // BIOS VERSION
    string releaseStrSmbios( smbios_struct_get_string_from_offset(s, 8) ); // RELEASE DATE

    strip_trailing_whitespace(biosVendorStrSmbios);
    strip_trailing_whitespace(versionStrSmbios);
    strip_trailing_whitespace(releaseStrSmbios);

    string biosVendorStrSmbios2( smbios_struct_get_string_number(s, 1) ); //BIOS VENDOR
    string versionStrSmbios2( smbios_struct_get_string_number(s, 2) ); //BIOS VERSION
    string releaseStrSmbios2( smbios_struct_get_string_number(s, 3) ); //RELEASE DATE

    strip_trailing_whitespace(biosVendorStrSmbios2);
    strip_trailing_whitespace(versionStrSmbios2);
    strip_trailing_whitespace(releaseStrSmbios2);

    char *versionStrLib_raw = sysinfo_get_bios_version();
    const string versionStrLib(versionStrLib_raw);
    sysinfo_string_free(versionStrLib_raw);

    //cout << "versionStr: '" << versionStr << "'" << endl;
    //cout << "versionStrLib: '" << versionStrLib << "'" << endl;
    //cout << "versionStrSmbios: '" << versionStrSmbios << "'" << endl;
    //cout << "versionStrSmbios2: '" << versionStrSmbios2 << "'" << endl;

    CPPUNIT_ASSERT_EQUAL( versionStr, versionStrLib );
    CPPUNIT_ASSERT_EQUAL( versionStr, versionStrSmbios );
    CPPUNIT_ASSERT_EQUAL( versionStrSmbios, versionStrSmbios2 );

    CPPUNIT_ASSERT_EQUAL( biosVendorStr, biosVendorStrSmbios );
    CPPUNIT_ASSERT_EQUAL( biosVendorStrSmbios, biosVendorStrSmbios2 );

    CPPUNIT_ASSERT_EQUAL( releaseStr, releaseStrSmbios );
    CPPUNIT_ASSERT_EQUAL( releaseStrSmbios, releaseStrSmbios2 );

    STD_TEST_END("");
}


void
testCsmbios::testIdByte()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    int   systemId   = sysinfo_get_dell_system_id  ();

    string idStr = getTestInputString("idByte");
    int id  = strtol( idStr.c_str(), 0, 0);

    CPPUNIT_ASSERT_EQUAL ( id, systemId );

    STD_TEST_END("");
}

void
testCsmbios::testServiceTag()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");
    string serviceTagStr = "";

    char *serviceTag   = sysinfo_get_service_tag();
    if (serviceTag)
        serviceTagStr = serviceTag;
    sysinfo_string_free(serviceTag);

    string expectedTag = getTestInputString("serviceTag");

    CPPUNIT_ASSERT_EQUAL ( expectedTag, serviceTagStr );

    STD_TEST_END("");
}

void testCsmbios::testForLeaks()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    for (int i=0;i<10;++i)
    {
        struct smbios_table *s = smbios_table_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_NEW);
        struct smbios_table *t = smbios_table_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_NEW);
        smbios_table_free(s);
        s = 0;
        smbios_table_free(t);
        t = 0;
    }

    STD_TEST_END("");
}

