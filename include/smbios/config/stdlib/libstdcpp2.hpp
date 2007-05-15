//  (C) Copyright John Maddock 2001. 
//  (C) Copyright Jens Maurer 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  config for libstdc++ v2 (packaged with gcc 2.96, RHAS21 or RH7.x)
//  not much to go in here:

#define LIBSMBIOS_STDLIB "GNU libstdc++ version < 3 (cannot autodetect)."

// forward compat for newer names
// GCC 2.96 has only ios, but the std says it has been renamed to ios_base
#if __GNUC__ < 3
#   define ios_base ios
#endif

#ifndef _GLIBCPP_USE_WCHAR_T
#  define LIBSMBIOS_NO_CWCHAR
#  define LIBSMBIOS_NO_CWCTYPE
#  define LIBSMBIOS_NO_STD_WSTRING
#  define LIBSMBIOS_NO_STD_WSTREAMBUF
#endif
 

