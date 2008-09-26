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
    if (written < (size * nmemb))
        throw std::exception();
    return written;
}


