//  Boost config.hpp configuration header file  ------------------------------//

//  (C) Copyright John Maddock 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  CAUTION: This file is intended to be completely stable -
//           DO NOT MODIFY THIS FILE!
//
//  Modified for libsmbios project: 2004-03-28 by Michael Brown

#ifndef LIBSMBIOS_CONFIG_H
#define LIBSMBIOS_CONFIG_H

// if we don't have a user config, then use the default location:
#if !defined(LIBSMBIOS_USER_CONFIG) && !defined(LIBSMBIOS_NO_USER_CONFIG)
#  define LIBSMBIOS_USER_CONFIG <smbios/config/user.hpp>
#endif
// include it first:
#ifdef LIBSMBIOS_USER_CONFIG
#  include LIBSMBIOS_USER_CONFIG
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(LIBSMBIOS_COMPILER_CONFIG) && !defined(LIBSMBIOS_NO_COMPILER_CONFIG) && !defined(LIBSMBIOS_NO_CONFIG)
#  include <smbios/config/select_compiler_config.hpp>
#endif
// if we have a compiler config, include it now:
#ifdef LIBSMBIOS_COMPILER_CONFIG
#  include LIBSMBIOS_COMPILER_CONFIG
#endif

// if we don't have a std library config set, try and find one:
#if !defined(LIBSMBIOS_STDLIB_CONFIG) && !defined(LIBSMBIOS_NO_STDLIB_CONFIG) && !defined(LIBSMBIOS_NO_CONFIG)
#  include <smbios/config/select_stdlib_config.hpp>
#endif
// if we have a std library config, include it now:
#ifdef LIBSMBIOS_STDLIB_CONFIG
#  include LIBSMBIOS_STDLIB_CONFIG
#endif

// if we don't have a platform config set, try and find one:
#if !defined(LIBSMBIOS_PLATFORM_CONFIG) && !defined(LIBSMBIOS_NO_PLATFORM_CONFIG) && !defined(LIBSMBIOS_NO_CONFIG)
#  include <smbios/config/select_platform_config.hpp>
#endif
// if we have a platform config, include it now:
#ifdef LIBSMBIOS_PLATFORM_CONFIG
#  include LIBSMBIOS_PLATFORM_CONFIG
#endif

// get config suffix code:
#include <smbios/config/suffix.hpp>

#endif  // LIBSMBIOS_CONFIG_H











