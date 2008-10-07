//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Jens Maurer 2001 - 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  linux specific config options:

#define LIBSMBIOS_C_PLATFORM "linux"
#define LIBSMBIOS_C_PLATFORM_LINUX

// we can assume gettext on linux
#define LIBSMBIOS_C_HAS_GETTEXT
#define LIBSMBIOS_C_HAS_UNISTD_H
#include <unistd.h>

