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
std::string global_programDirname;
std::string global_writeDirectory;
std::string global_testName;
std::string global_testDirectory;
int global_DebugLevel = 9;

int
main (int argc, char **argv)
{
    global_argc = argc;
    global_argv = argv;

    if( argc < 5 )
    {
        cout <<
            "\nusage:\n"
            "   <program> <cppunit_directory> <writeable_directory>\n"
            "   Where both directory names are required arguments.\n"
            "      cppunit_directory  - location where platform/ directory is located.\n"
            "      writeable_directory- location of a writeable dir for unit test.\n"
            "      test_name          - name of test.\n"
            "      test_dir           - location of unit test data files.\n"
            "\n"
        << endl;
        exit(1);
    }

    // set up this global var. enough stuff uses it to make it a global...
    global_programDirname = argv[1];
    global_writeDirectory = argv[2];
    global_testName = argv[3];
    global_testDirectory = argv[4];

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
