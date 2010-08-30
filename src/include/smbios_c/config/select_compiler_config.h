//  Boost compiler configuration selection header file

//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Martin Wille 2003.
//  (C) Copyright Guillaume Melquiond 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.
//
//  Modified on 2004-03-28 for libsmbios by Michael Brown
//    -- libsmbios only supports limited compiler and platform configs, so we
//    have removed some of the compilers supported by boost. We can add a
//    few back in if they are needed in the future (IBM, Metroworks, etc.)
//
//    To add new compiler back in, please copy the relevant lines from boost.


// locate which compiler we are using and define
// LIBSMBIOS_COMPILER_CONFIG as needed:

# if defined __GNUC__
//  GNU C++:
#   define LIBSMBIOS_C_COMPILER_CONFIG "smbios_c/config/compiler/gcc.h"

#elif defined _MSC_VER
//  Microsoft Visual C++
//
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#   define LIBSMBIOS_C_COMPILER_CONFIG "smbios_c/config/compiler/visualc.h"

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
// Sun Studio Compiler
#define LIBSMBIOS_C_COMPILER_CONFIG "smbios_c/config/compiler/sunpro_cc.h"

#elif defined (LIBSMBIOS_C_ASSERT_CONFIG)
// this must come last - generate an error if we don't
// recognise the compiler:
#  error "Unknown compiler - please report to libsmbios maintainer."

#endif
