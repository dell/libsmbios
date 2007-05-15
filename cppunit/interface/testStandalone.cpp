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

#include <iomanip>
#include <fstream>

#include "testStandalone.h"
#include "smbios/SmbiosDefs.h"

// specific to unit tests. Users do not need to include this,
// so it is not in testStandalone.h
#include "smbios/IMemory.h"
#include "smbios/ISmi.h"
#include "smbios/IObserver.h"

#include "smbios/version.h"

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

void testStandalone::setUp()
{
    string programDirname = getCppunitTopDirectory();
    string writeDirectory = getWritableDirectory();
    string xmlFile = programDirname + getXmlFile();

    // copy the memdump.dat file. We do not write to it, but rw open will fail
    // if we do not copy it
    string memdumpOrigFile = programDirname + getTestDirectory() + "/memdump.dat";
    if(!fileExists(memdumpOrigFile))
        memdumpOrigFile = getTestDirectory() + "/memdump.dat";
    string memdumpCopyFile = writeDirectory + "/memdump-copy.dat";
    copyFile( memdumpCopyFile, memdumpOrigFile );

    // copy the CMOS file. We are going to write to it and do not wan to mess up
    // the pristine unit test version
    string cmosOrigFile = programDirname + getTestDirectory() + "/cmos.dat";
    if(!fileExists(cmosOrigFile))
        cmosOrigFile = getTestDirectory() + "/cmos.dat";
    string cmosCopyFile = writeDirectory + "/cmos-copy.dat";
    copyFile( cmosCopyFile, cmosOrigFile );

    // Smi output file.
    string smiOutput = writeDirectory + "/smi-output.dat";

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
}

void testStandalone::resetFactoryToBuiltinXml()
{
    smbios::SmbiosFactory::getFactory()->setParameter("xmlFile", "");
}

void testStandalone::tearDown()
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
}

//
//
// TABLE tests
//
//

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
    smbios::ISmbiosTable::iterator item2;

    item1 = (*table)["BIOS Information"];
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
    CPPUNIT_ASSERT_EQUAL( (*table)["BIOS Information"]->getType(), item1->getType() );

    item1 = (*table)["System Information"];
    item2 = (*table)[smbios::System_Information];

    CPPUNIT_ASSERT_EQUAL( item1->getHandle(),    item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( getItemHandle(*item1), item2->getHandle() );
    CPPUNIT_ASSERT_EQUAL( item1->getLength(),    item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( getItemLength(*item1), item2->getLength() );
    CPPUNIT_ASSERT_EQUAL( item1->getType(),      item2->getType()   );
    CPPUNIT_ASSERT_EQUAL( getItemType(*item1)  , item2->getType()   );

    CPPUNIT_ASSERT_EQUAL( (*table)[smbios::System_Information]->getType(), item1->getType() );
    CPPUNIT_ASSERT_EQUAL( (*table)["System Information"]->getType(), item1->getType() );


    STD_TEST_END("");
}

void testStandalone::testTable_Subscript_builtinXml()
{
    resetFactoryToBuiltinXml();
    testTable_Subscript();
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

void testStandalone::testEntryCount_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testEntryCount();
}

void
testStandalone::testConstIterator ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
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

void testStandalone::testConstIterator_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testConstIterator();
}

void
testStandalone::testSubscriptOperator1 ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    int tableEntriesCounted = 0;
    // table[-1] is special, it returns all objects.
    // This test should be identical to testConstIterator (both walk all entries)
    for( smbios::ISmbiosTable::iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        tableEntriesCounted++;
        (void) *item;  // do this to sniff out possible errors with deref iterator.
    }
    CPPUNIT_ASSERT_EQUAL( table->getNumberOfEntries(), tableEntriesCounted );
    STD_TEST_END("");
}

void testStandalone::testSubscriptOperator1_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testSubscriptOperator1();
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

void testStandalone::testSubscriptOperator2_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testSubscriptOperator2();
}

void
testStandalone::testSubscriptOperator3 ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // There should normally be more than one "Port Connector" block. Check.
    //                               "Port Connector" block is type == 8
    //   memdump-opti.txt     has 12
    //   memdump-PAweb110.txt has 17
    //   memdump-PAweb110.txt has 9  *unverified...
    //  update this test if it turns out that there are actual systems with <2 Port Connector blocks
    //
    int tableEntriesCounted = 0;
    for( smbios::ISmbiosTable::iterator item = (*table)[8] ; item != table->end(); ++item)
    {
        (void) *item;  // do this to sniff out possible errors with deref iterator.
        tableEntriesCounted++;
    }
    CPPUNIT_ASSERT( 1 < tableEntriesCounted );
    STD_TEST_END("");
}

void testStandalone::testSubscriptOperator3_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testSubscriptOperator3();
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
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    // grab the first bios block
    smbios::ISmbiosTable::iterator position1 = (*table)[smbios::BIOS_Information];
    smbios::ISmbiosItem &item1 = *position1; //reference

    // use another iterator and grab another
    smbios::ISmbiosTable::iterator position2 = (*table)[smbios::BIOS_Information];
    smbios::ISmbiosItem &item2 = *position2; //reference

    // Check that they both gave the same thing
    // The address of each should be equal
    CPPUNIT_ASSERT_EQUAL(  &item1, &item2 );  // easiest to read
    CPPUNIT_ASSERT_EQUAL(  &(*position1), &item2 ); // same test, written differently
    CPPUNIT_ASSERT_EQUAL(  &item1       , &(*position2) ); // same test, written differently
    CPPUNIT_ASSERT_EQUAL(  &(*position1), &(*position2) ); // same test, written differently

    // use another iterator and grab another _different_ pointer.
    smbios::ISmbiosTable::iterator position3 = (*table)[smbios::System_Information];
    smbios::ISmbiosItem &item3 = *position3; //reference

    // Check that these are different...
    CPPUNIT_ASSERT( &item1 != &item3 );
    CPPUNIT_ASSERT( &item2 != &item3 );

    CPPUNIT_ASSERT_EQUAL( item1.getType(), static_cast<u8>(smbios::BIOS_Information) );
    CPPUNIT_ASSERT_EQUAL( item2.getType(), static_cast<u8>(smbios::BIOS_Information) );
    CPPUNIT_ASSERT_EQUAL( item3.getType(), static_cast<u8>(smbios::System_Information) );

    STD_TEST_END("");
}

void testStandalone::testItemIdentity_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testItemIdentity();
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
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::iterator item = table->begin();
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

void testStandalone::testEachItemAccessors_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testStandalone::testEachItemAccessors();
}

void testStandalone::testItem_GetBiosInfo()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // Purpose:
    //      The purpose of this test is to exercise all of the getXX()
    //      functions that are avalable using std SMBIOS tables that are
    //      available on all dumps. Do not rely on any specific value in any
    //      table in these tests as this test will be run against many
    //      tables.
    //

    // BEGIN EXAMPLE factory
    // table should not be deleted when we are finished. It is managed by the
    // factory. Factory will delete it for us when ->reset() is called.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::iterator item1 = (*table)["BIOS Information"];

    //
    // here is an example XML for BIOS Information Block, which we test below.
    //
    //<FIELD offset="0h" name="Type" length="BYTE" usage="STRUCTURE_TYPE"/>
    //<FIELD offset="1h" name="Length" length="BYTE" usage="SIZE"/>
    //<FIELD offset="2h" name="Handle" length="WORD" usage="HANDLE"/>
    //<FIELD offset="4h" name="Vendor" length="BYTE" usage="STRING"/>
    //<FIELD offset="5h" name="BIOS Version" length="BYTE" usage="STRING"/>
    //<FIELD offset="6h" name="BIOS Starting Address Segment" length="WORD" usage="ADDRESS"/>
    //<FIELD offset="8h" name="BIOS Release Date" length="BYTE" usage="STRING"/>
    //<FIELD offset="9h" name="BIOS ROM Size" length="BYTE" usage="SIZE"/>
    //<FIELD offset="Ah" name="BIOS Characteristics" length="QWORD" usage="BITFIELD">
    //

    u64 ull1, ull2;
    u16 v1, v2, v3;
    u8  u1, u2, u3;
    int int1, int2;
    string str1, str2;

    // Test Philosophy:
    //      There are multiple ways to access any particular item in the table.
    //      If you have an XML smbios file, you can access by string name and
    //      the code will look the name up in the XML.
    //
    //      We play the different access modes off each other below to ensure
    //      that each access mode returns the exact same data.
    u3=0;
    u1 = getU8_FromItem(*item1, "Type");
    u2 = item1->getType();
    getData(*item1, 0, u3);
    CPPUNIT_ASSERT_EQUAL (u1, u2);
    CPPUNIT_ASSERT_EQUAL (u1, u3);

    u3=0;
    u1 = getU8_FromItem(*item1, "Length");
    u2 = item1->getLength();
    getData(*item1, 1, u3);
    CPPUNIT_ASSERT_EQUAL (u1, u2);
    CPPUNIT_ASSERT_EQUAL (u1, u3);

    v3=0;
    v1 = getU16_FromItem(*item1, "Handle");
    v2 = item1->getHandle();
    getData(*item1, 2, v3);
    CPPUNIT_ASSERT_EQUAL (v1, v2);
    CPPUNIT_ASSERT_EQUAL (v1, v3);

    str1 = getString_FromItem(*item1, "Vendor") ;
    str2 = getString_FromItem(*item1, 0x4 ) ;
    CPPUNIT_ASSERT_EQUAL (str1, str2);

    str1 = getString_FromItem(*item1, "BIOS Version") ;
    str2 = getString_FromItem(*item1, 0x5 ) ;
    CPPUNIT_ASSERT_EQUAL (str1, str2);

    v3=0;
    v1 = getU16_FromItem(*item1, "BIOS Starting Address Segment");
    v2 = getU16_FromItem(*item1, 0x6 );
    getData(*item1, 0x6, v3);
    CPPUNIT_ASSERT_EQUAL (v1, v2);
    CPPUNIT_ASSERT_EQUAL (v1, v3);

    str1 = getString_FromItem(*item1, "BIOS Release Date");
    str2 = getString_FromItem(*item1, 0x8 );
    CPPUNIT_ASSERT_EQUAL (str1, str2);

    int1 = getU8_FromItem(*item1, "BIOS ROM Size");
    int2 = getU8_FromItem(*item1, 0x9);
    CPPUNIT_ASSERT_EQUAL (int1, int2);

    ull1 = getU64_FromItem(*item1, "BIOS Characteristics");
    ull2 = getU64_FromItem(*item1, 0xA);
    // will not compile on VC++
    //    CPPUNIT_ASSERT_EQUAL (ull1, ull2);

    //First get some bits from the BIOS Characteristics bitfield and compare to U64
    u32 bitfield = 0;
    getBits_FromItem(*item1, 0xA, &bitfield, 0, 15);
    CPPUNIT_ASSERT_EQUAL (static_cast<u32>(ull2 & 0x000000000000FFFFL), bitfield);

    // cannot access bits > 64
    ASSERT_THROWS( getBits_FromItem(*item1, 0xA, &bitfield, 48, 96 ),  smbios::DataOutOfBounds );

    //Get some bits from the BIOS Characteristics bitfield using the other method
    //and compare it to the U64
    u32 tempval=0;
    bitfield  = 0;

    getBits_FromItem(*item1, "BIOS Characteristics", "Reserved0", &tempval);
    bitfield  = tempval;

    getBits_FromItem(*item1, "BIOS Characteristics", "Reserved1", &tempval);
    bitfield |= (tempval << 1);

    getBits_FromItem(*item1, "BIOS Characteristics", "Unknown", &tempval);
    bitfield |= (tempval << 2);

    getBits_FromItem(*item1, "BIOS Characteristics", "BIOS Characteristics Not Supported", &tempval);
    bitfield |= (tempval << 3);

    getBits_FromItem(*item1, "BIOS Characteristics", "ISA is supported", &tempval);
    bitfield |= (tempval << 4);

    getBits_FromItem(*item1, "BIOS Characteristics", "MCA is supported", &tempval);
    bitfield |= (tempval << 5);

    getBits_FromItem(*item1, "BIOS Characteristics", "EISA is supported", &tempval);
    bitfield |= (tempval << 6);

    getBits_FromItem(*item1, "BIOS Characteristics", "PCI is supported", &tempval);
    bitfield |= (tempval << 7);

    getBits_FromItem(*item1, "BIOS Characteristics", "PC Card (PCMCIA) is supported", &tempval);
    bitfield |= (tempval << 8);

    getBits_FromItem(*item1, "BIOS Characteristics", "Plug and Play is supported", &tempval);
    bitfield |= (tempval << 9);

    getBits_FromItem(*item1, "BIOS Characteristics", "APM is supported", &tempval);
    bitfield |= (tempval << 10);

    getBits_FromItem(*item1, "BIOS Characteristics", "BIOS is Upgradeable (Flash)", &tempval);
    bitfield |= (tempval << 11);

    getBits_FromItem(*item1, "BIOS Characteristics", "BIOS shadowing is allowed", &tempval);
    bitfield |= (tempval << 12);

    getBits_FromItem(*item1, "BIOS Characteristics", "VL-VESA is supported", &tempval);
    bitfield |= (tempval << 13);

    getBits_FromItem(*item1, "BIOS Characteristics", "ESCD support is available", &tempval);
    bitfield |= (tempval << 14);

    getBits_FromItem(*item1, "BIOS Characteristics", "Boot from CD is supported", &tempval);
    bitfield |= (tempval << 15);

    //cout << "BIOS Characteristics Bitfield (0x" << hex << bitfield << ")" << dec << endl;
    //Compare the lower 15 bits with the bitfield we just retrieved
    CPPUNIT_ASSERT_EQUAL (static_cast<u32>(ull2 & 0x000000000000FFFFL), bitfield);

    STD_TEST_END("");
}

void testStandalone::testItem_GetBiosInfo_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testItem_GetBiosInfo();
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
    ASSERT_THROWS( (*p)["BIOS Information"], smbios::NotImplemented );
    ASSERT_THROWS( (*q)["BIOS Information"], smbios::NotImplemented );

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

    ASSERT_THROWS( getU8_FromItem(*item1, "BIOS Version"), smbios::NotImplemented);
    ASSERT_THROWS( getU8_FromItem(*item1, "BIOS Version"), smbios::NotImplemented);
    ASSERT_THROWS( getU8_FromItem(*item1, "BIOS Version"), smbios::NotImplemented);
    ASSERT_THROWS( getU8_FromItem(*item1, "BIOS Version"), smbios::NotImplemented);
    ASSERT_THROWS( getString_FromItem(*item1, "BIOS Version"), smbios::NotImplemented);

    STD_TEST_END("");
}

void testStandalone::testItem_GetSystemInfo()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // PURPOSE:
    //      Same purpose as testGet_BiosInfo()

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::iterator item1 = ( *table )["System Information"];

    //
    // here is an example XML for System Information Block, which we test below.
    //
    // <FIELD offset="0h" name="Type" length="BYTE" usage="STRUCTURE_TYPE"/>
    // <FIELD offset="1h" name="Length" length="BYTE" usage="SIZE"/>
    // <FIELD offset="2h" name="Handle" length="WORD" usage="HANDLE"/>
    // <FIELD offset="4h" name="Manufacturer" length="BYTE" usage="STRING"/>
    // <FIELD offset="5h" name="Product Name" length="BYTE" usage="STRING"/>
    // <FIELD offset="6h" name="Version" length="BYTE" usage="STRING"/>
    // <FIELD offset="7h" name="Serial Number" length="BYTE" usage="STRING"/>
    // <FIELD offset="8h" version="2.1" name="UUID" length="BYTE" count="16" usage="NUMBER"/>
    // <FIELD offset="18h" version="2.1" name="Wake-up Type" length="BYTE" usage="ENUM">
    //
    //
    string str1, str2;
    int int1, int2;

    int1 = getU8_FromItem(*item1, "Type");
    int2 = item1->getType();
    CPPUNIT_ASSERT_EQUAL (int1, int2);

    int1 = getU8_FromItem(*item1, "Length");
    int2 = item1->getLength();
    CPPUNIT_ASSERT_EQUAL (int1, int2);

    int1 = getU16_FromItem(*item1, "Handle");
    int2 = item1->getHandle();
    CPPUNIT_ASSERT_EQUAL (int1, int2);

    str1 = getString_FromItem(*item1, "Manufacturer") ;
    str2 = getString_FromItem(*item1, 0x4 ) ;
    CPPUNIT_ASSERT_EQUAL (str1, str2);

    str1 = getString_FromItem(*item1, "Product Name") ;
    str2 = getString_FromItem(*item1, 0x5 ) ;
    CPPUNIT_ASSERT_EQUAL (str1, str2);

#if 0
    //
    // This is not a good test case because several
    // of our BIOSs have a '\0' in the header for this
    // string, which means this string does not
    // exist. Lowlevel code will throw an exception.
    //
    str1 = getString_FromItem(*item1, "Version") ;
    str2 = getString_FromItem(*item1, 0x6 ) ;
    CPPUNIT_ASSERT_EQUAL( str1, str2 );
#endif

    try
    {
        str1 = getString_FromItem(*item1, "Serial Number") ;
        str2 = getString_FromItem(*item1, 0x7 ) ;
        CPPUNIT_ASSERT_EQUAL (str1, str2);
    }
    catch( const exception & )
    {
        // 4G systems do not support Serial Number.
    }

#if 0
    //
    // These are not good test cases because they are SMBIOS 2.3
    // additions and are not guaranteed to be present.
    //
    u8 val1, val2;
    val1 =  item1->getU8("UUID") ;
    val2 = item1->getU8( 0x8 ) ;
    CPPUNIT_ASSERT_EQUAL( val1, val2 );

    val1 = item1->getU8("Wake-up Type") ;
    val2 = item1->getU8( 0x9 ) ;
    CPPUNIT_ASSERT_EQUAL( val1, val2 );
#endif

    STD_TEST_END("");
}

void testStandalone::testItem_GetSystemInfo_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testItem_GetSystemInfo();
}

// helper because this isn't external API
extern const string getStringForType(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, const int searchForType );
#include "SmbiosXmlImpl.h"

void testStandalone::testSmbiosXml()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
            smbios::SmbiosFactory::getFactory()->getSingleton();
    
    // test reverse name mapping function
    string bios = dynamic_cast<smbios::SmbiosTableXml *>(table)->getStringForType(0);
    string expected = "BIOS Information";
    CPPUNIT_ASSERT_EQUAL ( bios, expected );

    // test if "PCI Supported" bit is set, should always be set.
    smbios::ISmbiosTable::iterator item = (*table)["BIOS Information"];
    CPPUNIT_ASSERT_EQUAL( isBitSet( &(*item), 0xA,  0x7 ), true );
    
    STD_TEST_END("");
}

void testStandalone::testTypeMismatch()
{
    STD_TEST_START(getTestName().c_str() << "  " );
    // PURPOSE:
    //      The purpose of this test is to test all types of invalid item
    //      access. The getXX(string) methods all validate that the field
    //      passed to them is actually of type XX. If a mismatch is found, an
    //      exception is thrown.
    //
    //      Each test validates that an exception is thrown and the type.

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    smbios::ISmbiosTable::iterator item1 = (*table)["BIOS Information"];

    //
    // refer to testGet_BiosInfo() for XML sample for BIOS INFORMATION BLOCK
    //

    // ASSERT_THROWS is not a CPPUNIT macro, it is our custom macro.
    ASSERT_THROWS( getU8_FromItem (*item1, "Handle"), smbios::ParseException );
    ASSERT_THROWS( getU16_FromItem(*item1, "Type"), smbios::ParseException );
    ASSERT_THROWS( getU32_FromItem(*item1, "Type"), smbios::ParseException );
    ASSERT_THROWS( getU64_FromItem(*item1, "Type"), smbios::ParseException );
    ASSERT_THROWS( getString_FromItem(*item1, "Type"), smbios::ParseException );
    ASSERT_THROWS( getBits_FromItem(*item1, "Type", "foo", NULL), smbios::ParseException );

    STD_TEST_END("");
}

void testStandalone::testTypeMismatch_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testStandalone::testTypeMismatch();
}


void
testStandalone::testGetBoundaries()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    // do not delete. Factory manages lifetime.
    smbios::ISmbiosTable *table =
        smbios::SmbiosFactory::getFactory()->getSingleton();

    for( smbios::ISmbiosTable::iterator item = (*table)[-1] ; item != table->end(); ++item)
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

void testStandalone::testGetBoundaries_builtinXml ()
{
    resetFactoryToBuiltinXml();
    testStandalone::testGetBoundaries();
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

    std::auto_ptr<smi::ISmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI_RAW);
    smi->setCommandIOMagic( 0x1234, 0x56 );

    smi::IDellCallingInterfaceSmi *ci = dynamic_cast<smi::IDellCallingInterfaceSmi *>(&*smi);
    ci->setClass( 0xAABB );
    ci->setSelect( 0xCCDD );
    ci->setArg( 0, 0xA1A2A3A4 );
    ci->setArg( 1, 0xB1B2B3B4 );
    ci->setArg( 2, 0xC1C2C3C4 );
    ci->setArg( 3, 0xD1D2D3D4 );
    try
    {   /* This is expected to fail in unit test, no good way to simulate them*/
        ci->execute();
    }
    catch( const smi::UnhandledSmi & ) {}

    STD_TEST_END("");
}

void
testStandalone::testSmi_callingInterface_physaddr ()
{
    STD_TEST_START(getTestName().c_str() << "  " );

    std::auto_ptr<smi::ISmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI_RAW);
    smi->setCommandIOMagic( 0x1234, 0x56 );

    smi::IDellCallingInterfaceSmi *ci = dynamic_cast<smi::IDellCallingInterfaceSmi *>(&*smi);
    ci->setClass( 0xAABB );
    ci->setSelect( 0xCCDD );
    ci->setArgAsPhysicalAddress(0, 0);
    ci->setArgAsPhysicalAddress(1, 1);
    ci->setArgAsPhysicalAddress(2, 2);
    ci->setArgAsPhysicalAddress(3, 3);
    try
    {   /* This is expected to fail in unit test, no good way to simulate them*/
        ci->execute();
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

    string fromSystem = SMBIOSGetLibraryVersionString();
    string testInput  = LIBSMBIOS_RELEASE_VERSION;

    CPPUNIT_ASSERT_EQUAL ( testInput, fromSystem );
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

