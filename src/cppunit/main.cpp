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

#include <string>
#include <string.h>   // strncat, strndup, and friends
#include <fstream>
#include <iostream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/XmlOutputter.h>

#include "smbios/IMemory.h"
#include "smbios/ICmosRW.h"
#include "smbios/ISmi.h"
#include "smbios/ISmbios.h"
#include "smbios/IToken.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/obj/cmos.h"
#include "smbios_c/obj/smi.h"
#include "main.h"

using namespace std;

int global_argc;
char ** global_argv;
int global_DebugLevel = 9;

int
main (int argc, char **argv)
{
    global_argc = argc;
    global_argv = argv;

    std::ofstream outputFile("testResults.xml");
    CppUnit::TextUi::TestRunner runner;

    CppUnit::TestFactoryRegistry & registry =
        CppUnit::TestFactoryRegistry::getRegistry ();

    CppUnit::XmlOutputter* outputter = new CppUnit::XmlOutputter( &runner.result(), outputFile );

    runner.setOutputter(outputter);
    runner.addTest (registry.makeTest ());

    bool wasSuccessful = runner.run ("",        //std::string testName ="",
                                     false, //bool doWait = false, (lets user press <return> after tests)
                                     true,  //bool doPrintResult = true,
                                     false  //bool doPrintProgress = true );  // prints dots "."
                                    );

    outputFile.close();

    // need to use _reverse_ logic here because the shell is backwards!
    return !wasSuccessful;
}

static int smi_ut_init_fn(struct dell_smi_obj *smi)
{
    return 0;
}

string setupMemoryForUnitTest(string testdir, string writedir)
{
    string memdumpOrigFile = testdir + "/memdump.dat";
    string memdumpCopyFile = writedir + "/memdump-copy.dat";
    copyFile( memdumpCopyFile, memdumpOrigFile );

    // C++
    memory::MemoryFactory::getFactory()->setParameter("memFile", memdumpCopyFile);
    memory::MemoryFactory::getFactory()->setMode( memory::MemoryFactory::UnitTestMode );

    // C
    memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, memdumpCopyFile.c_str());

    return memdumpCopyFile;
}

string setupCmosForUnitTest(string testdir, string writedir)
{
    string cmosOrigFile = testdir + "/cmos.dat";
    string cmosCopyFile = writedir + "/cmos-copy.dat";
    copyFile( cmosCopyFile, cmosOrigFile );

    // C++
    cmos::  CmosRWFactory::getFactory()->setParameter("cmosMapFile", cmosCopyFile);
    cmos::  CmosRWFactory::getFactory()->setMode( factory::IFactory::UnitTestMode );

    // C
    cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, cmosCopyFile.c_str());


    return cmosCopyFile;
}


void setupSmbiosForUnitTest()
{
    smbios::SmbiosFactory::getFactory()->setParameter("offset", 0);
    smbios::SmbiosFactory::getFactory()->setMode(smbios::SmbiosFactory::UnitTestMode);
}

void setupSmiForUnitTest(string testdir, string writedir)
{
    // C
    string d = writedir + "/";
    struct dell_smi_obj *smi = 0;
    smi = dell_smi_factory(DELL_SMI_GET_SINGLETON | DELL_SMI_UNIT_TEST_MODE, smi_ut_init_fn);
    dell_smi_obj_free(smi);

    // C++
    string smiOutput = writedir + "/smi-output.dat";
    smi::SmiFactory::getFactory()->setParameter("smiFile", smiOutput);
    smi::SmiFactory::getFactory()->setMode( smi::SmiFactory::UnitTestMode );
}

void setupForUnitTesting(string testdir, string writedir)
{
    setupMemoryForUnitTest(testdir, writedir);
    setupCmosForUnitTest(testdir, writedir);
    setupSmiForUnitTest(testdir, writedir);
    setupSmbiosForUnitTest();
}

void reset()
{
    smbios::TokenTableFactory::getFactory()->reset();
    smbios::SmbiosFactory::getFactory()->reset();
    memory::MemoryFactory::getFactory()->reset();
    cmos::CmosRWFactory::getFactory()->reset();
    smi::SmiFactory::getFactory()->reset();
}

string &strip_trailing_whitespace(string &s)
{
    while (  s[ s.length() - 1 ] == ' ' )
        s.erase( s.length() - 1, 1 );

    return s;
}

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

size_t FWRITE(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    // TODO: handle short write
    if (written < nmemb)
        throw std::exception();
    return written;
}


