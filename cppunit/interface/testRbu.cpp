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

#include <fstream>
#include <cctype>

#include "testRbu.h"

// specific to unit tests. Users do not need to include this,
// so it is not in testPlatform.h
#include "smbios/IMemory.h"
#include "smbios/ISmi.h"
#include "smbios/ISmbiosXml.h"
#include "smbios/IToken.h"

using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testRbu);

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

void testRbu::setUp()
{
    string xmlFile = getCppunitTopDirectory() + getXmlFile();

    string testInput = getCppunitTopDirectory() + getTestDirectory() + "/testInput.xml";
    if(!fileExists(testInput))
        testInput = getTestDirectory() + "/testInput.xml"; 

    // copy the memdump.dat file. We do not write to it, but rw open will fail
    // if we do not copy it
    string memdumpOrigFile = getCppunitTopDirectory() + getTestDirectory() + "/memdump.dat";
    if(!fileExists(memdumpOrigFile))
        memdumpOrigFile = getTestDirectory() + "/memdump.dat";
    string memdumpCopyFile = getWritableDirectory() + "/memdump-copy.dat";
    copyFile( memdumpCopyFile, memdumpOrigFile );

    // copy the CMOS file. We are going to write to it and do not wan to mess up
    // the pristine unit test version
    string cmosOrigFile = getCppunitTopDirectory() + getTestDirectory() + "/cmos.dat";
    if(!fileExists(cmosOrigFile))
        cmosOrigFile = getTestDirectory() + "/cmos.dat";
    string cmosCopyFile = getWritableDirectory() + "/cmos-copy.dat";
    copyFile( cmosCopyFile, cmosOrigFile );

    // Smi output file.
    string smiOutput = getWritableDirectory() + "/smi-output.dat";

    // set up XML factory. from here on, we can just say SmbiosFactory.
    smbios::SmbiosXmlFactory::getFactory();

    // normal users of the smbios classes need not
    // set the four parameters below. They should all be set inside the factory
    // properly by default. We override stuff here to have
    // the smbios, cmos, etc classes use file dumps instead of
    // real memory/cmos/etc.
    smbios::SmbiosFactory::getFactory()->setParameter("memFile", memdumpCopyFile);
    smbios::SmbiosFactory::getFactory()->setParameter("offset", 0);
    smbios::SmbiosFactory::getFactory()->setMode(smbios::SmbiosFactory::UnitTestMode);

    cmos::  CmosRWFactory::getFactory()->setParameter("cmosMapFile", cmosCopyFile);
    cmos::  CmosRWFactory::getFactory()->setMode( factory::IFactory::UnitTestMode );

    memory::MemoryFactory::getFactory()->setParameter("memFile", memdumpCopyFile);
    memory::MemoryFactory::getFactory()->setMode( memory::MemoryFactory::UnitTestMode );

    smi::SmiFactory::getFactory()->setParameter("smiFile", smiOutput);
    smi::SmiFactory::getFactory()->setMode( smi::SmiFactory::UnitTestMode );

    // The parameter below will normally need to be set by the client code.
    smbios::SmbiosFactory::getFactory()->setParameter("xmlFile", xmlFile);

    doc = 0;
    parser = 0;
    InitXML();
    parser = xmlutils::getParser();
    compatXmlReadFile(parser, doc, testInput.c_str());
}

void testRbu::tearDown()
{
    // the factory is static. If we do not reset the factory, the next
    // unit test may accidentally get the wrong objects.
    // Lifetime rules: CmosTokenTable cannot live longer than the ISmbiosTable
    // object used in its construction.
    smbios::TokenTableFactory::getFactory()->reset();

    smbios::SmbiosFactory::getFactory()->reset();

    memory::MemoryFactory::getFactory()->reset();

    cmos::CmosRWFactory::getFactory()->reset();

    smi::SmiFactory::getFactory()->reset();

    if (parser)
        xmlFreeParser(parser);

    if (doc)
        xmlFreeDoc(doc);

    FiniXML();
}

// testInput.xml tests
string testRbu::getTestInputString( string toFind, string section )
{
    if (!doc)
        throw skip_test();

    string foundString = "";

    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *domSection = xmlutils::findElement( xmlDocGetRootElement(doc), section, "", "" );
        if(!domSection) throw skip_test();
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *domElem = xmlutils::findElement( domSection, toFind, "", "" );
        if(!domElem) throw skip_test();
        foundString = xmlutils::getNodeText( domElem );
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    return foundString;
}


//
//
// TABLE tests
//
//

string stringToLower(string in)
{
    for(unsigned int i=0;i<in.length();i++)
    {
        in[i] = tolower(in[i]);
    }
    return in;
}

void testRbu::testRbuBadData()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    ASSERT_THROWS( rbu::RbuFactory::getFactory()->makeNew("nonexistent_file"), rbu::HdrFileIOError );

    string bad_hdr_filename = getCppunitTopDirectory() + getTestDirectory() + "/bad_hdr.hdr";
    if(!fileExists(bad_hdr_filename))
        bad_hdr_filename = getTestDirectory() + "/bad_hdr.hdr";

    ASSERT_THROWS( rbu::RbuFactory::getFactory()->makeNew(bad_hdr_filename), rbu::InvalidHdrFile );

    STD_TEST_END("");
}

auto_ptr<rbu::IRbuHdr> testRbu::checkHdrInfo(string name)
{
    string hdr_a_name = getCppunitTopDirectory() + getTestDirectory() + "/" + getTestInputString("filename", name);
    if(!fileExists(hdr_a_name))
        hdr_a_name = getTestDirectory() + "/" + getTestInputString("filename", name);

    auto_ptr<rbu::IRbuHdr> hdrA (rbu::RbuFactory::getFactory()->makeNew(hdr_a_name));
    string expectedBiosVer = getTestInputString("biosver", name);
    string actualBiosVer = stringToLower(hdrA->getBiosVersion());
    CPPUNIT_ASSERT_EQUAL ( expectedBiosVer, actualBiosVer );

    unsigned int actualMajor, actualMinor, expectedMajor, expectedMinor;
    hdrA->getHdrVersion(actualMajor, actualMinor);
    expectedMajor = strtoul(getTestInputString("hdrmajorver", name).c_str(), 0, 0);
    expectedMinor = strtoul(getTestInputString("hdrminorver", name).c_str(), 0, 0);
    CPPUNIT_ASSERT_EQUAL (expectedMajor, actualMajor);
    CPPUNIT_ASSERT_EQUAL (expectedMinor, actualMinor);
    CPPUNIT_ASSERT_EQUAL ( true, checkSystemId(*hdrA, strtoul(getTestInputString("sysid", name).c_str(), 0, 0)));

    return hdrA;
}

void testRbu::testRbuBasic()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    auto_ptr<rbu::IRbuHdr> hdr_152_a09 = checkHdrInfo("hdr_152_a09");
    auto_ptr<rbu::IRbuHdr> hdr_152_x09 = checkHdrInfo("hdr_152_x09");
    auto_ptr<rbu::IRbuHdr> hdr_152_p09 = checkHdrInfo("hdr_152_p09");
    auto_ptr<rbu::IRbuHdr> hdr_152_a10 = checkHdrInfo("hdr_152_a10");
    auto_ptr<rbu::IRbuHdr> hdr_1b1_000208 = checkHdrInfo("hdr_1b1_000208");
    auto_ptr<rbu::IRbuHdr> hdr_1b1_000209 = checkHdrInfo("hdr_1bb_000209");
    auto_ptr<rbu::IRbuHdr> hdr_1b1_990208 = checkHdrInfo("hdr_1bb_990209");

    STD_TEST_END("");
}


void testRbu::testRbuOldVerCompare()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    auto_ptr<rbu::IRbuHdr> hdr_152_a09 = checkHdrInfo("hdr_152_a09");
    auto_ptr<rbu::IRbuHdr> hdr_152_x09 = checkHdrInfo("hdr_152_x09");
    auto_ptr<rbu::IRbuHdr> hdr_152_p09 = checkHdrInfo("hdr_152_p09");
    auto_ptr<rbu::IRbuHdr> hdr_152_a10 = checkHdrInfo("hdr_152_a10");

    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_152_a09->getBiosVersion(), hdr_152_a09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_152_x09->getBiosVersion(), hdr_152_x09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_152_p09->getBiosVersion(), hdr_152_p09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_152_a10->getBiosVersion(), hdr_152_a10->getBiosVersion()));


    //CPPUNIT_ASSERT_EQUAL( EXPECTED, ACTUAL );
    CPPUNIT_ASSERT_EQUAL( 1, rbu::compareBiosVersion(hdr_152_a09->getBiosVersion(), hdr_152_a10->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_152_a10->getBiosVersion(), hdr_152_a09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_152_a10->getBiosVersion(), hdr_152_x09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_152_x09->getBiosVersion(), hdr_152_p09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_152_a09->getBiosVersion(), hdr_152_a10->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_152_x09->getBiosVersion(), hdr_152_a09->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_152_p09->getBiosVersion(), hdr_152_x09->getBiosVersion()));

    // synthetic comparisons
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("P01", "Q00"));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion("Q01", "P00"));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("U00", "T01"));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion("Y01", "Z00"));

    // mixed vers
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion("A01", "0.2.8"));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("3.2.1", "A01"));

    STD_TEST_END("");
}


void testRbu::testRbuNewVerCompare()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    auto_ptr<rbu::IRbuHdr> hdr_1b1_000208 = checkHdrInfo("hdr_1b1_000208");
    auto_ptr<rbu::IRbuHdr> hdr_1b1_000209 = checkHdrInfo("hdr_1bb_000209");
    auto_ptr<rbu::IRbuHdr> hdr_1b1_990209 = checkHdrInfo("hdr_1bb_990209");

    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_1b1_000208->getBiosVersion(), hdr_1b1_000208->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_1b1_000209->getBiosVersion(), hdr_1b1_000209->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( 0, rbu::compareBiosVersion(hdr_1b1_990209->getBiosVersion(), hdr_1b1_990209->getBiosVersion()));

    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_1b1_000209->getBiosVersion(), hdr_1b1_000208->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_1b1_000208->getBiosVersion(), hdr_1b1_000209->getBiosVersion()));

    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_1b1_990209->getBiosVersion(), hdr_1b1_000208->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion(hdr_1b1_990209->getBiosVersion(), hdr_1b1_000209->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_1b1_000208->getBiosVersion(), hdr_1b1_990209->getBiosVersion()));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion(hdr_1b1_000209->getBiosVersion(), hdr_1b1_990209->getBiosVersion()));

    // synthetic comparisons
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("0.2.8", "99.2.4"));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("1.2.8", "0.2.4"));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion("1.2.8", "2.2.4"));
    CPPUNIT_ASSERT_EQUAL(  1, rbu::compareBiosVersion("1.2.8", "1.3.4"));
    CPPUNIT_ASSERT_EQUAL( -1, rbu::compareBiosVersion("1.4.8", "1.3.4"));

    STD_TEST_END("");
}

// not part of public API, so declare here
namespace rbu {
extern void splitNewVersion(std::string ver, unsigned int &maj, unsigned int &min, unsigned int &ext);
}

void testRbu::testRbuNewVerSplit()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    unsigned int maj, min, ext, expmaj, expmin, expext;
    string ver;

    // good version
    ver = "0.2.9";
    expmaj = 0;
    expmin = 2;
    expext = 9;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // high version
    ver = "99.2.9";
    expmaj = 99;
    expmin = 2;
    expext = 9;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // max legal len
    ver = "88.88.88";
    expmaj = 88;
    expmin = 88;
    expext = 88;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: trailing period
    ver = "100.100.100.";
    expmaj = 100;
    expmin = 100;
    expext = 100;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: missing ext
    ver = "100.100.";
    expmaj = 100;
    expmin = 100;
    expext = 0;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: missing .ext
    ver = "0.2";
    expmaj = 0;
    expmin = 2;
    expext = 0;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: missing min.ext
    ver = "100.";
    expmaj = 100;
    expmin = 0;
    expext = 0;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: missing .min.ext
    ver = "100";
    expmaj = 100;
    expmin = 0;
    expext = 0;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    // bad: trailing junk
    ver = "100.100.100Junk";
    expmaj = 100;
    expmin = 100;
    expext = 100;
    rbu::splitNewVersion(ver, maj, min, ext);
    CPPUNIT_ASSERT_EQUAL( expmaj, maj );
    CPPUNIT_ASSERT_EQUAL( expmin, min );
    CPPUNIT_ASSERT_EQUAL( expext, ext );

    STD_TEST_END("");
}


void testRbu::testRbuOutput()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    std::ostringstream out;
    auto_ptr<rbu::IRbuHdr> hdr_152_a09 = checkHdrInfo("hdr_152_a09");
    out << *hdr_152_a09 << endl;

    STD_TEST_END("");
}

