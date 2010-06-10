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

#ifndef C_SYSINFO_H
#define C_SYSINFO_H

#undef DEBUG_MODULE_NAME
#define DEBUG_MODULE_NAME "DEBUG_SYSINFO_C"

#define MAX_SMI_TAG_SIZE 12
#define ERROR_BUFSIZE 1024

__hidden void sysinfo_clearerr();
__hidden char *sysinfo_get_module_error_buf();
__hidden char * smbios_struct_get_string_from_table(u8 type, u8 offset);
__hidden void strip_trailing_whitespace( char *str );
__hidden char *getTagFromSMI(u16 select);
__hidden u32 setTagUsingSMI(u16 select, const char *, u16);

#endif
