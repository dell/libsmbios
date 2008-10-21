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

// system
#include <string.h>

#include "testPlatform.h"
#include "smbios/SmbiosDefs.h"

// specific to unit tests. Users do not need to include this,
// so it is not in testPlatform.h
#include "smbios/IMemory.h"
#include "smbios/ISmi.h"
#include "smbios/IObserver.h"

#include "outputctl.h"
#include "main.h"


using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testPlatform);

void testPlatform::setUp()
{
    setupForUnitTesting(getTestDirectory(), getWritableDirectory());

    doc = 0;
    parser = 0;
    InitXML();
    parser = xmlutils::getParser();
    string testInput = getTestDirectory() + "/testInput.xml"; 
    compatXmlReadFile(parser, doc, testInput.c_str());
}

void testPlatform::tearDown()
{
    // the factory is static. If we do not reset the factory, the next
    // unit test may accidentally get the wrong objects.
    // Lifetime rules: CmosTokenTable cannot live longer than the ISmbiosTable
    // object used in its construction.
    reset();

    if (parser)
        xmlFreeParser(parser);

    if (doc)
        xmlFreeDoc(doc);

    FiniXML();
}

// checkSkipTest for Skipping known BIOS Bugs.
void 
testPlatform::checkSkipTest(string testName)
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

//
// CMOS Token
//

void
testPlatform::testCmosChecksum ()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    smbios::TokenTableFactory *ttFactory;
    ttFactory = smbios::TokenTableFactory::getFactory() ;
    const smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

    smbios::ITokenTable::const_iterator token = tokenTable->begin();
    while( token != tokenTable->end() )
    {
        (void) *token;
        ++token;
    }

    cmos::ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();
    observer::IObservable *ob = dynamic_cast<observer::IObservable *>(cmos);
    bool doUpdate = false;
    if( ob )
        ob->notify(&doUpdate);

    STD_TEST_END("");
}

void
testPlatform::testCmosWriting ()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    smbios::TokenTableFactory *ttFactory;
    ttFactory = smbios::TokenTableFactory::getFactory() ;
    smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
    const smbios::ITokenTable *tokenTableC = ttFactory->getSingleton();

    ASSERT_THROWS( (*tokenTable) ["la la la"], smbios::NotImplemented );
    ASSERT_THROWS( (*tokenTableC)["la la la"], smbios::NotImplemented );

    // test [] on const table.
    (void) tokenTableC[0xFE];

    ostringstream ost;
    ost << *tokenTable << endl;
    ost << *tokenTableC << endl;

    // test const iterator
    smbios::ITokenTable::const_iterator tokenC = tokenTableC->begin();
    while( tokenC != tokenTableC->end() )
    {
        (void) *tokenC;
        (void) tokenC->isString();

        tokenC++;
    }

    // refuse to write to cmos unless the checksum is correct
    cmos::ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();
    observer::IObservable *ob = dynamic_cast<observer::IObservable *>(cmos);
    bool doUpdate = false;
    if( ob )
        ob->notify(&doUpdate);

    smbios::ITokenTable::iterator token = tokenTable->begin();
    while( token != tokenTable->end() )
    {
        //cout << *token << endl;
        if( token->isString() )
        {
            const char *testStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnop";
            const u8 *testStrU8 = reinterpret_cast<const u8*>(testStr);
            u8 *myStr=0;
            u8 *myStr1=0;
            try
            {
                // not really a valid test anymore since SMI tokens can be
                // accessed as bit or string.
                //ASSERT_THROWS( token->activate(), smbios::InvalidAccessMode );
                //ASSERT_THROWS( token->isActive(), smbios::InvalidAccessMode );

                unsigned int size = token->getStringLength() + 1;

                myStr1 = new u8[ size ];
                memset( myStr1, 0, size );

                try
                {
                    token->getString( myStr1, size );
                }
                catch(const smi::UnhandledSmi &)
                {   /* if token is a smi token, cannot unit test. */
                    delete [] myStr1;
                    goto next_token;
                }

                token->setString( testStrU8, strlen(testStr) + 1 );

                CPPUNIT_ASSERT( size <= strlen(testStr)+1 );

                myStr = new u8[ size ];
                memset( myStr, 0, size );
                token->getString( myStr, size );

                // return might be smaller, only compare up to what was stored.
                if( 0 != memcmp( testStr, reinterpret_cast<char*>(myStr), size - 1 ) )
                {
                    // FAILED
                    ostringstream ost;
                    ost << "String set on token failed." << endl;
                    ost << (*token) << endl;
                    ost << "Size of string to compare is: " << size-1 << endl;
                    ost << "Original data: (" << myStr1  << ")" << endl;
                    ost << "Wrote        : (" << testStr << ")" << endl;
                    ost << "Read back    : (" << myStr   << ")" << endl;
                    CPPUNIT_FAIL( ost.str().c_str() );
                }
            }
            catch(...)
            {
                delete [] myStr1;
                delete [] myStr;
                myStr1 = 0;
                myStr = 0;
                throw;
            }
            delete [] myStr1;
            delete [] myStr;
            myStr1 = 0;
            myStr = 0;
        }
        else
        {
            try
            {
                token->activate();
            }
            catch(const smi::UnhandledSmi &)
            {   /* if token is a smi token, cannot unit test. */
                goto next_token;
            }
            if( ! token->isActive() )
            {
                ostringstream ost;
                ost << "Failed to SET bit token. Token data: " << endl;
                ost << (*token);
                CPPUNIT_FAIL( ost.str().c_str() );
            }
            // not really a valid test anymore since SMI tokens can be
            // accessed as bit or string.
            //ASSERT_THROWS( token->setString(0, 0), smbios::InvalidAccessMode );
            //ASSERT_THROWS( token->getStringLength(), smbios::InvalidAccessMode );
        }

next_token:
        // test post-increment behaves properly
        smbios::ITokenTable::iterator before = token;
        smbios::ITokenTable::iterator after = token++;
        CPPUNIT_ASSERT( before == after );
    }

    // recheck the checksums.
    // ensure that we wrote correct checksums out
    if( ob )
        ob->notify(&doUpdate);

    STD_TEST_END("");
}



//
// System Info
//


void
testPlatform::testSystemInfo()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    int   systemId   = 0;
    const char *systemName = 0;
    const char *serviceTag = 0;
    const char *assetTag   = 0;
    const char *biosVersion   = 0;
    const char *vendorName    = 0;

    try
    {
        systemId   = SMBIOSGetDellSystemId();
        systemName = SMBIOSGetSystemName();
        serviceTag = SMBIOSGetServiceTag();
        assetTag   = SMBIOSGetAssetTag();
        biosVersion   = SMBIOSGetBiosVersion();
        vendorName    = SMBIOSGetVendorName();

        int   isDell        = SMBIOSIsDellSystem();

        (void) systemId; //avoid unused var warning
        (void) isDell; //avoid unused var warning

        //We should at least get an empty string from these
        //methods.  Never a null string.
        CPPUNIT_ASSERT(systemId != 0);
        CPPUNIT_ASSERT(systemName != 0);
        //CPPUNIT_ASSERT(serviceTag != 0); // svc tag can legitimately be 0
        //CPPUNIT_ASSERT(assetTag != 0); //This fails on latitude so we comment out for now.
        CPPUNIT_ASSERT(biosVersion != 0);
        CPPUNIT_ASSERT(vendorName != 0);
    }
    catch(...)
    {
        SMBIOSFreeMemory( systemName );
        SMBIOSFreeMemory( serviceTag );
        SMBIOSFreeMemory( assetTag );
        SMBIOSFreeMemory( biosVersion );
        SMBIOSFreeMemory( vendorName );

        throw;
    }

    SMBIOSFreeMemory( systemName );
    SMBIOSFreeMemory( serviceTag );
    SMBIOSFreeMemory( assetTag );
    SMBIOSFreeMemory( biosVersion );
    SMBIOSFreeMemory( vendorName );

    STD_TEST_END("");
}


// testInput.xml tests
string testPlatform::getTestInputString( string toFind, string section )
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


void
testPlatform::testIdByte()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    int   systemId   = SMBIOSGetDellSystemId  ();

    string idStr = getTestInputString("idByte");
    int id  = strtol( idStr.c_str(), 0, 0);

    CPPUNIT_ASSERT_EQUAL ( id, systemId );

    STD_TEST_END("");
}

string safeConvertToString( const char *str )
{
    string fromSystem = "";
    if( 0 != str )
    {
        fromSystem = str;
    }
    SMBIOSFreeMemory(str);
    return fromSystem;
}

void
testPlatform::testSystemName()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    string fromSystem = safeConvertToString( SMBIOSGetSystemName() );
    string testInput  = getTestInputString( "systemName" );

    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );
    STD_TEST_END("");
}

void
testPlatform::testServiceTag()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    string fromSystem = safeConvertToString( SMBIOSGetServiceTag() );
    string testInput  = getTestInputString( "serviceTag" );

    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );

    STD_TEST_END("");
}

//  not part of public API, so just wing it here so that we can do the unit test.
extern char *getServiceTagFromCMOSToken();

void
testPlatform::testServiceTagWriting()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    string fromSystem = safeConvertToString( SMBIOSGetServiceTag() );
    string testInput  = getTestInputString( "serviceTag" );

    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );

    string rawCMOSOrig("");
    try
    {
        rawCMOSOrig = safeConvertToString( getServiceTagFromCMOSToken() );
    }
    catch(const exception &)
    {
        // if service tag is not in SMBIOS, we cannot do the
        // tests below
        throw skip_test();
    }

    CPPUNIT_ASSERT_EQUAL ( testInput, rawCMOSOrig );

    string tagToSet, shouldBe, rawCMOSNew;

    // test std 5-char svc tag
    tagToSet = shouldBe = "NEWTG";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // test std 7-char svc tag (alphabet 1/3)
    tagToSet = shouldBe = "BCDFGHJ";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // test std 7-char svc tag (alphabet 2/3)
    tagToSet = shouldBe = "KLMNPQR";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // test std 7-char svc tag (alphabet 3/3)
    tagToSet = shouldBe = "STVWXYZ";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (1)
    tagToSet = shouldBe = "A";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (2)
    tagToSet = shouldBe = "AB";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (3)
    tagToSet = shouldBe = "ABC";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (4)
    tagToSet = shouldBe = "ABCD";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (6)
    tagToSet = "12DFGH";
    shouldBe = "12DFGH0";  // invalid/missing chars for 7-char svc tag 
                    // converted to '0'
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (7)
    tagToSet = shouldBe = "XGYZYYY";
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // odd size (8)
    tagToSet = "MNPQMNPQ";
    shouldBe = "MNPQMNP"; // extra chars ignored
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );

    // invalid chars in 7-char tag
    tagToSet = "ABEIOUD";
    shouldBe = "AB0000D"; // invalid chars turned into '0';
    SMBIOSSetServiceTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));
    rawCMOSNew = safeConvertToString( getServiceTagFromCMOSToken() );
    CPPUNIT_ASSERT_EQUAL ( shouldBe, rawCMOSNew );


    STD_TEST_END("");
}

// not part of public API
extern char *getAssetTagFromToken();

void
testPlatform::testAssetTag()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    string fromSystem = safeConvertToString( SMBIOSGetAssetTag() );
    string testInput  = getTestInputString( "assetTag" );
    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );

    string tagToSet = "1234567890"; 
    SMBIOSSetAssetTag("", tagToSet.c_str(), strlen(tagToSet.c_str()));

    try
    {
        // only do this part of the test if system has CMOS tag
        // This will throw exception if not present
        fromSystem = safeConvertToString( getAssetTagFromToken() );
        CPPUNIT_ASSERT_EQUAL ( tagToSet, fromSystem );
    }
    catch (const exception &)
    {
    }

    STD_TEST_END("");
}

void
testPlatform::testBiosVersion()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    string fromSystem = safeConvertToString( SMBIOSGetBiosVersion() );
    string testInput  = getTestInputString( "biosVersion" );

    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );

    STD_TEST_END("");
}

void
testPlatform::testIsDell()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    int   isDell   = SMBIOSIsDellSystem  ();

    string strval = getTestInputString( "isDellSystem" );
    int isDellExpected = strtol( strval.c_str(), 0, 0);

    CPPUNIT_ASSERT_EQUAL ( isDell, isDellExpected );

    STD_TEST_END("");
}

void testPlatform::testVariousAccessors()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    // table should not be deleted when we are finished. It is managed by the
    // factory. Factory will delete it for us when ->reset() is called.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::const_iterator item = (*table)[smbios::BIOS_Information] ;

    string vendorStr="";
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
        vendorStr = xmlutils::getNodeText( vendor );
        versionStr = xmlutils::getNodeText( version );
        releaseStr = xmlutils::getNodeText( release );

    }
    catch( const exception & )
    {
        throw skip_test();
    }

    string string1( getString_FromItem(*item, 4) ); // BIOS VENDOR
    string string2( getString_FromItem(*item, 5) ); // BIOS VERSION
    string string3( getString_FromItem(*item, 8) ); // RELEASE DATE

    string string4( item->getStringByStringNumber(1) ); //BIOS VENDOR
    string string5( item->getStringByStringNumber(2) ); //BIOS VERSION
    string string6( item->getStringByStringNumber(3) ); //RELEASE DATE

    strip_trailing_whitespace(string1);
    strip_trailing_whitespace(string2);
    strip_trailing_whitespace(string3);
    strip_trailing_whitespace(string4);
    strip_trailing_whitespace(string5);
    strip_trailing_whitespace(string6);

    CPPUNIT_ASSERT_EQUAL( vendorStr, string1 );
    CPPUNIT_ASSERT_EQUAL( versionStr, string2 );
    CPPUNIT_ASSERT_EQUAL( releaseStr, string3 );

    CPPUNIT_ASSERT_EQUAL( string1, string4 );
    CPPUNIT_ASSERT_EQUAL( string2, string5 );
    CPPUNIT_ASSERT_EQUAL( string3, string6 );

    STD_TEST_END("");
}

void
testPlatform::testStateBytes()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    if( ! SMBIOSHasNvramStateBytes() )
        throw skip_test();

    int testValue = 0;
    SMBIOSGetNvramStateBytes( 0x0000 );

    // test DSA mode
    testValue = 0x1234;
    SMBIOSSetNvramStateBytes( testValue, 0x0000 );
    CPPUNIT_ASSERT_EQUAL( testValue, SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0x8000 ) ); // toolkit should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0xF100 ) ); // other should see 0

    // test DSA mode (proper mask off)
    testValue = 0x9234; // we should not be able to set topmost bit
    SMBIOSSetNvramStateBytes( testValue, 0x0000 );
    CPPUNIT_ASSERT_EQUAL( (testValue & ~0x8000), SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0x8000 ) ); // toolkit should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0xF100 ) ); // other should see 0

    // test Toolkit mode
    testValue = 0x0234; // we should not be able to set topmost bit
    SMBIOSSetNvramStateBytes( testValue, 0x8000 );
    CPPUNIT_ASSERT_EQUAL( testValue, SMBIOSGetNvramStateBytes( 0x8000 ) ); //toolkit sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0xF100 ) ); // other should see 0

    // test Toolkit mode (proper mask off)
    testValue = 0x7234; // we should not be able to set topmost nibble (4 bits)
    SMBIOSSetNvramStateBytes( testValue, 0x8000 );
    CPPUNIT_ASSERT_EQUAL( (testValue & ~0xF000), SMBIOSGetNvramStateBytes( 0x8000 ) ); //toolkit sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0xF100 ) ); // other should see 0

    // test other mode
    testValue = 0x0034; // we should not be able to set topmost byte
    SMBIOSSetNvramStateBytes( testValue, 0xF100 );
    CPPUNIT_ASSERT_EQUAL( testValue, SMBIOSGetNvramStateBytes( 0xF100 ) ); // other sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,    SMBIOSGetNvramStateBytes( 0x8000 ) ); // DSA should see 0

    // test other mode (proper mask off)
    testValue = 0x7234; // we should not be able to set topmost byte
    SMBIOSSetNvramStateBytes( testValue, 0xF100 );
    CPPUNIT_ASSERT_EQUAL( (testValue & ~0xFF00), SMBIOSGetNvramStateBytes( 0xF100 ) ); // other sees real value
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0x0000 ) ); // DSA should see 0
    CPPUNIT_ASSERT_EQUAL( 0x0000,                SMBIOSGetNvramStateBytes( 0x8000 ) ); // DSA should see 0

    STD_TEST_END("");
}

void
testPlatform::testUpBoot()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    if( ! SMBIOSHasBootToUp() )
        throw skip_test();

    SMBIOSSetBootToUp(1);
    CPPUNIT_ASSERT_EQUAL( 1, SMBIOSGetBootToUp() );

    SMBIOSSetBootToUp(0);
    CPPUNIT_ASSERT_EQUAL( 0, SMBIOSGetBootToUp() );

    SMBIOSSetBootToUp(1);
    CPPUNIT_ASSERT_EQUAL( 1, SMBIOSGetBootToUp() );

    SMBIOSSetBootToUp(0);
    CPPUNIT_ASSERT_EQUAL( 0, SMBIOSGetBootToUp() );

    STD_TEST_END("");
}


void
testPlatform::testOutOfBounds()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::const_iterator item = (*table)[smbios::BIOS_Information] ;

    // access string '0' should always throw. (string offset start at 1, per
    // spec)
    ASSERT_THROWS( item->getStringByStringNumber(0), smbios::StringUnavailable );

    if (!doc)
        throw skip_test();

    int numStrings = 0;
    // pull info out of xml
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *smbios = xmlutils::findElement( xmlDocGetRootElement(doc), "smbios", "", "" );
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *biosInfo = xmlutils::findElement( smbios, "biosInformation", "", "" );
        numStrings = strtoul( xmlutils::safeGetAttribute( biosInfo, "numstrings" ).c_str(), 0, 0 );
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    // Should not throw (cast to void to eliminate unused var warn)
    if( numStrings > 0 )
        (void) (item->getStringByStringNumber(numStrings));

    ASSERT_THROWS( item->getStringByStringNumber(numStrings + 1), smbios::StringUnavailable );
    ASSERT_THROWS( item->getStringByStringNumber(numStrings + 2), smbios::StringUnavailable );

    STD_TEST_END("");
}


void
testPlatform::testConstructionOffset1()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    if (!doc)
        throw skip_test();

    u32 offset = 0;
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *smbios = xmlutils::findElement( xmlDocGetRootElement(doc), "smbios", "", "" );
        offset = strtoul( xmlutils::safeGetAttribute( smbios, "offset" ).c_str(), 0, 0 );
        if( 0 == offset )
        {
            throw skip_test();
        }
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    smbios::SmbiosFactory::getFactory()->setParameter("offset", offset);
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    int tableEntriesCounted = 0;
    smbios::ISmbiosTable::const_iterator item = table->begin();
    while( item != table->end() )
    {
        tableEntriesCounted++;
        (void) *item;  // do this to sniff out possible errors with deref iterator.
        ++item;
    }

    CPPUNIT_ASSERT( tableEntriesCounted == table->getNumberOfEntries() );
    STD_TEST_END("");
}

void
testPlatform::testConstructionOffset2()
{
    STD_TEST_START_CHECKSKIP(getTestName().c_str() << "  ");

    if (!doc)
        throw skip_test();

    u32 offset = 0;
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *smbios = xmlutils::findElement( xmlDocGetRootElement(doc), "smbios", "", "" );
        offset = strtoul( xmlutils::safeGetAttribute( smbios, "offset" ).c_str(), 0, 0 );
        if( 0 == offset )
        {
            throw skip_test();
        }
    }
    catch( const exception & )
    {
        throw skip_test();
    }

    //
    // TEST BAD FILENAMES
    // construct our table with various invalid file names
    //
    smbios::SmbiosFactory *factory = smbios::SmbiosFactory::getFactory();

    // use auto_ptr so in case it does _not_ throw, we don't leak mem in the test.
    //  DO NOT USE ->getSingleton() here... we use ->makeNew() on purpose.
    //  This is an internal test and the parameters of this test mean we should _not_ use a singleton.
    factory->setParameter("offset", 1);
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable>p(factory->makeNew()), smbios::IException );

    factory->setParameter("offset", 1000);
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable>p(factory->makeNew()), smbios::IException );

    factory->setParameter("offset", 0xFFFFFUL ); // F_BLOCK_END (private definition)
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable>p(factory->makeNew()), smbios::IException );

    factory->setParameter("offset", 0xFFFFFUL - 1); // F_BLOCK_END (private definition)
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable>p(factory->makeNew()), smbios::IException );

    smbios::SmbiosFactory::getFactory()->setParameter("offset", offset + 1);
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable> table( factory->makeNew() ), smbios::IException );

    smbios::SmbiosFactory::getFactory()->setParameter("offset", offset - 1);
    ASSERT_THROWS( auto_ptr<const smbios::ISmbiosTable> table( factory->makeNew() ), smbios::IException );

    // should not be able to use no-argument constructor...
    // UNCOMMENT TO CHECK.
    // THIS TEST DOES NOT COMPILE. IT SHOULD NOT COMPILE
    // DUE TO THE DEFINITION OF SmbiosTable.
    //ASSERT_THROWS( smbios::SmbiosTableFileIo myTable1, smbios::PermissionException);

    STD_TEST_END("");
}


