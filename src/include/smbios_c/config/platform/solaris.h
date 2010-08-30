//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Jens Maurer 2001 - 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Solaris specific config options:

#define LIBSMBIOS_C_PLATFORM "Solaris"
#define LIBSMBIOS_C_PLATFORM_SOLARIS

// we can assume gettext on Solaris
#define LIBSMBIOS_C_HAS_GETTEXT
#define LIBSMBIOS_C_HAS_UNISTD_H
#include <unistd.h>

