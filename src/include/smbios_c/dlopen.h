// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
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


#ifndef C_DLOPEN_H
#define C_DLOPEN_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"

EXTERN_C_BEGIN;

// Use this header if you are using dlopen to ensure that you get the same ABI
// version as what you are compiling against
#define LIBSMBIOS_C_LIBNAME "libsmbios_c.so.2"

EXTERN_C_END;

#endif  /* C_DLOPEN_H */
