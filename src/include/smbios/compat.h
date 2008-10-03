/* vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
 */
/*
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */


#ifndef LIBSMBIOS_COMPAT_HPP_INCLUDED
#define LIBSMBIOS_COMPAT_HPP_INCLUDED

#include "smbios/config.hpp"
#include "smbios/config/auto_link.hpp"

/*
 * CHANGES TO THIS FILE CAUSE A WHOLE-PROJECT REBUILD!
 *      Keep changes here to a minimum!
*/

#if defined(LIBSMBIOS_HAS_DECLSPEC) && defined(LIBSMBIOS_ALL_DYN_LINK)
#       if defined(LIBSMBIOS_SOURCE) || defined(LIBSMBIOS_SOURCE)
#           define DLL_SPEC  __declspec(dllexport)
#       else
#           define DLL_SPEC  __declspec(dllimport)
#       endif  /* SMBIOS_EXPORTS */
#endif

#ifndef DLL_SPEC
#   define DLL_SPEC
#endif


#ifdef LIBSMBIOS_PLATFORM_WIN32
#   define WIN32_LEAN_AND_MEAN		/* Exclude rarely-used stuff from Windows headers */
#   include <windows.h>
/* Windows is lame. */
#   define _snprintf    snprintf

#endif /* LIBSMBIOS_PLATFORM_WIN32 */
#endif /* COMPAT_HPP */
