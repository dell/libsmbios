//  abi_sufffix header  -------------------------------------------------------//

// © Copyright John Maddock 2003

// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt).

// This header should be #included AFTER code that was preceded by a #include
// <boost/config/abi_prefix.h>.

#ifndef LIBSMBIOS_C_CONFIG_ABI_PREFIX_H
# error Header smbios_c/config/abi_suffix.h must only be used after smbios_c/config/abi_prefix.h
#else
# undef LIBSMBIOS_C_CONFIG_ABI_PREFIX_H
#endif

// the suffix header occurs after all of our code:
#ifdef LIBSMBIOS_C_HAS_ABI_HEADERS
#  include LIBSMBIOS_C_ABI_SUFFIX
#endif


