//  abi_prefix header  -------------------------------------------------------//

// © Copyright John Maddock 2003
   
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt).

#ifndef LIBSMBIOS_CONFIG_ABI_PREFIX_HPP
# define LIBSMBIOS_CONFIG_ABI_PREFIX_HPP
#else
# error double inclusion of header smbios/config/abi_prefix.hpp is an error
#endif

#include <smbios/config.hpp>

// this must occur after all other includes and before any code appears:
#ifdef LIBSMBIOS_HAS_ABI_HEADERS
#  include LIBSMBIOS_ABI_PREFIX
#endif
