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


#ifndef C_CMOS_H
#define C_CMOS_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

/** Read byte from CMOS.
 *  @param byte  pointer to buffer were byte will be stored
 *  @param indexPort  the io port where we write the offset
 *  @param dataPort  the io port where we will read the resulting value
 *  @param offset  the offset into cmos (usually cmos is 256 byte banks)
 *  @return  0 on success, < 0 on failure
 */
int DLL_SPEC cmos_read_byte (u8 *byte, u32 indexPort, u32 dataPort, u32 offset);

/** Write byte to CMOS.
 *  @param byte  byte to write
 *  @param indexPort  the io port where we write the offset
 *  @param dataPort  the io port where we will write the byte
 *  @param offset  the offset into cmos (usually cmos is 256 byte banks)
 *  @return  0 on success, < 0 on failure
 */
int DLL_SPEC cmos_write_byte(u8 byte,  u32 indexPort, u32 dataPort, u32 offset);

/** Run all registered CMOS callbacks.
 * Higher layers can register callbacks that are run when any byte in CMOS is
 * changed. Presently, all these callbacks are used to update checksums in
 * CMOS.  If do_update is false, return code indicates if checksums are
 * currently correct.
 * @param do_update  should callback update checksum if it is wrong
 * @return The return value of all callbacks is 'or'-ed together. Checksum
 * callbacks return 0 if checksum is good and do_update is false. (otherwise
 * they just write the correct checksum)
 */
int DLL_SPEC cmos_run_callbacks(bool do_update);

// not yet implemented
//size_t cmos_fmt_err(const struct cmos_obj *, char *buf, size_t len);

EXTERN_C_END;

#endif  /* CMOS_H */
