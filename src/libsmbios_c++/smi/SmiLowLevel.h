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
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */


#ifndef SMILOWLEVEL_H
#define SMILOWLEVEL_H

#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smi
{
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
        /*this should be 'u8 command_buffer[]', but it is not supported in VC6
          therefore, we just hack it, I guess. Remember to subtract this from
          the size when taking the sizeof this struct
        */
        u8  command_buffer_start;
    }
    LIBSMBIOS_PACKED_ATTR;

#define KERNEL_SMI_MAGIC_NUMBER (0x534D4931)   /* "SMI1" */
#define DELL_CALLINTF_SMI_MAGIC_NUMBER   (0x42534931)  /* "BSI1" */

    struct calling_interface_command_buffer
    {
        u16	     smiClass;
        u16	     smiSelect;
        union {  /* to match BIOS docs, can use exact arg names specified in doc */
            u32	     inputArgs[4];
            struct
            {
                u32 cbARG1;
                u32 cbARG2;
                u32 cbARG3;
                u32 cbARG4;
            };
        };
        union {  /* to match BIOS docs, can use exact res names specified in doc */
            u32      outputRes[4];
            struct
            {
                s32 cbRES1;
                s32 cbRES2;
                s32 cbRES3;
                s32 cbRES4;
            };
        };
    }
    LIBSMBIOS_PACKED_ATTR;

    enum
    {
        class_user_password =  9,
        class_admin_password = 10,
    };


#if defined(_MSC_VER)
#pragma pack(pop)
#endif
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* SMILOWLEVEL_H */
