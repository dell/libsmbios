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

#ifndef C_SMI_H
#define C_SMI_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

enum {
    cbARG1 = 0,
    cbARG2 = 1,
    cbARG3 = 2,
    cbARG4 = 3,
    cbRES1 = 0,
    cbRES2 = 1,
    cbRES3 = 2,
    cbRES4 = 3,
};

void dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);

// prepackaged smi functions
int getPropertyOwnershipTag(char *tagBuf, size_t size);
int setPropertyOwnershipTag(const char *assword, const char *newTag, size_t size);

int readNVStorage         (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);
int readBatteryModeSetting(u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);
int readACModeSetting     (u32 location, u32 *curValue, u32 *minValue, u32 *maxValue);

int writeNVStorage         (const char *password, u32 location, u32 value);
int writeBatteryModeSetting(const char *password, u32 location, u32 value);
int writeACModeSetting     (const char *password, u32 location, u32 value);

EXTERN_C_END;

#endif  /* C_SMI_H */
