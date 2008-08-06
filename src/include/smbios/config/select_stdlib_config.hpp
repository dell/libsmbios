//  Boost compiler configuration selection header file

//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Jens Maurer 2001 - 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


//  See http://www.boost.org for most recent version.

// locate which std lib we are using and define LIBSMBIOS_STDLIB_CONFIG as needed:

// we need to include a std lib header here in order to detect which
// library is in use, use <utility> as it's about the smallest
// of the std lib headers - do not rely on this header being included -
// users can short-circuit this header if they know whose std lib
// they are using.

#include <utility>

#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
// STLPort library; this _must_ come first, otherwise since
// STLport typically sits on top of some other library, we
// can end up detecting that first rather than STLport:
#  define LIBSMBIOS_STDLIB_CONFIG "smbios/config/stdlib/stlport.hpp"

#elif defined(__GLIBCPP__)
// GNU libstdc++ 3
#  define LIBSMBIOS_STDLIB_CONFIG "smbios/config/stdlib/libstdcpp3.hpp"

#elif defined( __GNUC__ ) 
// GNU libstdc++ < 3  ?? maybe there is a better test for this?
#  define LIBSMBIOS_STDLIB_CONFIG "smbios/config/stdlib/libstdcpp2.hpp"

#elif defined (LIBSMBIOS_ASSERT_CONFIG)
// this must come last - generate an error if we don't
// recognise the library:
#  error "Unknown standard library - please configure and report the results to libsmbios maintainer."

#endif



