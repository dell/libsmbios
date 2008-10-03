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


#ifndef CMOSTOKENLOWLEVEL_H
#define CMOSTOKENLOWLEVEL_H

// compat header should always be first header
#include "smbios/compat.h"

#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{
#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
    enum  // Smbios Structure types
    {
        DellIndexedIoTokenType = 0xD4,
        DellProtectedAreaType1 = 0xD5,
        DellProtectedAreaType2 = 0xD6,
        DellCallingInterface   = 0xDA,
    };

    enum // Token types
    {
        TokenTypeEOT = 0xffff,
    };

    struct indexed_io_token
    {
        u16 tokenId;
        u8  location;
        u8  andMask;
        union {
            u8 orValue;
            u8 stringLength;
        };
    }
    LIBSMBIOS_PACKED_ATTR;

    struct indexed_io_access_structure
    { /* 0xD4 structure */
        u8	     type;
        u8	     length;
        u16	     handle;
        u16      indexPort;
        u16      dataPort;
        u8       checkType;
        u8       checkedRangeStartIndex;
        u8       checkedRangeEndIndex;
        u8       checkValueIndex;
        //variable number of tokens present, but at least one.
        struct   indexed_io_token  first_token;
    }
    LIBSMBIOS_PACKED_ATTR;


    struct dell_protected_value_1_structure
    {  /* 0xD5 structure */
        u8	     type;
        u8	     length;
        u16	     handle;
        u16      tokenId;
        u8       valueLen;
        u8       valueFormat;
        u16      validationKey;
        u16      indexPort;
        u16      dataPort;
        u8       checkType;
        u8       valueStartIndex;
        u8       checkIndex;
    }
    LIBSMBIOS_PACKED_ATTR;

    struct dell_protected_value_2_structure
    {  /* 0xD6 structure */
        u8	     type;
        u8	     length;
        u16	     handle;
        u16      tokenId;
        u8       valueLen;
        u8       valueFormat;
        u16      validationKey;
        u16      indexPort;
        u16      dataPort;
        u8       checkType;
        u8       valueStartIndex;
        u8       checkIndex;
        u8       rangeCheckType;
        u8       rangeCheckStart;
        u8       rangeCheckEnd;
        u8       rangeCheckIndex;
    }
    LIBSMBIOS_PACKED_ATTR;

    struct calling_interface_token
    {
        u16 tokenId;
        u16 location;  // 0 for string tokens
        union {
            u16 value;
            u16 stringLength;
        };
    }
    LIBSMBIOS_PACKED_ATTR;

    struct calling_interface_structure
    { /* 0xDA structure */
        u8	     type;
        u8	     length;
        u16	     handle;

        u16      cmdIOAddress;
        u8       cmdIOCode;
        u32      supportedCmds;

        //variable number of tokens present, zero or more possible
        //would _really_ like to do:
        //   struct   calling_interface_token  token_array[];
        //
        //but cannot because it is a gcc extension. :-(
        struct   calling_interface_token  first_token;
    }
    LIBSMBIOS_PACKED_ATTR;

    enum  // protected value format types
    {
        pvFmtAlphaNumericScanCode = 0,
        pvFmtAlphaNumericAscii    = 1,
        pvFmtAlphaNumericScanCodeNS = 2,
        pvFmtAlphaNumericAsciiNS   = 3,
    };


#if defined(_MSC_VER)
#pragma pack(pop)
#endif
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* CMOSTOKENLOWLEVEL_H */
