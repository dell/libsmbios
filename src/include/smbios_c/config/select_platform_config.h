//  Boost compiler configuration selection header file

//  (C) Copyright John Maddock 2001 - 2002.
//  (C) Copyright Jens Maurer 2001.
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
//    To add new platform back in, please copy the relevant lines from boost.


// locate which platform we are on and define LIBSMBIOS_PLATFORM_CONFIG as needed.
// Note that we define the headers to include using "header_name" not
// <header_name> in order to prevent macro expansion within the header
// name (for example "linux" is a macro on linux systems).

#if defined(linux) || defined(__linux) || defined(__linux__)
// linux:
#  define LIBSMBIOS_C_PLATFORM_CONFIG "smbios_c/config/platform/linux.h"

#elif defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
// win64:
#  define LIBSMBIOS_C_PLATFORM_CONFIG "smbios_c/config/platform/win64.h"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// win32:
#  define LIBSMBIOS_C_PLATFORM_CONFIG "smbios_c/config/platform/win32.h"

#elif defined(sun)
// solaris
#  define LIBSMBIOS_C_PLATFORM_CONFIG "smbios_c/config/platform/solaris.h"

#else

#  if defined (LIBSMBIOS_C_ASSERT_CONFIG)
      // this must come last - generate an error if we don't
      // recognise the platform:
#     error "Unknown platform - please report to libsmbios maintainer."
#  endif

#endif



