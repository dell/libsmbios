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

#ifndef C_MEMORY_H
#define C_MEMORY_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

/** Read byte range from physical memory address.
 * This function will read a range of bytes from a physical memory address.
 * Note that some OS have severe restrictions on which addresses may be read
 * and written, as well as security restrictions on which security levels are
 * allowed this access.
 *  @param buffer  pointer to buffer were memory will be copied
 *  @param offset  starting memory offset
 *  @param length  how many bytes of memory to copy
 *  @return  0 on success, < 0 on failure
 *          -1 general failure
 *          -5 bad memory_access_object (could not instantiate singleton?)
 *          -6 bad buffer pointer
 */
LIBSMBIOS_C_DLL_SPEC int memory_read(void *buffer, u64 offset, size_t length);

/** Write a buffer to a physical memory address.
 * This function will write a range of bytes to a physical memory address.
 * Note that some OS have severe restrictions on which addresses may be read
 * and written, as well as security restrictions on which security levels are
 * allowed this access.
 *  @param buffer  pointer to buffer containing contents to write
 *  @param offset  starting memory offset
 *  @param length  how many bytes of memory to copy
 *  @return  0 on success, < 0 on failure
 *          -1 general failure
 *          -5 bad memory_access_object (could not instantiate singleton?)
 *          -6 bad buffer pointer
 */
LIBSMBIOS_C_DLL_SPEC int memory_write(void *buffer, u64 offset, size_t length);

/** Search a range of physical addresses for a pattern.
 * Note that some OS have severe restrictions on which addresses may be read
 * and written, as well as security restrictions on which security levels are
 * allowed this access.
 *  @param pat  buffer containing byte pattern to search for
 *  @param patlen length of pattern
 *  @param start  physical address offset to start search
 *  @param end  ending physical address offset
 *  @param stride search for pattern only where physical addresses % stride == 0
 *  @return  -1 on failure. offset of memory address where pattern found on success
 */
LIBSMBIOS_C_DLL_SPEC s64 memory_search(const char *pat, size_t patlen, u64 start, u64 end, u64 stride);

// Following calls must be properly nested in equal pairs

/** Optimize memory device access - request memory device be kept open between calls.
 * By default, the memory device is closed between subsequent calls to
 * read/write.  This is to prevent file descriptor leakage by the libsmbios
 * library. At times, however, the overhead of reopening the memory device file
 * on every access is simply too great. This happens, for example, on memory
 * searches, and can add considerable overhead. This function requests that the
 * memory subsystem leave the device open between calls.  Must be properly
 * nested with memory_suggest_close().
 *
 * No parameters, no return.
 */
LIBSMBIOS_C_DLL_SPEC void LIBSMBIOS_C_DLL_SPEC memory_suggest_leave_open();

/** Optimize memory device access - request memory device be closed between calls.
 * By default, the memory device is closed between subsequent calls to
 * read/write.  This is to prevent file descriptor leakage by the libsmbios
 * library. At times, however, the overhead of reopening the memory device file
 * on every access is simply too great. This happens, for example, on memory
 * searches, and can add considerable overhead. This function cancels a
 * previous request to leave the device open between calls.  Must be properly
 * nested with memory_suggest_leave_open().
 *
 * No parameters, no return.
 */
LIBSMBIOS_C_DLL_SPEC void memory_suggest_close();

/** Returns string describing the last error condition.
 * Can return 0. The buffer used is guaranteed to be valid until the next call
 * to any memory_* function. Copy the contents if you need it longer.
 */
LIBSMBIOS_C_DLL_SPEC const char * memory_strerror();


EXTERN_C_END;

#endif  /* MEMORY_H */
