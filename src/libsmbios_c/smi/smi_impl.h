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

#ifndef SMBIOS_IMPL_H
#define SMBIOS_IMPL_H

#include "smbios_c/compat.h"
#include "smbios_c/smi.h"
#include "smbios_c/types.h"

#include "smbios_c/config/abi_prefix.h"

EXTERN_C_BEGIN;

#undef DEBUG_MODULE_NAME
#define DEBUG_MODULE_NAME "DEBUG_SMI_C"

#define KERNEL_SMI_MAGIC_NUMBER (0x534D4931)   /* "SMI1" */
#define DELL_CALLINTF_SMI_MAGIC_NUMBER   (0x42534931)  /* "BSI1" */

enum {
    class_user_password =  9,
    class_admin_password = 10,
};

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
/* cut and paste from kernel sources */
struct callintf_cmd
{
    u32 magic;
    u32 ebx;
    u32 ecx;
    u16 command_address;
    u8  command_code;
    u8  reserved;
    u8  command_buffer_start[];
}
LIBSMBIOS_C_PACKED_ATTR;


struct smi_cmd_buffer
{
    u16 smi_class;
    u16 smi_select;
    union {  /* to match BIOS docs, can use exact arg names specified in doc */
        u32	     arg[4];
        struct
        {
            u32 cbARG1;
            u32 cbARG2;
            u32 cbARG3;
            u32 cbARG4;
        };
    };
    union {  /* to match BIOS docs, can use exact res names specified in doc */
        u32      res[4];
        struct
        {
            s32 cbRES1;
            s32 cbRES2;
            s32 cbRES3;
            s32 cbRES4;
        };
    };
}
LIBSMBIOS_C_PACKED_ATTR;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

#define ERROR_BUFSIZE 1024

struct dell_smi_obj
{
    int initialized;
    u16 command_address;
    u8  command_code;
    int (*execute)(struct dell_smi_obj *);
    struct smi_cmd_buffer smi_buf;
    u8 *physical_buffer[4];
    size_t physical_buffer_size[4];
    char *errstring;
};

int __hidden init_dell_smi_obj(struct dell_smi_obj *);
int __hidden init_dell_smi_obj_std(struct dell_smi_obj *);



EXTERN_C_END;

#include "smbios_c/config/abi_suffix.h"

#endif /* SMBIOS_IMPL_H */
