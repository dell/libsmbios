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


#ifndef _OUTPUTCTL_H
#define _OUTPUTCTL_H

#include <exception>
#include <typeinfo>     // for typeid()
#include <iostream>
#include <iomanip>

// A collection of macros to use in the unit tests.
// They print things out in a standard format, or shorten unit tests.

class skip_test : public std::exception
    { public: virtual ~skip_test() throw() {}; };

#undef DCOUT
#undef DCERR
#if defined(DEBUG_TEST_OUTPUT)
#   include <iostream>
#   define DCOUT(line)  do { cout << line; } while(0)
#   define DCERR(line)  do { cerr << line; } while(0)
#else
#   define DCOUT(line)  do { } while(0)
#   define DCERR(line)  do { } while(0)
#endif

#if defined(LIBSMBIOS_HAS_PRETTY_FUNCTION) || defined(LIBSMBIOS_C_HAS_PRETTY_FUNCTION)
#define WHEREAMI "\t" << __PRETTY_FUNCTION__ << "... "
#else
#define WHEREAMI typeid(*this).name() << " (line " << __LINE__ << ")... "
#endif

#define GET_FLAGS() std::ios::fmtflags old_opts = cout.flags()
#define RESTORE_FLAGS() cout.flags(old_opts)

#define startTest(arg) do{cout << arg << WHEREAMI;        }while(0)
#define passTest(arg)  do{cout << "[ ok ]" << arg << endl;} while(0)
#define failTest(arg)  do{cout << "[FAIL]" << arg << endl;} while(0)
#define skipTest(arg)  do{cout << "[SKIP]" << arg << endl;} while(0)

// standard stuff

// Standard test start/end header
#if defined(LIBSMBIOS_HAS_PRETTY_FUNCTION) || defined(LIBSMBIOS_C_HAS_PRETTY_FUNCTION)
#define STD_TEST_START_CHECKSKIP(arg) startTest(arg);  bool skip=false; cout << flush; try { checkSkipTest(__FUNCTION__)
#define STD_TEST_START(arg)           startTest(arg);  bool skip=false; cout << flush; try {
#else
#define STD_TEST_START_CHECKSKIP(arg) startTest(arg);  bool skip=false; cout << flush; try {
#define STD_TEST_START(arg)           startTest(arg);  bool skip=false; cout << flush; try {
#endif

#define STD_TEST_END(arg)                           \
        } catch (const skip_test &) {               \
                skip = true;                        \
        } catch ( const CppUnit::Exception &e ) {   \
                failTest(arg);                      \
                throw;                              \
        } catch ( const std::exception &e ) {       \
                failTest(arg);                      \
                CPPUNIT_FAIL( e.what() );           \
        } catch (...) {         \
                failTest(arg);  \
                throw;          \
        }                       \
        if( skip )              \
            skipTest(arg);      \
        else                    \
            passTest(arg)

// extra macros that make CPPUNIT tests shorter.
#define ASSERT_THROWS( expr, exc )  \
    do {                        \
    bool caught = false;        \
    try                         \
    {                           \
        expr;                   \
    }                           \
    catch( const exc & )        \
    {                           \
            caught = true;      \
    }                           \
    catch( const std::exception &e ) \
    {                           \
        ostringstream ost;      \
        ost << "Executed: " #expr "\nCaught wrong exception. Expected: " #exc;    \
        ost << "\nLine: " << __LINE__;   \
        ost << "\nFile: " << __FILE__;   \
        ost << "\nException Caught: " << typeid(e).name();        \
        CPPUNIT_FAIL (ost.str().c_str());   \
    }                           \
    catch( ... )                \
    {                           \
        ostringstream ost;      \
        ost << "Executed: " #expr "\nCaught wrong exception. Expected: " #exc;    \
        ost << "\nLine: " << __LINE__;   \
        ost << "\nFile: " << __FILE__;   \
        CPPUNIT_FAIL (ost.str().c_str());   \
    }                           \
    if ( ! caught )             \
        CPPUNIT_FAIL ("Executed: " #expr "\nShould have thrown an exception, but did not. Expected: " #exc);\
    } while(0)



#endif  /* ! defined _OUTPUTCTL_H */
