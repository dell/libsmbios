/* vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
 *
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


#ifndef LIBSMBIOS_C_MESSAGE_H
#define LIBSMBIOS_C_MESSAGE_H

/* This header file should only ever be included in .cpp files. It should never
 * be included in header files, due to the fact that it will sometimes mess up
 * portions of the standard library because of the redefinitions.
 *
 * Always include this file as the last include in the .cpp file, if it is
 * needed.
 */


#include "smbios_c/compat.h"

#if defined(LIBSMBIOS_C_HAS_GETTEXT)
#   include <libintl.h>
#   define _(String) dgettext (GETTEXT_PACKAGE, String)
#   define gettext_noop(String) String
#   define N_(String) gettext_noop (String)
#   define LIBSMBIOS_C_GETTEXT_DEBUG_STRING "HAS GETTEXT"
#else
#   define _(string) string
#   define N_(string) string
#   define textdomain(Domain)
#   define bindtextdomain(Package, Directory)
#   define LIBSMBIOS_C_GETTEXT_DEBUG_STRING "gettext compiled with stubs and no translation"
#endif


#endif /* LIBSMBIOS_C_MESSAGE_H */
