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

#include "testStandalone.h"
#include "smbios/SmbiosDefs.h"

// specific to unit tests. Users do not need to include this,
// so it is not in testStandalone.h
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
CPPUNIT_TEST_SUITE_REGISTRATION (testStandalone);

void testStandalone::setUp()
{
    setupForUnitTesting(getTestDirectory(), getWritableDirectory());
}

void testStandalone::tearDown()
{
    // the factory is static. If we do not reset the factory, the next
    // unit test may accidentally get the wrong objects.
    // Lifetime rules: CmosTokenTable cannot live longer than the ISmbiosTable
    // object used in its construction.
    reset();
}

//
//
// TABLE tests
//
//

void testStandalone::testSmbiosTableBase()
{}

void testStandalone::testSmbiosTableBase_iterNextItem()
{}

void testStandalone::testSmbiosTableBase_findItemByHandle()
{}

void testStandalone::testSmbiosTableBase_findItemByType()
{}


void testStandalone::testTable_Subscript()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // PURPOSE:
    //      The purpose of this test is to test the subscript operator [].
    //      It tests these operators using string and int args and tests
    //      it outside of a loop.

    // table should not be deleted when we are finished. It is managed by the
    // factory. Factory will delete it for us when ->reset() is called.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::iterator item1;
    smbios::ISmbiosTable::const_iterator item2;

    item1 = (*table)[smbios::BIOS_Information];
    item2 = (*table)[smbios::BIOS_Information];

    // Use CPPUNIT_ASSERT_EQUAL when testing equality.
    // It gives better diagnostics on failure.
    CPPUNIT_ASSERT_EQUAL( item1->getHandle(),    item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( getItemHandle(*item1), item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( item1->getLength(),    item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( getItemLength(*item1), item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( item1->getType()  ,    item2->getType()   );
    CPPUNIT_ASSERT_EQUAL( getItemType(*item1)  , item2->getType()   );

    CPPUNIT_ASSERT_EQUAL( (*table)[smbios::BIOS_Information]->getType(), item1->getType() );

    item1 = (*table)[smbios::System_Information];
    item2 = (*table)[smbios::System_Information];

    CPPUNIT_ASSERT_EQUAL( item1->getHandle(),    item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( getItemHandle(*item1), item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( item1->getLength(),    item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( getItemLength(*item1), item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( item1->getType(),      item2->getType()   );
    CPPUNIT_ASSERT_EQUAL( getItemType(*item1)  , item2->getType()   );

    CPPUNIT_ASSERT_EQUAL( (*table)[smbios::System_Information]->getType(), item1->getType() );


    STD_TEST_END("");
}

void
testStandalone::testEntryCount ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // test streamify() while we are at it.
    ostringstream ost;
    int tableEntriesCounted = 0;
    smbios::ISmbiosTable::iterator item = table->begin();
    while( item != table->end() )
    {
        tableEntriesCounted++;
        ost << *item << endl;
        ++item;
    }

    CPPUNIT_ASSERT_EQUAL( tableEntriesCounted, table->getNumberOfEntries() );
    STD_TEST_END("");
}

void
testStandalone::testConstIterator ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    const smbios::ISmbiosTable *constTable = &*table;

    int tableEntriesCounted = 0;
    smbios::ISmbiosTable::const_iterator item = constTable->begin();
    while( item != constTable->end() )
    {
        tableEntriesCounted++;
        (void) *item;  // do this to sniff out possible errors with deref iterator.

        ++item;
    }
    CPPUNIT_ASSERT_EQUAL( tableEntriesCounted, constTable->getNumberOfEntries() );
    STD_TEST_END("");
}

void
testStandalone::testSubscriptOperator1 ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    int tableEntriesCounted = 0;
    // table[-1] is special, it returns all objects.
    // This test should be identical to testConstIterator (both walk all entries)
    for( smbios::ISmbiosTable::const_iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        tableEntriesCounted++;
        (void) *item;  // do this to sniff out possible errors with deref iterator.
    }
    CPPUNIT_ASSERT_EQUAL( table->getNumberOfEntries(), tableEntriesCounted );
    STD_TEST_END("");
}

void
testStandalone::testSubscriptOperator2 ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

// it turns out that 8450 has 3 BIOS Information blocks.

//    // do not delete. Factory manages lifetime.
//    smbios::ISmbiosTable *table =
//        smbios::SmbiosFactory::getFactory()->getSingleton();
//
//    // There should probably be only one "BIOS Information" block. Check.
//    //  update this test if it turns out that there are actual systems with >1 Bios Info block
//    //
//    int tableEntriesCounted = 0;
//    for( smbios::ISmbiosTable::iterator item = (*table)[0] ; item != table->end(); ++item)
//    {
//        (void) *item;  // do this to sniff out possible errors with deref iterator.
//        tableEntriesCounted++;
//    }
//    CPPUNIT_ASSERT_EQUAL( 1, tableEntriesCounted );

    STD_TEST_END("");
}

void
testStandalone::testSubscriptOperator3 ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // There should normally be more than one "Port Connector" block. Check.
    //                               "Port Connector" block is type == 8
    //   memdump-opti.txt     has 12
    //   memdump-PAweb110.txt has 17
    //   memdump-PAweb110.txt has 9  *unverified...
    //  update this test if it turns out that there are actual systems with <2 Port Connector blocks
    //
    int tableEntriesCounted = 0;
    for( smbios::ISmbiosTable::const_iterator item = (*table)[8] ; item != table->end(); ++item)
    {
        (void) *item;  // do this to sniff out possible errors with deref iterator.
        tableEntriesCounted++;
    }
    CPPUNIT_ASSERT( 1 < tableEntriesCounted );
    STD_TEST_END("");
}



//
//
// ITEM tests
//
//

void testStandalone::testStreamify()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // BEGIN EXAMPLE iterator
    // table should not be deleted when we are finished. It is managed by the
    // factory. Factory will delete it for us when ->reset() is called.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    ostringstream ost;
    ost << *table << endl;

    // iterate over all PORT INFORMATION BLOCKS
    for( smbios::ISmbiosTable::iterator item = (*table)[8] ; item != table->end(); ++item)
    {
        ost << *item << endl;
    }
    // END EXAMPLE iterator

    STD_TEST_END("");
}

void
testStandalone::testItemIdentity ()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // test that when we grab things out of the
    // table, we get copies of the same thing
    // when we ask for the same thing, rather than
    // separate items with the same data.
    //
    // we use references below to make the CPPUNIT_ASSERT
    // a bit easier to read.

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // grab the first bios block
    smbios::ISmbiosTable::const_iterator position1 = (*table)[smbios::BIOS_Information];
    const smbios::ISmbiosItem &item1 = *position1; //reference

    // use another iterator and grab another
    smbios::ISmbiosTable::const_iterator position2 = (*table)[smbios::BIOS_Information];
    const smbios::ISmbiosItem &item2 = *position2; //reference

    // Check that they both gave the same thing
    // The address of each should be equal
    CPPUNIT_ASSERT_EQUAL(  &item1, &item2 );  // easiest to read
    CPPUNIT_ASSERT_EQUAL(  &(*position1), &item2 ); // same test, written differently
    CPPUNIT_ASSERT_EQUAL(  &item1       , &(*position2) ); // same test, written differently
    CPPUNIT_ASSERT_EQUAL(  &(*position1), &(*position2) ); // same test, written differently

    // use another iterator and grab another _different_ pointer.
    smbios::ISmbiosTable::const_iterator position3 = (*table)[smbios::System_Information];
    const smbios::ISmbiosItem &item3 = *position3; //reference

    // Check that these are different...
    CPPUNIT_ASSERT( &item1 != &item3 );
    CPPUNIT_ASSERT( &item2 != &item3 );

    CPPUNIT_ASSERT_EQUAL( item1.getType(), static_cast<u8>(smbios::BIOS_Information) );
    CPPUNIT_ASSERT_EQUAL( item2.getType(), static_cast<u8>(smbios::BIOS_Information) );
    CPPUNIT_ASSERT_EQUAL( item3.getType(), static_cast<u8>(smbios::System_Information) );

    STD_TEST_END("");
}

void
testStandalone::testEachItemAccessors ()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // test getU{8|16}_FromItem() functions
    // no way to test in generic table the getU32
    // or getU64
    //
    // primarily to ensure that getUx_FromItem() has the endianness correct

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::const_iterator item = table->begin();
    while( item != table->end() )
    {
        u8 type1 = item->getType();
        u8 type2 = getU8_FromItem(*item, 0);
        CPPUNIT_ASSERT_EQUAL( type1, type2 );

        u8 len1 = item->getLength();
        u8 len2 = getU8_FromItem(*item, 1);
        CPPUNIT_ASSERT_EQUAL( len1, len2 );

        u16 handle1 = item->getHandle();
        u16 handle2 = getU16_FromItem(*item, 2);
        CPPUNIT_ASSERT_EQUAL( handle1, handle2 );

        ++item;
    }

    STD_TEST_END("");
}

void testStandalone::testNonXml()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    smbios::SmbiosFactory *factory = smbios::SmbiosFactory::getFactory();
    factory->reset();

    //
    // This is now a NON-XML factory.
    // This call nails the _instance variable to a regular non-xml factory
    // instance. Subsequent setup calls in setUp ensure it is properly set
    // with the correct paths and whatnot.
    tearDown();
    factory = smbios::SmbiosFactory::getFactory();
    setUp();

    // Ok, none of these are related to construction offset, but it is good to
    // run these tests while we have a handy reference to a NON-XML factory.
    auto_ptr<smbios::ISmbiosTable>p(factory->makeNew());
    auto_ptr<const smbios::ISmbiosTable>q(factory->makeNew());

    smbios::ISmbiosTable::iterator item1 = (*p)[smbios::BIOS_Information];
    smbios::ISmbiosTable::const_iterator item2 = (*q)[smbios::BIOS_Information];

    // test streamify() for non-XML SmbiosItem class while we are here.
    ostringstream ost;
    ost << *item1 << endl;

    CPPUNIT_ASSERT_EQUAL( item1->getHandle(), item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( item1->getHandle(), getU16_FromItem(*item2, 2) );
    CPPUNIT_ASSERT_EQUAL( item1->getLength(), item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( item1->getLength(), getU8_FromItem(*item2, 1) );
    CPPUNIT_ASSERT_EQUAL( item1->getType()  , item2->getType()   );
    CPPUNIT_ASSERT_EQUAL( item1->getType()  , getU8_FromItem(*item2, 0)   );

    STD_TEST_END("");
}

void testStandalone::testSmbiosXml()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
            smbios::SmbiosFactory::getFactory()->getSingleton();
    
    // test if "PCI Supported" bit is set, should always be set.
    smbios::ISmbiosTable::const_iterator item = (*table)[smbios::BIOS_Information];
    CPPUNIT_ASSERT_EQUAL( isBitSet( &(*item), 0xA,  0x7 ), true );
    
    STD_TEST_END("");
}



void
testStandalone::testGetBoundaries()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    const smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    for( smbios::ISmbiosTable::const_iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        int len = item->getLength();

        // none of these should throw.
        (void) getU8_FromItem (*item, len - 1);
        (void) getU16_FromItem(*item, len - 2);
        (void) getU32_FromItem(*item, len - 4);

        // not all items are large enough. only attempt if item is at least 8 bytes
        if( len >= 8 )
            (void) getU64_FromItem(*item, len - 8);

        ASSERT_THROWS( getU8_FromItem (*item, len - 0), smbios::DataOutOfBounds);
        ASSERT_THROWS( getU16_FromItem(*item, len - 1), smbios::DataOutOfBounds);
        ASSERT_THROWS( getU32_FromItem(*item, len - 3), smbios::DataOutOfBounds);
        ASSERT_THROWS( getU64_FromItem(*item, len - 7), smbios::DataOutOfBounds);
    }

    STD_TEST_END("");
}



//
// Memory tests
//
void
testStandalone::testMemoryBadFiles ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    memory::MemoryFactory *memFact = memory::MemoryFactory::getFactory();

    memFact->setParameter("memFile", "");
    ASSERT_THROWS( memFact->makeNew() , memory::AccessError);

    memFact->setParameter("memFile", "nonexistentfile");
    ASSERT_THROWS( memFact->makeNew() , memory::AccessError);

    memory::MemoryFactory::getFactory()->setMode( 2 );
    ASSERT_THROWS( memFact->makeNew(), smbios::NotImplemented);

    memory::MemoryFactory::getFactory()->setMode( memory::MemoryFactory::UnitTestMode );

    STD_TEST_END("");
}

void
testStandalone::testMemoryFuncs ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    memory::MemoryFactory *memFact = memory::MemoryFactory::getFactory();

    memory::IMemory *mem =  memFact->getSingleton();

    mem->getByte( 0 );

    ASSERT_THROWS( mem->getByte( 0x10000000UL ), memory::AccessError);

    mem->putByte( 0, 254 );
    int b = mem->getByte( 0 );

    CPPUNIT_ASSERT_EQUAL ( b, 254 );
    STD_TEST_END("");
}




//
// CMOS Token
//


void
testStandalone::testCmosConstructor ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    smbios::TokenTableFactory *ttFactory;
    ttFactory = smbios::TokenTableFactory::getFactory() ;
    smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

    ostringstream ost;

    smbios::ITokenTable::iterator token = tokenTable->begin();
    while( token != tokenTable->end() )
    {
        ost << *token << endl;
        ++token;
    }

    STD_TEST_END("");
}


//
// SMI Tests
//

void
testStandalone::testSmi_callingInterface()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);

    smi->setClass( 0xAABB );
    smi->setSelect( 0xCCDD );
    smi->setArg( 0, 0xA1A2A3A4 );
    smi->setArg( 1, 0xB1B2B3B4 );
    smi->setArg( 2, 0xC1C2C3C4 );
    smi->setArg( 3, 0xD1D2D3D4 );
    try
    {   /* This is expected to fail in unit test, no good way to simulate them*/
        smi->execute();
    }
    catch( const smi::UnhandledSmi & ) {}

    STD_TEST_END("");
}

void
testStandalone::testSmi_callingInterface_physaddr ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);
    smi->setClass( 0xAABB );
    smi->setSelect( 0xCCDD );
    smi->setArgAsPhysicalAddress(0, 0);
    smi->setArgAsPhysicalAddress(1, 1);
    smi->setArgAsPhysicalAddress(2, 2);
    smi->setArgAsPhysicalAddress(3, 3);
    try
    {   /* This is expected to fail in unit test, no good way to simulate them*/
        smi->execute();
    }
    catch( const smi::UnhandledSmi & ) {}

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
testStandalone::testLibraryVersion()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    string version = SMBIOSGetLibraryVersionString();
    cout << "Libsmbios version: " << version << "  ";
    //string testInput  = LIBSMBIOS_RELEASE_VERSION;
    //CPPUNIT_ASSERT_EQUAL ( testInput, version );
    STD_TEST_END("");
}

void
testStandalone::testException()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    std::string actual = "";
    smbios::Exception<smbios::IException> foo;
    string source = "";
    string expected = "";
    foo.setParameter("foo", "happy");
    foo.setParameter("bar", 42);
    foo.setParameter("recursive", "%(foo)s");
    foo.setParameter("recursiverecursive", "%(recursive)s");

    source = "The %% cat is %(foo)s. The best number is %(bar)i. %";
    expected = "The % cat is happy. The best number is 42. %";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    // test copy constructor
    smbios::Exception<smbios::IException> bar = foo;
    actual = bar.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = "The %% cat is %(recursive)s. The best number is %(bar)i. %";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = "The %% cat is %(recursiverecursive)s. The best number is %(bar)i. %";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = "The %% cat %is %(recursiverecursive)s. The best number is %(bar)i. %";
    expected = "The % cat %is happy. The best number is 42. %";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = " %(a_really_long_variable_longer_than_32_characters)s";
    expected = " %(a_really_long_variable_longer_than_32_characters)s";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = " %(no_closing_paren";
    expected = " %(no_closing_paren";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = " %(a_var_with_no_type)";
    expected = " %(a_var_with_no_type)";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = " %(a_var_with_no_type)  ";
    expected = " %(a_var_with_no_type)  ";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    source = " %";
    expected = " %";
    foo.setMessageString( source );
    actual = foo.what();
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    STD_TEST_END("");
}

