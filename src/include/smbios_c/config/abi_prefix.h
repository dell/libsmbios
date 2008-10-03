//  abi_prefix header  -------------------------------------------------------//

// © Copyright John Maddock 2003

// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt).

#ifndef LIBSMBIOS_CONFIG_ABI_PREFIX_H
# define LIBSMBIOS_CONFIG_ABI_PREFIX_H
#else
# error double inclusion of header smbios_c/config/abi_prefix.h is an error
#endif

#include <smbios_c/config.h>

// this must occur after all other includes and before any code appears:
#ifdef LIBSMBIOS_HAS_ABI_HEADERS
#  include LIBSMBIOS_ABI_PREFIX
#endif
